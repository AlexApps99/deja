#pragma once
#include <bmp2.h>
#include <pico/time.h>

#include <optional>

#include "i2c.hpp"

class Bmp280 {
   public:
    struct Data {
        // Altitude (meters)
        f32 altitude;
        // Temperature (degrees Celcius)
        f32 temperature;
    };
    inline Bmp280(I2C &i2c, u8 addr = BMP2_I2C_ADDR_PRIM)
        : i2c(i2c), dev_addr(addr) {
        bmp.intf_ptr = this;
    }

    bool init();

    // Returns last measured data
    std::optional<Bmp280::Data> get_data();

   private:
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
};