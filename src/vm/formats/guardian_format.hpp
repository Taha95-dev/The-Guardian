#pragma once
#include "format_registry.hpp"
#include <cstdint>
#include <vector>
#include <cstring>

namespace guardian::vm::formats {

// ============================================
// GUARDIAN BINARY FORMAT (.gbin)
// ============================================
// 
// Header:
//   Magic:    "GURD" (4 bytes)
//   Version:  0x0001 (2 bytes)
//   Flags:    0x0000 (2 bytes)
//   Entry:    0x00000000 (4 bytes)
//   Count:    0x00000000 (4 bytes)
//   Reserved: 0x0000000000000000 (8 bytes)
//   (Total: 24 bytes)
//
// Instructions follow the header.
// ============================================

struct GuardianHeader {
    uint32_t magic;      // 'GURD'
    uint16_t version;
    uint16_t flags;
    uint32_t entry_point;
    uint32_t instruction_count;
    uint64_t reserved;
};

// Register the Guardian format
void register_guardian_format();

// Parser for Guardian format
Bytecode parse_guardian_binary(const std::vector<uint8_t>& data);

// Generator for Guardian format
std::vector<uint8_t> generate_guardian_binary(const Bytecode& bytecode);

} // namespace guardian::vm::formats
