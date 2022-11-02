#include "cdc.hpp"

#include <pico/stdio.h>

#define STDIO_HANDLE_STDIN 0
#define STDIO_HANDLE_STDOUT 1

// awful hacks
extern "C" {
int _read(int handle, char *buffer, int length);
int _write(int handle, char *buffer, int length);
}

CDC::CDC() { stdio_usb_init(); }

CDC::~CDC() {}

void CDC::read(std::vector<u8> &buf) {
    size_t n = 0;
    char block[CDC_BLOCK_SIZE] = {0};
    do {
        // this wouldn't usually work but I did some hacking
        n = _read(STDIO_HANDLE_STDIN, block, CDC_BLOCK_SIZE);
        if (n > 0)
            buf.insert(buf.end(), std::begin(block), std::begin(block) + n);
    } while (n == CDC_BLOCK_SIZE);
}

void CDC::write(std::vector<u8> &buf) {
    size_t len = buf.size();
    if (len <= 0) return;
    size_t n = _write(STDIO_HANDLE_STDOUT, (char *)buf.data(), len);
    if (n > 0) buf.erase(buf.begin(), buf.begin() + n);
}