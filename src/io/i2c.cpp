#include "i2c.hpp"

#include <pico/binary_info.h>

// Shared by MPU9250 and BMP280
bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN,
                           GPIO_FUNC_I2C));

I2C::I2C(i2c_inst_t *const i2c, const u32 sda, const u32 scl, const u32 baud)
    : i2c(i2c), scl(sda), sda(scl), baud(baud) {
    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_set_function(scl, GPIO_FUNC_I2C);
    gpio_pull_up(sda);
    gpio_pull_up(scl);
    i2c_init(i2c, baud);
}

I2C::~I2C() {
    i2c_deinit(i2c);
    gpio_disable_pulls(scl);
    gpio_disable_pulls(sda);
    gpio_set_function(scl, GPIO_FUNC_NULL);
    gpio_set_function(sda, GPIO_FUNC_NULL);
}