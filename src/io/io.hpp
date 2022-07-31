#pragma once

#include <pb_decode.h>
#include <pb_encode.h>

#include <vector>

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

class Motor {
   public:
    explicit inline operator f32() const { return speed; }
    inline f32 set_speed(const f32 speed, const f32 max_speed) {
        this->speed = safe_speed(speed, max_speed);
        apply_speed();
        return this->speed;
    }

   protected:
    f32 speed = 0.0;

   private:
    inline f32 safe_speed(const f32 speed, const f32 max_speed) {
        f32 safe_speed = speed > max_speed ? max_speed : speed;
        safe_speed = safe_speed >= 0.0 ? safe_speed : 0.0;
        safe_speed = safe_speed <= 1.0 ? safe_speed : 1.0;
        return safe_speed;
    }
    // Speed [0.0, 1.0]
    virtual void apply_speed(void) = 0;
};