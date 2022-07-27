//#include "pico/stdio.h"
#include "comm.hpp"
#include "common.hpp"
#include "io.hpp"
#include "log.hpp"
#include "pico/binary_info.h"
#include "pico/stdio_usb.h"
#include "pico/stdlib.h"

bi_decl(bi_program_description("Drone firmware by AlexApps99"));
// bi_decl(bi_4pins_with_func(PICO_DEFAULT_SPI_RX,PICO_DEFAULT_SPI_TX,PICO_DEFAULT_SPI_SCK_PIN,PICO_DEFAULT_SPI_CSN,GPIO_FUNC_SPI));
// bi_decl(bi_1pin_with_name(0, "e"));

int main() {
    stdio_usb_init();
    Log::set_stdio();
    Linfo("Start up successful!");
    return 0;
}
