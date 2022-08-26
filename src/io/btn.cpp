#include "btn.hpp"

#include <hardware/gpio.h>
#include <hardware/sync.h>

#include "../irq.hpp"

Btn::Btn(unsigned int gpio, std::function<void(int64_t us_held)> cb)
    : gpio(gpio) {
    gpio_pull_up(gpio);
    IrqGpio::add_irq(gpio, GPIO_IRQ_EDGE_FALL, [this, cb]() {
        absolute_time_t t = get_absolute_time();
        uint32_t state = save_and_disable_interrupts();
        absolute_time_t last = this->last_press;
        // 0.1s delay between presses to debounce
        if (is_nil_time(this->last_press) ||
            absolute_time_diff_us(this->last_press, t) > 100000) {
            this->last_press = t;
            restore_interrupts(state);
            // TODO
            cb(0);
        } else {
            restore_interrupts(state);
        }
    });
}
Btn::~Btn() { IrqGpio::remove_irq(gpio); }