#pragma once

#include <functional>

#include "i2c.hpp"

// ONLY ONE INSTANCE CAN EXIST
class Mpu9250 {
   public:
    static constexpr unsigned int SAMPLE_RATE = 10;
    static constexpr f32 GYRO_RES = 500.0 / 32768.0;
    struct Data {
        short gyro[3] = {0};
        short accel[3] = {0};
        short mag[3] = {0};
    };

    // IMPORTANT: the callback is called from an INTERRUPT
    Mpu9250(I2C& i2c, u32 int_p, std::function<void(Data data)> cb);
    ~Mpu9250();
    void init();
    void interrupt_handler();

   private:
    std::function<void(Data data)> cb;

    I2C& i2c;
    u32 int_p;
};