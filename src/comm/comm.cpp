#include "comm.hpp"

#include <pico/sync.h>

template <bool isDrone>
auto Comm<isDrone>::receive() -> std::optional<In> {
    // check what the length of the incoming message is
    u32 num = 0;
    u32 len = in.size();
    if (len == 0) return std::nullopt;
    u8 x = 0;
    bool valid = false;
    for (; x < 5 && x < len; x++) {
        u8 v = in[x];
        num |= (u32)(v & 0b01111111) << (7 * x);
        if ((v & 0b10000000) == 0) {
            valid = true;
            break;
        }
    }

    if (!valid || len < ++x + num) return std::nullopt;

    In g;
    pb_istream_t pb_istream = {
        .callback =
            [](pb_istream_t *stream, uint8_t *buf, size_t count) {
                Comm *c = (Comm *)stream->state;
                if (count > c->in.size()) {
                    return false;
                } else {
                    std::memcpy(buf, c->in.data(), count);
                    c->in.erase(c->in.begin(), c->in.begin() + count);
                    return true;
                }
            },
        .state = this,
        .bytes_left = in.size()};
    // TODO this will break
    if (pb_decode_ex(&pb_istream, &IN_M, &g, PB_DECODE_DELIMITED)) {
        return g;
    } else {
        // TODO something went wrong, best throw out the message
        Lerror("PbD %s", PB_GET_ERROR(&pb_istream));
        return std::nullopt;
    }
}

template <bool isDrone>
bool Comm<isDrone>::send(const Out &g) {
    u32 state = save_and_disable_interrupts();
    pb_ostream_t pb_ostream = {
        .callback =
            [](pb_ostream_t *stream, const u8 *buf, size_t count) {
                Comm *c = (Comm *)stream->state;
                c->out.insert(c->out.end(), buf, buf + count);
                return true;
            },
        .state = this,
        .max_size = SIZE_MAX,
        .bytes_written = 0,
    };
    if (pb_encode_ex(&pb_ostream, &OUT_M, &g, PB_ENCODE_DELIMITED)) {
        restore_interrupts(state);
        return true;
    } else {
        // Lerror("PbE %s", PB_GET_ERROR(&pb_ostream));
        restore_interrupts(state);
        return false;
    }
}

template <>
void Comm<true>::log(const LogLevel level, const char *msg) {
    Drone d = {
        .time = 0,
        .which_msg = Drone_log_tag,
        .msg =
            {
                .log =
                    {
                        .level = (Drone_Log_Level)level,
                        .text = {0},
                    },
            },
    };
    strncpy(d.msg.log.text, msg, 256);
    d.msg.log.text[255] = '\0';
    send(d);
}

template <>
void Comm<false>::log(const LogLevel level, const char *msg) {}

template <bool isDrone>
void Comm<isDrone>::flush() {
    u32 state = save_and_disable_interrupts();
    com.write(out);
    com.read(in);
    restore_interrupts(state);
}

template class Comm<false>;
template class Comm<true>;