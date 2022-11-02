#include "bmp280.hpp"

#include <bmp2.h>
#include <hardware/gpio.h>
#include <hardware/i2c.h>
#include <pico/time.h>
#include <string.h>

#include <cmath>
#include <optional>

#include "../common.hpp"
#include "i2c.hpp"

// https://github.com/raspberrypi/pico-examples/blob/master/i2c/bmp280_i2c/bmp280_i2c.c
// https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmp280-ds001.pdf

static const char *err_msg(i8 res) {
    switch (res) {
        case BMP2_OK:
            return nullptr;
        case BMP2_E_NULL_PTR:
            return "Null pointer error. It occurs when the user tries to "
                   "assign value (not address) to a pointer, which has "
                   "been initialized to NULL.";
            break;
        case BMP2_E_COM_FAIL:
            return "Communication failure error. It occurs due to "
                   "read/write operation failure and also "
                   "due to power failure during communication";
        case BMP2_E_INVALID_LEN:
            return "Invalid length error. Occurs when length of data to be "
                   "written is zero";
        case BMP2_E_DEV_NOT_FOUND:
            return "Device not found error. It occurs when the device chip "
                   "id is incorrectly read";
        case BMP2_E_UNCOMP_TEMP_RANGE:
            return "Uncompensated temperature data not in valid range "
                   "error.";
        case BMP2_E_UNCOMP_PRESS_RANGE:
            return "Uncompensated pressure data not in valid range error.";
        case BMP2_E_UNCOMP_TEMP_AND_PRESS_RANGE:
            return "Uncompensated pressure data and uncompensated "
                   "temperature data are not in valid range error.";
        default:
            return "Unknown error code";
    }
}

bool Bmp280::init() {
    const char *msg = nullptr;
    bmp2_config conf = {0};

    // Initialize BMP2
    msg = err_msg(bmp2_init(&bmp));
    if (msg) goto err;
    // Reset BMP2 and wait a bit
    msg = err_msg(bmp2_soft_reset(&bmp));
    if (msg) goto err;
    sleep_ms(100);
    // Get current config before modifying
    msg = err_msg(bmp2_get_config(&conf, &bmp));
    if (msg) goto err;

    // highest performance settings (temperature not too high)
    conf.filter = BMP2_FILTER_COEFF_16;
    conf.os_mode = BMP2_OS_MODE_ULTRA_HIGH_RESOLUTION;
    conf.os_temp = BMP2_OS_1X;
    conf.os_pres = BMP2_OS_8X;
    conf.odr = BMP2_ODR_0_5_MS;

    // Set new config
    msg = err_msg(bmp2_set_config(&conf, &bmp));
    if (msg) goto err;
    // Set power mode
    msg = err_msg(bmp2_set_power_mode(BMP2_POWERMODE_NORMAL, &conf, &bmp));
    if (msg) goto err;
    msg = err_msg(bmp2_compute_meas_time(&interval_us, &conf, &bmp));
    if (msg) goto err;

    Ldebug("BMP interval: %u us", interval_us);
    add_repeating_timer_us(interval_us, int_handler, this, &timer);
    return true;

err:
    Lerror("%s", msg);
    return false;
}

// Convert pressure and temperature (compensated BMP units) to height in meters
// https://en.wikipedia.org/wiki/Hypsometric_equation
void Bmp280::get_data() {
    const char *msg = nullptr;
    bmp2_status status = {0};
    bmp2_data comp_data = {0};

    msg = err_msg(bmp2_get_status(&status, &bmp));
    if (msg) goto err;

    // I am missing a lot of the MEAS_DONE windows because it's only 0.5ms long
    // You can read at any point, though
    // currently I'm just going to read at every opportunity, but it could be
    // wise to save the old value in case it's duplicated
    // (saving on some calculations)
    // I could also take the approach of watching for changes in measurement
    // state, that would require more frequent polling

    // if (status.measuring == BMP2_MEAS_DONE)
    {
        msg = err_msg(bmp2_get_sensor_data(&comp_data, &bmp));
        if (msg) goto err;

        // Gravity
        constexpr f32 g = 9.80665;
        // Molar mass of Earth's air
        constexpr f32 M = 0.02896968;
        // Universal gas constant
        constexpr f32 R0 = 8.314462618;
        // Specific gas constant for dry air
        constexpr f32 R = R0 / M;
        constexpr f32 R_g = R / g;

        // This includes the division term for the temperature
        constexpr f32 fac = R_g / 100.0;

        u32 t = comp_data.temperature + 27315;

        if (std::isnan(origin_pressure)) {
            origin_pressure = comp_data.pressure;
            return;
        }

        f32 delta =
            fac * (f32)t * logf(origin_pressure / (f32)comp_data.pressure);
        cb(delta);
        return;
    }
    // else {
    //    if (status.measuring != BMP2_MEAS_ONGOING)
    //        Ldebug("BMP %hhu", status.measuring);
    //    return;
    //}

err:
    Lerror("%s", msg);
}

BMP2_INTF_RET_TYPE Bmp280::read(u8 reg_addr, u8 *reg_data, u32 length,
                                void *intf_ptr) {
    Bmp280 *self = static_cast<Bmp280 *>(intf_ptr);
    u8 slave_addr = self->dev_addr;
    if ((self->i2c.write_blocking(slave_addr, &reg_addr, 1, true) != 1) ||
        (self->i2c.read_blocking(slave_addr, reg_data, length, false) !=
         length))
        return -1;

    return 0;
}

BMP2_INTF_RET_TYPE Bmp280::write(u8 reg_addr, const u8 *reg_data, u32 length,
                                 void *intf_ptr) {
    Bmp280 *self = static_cast<Bmp280 *>(intf_ptr);
    u8 slave_addr = self->dev_addr;
    unsigned char buf[length + 1];
    buf[0] = reg_addr;
    memcpy(&buf[1], reg_data, length);

    if (self->i2c.write_blocking(slave_addr, &buf[0], length + 1, false) !=
        length + 1)
        return -1;

    return 0;
}

void Bmp280::delay_us(u32 period, void *intf_ptr) { sleep_us(period); }

bool Bmp280::int_handler(repeating_timer_t *d) {
    Bmp280 *b = static_cast<Bmp280 *>(d->user_data);
    b->get_data();
    return true;
}