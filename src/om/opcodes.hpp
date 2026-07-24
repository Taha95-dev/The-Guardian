#pragma once

#include <cstdint>

namespace om {

enum class Opcode : uint8_t {
    HALT = 0x00,
    
    PUSH_INT = 0x10,
    PUSH_STRING = 0x13,
    
    STORE = 0x60,
    LOAD = 0x61,
    
    PRINT = 0x80,
    PRINTLN = 0x81,
};

} // namespace om
