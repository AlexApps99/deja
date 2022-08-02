//#include "pico/stdio.h"
#include <pico/binary_info.h>
#include <pico/stdio_usb.h>
#include <pico/stdlib.h>

#include "comm/comm.hpp"
#include "common.hpp"
#include "io/io.hpp"

bi_decl(bi_program_description("Drone firmware by AlexApps99"));

struct IO {
    IO()
        : i2c(i2c0, PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, 400000),
          bmp280(i2c),
          hcsr04(PIN_HC_SR04_TRIG, PIN_HC_SR04_ECHO),
          gm(PIN_PM_VOLTAGE, PIN_PM_CURRENT),
          motors{
              MotorPWM::MOTOR_PINS[0], MotorPWM::MOTOR_PINS[1],
              MotorPWM::MOTOR_PINS[2], MotorPWM::MOTOR_PINS[3],
              MotorPWM::MOTOR_PINS[4], MotorPWM::MOTOR_PINS[5],
          } {}

    I2C i2c;
    Bmp280 bmp280;
    HcSr04 hcsr04;
    PowerMon gm;
    MotorPWM motors[NUM_MOTORS];
};

int main() {
    IO io{};
    stdio_usb_init();
    Log::set_stdio();
    Linfo("Start up successful!");
    return 0;
}
