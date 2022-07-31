#include <bmp2.h>
#include <hardware/gpio.h>
#include <hardware/i2c.h>
#include <pico/time.h>
#include <string.h>

#include "i2c.hpp"

// TODO implement BMP280
// https://github.com/raspberrypi/pico-examples/blob/master/i2c/bmp280_i2c/bmp280_i2c.c
// https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmp280-ds001.pdf

class BMP280 {
   public:
    BMP280(I2C &i2c) : i2c(i2c) {}
    I2C &i2c;

   private:
    static BMP2_INTF_RET_TYPE read(u8 reg_addr, u8 *reg_data, u32 length,
                                   void *intf_ptr) {
        BMP280 *self = static_cast<BMP280 *>(intf_ptr);
        u8 slave_addr = self->dev_addr;
        if ((self->i2c.write_blocking(slave_addr, &reg_addr, 1, true) != 1) ||
            (self->i2c.read_blocking(slave_addr, reg_data, length, false) !=
             length))
            return -1;

        return 0;
    }

    static BMP2_INTF_RET_TYPE write(u8 reg_addr, const u8 *reg_data, u32 length,
                                    void *intf_ptr) {
        BMP280 *self = static_cast<BMP280 *>(intf_ptr);
        u8 slave_addr = self->dev_addr;
        unsigned char buf[length + 1];
        buf[0] = reg_addr;
        memcpy(&buf[1], reg_data, length);

        if (self->i2c.write_blocking(slave_addr, &buf[0], length + 1, false) !=
            length + 1)
            return -1;

        return 0;
    }

    inline static void delay_us(u32 period, void *intf_ptr) {
        sleep_us(period);
    }
    bmp2_dev bmp = {0};
    // TODO support secondary in constructor
    u8 dev_addr = BMP2_I2C_ADDR_PRIM;
};