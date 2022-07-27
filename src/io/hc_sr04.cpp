#include "../common.hpp"
#include "../io.hpp"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include "hc_sr04.pio.h"
#include "pico/binary_info.h"

bi_decl(bi_2pins_with_names(PIN_HC_SR04_TRIG, "HC-SR04 TRIG", PIN_HC_SR04_ECHO,
                            "HC-SR04 ECHO"));

// uses PIO0 to handle ultrasonic sensor
// must be a singleton so IRQ can work safely
class HcSr04 : Ultrasonic {
   public:
    ~HcSr04() {
        irq_remove_handler(PIO0_IRQ_0, HcSr04::irq_handler);
        pio_sm_set_enabled(pio, sm, false);
        gpio_set_function(trig, trig_old);
        pio_sm_unclaim(pio, sm);
    }

    static inline HcSr04& instance() { return *hc_sr04; }
    static inline void setup(u32 trig, u32 echo) {
        if (!hc_sr04) hc_sr04 = new HcSr04(trig, echo);
    }

   private:
    HcSr04(u32 trig, u32 echo) : pio(pio0), trig(trig), echo(echo) {
        offset = pio_add_program(pio, &hc_sr04_program);
        sm = pio_claim_unused_sm(pio, true);
        trig_old = gpio_get_function(trig);
        hc_sr04_init(pio, sm, offset, trig, echo);
        irq_add_shared_handler(PIO0_IRQ_0, HcSr04::irq_handler,
                               PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);
    }

    static void irq_handler() {
        HcSr04& hc = instance();
        // TODO add data, reset irq etc
    }

    PIO pio;
    uint sm;
    uint offset;

    u32 trig;
    u32 echo;
    gpio_function trig_old;

    static HcSr04* hc_sr04;
};

HcSr04* HcSr04::hc_sr04 = nullptr;
