#include "hardware/gpio.h"
#include "pico/binary_info.h"

bi_decl(bi_2pins_with_func(PICO_DEFAULT_UART_RX_PIN, PICO_DEFAULT_UART_TX_PIN,
                           GPIO_FUNC_UART));