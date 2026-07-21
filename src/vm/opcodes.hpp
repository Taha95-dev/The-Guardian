#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <memory>

namespace guardian::vm {

enum class Opcode : uint8_t {
    HALT = 0x00,
    NOP = 0x01,
    
    // Stack operations
    PUSH_INT = 0x10,
    PUSH_FLOAT = 0x11,
    PUSH_BOOL = 0x12,
    PUSH_STRING = 0x13,
    PUSH_NULL = 0x14,
    POP = 0x15,
    DUP = 0x16,
    
    // Arithmetic
    ADD = 0x20,
    SUB = 0x21,
    MUL = 0x22,
    DIV = 0x23,
    MOD = 0x24,
    
    // Comparison
    EQ = 0x30,
    NEQ = 0x31,
    LT = 0x32,
    GT = 0x33,
    LTE = 0x34,
    GTE = 0x35,
    
    // Logic
    AND = 0x40,
    OR = 0x41,
    NOT = 0x42,
    
    // Control flow
    JMP = 0x50,
    JMP_IF = 0x51,
    JMP_IF_NOT = 0x52,
    CALL = 0x53,
    RET = 0x54,
    
    // Variables (stored in main molecule)
    STORE = 0x60,
    LOAD = 0x61,
    
    // Functions
    DEFINE_FN = 0x70,
    CALL_FN = 0x71,
    RETURN = 0x72,
    
    // Print
    PRINT = 0x80,
    PRINTLN = 0x81,
};

} // namespace guardian::vm
