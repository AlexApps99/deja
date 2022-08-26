#include "irq.hpp"

#include <hardware/gpio.h>
#include <hardware/sync.h>

#include <algorithm>

void IrqGpio::interrupt_handler(uint gpio, uint32_t events) {
    for (const Irq& i : interrupts) {
        if (i.gpio == gpio && (i.events & events) != 0) {
            i.cb();
            break;
        }
    }
}

// TODO a critical section should be here
void IrqGpio::add_irq(IrqGpio::Irq irq) {
    uint32_t status = save_and_disable_interrupts();
    bool empty = interrupts.empty();
    interrupts.push_back(irq);
    restore_interrupts(status);
    if (empty) {
        gpio_set_irq_enabled_with_callback(irq.gpio, irq.events, true,
                                           &interrupt_handler);
    } else {
        gpio_set_irq_enabled(irq.gpio, irq.events, true);
    }
}

void IrqGpio::remove_irq(unsigned int gpio) {
    uint32_t status = save_and_disable_interrupts();
    interrupts.erase(std::remove_if(interrupts.begin(), interrupts.end(),
                                    [gpio](const Irq& v) {
                                        bool del = v.gpio == gpio;
                                        if (del)
                                            gpio_set_irq_enabled(
                                                v.gpio, v.events, false);
                                        return del;
                                    }),
                     interrupts.end());
    restore_interrupts(status);
}

std::vector<IrqGpio::Irq> IrqGpio::interrupts{};