#pragma once

#include <pico/time.h>

#include <functional>

// Debouncing button
class Btn {
   public:
    // Set pull-up, add IRQ
    Btn(unsigned int gpio, std::function<void(int64_t us_held)> cb);
    // remove IRQ
    ~Btn();

   private:
    unsigned int gpio;
    absolute_time_t last_press = nil_time;
};