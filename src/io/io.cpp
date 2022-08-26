#include "io.hpp"

IO::IO(std::function<void(Mpu9250::Data data)> mpu_cb,
       std::function<void(f32 delta_height)> bmp_cb,
       std::function<void(int64_t us_held)> button_cb)
    : i2c(i2c0, PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, 400000),
      bmp280(i2c, bmp_cb),
      hcsr04(PIN_HC_SR04_TRIG, PIN_HC_SR04_ECHO),
      motors{
          MotorPWM::MOTOR_PINS[0], MotorPWM::MOTOR_PINS[1],
          MotorPWM::MOTOR_PINS[2], MotorPWM::MOTOR_PINS[3],
          MotorPWM::MOTOR_PINS[4], MotorPWM::MOTOR_PINS[5],
      },
      mpu(i2c, PIN_MPU9250_INT, mpu_cb),
      action(PIN_ACTION, button_cb) {
    mpu.init();
    bmp280.init();
}

IO::~IO() {}