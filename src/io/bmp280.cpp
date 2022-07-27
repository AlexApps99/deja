#include "hardware/gpio.h"
#include "pico/binary_info.h"

bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN,
                           GPIO_FUNC_I2C));