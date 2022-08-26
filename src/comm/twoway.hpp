#pragma once
#include <vector>

#include "../common.hpp"

class TwoWay {
   public:
    // Appends all new data to buffer
    virtual void read(std::vector<u8> &buf) = 0;
    // Removes all written data from front of buffer
    virtual void write(std::vector<u8> &buf) = 0;
};