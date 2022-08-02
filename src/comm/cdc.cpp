#include "cdc.hpp"

CDC::CDC() {}

CDC::~CDC() {}

bool CDC::out(pb_ostream_t *stream, const u8 *buf, size_t count) {
    if (buf)
        return fwrite(buf, count, 1, stdout) == 1;
    else
        return true;
}

bool CDC::in(pb_istream_t *stream, u8 *buf, size_t count) {
    if (buf) {
        return fread(buf, count, 1, stdin) == 1;

    } else {
        while (count-- && fgetc(stdin) != EOF)
            ;
        return count == 0;
    }
}