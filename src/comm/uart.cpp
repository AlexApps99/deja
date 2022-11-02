#include "uart.hpp"

#include <hardware/gpio.h>
#include <hardware/irq.h>
#include <hardware/uart.h>

Uart::Uart(uint baud) {
    uart_init(uart1, baud);
    gpio_set_function(PIN_UART1_RX, GPIO_FUNC_UART);
    gpio_set_function(PIN_UART1_TX, GPIO_FUNC_UART);
    if (!instance) instance = this;
    irq_set_exclusive_handler(UART1_IRQ, );
    uart_set_irq_enables(uart1, true, false);
}

void Uart::irq_cb(){

    instance->buf}

Uart::~Uart() {
    uart_deinit(uart1);
}

void Uart::read(std::vector<u8> &buf) {
    // TODO
}

void Uart::write(std::vector<u8> &buf) {
    size_t len = buf.size();
    if (len <= 0) return;
    uart_write_blocking(uart1, buf.data(), len);
    buf.clear();
}

Uart *uart = nullptr;