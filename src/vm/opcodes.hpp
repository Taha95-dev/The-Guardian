#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <variant>  // ← THIS WAS MISSING!
#include <iostream>

namespace guardian::vm {

// ============================================
// VALUE TYPES
// ============================================
using Value = std::variant<int64_t, double, bool, char, std::string, void*>;

// ============================================
// OPCODES
// ============================================
enum class OpCode : uint16_t {
    NOP = 0x0000,
    HALT = 0x0001,
    DEBUG = 0x0002,
    PUSH = 0x0010,
    POP = 0x0011,
    DUP = 0x0012,
    SWAP = 0x0013,
    DROP = 0x0014,
    ADD = 0x0020,
    SUB = 0x0021,
    MUL = 0x0022,
    DIV = 0x0023,
    MOD = 0x0024,
    NEG = 0x0025,
    AND = 0x0030,
    OR = 0x0031,
    XOR = 0x0032,
    NOT = 0x0033,
    SHL = 0x0034,
    SHR = 0x0035,
    CMP_EQ = 0x0040,
    CMP_NE = 0x0041,
    CMP_LT = 0x0042,
    CMP_GT = 0x0043,
    CMP_LE = 0x0044,
    CMP_GE = 0x0045,
    LOGIC_AND = 0x0050,
    LOGIC_OR = 0x0051,
    LOGIC_NOT = 0x0052,
    JMP = 0x0060,
    JZ = 0x0061,
    JNZ = 0x0062,
    CALL = 0x0063,
    RET = 0x0064,
    LOAD = 0x0070,
    STORE = 0x0071,
    LOAD_CONST = 0x0072,
    DECLARE = 0x0080,
    ASSIGN = 0x0081,
    GET_VAR = 0x0082,
    SET_VAR = 0x0083,
    DEFINE = 0x0090,
    CALL_FN = 0x0091,
    RETURN = 0x0092,
    PRINT = 0x00A0,
    INPUT = 0x00A1,
    ARRAY_NEW = 0x00B0,
    ARRAY_GET = 0x00B1,
    ARRAY_SET = 0x00B2,
    ARRAY_SIZE = 0x00B3,
    STR_CONCAT = 0x00C0,
    STR_LEN = 0x00C1,
    SANDBOX = 0x00D0,
    PERMISSION = 0x00D1,
    THREAD_NEW = 0x00E0,
    THREAD_JOIN = 0x00E1,
    EXT_0 = 0xFF00,
    EXT_1 = 0xFF01,
    EXT_2 = 0xFF02,
    EXT_3 = 0xFF03,
    EXT_4 = 0xFF04,
    EXT_5 = 0xFF05,
    EXT_6 = 0xFF06,
    EXT_7 = 0xFF07,
    EXT_8 = 0xFF08,
    EXT_9 = 0xFF09,
};

// ============================================
// INSTRUCTION
// ============================================
struct Instruction {
    OpCode opcode;
    std::vector<uint64_t> operands;
    
    Instruction() : opcode(OpCode::NOP) {}
    Instruction(OpCode op) : opcode(op) {}
    Instruction(OpCode op, const std::vector<uint64_t>& ops) : opcode(op), operands(ops) {}
};

// ============================================
// BYTECODE
// ============================================
using Bytecode = std::vector<Instruction>;

// ============================================
// UTILITY
// ============================================
std::string opcode_to_string(OpCode opcode);
OpCode string_to_opcode(const std::string& str);

} // namespace guardian::vm
