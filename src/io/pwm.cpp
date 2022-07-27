#include "hardware/pwm.h"

#include "../common.hpp"
#include "../io.hpp"
#include "hardware/gpio.h"
#include "pico/binary_info.h"

bi_decl(bi_4pins_with_func(PIN_PWM0, PIN_PWM1, PIN_PWM2, PIN_PWM3,
                           GPIO_FUNC_PWM));
bi_decl(bi_4pins_with_func(PIN_PWM4, PIN_PWM5, PIN_PWM6, PIN_PWM7,
                           GPIO_FUNC_PWM));

// 1000us - 2000us
// 125mhz

class MotorPWM : Motor {
   public:
    // pins cannot share slices or bad things will happen!
    MotorPWM(u32 pin)
        : slice_num(pwm_gpio_to_slice_num(pin)),
          channel(pwm_gpio_to_channel(pin)),
          pin(pin),
          conf(pwm_get_default_config()) {
        // (125MHz * [1000,2000]us) / 62500 = [2, 4]
        // 62500 = common divisors of [125000, 250000] within 0xFFFF
        // (divider that would make 1 cycle [1000,2000]us) / 62500
        // means divider that makes 62500 cycles [1000,2000]us
        // so if I have a divider of 4, 62500 cycles = 2000us
        // so 31250 = 1000us, I can control speed using wrap
        pwm_config_set_clkdiv_int(&conf, 4);
        // 1000us
        pwm_config_set_wrap(&conf, 62500 / 2);
        pwm_init(slice_num, &conf, true);
        pwm_set_chan_level(slice_num, channel, 62500 / 4);
        pin_old = gpio_get_function(pin);
        gpio_set_function(pin, GPIO_FUNC_PWM);
    }

    ~MotorPWM() {
        pwm_set_enabled(slice_num, false);
        gpio_set_function(pin, pin_old);
    }

   private:
    u32 slice_num;
    u32 channel;
    u32 pin;
    pwm_config conf;

    gpio_function pin_old;

    void apply_speed(void) override {
        // see math explanation
        f32 v = (62500.0 / 2.0) * speed;
        u16 n = (62500 / 2) + (u16)v;
        pwm_set_wrap(slice_num, n);
        // 50% duty
        pwm_set_chan_level(slice_num, channel, n / 2);
    }
};
