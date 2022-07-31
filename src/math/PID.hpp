#pragma once

// https://en.wikipedia.org/wiki/PID_controller#Pseudocode
class PID {
   public:
    float kp;
    float ki;
    float kd;
    float dt;

    // Initialize PID with PID factors and delta time
    inline PID(float kp, float ki, float kd, float dt)
        : kp(kp), ki(ki), kd(kd), dt(dt) {
        update_params();
    }

    // Iterate PID with error as (setpoint - measured value)
    inline float iterate(float error) {
        error2 = error1;
        error1 = error0;
        error0 = error;
        output += a0 * error0 + a1 * error1 + a2 * error2;
        return output;
    }

    // Reset internal state of PID
    inline void reset() {
        error0 = 0.0;
        error1 = 0.0;
        error2 = 0.0;
        output = 0.0;
    }

    // Update Kp, Ki, Kd, dt
    inline void update_params() {
        // assert!(self.dt > 0.0);
        a0 = kp + ki * dt + kd / dt;
        a1 = -kp - 2.0 * kd / dt;
        a2 = kd / dt;
    }

   private:
    float a0 = 0.0;
    float a1 = 0.0;
    float a2 = 0.0;

    float error0 = 0.0;
    float error1 = 0.0;
    float error2 = 0.0;
    float output = 0.0;
};