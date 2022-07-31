//#include "pico/stdio.h"
#include <pico/binary_info.h>
#include <pico/stdio_usb.h>
#include <pico/stdlib.h>

#include "comm.hpp"
#include "common.hpp"
#include "io/io.hpp"

bi_decl(bi_program_description("Drone firmware by AlexApps99"));

int main() {
    stdio_usb_init();
    Log::set_stdio();
    Linfo("Start up successful!");
    return 0;
}
