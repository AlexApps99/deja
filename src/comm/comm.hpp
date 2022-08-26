#pragma once

#include <pb_decode.h>
#include <pb_encode.h>

#include <cstring>
#include <optional>
#include <type_traits>
#include <vector>

#include "../common.hpp"
#include "msg.pb.h"
#include "twoway.hpp"

// What do I need to do?
// Wrap the stream so that it:
// - reads until it gets the length,
// - reads until it gets the amount of data,
// - parses,
// - repeats

template <bool isDrone>
class Comm final : public Log {
    using Out = typename std::conditional<isDrone, Drone, Ground>::type;
    using In = typename std::conditional<isDrone, Ground, Drone>::type;
    const pb_msgdesc_t OUT_M = isDrone ? Drone_msg : Ground_msg;
    const pb_msgdesc_t IN_M = isDrone ? Ground_msg : Drone_msg;

   public:
    inline Comm(TwoWay &com) : com(com) {}

    void log(const LogLevel level, const char *msg) override;

    // MUST CALL pb_release WHEN DROPPED
    std::optional<In> receive();

    bool send(const Out &g);

    void flush();

   private:
    TwoWay &com;
    std::vector<u8> out{};
    std::vector<u8> in{};
};