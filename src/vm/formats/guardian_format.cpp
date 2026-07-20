#include "guardian_format.hpp"
#include <iostream>
#include <stdexcept>
#include <cstring>

namespace guardian::vm::formats {

Bytecode parse_guardian_binary(const std::vector<uint8_t>& data) {
    if (data.size() < sizeof(GuardianHeader)) {
        throw std::runtime_error("Binary too small for Guardian header");
    }
    
    const GuardianHeader* header = reinterpret_cast<const GuardianHeader*>(data.data());
    
    // Validate magic
    if (header->magic != 0x44555247) { // 'GURD' in little-endian
        throw std::runtime_error("Invalid Guardian binary magic");
    }
    
    Bytecode bytecode;
    size_t offset = sizeof(GuardianHeader);
    
    for (uint32_t i = 0; i < header->instruction_count; i++) {
        if (offset + 1 > data.size()) break;
        
        uint8_t opcode_byte = data[offset++];
        OpCode opcode = static_cast<OpCode>(opcode_byte);
        
        if (offset + 1 > data.size()) break;
        uint8_t operand_count = data[offset++];
        
        std::vector<uint64_t> operands;
        for (uint8_t j = 0; j < operand_count; j++) {
            if (offset + 8 > data.size()) break;
            uint64_t operand;
            std::memcpy(&operand, data.data() + offset, 8);
            offset += 8;
            operands.push_back(operand);
        }
        
        bytecode.push_back(Instruction(opcode, operands));
    }
    
    return bytecode;
}

std::vector<uint8_t> generate_guardian_binary(const Bytecode& bytecode) {
    std::vector<uint8_t> result;
    result.reserve(sizeof(GuardianHeader) + bytecode.size() * 16);
    
    GuardianHeader header;
    header.magic = 0x44555247;
    header.version = 0x0001;
    header.flags = 0x0000;
    header.entry_point = 0;
    header.instruction_count = static_cast<uint32_t>(bytecode.size());
    header.reserved = 0;
    
    result.insert(result.end(), 
                  reinterpret_cast<const uint8_t*>(&header),
                  reinterpret_cast<const uint8_t*>(&header) + sizeof(header));
    
    for (const auto& instr : bytecode) {
        uint8_t opcode_byte = static_cast<uint8_t>(instr.opcode);
        result.push_back(opcode_byte);
        
        uint8_t operand_count = static_cast<uint8_t>(instr.operands.size());
        result.push_back(operand_count);
        
        for (uint64_t operand : instr.operands) {
            const uint8_t* operand_bytes = reinterpret_cast<const uint8_t*>(&operand);
            result.insert(result.end(), operand_bytes, operand_bytes + 8);
        }
    }
    
    return result;
}

void register_guardian_format() {
    BinaryFormat format;
    format.name = "Guardian Binary";
    format.magic = "GURD";
    format.version = 0x0001;
    format.flags = 0x0000;
    format.parser = parse_guardian_binary;
    format.generator = generate_guardian_binary;
    
    registry.register_format(format);
}

} // namespace guardian::vm::formats
