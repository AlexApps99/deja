#pragma once
#include <hardware/gpio.h>
#include <hardware/i2c.h>
#include <hardware/sync.h>

#include "../common.hpp"

class I2C {
   public:
    I2C(i2c_inst_t *const i2c, const u32 sda, const u32 scl, const u32 baud);
    ~I2C();

    inline int write_blocking_until(uint8_t addr, const uint8_t *src,
                                    size_t len, bool nostop,
                                    absolute_time_t until) {
        u32 state = save_and_disable_interrupts();
        int v = i2c_write_blocking_until(i2c, addr, src, len, nostop, until);
        restore_interrupts(state);
        return v;
    }
    inline int read_blocking_until(uint8_t addr, uint8_t *dst, size_t len,
                                   bool nostop, absolute_time_t until) {
        u32 state = save_and_disable_interrupts();
        int v = i2c_read_blocking_until(i2c, addr, dst, len, nostop, until);
        restore_interrupts(state);
        return v;
    }
    inline int write_timeout_us(uint8_t addr, const uint8_t *src, size_t len,
                                bool nostop, uint timeout_us) {
        u32 state = save_and_disable_interrupts();
        int v = i2c_write_timeout_us(i2c, addr, src, len, nostop, timeout_us);
        restore_interrupts(state);
        return v;
    }
    inline int read_timeout_us(uint8_t addr, uint8_t *dst, size_t len,
                               bool nostop, uint timeout_us) {
        u32 state = save_and_disable_interrupts();
        int v = i2c_read_timeout_us(i2c, addr, dst, len, nostop, timeout_us);
        restore_interrupts(state);
        return v;
    }
    inline int write_blocking(uint8_t addr, const uint8_t *src, size_t len,
                              bool nostop) {
        u32 state = save_and_disable_interrupts();
        int v = i2c_write_blocking(i2c, addr, src, len, nostop);
        restore_interrupts(state);
        return v;
    }
    inline int read_blocking(uint8_t addr, uint8_t *dst, size_t len,
                             bool nostop) {
        u32 state = save_and_disable_interrupts();
        int v = i2c_read_blocking(i2c, addr, dst, len, nostop);
        restore_interrupts(state);
        return v;
    }
    inline size_t get_write_available() {
        u32 state = save_and_disable_interrupts();
        size_t v = i2c_get_write_available(i2c);
        restore_interrupts(state);
        return v;
    }
    inline size_t get_read_available() {
        u32 state = save_and_disable_interrupts();
        size_t v = i2c_get_read_available(i2c);
        restore_interrupts(state);
        return v;
    }
    inline void write_raw_blocking(const uint8_t *src, size_t len) {
        u32 state = save_and_disable_interrupts();
        i2c_write_raw_blocking(i2c, src, len);
        restore_interrupts(state);
    }
    inline void read_raw_blocking(uint8_t *dst, size_t len) {
        u32 state = save_and_disable_interrupts();
        i2c_read_raw_blocking(i2c, dst, len);
        restore_interrupts(state);
    }
    inline uint get_dreq(bool is_tx) {
        u32 state = save_and_disable_interrupts();
        uint v = i2c_get_dreq(i2c, is_tx);
        restore_interrupts(state);
        return v;
    }

    i2c_inst_t *const i2c;
    const u32 sda;
    const u32 scl;
    const u32 baud;
};