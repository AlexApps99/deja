#include "cdc.hpp"

//#include <pico/stdio.h>

CDC::CDC() {}

CDC::~CDC() {}

bool CDC::out(pb_ostream_t *stream, const u8 *buf, size_t count) {
    if (buf)
        return fwrite(buf, 1, count, stdout) == count;
    else
        return true;
}

bool CDC::in(pb_istream_t *stream, u8 *buf, size_t count) {
    if (buf) {
        return fread(buf, 1, count, stdin) == count;

    } else {
        while (count-- && fgetc(stdin) != EOF)
            ;
        return count == 0;
    }
}