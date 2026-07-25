#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

namespace guardian::vm {

// ============================================
// OPCODES
// ============================================
enum class Opcode : uint8_t {
    // ============================================
    // BASIC OPCODES (0x00-0x0F)
    // ============================================
    NOP = 0x00,
    HALT = 0xFF,  // MOVED to 0xFF to avoid data conflicts
    
    // ============================================
    // STACK OPERATIONS (0x10-0x1F)
    // ============================================
    PUSH_INT = 0x10,
    PUSH_FLOAT = 0x11,
    PUSH_BOOL = 0x12,
    PUSH_STRING = 0x13,
    PUSH_NULL = 0x14,
    POP = 0x15,
    DUP = 0x16,
    SWAP = 0x17,
    ROT = 0x18,
    
    // ============================================
    // ARITHMETIC (0x20-0x2F)
    // ============================================
    ADD = 0x20,
    SUB = 0x21,
    MUL = 0x22,
    DIV = 0x23,
    MOD = 0x24,
    NEG = 0x25,
    INC = 0x26,
    DEC = 0x27,
    
    // ============================================
    // COMPARISON (0x30-0x3F)
    // ============================================
    EQ = 0x30,
    NEQ = 0x31,
    LT = 0x32,
    GT = 0x33,
    LTE = 0x34,
    GTE = 0x35,
    
    // ============================================
    // LOGIC (0x40-0x4F)
    // ============================================
    AND = 0x40,
    OR = 0x41,
    NOT = 0x42,
    
    // ============================================
    // CONTROL FLOW (0x50-0x5F)
    // ============================================
    JMP = 0x50,
    JMP_IF = 0x51,
    JMP_IF_NOT = 0x52,
    CALL = 0x53,
    RET = 0x54,
    
    // ============================================
    // VARIABLES (0x60-0x6F)
    // ============================================
    STORE = 0x60,
    LOAD = 0x61,
    STORE_GLOBAL = 0x62,
    LOAD_GLOBAL = 0x63,
    
    // ============================================
    // FUNCTIONS (0x70-0x7F)
    // ============================================
    DEFINE_FN = 0x70,
    CALL_FN = 0x71,
    RETURN = 0x72,
    
    // ============================================
    // PRINT (0x80-0x8F)
    // ============================================
    PRINT = 0x80,
    PRINTLN = 0x81,
    SPACE = 0x82,     // Print a space
    NEWLINE = 0x83,   // Print a newline
    
    // ============================================
    // ARRAYS (0x90-0x9F)
    // ============================================
    MAKE_ARRAY = 0x90,
    ARRAY_GET = 0x91,
    ARRAY_SET = 0x92,
    ARRAY_PUSH = 0x93,
    ARRAY_POP = 0x94,
    ARRAY_LEN = 0x95,
    
    // ============================================
    // DICTIONARIES (0xA0-0xAF)
    // ============================================
    MAKE_DICT = 0xA0,
    DICT_GET = 0xA1,
    DICT_SET = 0xA2,
    DICT_KEYS = 0xA3,
    DICT_VALUES = 0xA4,
    
    // ============================================
    // MOLECULES (0xB0-0xBF)
    // ============================================
    MAKE_MOLECULE = 0xB0,
    STORE_ATOM = 0xB1,
    GET_ATOM = 0xB2,
    REMOVE_ATOM = 0xB3,  // ADDED
};

// ============================================
// SYMBOL TYPES
// ============================================
enum class SymbolType : uint8_t {
    VARIABLE = 0x01,
    FUNCTION = 0x02,
    LABEL = 0x03,
    IMPORT = 0x04,
    EXPORT = 0x05,
};

// ============================================
// SYMBOL
// ============================================
struct Symbol {
    std::string name;
    SymbolType type;
    uint32_t index;
    uint32_t address;
};

// ============================================
// SECTION TYPES
// ============================================
enum class SectionType : uint8_t {
    CODE = 0x01,
    DATA = 0x02,
    SYMBOLS = 0x03,
    STRINGS = 0x04,
    DEBUG = 0x05,
};

// ============================================
// SECTION HEADER
// ============================================
struct SectionHeader {
    SectionType type;
    uint32_t size;
    uint32_t offset;
};

// ============================================
// BYTECODE HEADER
// ============================================
struct BytecodeHeader {
    uint32_t magic;          // "GDVN" = Guardian VM
    uint16_t version_major;
    uint16_t version_minor;
    uint32_t entry_point;
    uint32_t section_count;
    uint32_t total_size;
};

} // namespace guardian::vm
