#pragma once

#include <pb_decode.h>
#include <pb_encode.h>

#include "../common.hpp"

class TwoWay {
   public:
    pb_ostream_t pb_ostream = {.callback = out_cb,
                               .state = this,
                               .max_size = SIZE_MAX,
                               .bytes_written = 0};
    pb_istream_t pb_istream = {
        .callback = in_cb, .state = this, .bytes_left = SIZE_MAX};

   private:
    // Returns true on success
    virtual bool out(pb_ostream_t *stream, const u8 *buf, size_t count);
    // Returns true on success
    virtual bool in(pb_istream_t *stream, u8 *buf, size_t count);

    static bool out_cb(pb_ostream_t *stream, const u8 *buf, size_t count) {
        return static_cast<TwoWay *>(stream->state)->out(stream, buf, count);
    }
    static bool in_cb(pb_istream_t *stream, u8 *buf, size_t count) {
        return static_cast<TwoWay *>(stream->state)->in(stream, buf, count);
    }
};