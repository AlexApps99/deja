#pragma once

#include <stdint.h>

#include <functional>
#include <vector>

class IrqGpio {
    struct Irq {
        unsigned int gpio;
        uint32_t events;
        std::function<void()> cb;
    };
    static void add_irq(Irq irq);
    static std::vector<Irq> interrupts;
    static void interrupt_handler(unsigned int gpio, uint32_t events);

   public:
    inline static void add_irq(unsigned int gpio, uint32_t events,
                               std::function<void()> cb) {
        add_irq({.gpio = gpio, .events = events, .cb = cb});
    }

    static void remove_irq(unsigned int gpio);
};