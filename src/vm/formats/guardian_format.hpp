#pragma once

#include "format_registry.hpp"
#include "../../core/quark.hpp"
#include <vector>
#include <cstdint>
#include <string>

namespace guardian::vm::formats {

// ============================================
// GUARDIAN BINARY FORMAT (.gbin)
// ============================================
// 
// Header (24 bytes):
//   Magic:    "GURD" (4 bytes)
//   Version:  0x0001 (2 bytes)
//   Flags:    0x0000 (2 bytes)
//   Entry:    0x00000000 (4 bytes)
//   Count:    0x00000000 (4 bytes) - number of instructions
//   Reserved: 0x0000000000000000 (8 bytes)
//
// Instructions follow the header.
// Each instruction:
//   Opcode:   1 byte
//   Operands: variable length
// ============================================

struct GuardianHeader {
    uint32_t magic;      // 'GURD'
    uint16_t version;
    uint16_t flags;
    uint32_t entry_point;
    uint32_t instruction_count;
    uint8_t reserved[8];
};

// Instruction with operands
struct Instruction {
    uint8_t opcode;
    std::vector<uint64_t> operands;
    
    Instruction() : opcode(0) {}
    Instruction(uint8_t op) : opcode(op) {}
    Instruction(uint8_t op, const std::vector<uint64_t>& ops) 
        : opcode(op), operands(ops) {}
};

// Parse .gbin file
std::vector<Instruction> parse_guardian_binary(const std::vector<uint8_t>& data);

// Generate .gbin file from instructions
std::vector<uint8_t> generate_guardian_binary(const std::vector<Instruction>& instructions);

// Register the format
void register_guardian_format();

// Serialize quark to bytes
std::vector<uint8_t> serialize_quark(const guardian::Quark& quark);

// Deserialize quark from bytes
guardian::Quark deserialize_quark(const std::vector<uint8_t>& data, size_t& pos);

} // namespace guardian::vm::formats
