#pragma once
#include "../common.hpp"

// This must be the only user of ADC
class PowerMon {
   public:
    struct Data {
        f32 voltage;
        f32 current;
        f32 temperature;
    };
    PowerMon(u32 voltage, u32 current);

    PowerMon::Data poll();

   private:
    u32 voltage;
    u32 current;
};