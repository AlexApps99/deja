#include "mpu9250.hpp"

#include <hardware/gpio.h>
extern "C" {
#include <inv_mpu.h>
}
#include <pico/binary_info.h>

#include "../irq.hpp"

bi_decl(bi_1pin_with_name(PIN_MPU9250_INT, "MPU9250 INT"));

// I can't use FIFO because it doesn't seem to support compass
// however, 0x49/73 seems to be allowable on the FIFO?
// I need to test it, obviously (not a priority)

// TODO find an appropriate DLPF setting

Mpu9250::Mpu9250(I2C& i2c, u32 int_p, std::function<void(Data data)> cb)
    : i2c(i2c), int_p(int_p), cb(cb) {}

void Mpu9250::init() {
    mpu_init(i2c.i2c);
    // 2000 dps is higher than necessary
    mpu_set_gyro_fsr(500);
    mpu_set_accel_fsr(2);
    // mpu_set_lpf(42);
    mpu_set_sample_rate(SAMPLE_RATE);
    mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS);
    mpu_set_int_level(0);

    gpio_pull_down(int_p);
    IrqGpio::add_irq(int_p, GPIO_IRQ_EDGE_RISE,
                     [this]() { this->interrupt_handler(); });
}

Mpu9250::~Mpu9250() { IrqGpio::remove_irq(int_p); }

void Mpu9250::interrupt_handler() {
    Data out = {0};
    int ret_g = mpu_get_gyro_reg(out.gyro);
    int ret_a = mpu_get_accel_reg(out.accel);
    int ret_m = mpu_get_compass_reg(out.mag);
    Ldebug("%hd %hd %hd, %hd %hd %hd, %hd %hd %hd", out.gyro[0], out.gyro[1],
           out.gyro[2], out.accel[0], out.accel[1], out.accel[2], out.mag[0],
           out.mag[1], out.mag[2]);
    if (ret_g < 0 || ret_a < 0 || ret_m < 0) {
        Lerror("MPU read error %d %d %d", ret_g, ret_a, ret_m);
    } else {
        cb(out);
    }
}