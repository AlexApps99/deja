#pragma once

#include <vector>

#include "common.hpp"

class Motor {
   public:
    explicit inline operator f32() const { return speed; }
    inline f32 set_speed(const f32 speed, const f32 max_speed) {
        this->speed = safe_speed(speed, max_speed);
        apply_speed();
        return this->speed;
    }

   protected:
    f32 speed = 0.0;

   private:
    inline f32 safe_speed(const f32 speed, const f32 max_speed) {
        f32 safe_speed = speed > max_speed ? max_speed : speed;
        safe_speed = safe_speed >= 0.0 ? safe_speed : 0.0;
        safe_speed = safe_speed <= 1.0 ? safe_speed : 1.0;
        return safe_speed;
    }
    // Speed [0.0, 1.0]
    virtual void apply_speed(void) = 0;
};

template <typename T>
class Sensor {
   protected:
    std::vector<T> buf;
};

struct IMUData {
    f32 accel[3];
    f32 gyro[3];
    f32 mag[3];
};

class IMU : public Sensor<IMUData> {};

class Ultrasonic : public Sensor<f32> {};

class IO {
   public:
    Motor* motors[NUM_MOTORS];
    IMU imu;
    Ultrasonic ultrasonic;
};
