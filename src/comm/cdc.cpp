#include "cdc.hpp"

CDC::CDC() { stdio_usb_init(); }

CDC::~CDC() {}

void CDC::read(std::vector<u8> &buf) {
    size_t n = 0;
    u8 block[CDC_BLOCK_SIZE] = {0};
    do {
        n = fread(block, 1, CDC_BLOCK_SIZE, stdin);
        buf.insert(buf.end(), std::begin(block), std::begin(block) + n);
    } while (n == CDC_BLOCK_SIZE);
}

void CDC::write(std::vector<u8> &buf) {
    size_t n = 0;
    size_t len = buf.size();
    do {
        if (len <= 0) return;
        n = fwrite(buf.data(), 1, std::min(CDC_BLOCK_SIZE, len), stdout);
        if (n != 0) {
            buf.erase(buf.begin(), buf.begin() + n);
            len -= n;
        }
    } while (n == CDC_BLOCK_SIZE);
}