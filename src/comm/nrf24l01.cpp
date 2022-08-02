// TODO move NRF stuff here
// add IRQ, CE, SPI pins

#include "nrf24l01.hpp"

#include <RF24.h>
#include <hardware/gpio.h>
#include <hardware/irq.h>
#include <pico/binary_info.h>
#include <pico/sync.h>

#include <vector>

#include "twoway.hpp"

bi_decl(bi_2pins_with_func(PICO_DEFAULT_SPI_RX_PIN, PICO_DEFAULT_SPI_TX_PIN,
                           GPIO_FUNC_SPI));
bi_decl(bi_2pins_with_func(PICO_DEFAULT_SPI_SCK_PIN, PICO_DEFAULT_SPI_CSN_PIN,
                           GPIO_FUNC_SPI));
bi_decl(bi_2pins_with_names(PIN_NRF24_CE, "NRF24 CE", PIN_NRF24_IRQ,
                            "NRF24 IRQ"));

// NRF24L01 is half-duplex, so I need a protocol for switching.
// ACK payload doesn't cut it because it's 32 bytes per recieved packet,
// which means one side will be delayed and potentially bottlenecked.
//
// The way it works is each packet being sent has a byte prefixed,
// and each ack payload has 1 byte, each control the 0th bit.
// For the sender, the bit is 1 if it was the last thing sent.
// For the receiever, the bit is 1 if it has things to send.
// if both of these bits are 1, switching occurs.
//
// TODO: how should sending be started? on a timer? on write?
// TODO error checking
class NRF : public TwoWay {
   public:
    NRF(RF24 rf, u32 irq, bool is_drone) : rf(rf), irq(irq), send(!is_drone) {
        rf.begin();

        rf.setAddressWidth(3);
        rf.setChannel(69);
        rf.enableDynamicPayloads();
        rf.enableAckPayload();
        // TODO set up IRQ so nothing is lost
        rf.maskIRQ(true, true, true);

        rf.openWritingPipe(is_drone ? DRONE_TO_GROUND : GROUND_TO_DRONE);
        rf.openReadingPipe(1, is_drone ? GROUND_TO_DRONE : DRONE_TO_GROUND);
        gpio_set_irq_enabled(irq, GPIO_IRQ_EDGE_FALL, true);
        // TODO
        // gpio_add_raw_irq_handler(irq, nullptr);
        irq_set_enabled(IO_IRQ_BANK0, true);

        if (!send) {
            rf.startListening();
        }
    }
    ~NRF() {
        rf.toggleAllPipes(false);
        rf.powerDown();
    }

    std::vector<u8> unsent;
    std::vector<u8> unread;

   private:
    // Interrupts are disabled so nothing is messed up
    bool out(pb_ostream_t *stream, const u8 *buf, size_t count) override {
        u32 iq = save_and_disable_interrupts();
        unsent.insert(unsent.end(), buf, buf + count);
        restore_interrupts(iq);
        return true;
    }
    // Interrupts are disabled so nothing is messed up
    bool in(pb_istream_t *stream, u8 *buf, size_t count) override {
        u32 iq = save_and_disable_interrupts();
        if (unread.size() >= count) {
            std::copy(unread.begin(), unread.begin() + count, buf);
            unread.erase(unread.begin(), unread.begin() + count);
            restore_interrupts(iq);
            return true;
        } else {
            restore_interrupts(iq);
            return false;
        }
    }

    RF24 rf;
    u32 irq;
    bool send;
    const u8 DRONE_TO_GROUND[3] = {'D', '>', 'G'};
    const u8 GROUND_TO_DRONE[3] = {'G', '>', 'D'};
};