#pragma once

#include <pb_decode.h>
#include <pb_encode.h>

#include "common.hpp"
#include "log.hpp"
#include "msg.pb.h"

class Comm final : public Log {
   public:
    void log(const LogLevel level, const char *msg,
             const char *const pos) override {
        printf("[%-5s %s] %s\n", this->level(level), pos, msg);
    }
    Comm() {}

   private:
    static bool pb_ost_cb(pb_ostream_t *stream, const u8 *buf, size_t count) {
        return false;
    }
    pb_ostream_t pb_ost = {.callback = pb_ost_cb,
                           .state = this,
                           .max_size = 0,
                           .bytes_written = 0};
    static bool pb_ist_cb(pb_istream_t *stream, u8 *buf, size_t count) {
        return false;
    }
    pb_istream_t pb_ist = {
        .callback = pb_ist_cb, .state = this, .bytes_left = 0};
};