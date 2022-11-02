#pragma once

#include "twoway.hpp"

class Uart final : public TwoWay {
   public:
    Uart(uint baud = 115200);
    ~Uart();

   private:
    void read(std::vector<u8> &buf) override;
    void write(std::vector<u8> &buf) override;
    static void irq_cb(void);
    void fill_buf();
    std::vector<u8> buf{};

    static Uart *instance;
};