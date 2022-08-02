#include "hc_sr04.hpp"

#include <hardware/gpio.h>
#include <hardware/pio.h>
#include <pico/binary_info.h>

#include "../common.hpp"
#include "hc_sr04.pio.h"

bi_decl(bi_2pins_with_names(PIN_HC_SR04_TRIG, "HC-SR04 TRIG", PIN_HC_SR04_ECHO,
                            "HC-SR04 ECHO"));

HcSr04::HcSr04(u32 trig, u32 echo, PIO pio) : pio(pio), trig(trig), echo(echo) {
    offset = pio_add_program(pio, &hc_sr04_program);
    sm = pio_claim_unused_sm(pio, true);
    hc_sr04_init(pio, sm, offset, trig, echo);
}

f32 HcSr04::update() {
    uint len = pio_sm_get_rx_fifo_level(pio, sm);
    u32 data = 0;
    // Drain FIFO
    for (uint x = 0; x < len; x++) {
        data = pio_sm_get(pio, sm);
    }
    const f32 conv = 1e-6 * (343.2 / 2.0);
    pio_interrupt_clear(pio, 0);
    return data * conv;
}

HcSr04::~HcSr04() {
    pio_sm_set_enabled(pio, sm, false);
    gpio_set_function(trig, GPIO_FUNC_NULL);
    gpio_set_function(echo, GPIO_FUNC_NULL);
    pio_sm_unclaim(pio, sm);
}