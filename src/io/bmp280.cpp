#include "bmp280.hpp"

#include <bmp2.h>
#include <hardware/gpio.h>
#include <hardware/i2c.h>
#include <math.h>
#include <pico/time.h>
#include <string.h>

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

// TODO repeated_timer
bool Bmp280::init() {
    const char *msg = nullptr;
    bmp2_config conf = {0};

    // Initialize BMP2
    msg = err_msg(bmp2_init(&bmp));
    if (msg) goto err;
    // Get current config before modifying
    msg = err_msg(bmp2_get_config(&conf, &bmp));
    if (msg) goto err;

    // highest performance settings (temperature not too high)
    conf.filter = BMP2_FILTER_COEFF_16;
    conf.os_mode = BMP2_OS_MODE_ULTRA_HIGH_RESOLUTION;
    conf.os_temp = BMP2_OS_2X;
    conf.os_pres = BMP2_OS_16X;
    conf.odr = BMP2_ODR_0_5_MS;

    // Set new config
    msg = err_msg(bmp2_set_config(&conf, &bmp));
    if (msg) goto err;
    // Set power mode
    msg = err_msg(bmp2_set_power_mode(BMP2_POWERMODE_NORMAL, &conf, &bmp));
    if (msg) goto err;
    msg = err_msg(bmp2_compute_meas_time(&interval_us, &conf, &bmp));
    if (msg) goto err;
    return true;

err:
    Lerror("%s", msg);
    return false;
}

// Convert pressure in pascals to height in meters
// TODO use temperature too
// https://en.wikipedia.org/wiki/Barometric_formula#Pressure_equations
// https://en.wikipedia.org/wiki/Atmospheric_pressure#Altitude_variation
inline static f32 pa_to_m(u32 pa) {
    // Atmospheric pressure at sea level
    constexpr f32 p0 = 101325.0;
    // Temperature (kelvin) at sea level
    constexpr f32 T0 = 288.16;
    // Temperature lapse rate (K/m)
    constexpr f32 L = 0.00976;
    // Gravity
    constexpr f32 g = 9.80665;
    // Molar mass of Earth's air
    constexpr f32 M = 0.02896968;
    // Universal gas constant
    constexpr f32 R0 = 8.314462618;
    constexpr f32 T0_L = T0 / L;
    constexpr f32 fac = (R0 * L) / (g * M);
    return T0_L * (pow(pa / p0, fac) - 1.0);
}

std::optional<Bmp280::Data> Bmp280::get_data() {
    const char *msg = nullptr;
    bmp2_status status = {0};
    bmp2_data comp_data = {0};

    msg = err_msg(bmp2_get_status(&status, &bmp));
    if (msg) goto err;

    if (status.measuring == BMP2_MEAS_DONE) {
        msg = err_msg(bmp2_get_sensor_data(&comp_data, &bmp));
        if (msg) goto err;
        Bmp280::Data data{.altitude = pa_to_m(comp_data.pressure),
                          .temperature = (f32)(comp_data.temperature * 0.01)};
        return data;
    } else {
        return {};
    }

err:
    Lerror("%s", msg);
    return {};
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