#pragma once

#include <pb_decode.h>
#include <pb_encode.h>

#include <optional>
#include <type_traits>

#include "common.hpp"
#include "io/io.hpp"
#include "msg.pb.h"

template <bool isDrone>
class Comm final : public Log {
    using Out = std::conditional<isDrone, Drone, Ground>;
    using In = std::conditional<isDrone, Ground, Drone>;
    const pb_msgdesc_t OUT_M = isDrone ? Drone_msg : Ground_msg;
    const pb_msgdesc_t IN_M = isDrone ? Ground_msg : Drone_msg;

   public:
    void log(const LogLevel level, const char *msg,
             const char *const pos) override {
        printf("[%-5s %s] %s\n", this->level(level), pos, msg);
    }
    Comm() {}

    // MUST CALL pb_release WHEN DROPPED
    std::optional<In> plumb() {
        Ground g;
        if (pb_decode_ex(&com.pb_istream, &IN_M, &g,
                         PB_DECODE_DELIMITED | PB_DECODE_NULLTERMINATED)) {
            return g;
        } else {
            Lerror("PbD %s", PB_GET_ERROR(&com.pb_istream));
            return {};
        }
    }

    bool unplumb(const Out &g) {
        if (pb_encode_ex(&com.pb_ostream, &OUT_M, &g,
                         PB_ENCODE_DELIMITED | PB_ENCODE_NULLTERMINATED)) {
            return true;
        } else {
            Lerror("PbE %s", PB_GET_ERROR(&com.pb_ostream));
            return false;
        }
    }

   private:
    TwoWay com;
};