#pragma once
#include "../common.hpp"

struct PowerMonData {
    f32 voltage;
    f32 current;
    f32 temperature;
};

// This must be the only user of ADC
class PowerMon {
   public:
    PowerMon(u32 voltage, u32 current);

    PowerMonData poll();

   private:
    u32 voltage;
    u32 current;
};