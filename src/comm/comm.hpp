#pragma once

#include <pb_decode.h>
#include <pb_encode.h>

#include <optional>
#include <type_traits>

#include "../common.hpp"
#include "msg.pb.h"
#include "twoway.hpp"

// Returns u32_max on error
// throws away excess bits (but doesn't go above 5 bytes)
static inline u32 decode_u32(const char *data, const u32 len) {
    if (len == 0) {
    } else if (len == 1) {
        if ((data[0] & 0b10000000) == 0) {
            return data[0];
        }
    } else {
        u32 num = 0;
        for (u8 x = 0; x < len; x++) {
            if (x > 4) break;
            num |= (u32)(data[x] & 0b01111111) << (7 * x);
            if ((data[x] & 0b10000000) == 0) return num;
        }
    }
    return ~0;
}

// What do I need to do?
// Wrap the stream so that it:
// - reads until it gets the length,
// - reads until it gets the amount of data,
// - parses,
// - repeats

template <bool isDrone>
class Comm final : public Log {
    using Out = std::conditional<isDrone, Drone, Ground>;
    using In = std::conditional<isDrone, Ground, Drone>;
    const pb_msgdesc_t OUT_M = isDrone ? Drone_msg : Ground_msg;
    const pb_msgdesc_t IN_M = isDrone ? Ground_msg : Drone_msg;

   public:
    void log(const LogLevel level, const char *msg) override {
        if (isDrone) {
            Drone_Log log = {.level = level, .text = msg};
            unplumb(log);
        }
    }
    Comm(TwoWay com) : com(com) {}

    // MUST CALL pb_release WHEN DROPPED
    std::optional<In> plumb() {
        Ground g;
        // TODO this will break
        if (pb_decode_ex(&com.pb_istream, &IN_M, &g, PB_DECODE_DELIMITED)) {
            return g;
        } else {
            Lerror("PbD %s", PB_GET_ERROR(&com.pb_istream));
            return {};
        }
    }

    bool unplumb(const Out &g) {
        if (pb_encode_ex(&com.pb_ostream, &OUT_M, &g, PB_ENCODE_DELIMITED)) {
            return true;
        } else {
            Lerror("PbE %s", PB_GET_ERROR(&com.pb_ostream));
            return false;
        }
    }

   private:
    TwoWay com;
};