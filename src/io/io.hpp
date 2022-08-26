#pragma once

#include "bmp280.hpp"
#include "btn.hpp"
#include "hc_sr04.hpp"
#include "i2c.hpp"
#include "mpu9250.hpp"
#include "power.hpp"
#include "pwm.hpp"
#include "ublox.hpp"

struct IO {
    IO(std::function<void(Mpu9250::Data data)> mpu_cb,
       std::function<void(f32 delta_height)> bmp_cb,
       std::function<void(int64_t us_held)> button_cb);
    ~IO();

    I2C i2c;
    Bmp280 bmp280;
    HcSr04 hcsr04;
    MotorPWM motors[NUM_MOTORS];
    Mpu9250 mpu;
    Btn action;
};