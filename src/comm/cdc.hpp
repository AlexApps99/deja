#pragma once
#include <pb_decode.h>
#include <pb_encode.h>

#include "twoway.hpp"

class CDC : public TwoWay {
   public:
    CDC();
    ~CDC();

   private:
    bool out(pb_ostream_t *stream, const u8 *buf, size_t count) override;
    bool in(pb_istream_t *stream, u8 *buf, size_t count) override;
};