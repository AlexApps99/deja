#pragma once
#include <bmp2.h>
#include <pico/time.h>

#include <cmath>
#include <functional>
#include <optional>

#include "i2c.hpp"

class Bmp280 {
   public:
    inline Bmp280(I2C &i2c, std::function<void(f32 delta_height)> cb,
                  u8 addr = BMP2_I2C_ADDR_PRIM)
        : i2c(i2c), cb(cb), dev_addr(addr) {
        bmp.intf_ptr = this;
    }

    inline ~Bmp280() { cancel_repeating_timer(&timer); }

    Bmp280(const Bmp280 &) = delete;

    bool init();

   private:
    // calculates new delta height
    void get_data();

    static bool int_handler(repeating_timer_t *d);

    static BMP2_INTF_RET_TYPE read(u8 reg_addr, u8 *reg_data, u32 length,
                                   void *intf_ptr);

    static BMP2_INTF_RET_TYPE write(u8 reg_addr, const u8 *reg_data, u32 length,
                                    void *intf_ptr);

    static void delay_us(u32 period, void *intf_ptr);
    bmp2_dev bmp = {
        .intf = BMP2_I2C_INTF,
        .read = read,
        .write = write,
        .delay_us = delay_us,
    };
    I2C &i2c;
    u8 dev_addr;
    u32 interval_us = 0;
    repeating_timer_t timer = {0};
    // origin pressure (to be measured from), in Pa * 256
    f32 origin_pressure = NAN;
    std::function<void(f32 delta_height)> cb;
};