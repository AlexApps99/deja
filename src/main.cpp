//#include "pico/stdio.h"
#include <pico/binary_info.h>
#include <pico/stdio_usb.h>
#include <pico/stdlib.h>
#include <pico/time.h>

#include <cmath>

#include "comm/cdc.hpp"
#include "comm/comm.hpp"
#include "comm/uart.hpp"
#include "common.hpp"
#include "io/io.hpp"
#include "irq.hpp"
#include "math/MadgwickAHRS.hpp"

// What is the architecture of the drone?
// Sensors record data either on a loop, or using interrupts
// In the loop, PID coefficients are updated and the motors are changed
// things running outside the loop are responsible for filtering/processing
// their own data

bi_decl(bi_program_description("Drone firmware by AlexApps99"));

f32 motor_speed = 1.0;
// TODO add logging utilities for telemetry
int main() {
    CDC cdc{};
    // Uart uart{19200};
    //   sleep_ms(1000);
    Comm<true> c{cdc};
    c.set_global();
    // sleep_ms(5000);
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    MadgwickAHRS ahrs(1. / Mpu9250::SAMPLE_RATE);
    IO io(
        [&ahrs](Mpu9250::Data data) {
            f64 gx = data.gyro[0] * (f64)Mpu9250::GYRO_RES;
            f64 gy = data.gyro[1] * (f64)Mpu9250::GYRO_RES;
            f64 gz = data.gyro[2] * (f64)Mpu9250::GYRO_RES;
            f64 ax = data.accel[0];
            f64 ay = data.accel[1];
            f64 az = data.accel[2];
            f64 mx = data.mag[0];
            f64 my = data.mag[1];
            f64 mz = data.mag[2];

            ahrs.Update(gx, gy, gz, ax, ay, az, mx, my, mz);
            Ldebug("AHRS %f %f %f %f", ahrs.quaternion[0], ahrs.quaternion[1],
                   ahrs.quaternion[2], ahrs.quaternion[3])
        },
        [](f32 delta_height) { Ldebug("BMP %f", delta_height); },
        [](int64_t us_held) {
            Ldebug("Action");
            if (motor_speed < 0.5)
                motor_speed = 1.0;
            else
                motor_speed = 0.0;
        });

    c.flush();
    bool light_on = true;

    while (1) {
        gpio_put(PICO_DEFAULT_LED_PIN, light_on);
        light_on = !light_on;
        f32 h = io.hcsr04.update();
        if (!std::isnan(h)) Linfo("HC %f", h);
        Linfo("tick");
        c.flush();
        busy_wait_ms(1000);

        for (unsigned int x = 0; x < 6; x++) {
            io.motors[x].set_speed(motor_speed, 1.0);
        }
    }
    return 0;
}