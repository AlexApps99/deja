#include "power.hpp"

#include <hardware/adc.h>
#include <pico/binary_info.h>

#include "../common.hpp"

#define CUR_MUL 17.0
#define VOLT_MUL 10.1

bi_decl(bi_2pins_with_names(PIN_PM_VOLTAGE, "PM Voltage", PIN_PM_CURRENT,
                            "PM Current"));

PowerMon::PowerMon(u32 voltage, u32 current)
    : voltage(voltage), current(current) {
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_set_round_robin((voltage - 26) | (current - 26) | 0b10000);
    adc_gpio_init(voltage);
    adc_gpio_init(current);
}

PowerMonData PowerMon::poll() {
    const f32 conv = 3.3 / (1 << 12);
    f32 a = adc_read() * conv;
    f32 b = adc_read() * conv;
    f32 t = adc_read() * conv;
    f32 temp = 27.0 - (t - 0.706) / 0.001721;
    // This isn't well documented, so I had to refer to ArduCopter's source
    f32 cur, volt;
    if (voltage > current) {
        cur = a * CUR_MUL;
        volt = b * VOLT_MUL;
    } else {
        cur = b * CUR_MUL;
        volt = a * VOLT_MUL;
    }
    return {.voltage = volt, .current = cur, .temperature = temp};
}