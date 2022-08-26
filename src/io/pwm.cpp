#include "pwm.hpp"

#include <hardware/gpio.h>
#include <hardware/pwm.h>
#include <pico/binary_info.h>

#include "../common.hpp"

bi_decl(bi_4pins_with_func(PIN_PWM0, PIN_PWM1, PIN_PWM2, PIN_PWM3,
                           GPIO_FUNC_PWM));
bi_decl(bi_4pins_with_func(PIN_PWM4, PIN_PWM5, PIN_PWM6, PIN_PWM7,
                           GPIO_FUNC_PWM));

// 1000us - 2000us
// 125mhz

// pins cannot share slices or bad things will happen!
MotorPWM::MotorPWM(u32 pin)
    : slice_num(pwm_gpio_to_slice_num(pin)),
      channel(pwm_gpio_to_channel(pin)),
      pin(pin),
      conf(pwm_get_default_config()) {
    // clkdiv of 4 means (1/125MHz)*4*x = 1/490Hz
    // x = 63775.510
    // ((1/125MHz)*4)*x=2ms
    // x = 62500
    // 1ms -> 62500 / 2
    pwm_config_set_clkdiv_int(&conf, 4);
    // 1000us
    pwm_config_set_wrap(&conf, 63776);
    pwm_init(slice_num, &conf, true);
    pwm_set_chan_level(slice_num, channel, 62500 / 2);
    gpio_set_function(pin, GPIO_FUNC_PWM);
}

MotorPWM::~MotorPWM() {
    pwm_set_enabled(slice_num, false);
    gpio_set_function(pin, GPIO_FUNC_NULL);
}

void MotorPWM::apply_speed(void) {
    // see math explanation
    f32 v = (62500.0 / 2.0) * speed;
    u16 n = (62500 / 2) + (u16)v;
    pwm_set_chan_level(slice_num, channel, n);
}

const u32 MotorPWM::MOTOR_PINS[8] = {
    PIN_PWM0, PIN_PWM1, PIN_PWM2, PIN_PWM3,
    PIN_PWM4, PIN_PWM5, PIN_PWM6, PIN_PWM7,
};