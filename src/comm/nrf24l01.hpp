#pragma once

#include <RF24.h>

#include "twoway.hpp"

class NRF final : public TwoWay {
   public:
    NRF(RF24 rf, u32 irq, bool is_drone);
    ~NRF();

   private:
    void read(std::vector<u8> &buf) override;
    void write(std::vector<u8> &buf) override;

    RF24 rf;
    u32 irq;
    bool send;
    const u8 DRONE_TO_GROUND[3] = {'D', '>', 'G'};
    const u8 GROUND_TO_DRONE[3] = {'G', '>', 'D'};
};