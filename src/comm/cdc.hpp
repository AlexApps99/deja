#pragma once

#include "twoway.hpp"

#define CDC_BLOCK_SIZE 256U

class CDC final : public TwoWay {
   public:
    CDC();
    ~CDC();

   private:
    void read(std::vector<u8> &buf) override;
    void write(std::vector<u8> &buf) override;
};