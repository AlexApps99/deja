#pragma once
#include <hardware/pwm.h>

#include "../common.hpp"

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

class MotorPWM : public Motor {
   public:
    // pins cannot share slices or bad things will happen!
    MotorPWM(u32 pin);

    ~MotorPWM();

    static const u32 MOTOR_PINS[8];

   private:
    u32 slice_num;
    u32 channel;
    u32 pin;
    pwm_config conf;

    void apply_speed(void) override;
};