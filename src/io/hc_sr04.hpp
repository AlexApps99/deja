#pragma once
#include <hardware/pio.h>

#include "../common.hpp"

class HcSr04 {
   public:
    // Initialize HC-SR04 sensor with TRIG/ECHO pin and PIO
    HcSr04(u32 trig, u32 echo, PIO pio = pio0);

    // Get last distance value, and start another check
    f32 update();

    ~HcSr04();

   private:
    PIO pio;
    uint sm;
    uint offset;

    u32 trig;
    u32 echo;
};