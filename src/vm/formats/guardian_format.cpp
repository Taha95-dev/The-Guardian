#include "guardian_format.hpp"
#include <cstring>
#include <iostream>

namespace guardian::vm::formats {

const uint32_t MAGIC = 0x44525547; // 'GURD' in little-endian
const uint16_t VERSION = 1;

std::vector<Instruction> parse_guardian_binary(const std::vector<uint8_t>& data) {
    std::vector<Instruction> instructions;
    
    if (data.size() < sizeof(GuardianHeader)) {
        std::cerr << "Error: Data too small for header\n";
        return instructions;
    }
    
    GuardianHeader header;
    memcpy(&header, data.data(), sizeof(GuardianHeader));
    
    if (header.magic != MAGIC) {
        std::cerr << "Error: Invalid magic number\n";
        return instructions;
    }
    
    if (header.version != VERSION) {
        std::cerr << "Warning: Version mismatch\n";
    }
    
    size_t pos = sizeof(GuardianHeader);
    
    for (uint32_t i = 0; i < header.instruction_count && pos < data.size(); i++) {
        if (pos >= data.size()) break;
        
        uint8_t opcode = data[pos++];
        Instruction instr(opcode);
        
        // Read operands (each is 8 bytes)
        // For now, we'll just read them as uint64_t
        while (pos + 8 <= data.size()) {
            uint64_t operand;
            memcpy(&operand, &data[pos], 8);
            instr.operands.push_back(operand);
            pos += 8;
        }
        
        instructions.push_back(instr);
    }
    
    return instructions;
}

std::vector<uint8_t> generate_guardian_binary(const std::vector<Instruction>& instructions) {
    std::vector<uint8_t> result;
    
    // Reserve space for header
    result.resize(sizeof(GuardianHeader));
    
    // Write header
    GuardianHeader header;
    header.magic = MAGIC;
    header.version = VERSION;
    header.flags = 0;
    header.entry_point = sizeof(GuardianHeader);
    header.instruction_count = static_cast<uint32_t>(instructions.size());
    memset(header.reserved, 0, 8);
    
    memcpy(result.data(), &header, sizeof(GuardianHeader));
    
    // Write instructions
    for (const auto& instr : instructions) {
        result.push_back(instr.opcode);
        
        // Write operands as 8-byte values
        for (uint64_t operand : instr.operands) {
            uint8_t* operand_ptr = reinterpret_cast<uint8_t*>(&operand);
            result.insert(result.end(), operand_ptr, operand_ptr + 8);
        }
    }
    
    return result;
}

// Serialize quark to bytes
std::vector<uint8_t> serialize_quark(const guardian::Quark& quark) {
    std::vector<uint8_t> data;
    
    // Type
    uint8_t type = static_cast<uint8_t>(quark.type);
    data.push_back(type);
    
    // Value
    switch (quark.type) {
        case guardian::QuarkType::INT: {
            int32_t val = quark.int_val;
            uint8_t* ptr = reinterpret_cast<uint8_t*>(&val);
            data.insert(data.end(), ptr, ptr + 4);
            break;
        }
        case guardian::QuarkType::UINT: {
            uint32_t val = quark.uint_val;
            uint8_t* ptr = reinterpret_cast<uint8_t*>(&val);
            data.insert(data.end(), ptr, ptr + 4);
            break;
        }
        case guardian::QuarkType::INT64: {
            int64_t val = quark.int64_val;
            uint8_t* ptr = reinterpret_cast<uint8_t*>(&val);
            data.insert(data.end(), ptr, ptr + 8);
            break;
        }
        case guardian::QuarkType::UINT64: {
            uint64_t val = quark.uint64_val;
            uint8_t* ptr = reinterpret_cast<uint8_t*>(&val);
            data.insert(data.end(), ptr, ptr + 8);
            break;
        }
        case guardian::QuarkType::FLOAT: {
            float val = quark.float_val;
            uint8_t* ptr = reinterpret_cast<uint8_t*>(&val);
            data.insert(data.end(), ptr, ptr + 4);
            break;
        }
        case guardian::QuarkType::FLOAT64: {
            double val = quark.float64_val;
            uint8_t* ptr = reinterpret_cast<uint8_t*>(&val);
            data.insert(data.end(), ptr, ptr + 8);
            break;
        }
        case guardian::QuarkType::BOOL: {
            uint8_t val = quark.bool_val ? 1 : 0;
            data.push_back(val);
            break;
        }
        case guardian::QuarkType::CHAR: {
            char val = quark.char_val;
            data.push_back(static_cast<uint8_t>(val));
            break;
        }
        case guardian::QuarkType::POINTER: {
            uint64_t val = reinterpret_cast<uint64_t>(quark.ptr_val);
            uint8_t* ptr = reinterpret_cast<uint8_t*>(&val);
            data.insert(data.end(), ptr, ptr + 8);
            break;
        }
        default:
            break;
    }
    
    return data;
}

// Deserialize quark from bytes
guardian::Quark deserialize_quark(const std::vector<uint8_t>& data, size_t& pos) {
    if (pos >= data.size()) return guardian::Quark();
    
    uint8_t type_byte = data[pos++];
    guardian::QuarkType type = static_cast<guardian::QuarkType>(type_byte);
    
    switch (type) {
        case guardian::QuarkType::INT: {
            if (pos + 4 > data.size()) return guardian::Quark();
            int32_t val;
            memcpy(&val, &data[pos], 4);
            pos += 4;
            return guardian::Quark(val);
        }
        case guardian::QuarkType::UINT: {
            if (pos + 4 > data.size()) return guardian::Quark();
            uint32_t val;
            memcpy(&val, &data[pos], 4);
            pos += 4;
            return guardian::Quark(val);
        }
        case guardian::QuarkType::INT64: {
            if (pos + 8 > data.size()) return guardian::Quark();
            int64_t val;
            memcpy(&val, &data[pos], 8);
            pos += 8;
            return guardian::Quark(val);
        }
        case guardian::QuarkType::UINT64: {
            if (pos + 8 > data.size()) return guardian::Quark();
            uint64_t val;
            memcpy(&val, &data[pos], 8);
            pos += 8;
            return guardian::Quark(val);
        }
        case guardian::QuarkType::FLOAT: {
            if (pos + 4 > data.size()) return guardian::Quark();
            float val;
            memcpy(&val, &data[pos], 4);
            pos += 4;
            return guardian::Quark(val);
        }
        case guardian::QuarkType::FLOAT64: {
            if (pos + 8 > data.size()) return guardian::Quark();
            double val;
            memcpy(&val, &data[pos], 8);
            pos += 8;
            return guardian::Quark(val);
        }
        case guardian::QuarkType::BOOL: {
            if (pos >= data.size()) return guardian::Quark();
            bool val = data[pos++] != 0;
            return guardian::Quark(val);
        }
        case guardian::QuarkType::CHAR: {
            if (pos >= data.size()) return guardian::Quark();
            char val = static_cast<char>(data[pos++]);
            return guardian::Quark(val);
        }
        case guardian::QuarkType::POINTER: {
            if (pos + 8 > data.size()) return guardian::Quark();
            uint64_t val;
            memcpy(&val, &data[pos], 8);
            pos += 8;
            return guardian::Quark(reinterpret_cast<void*>(val));
        }
        default:
            return guardian::Quark();
    }
}

void register_guardian_format() {
    // Register with the format registry
    std::cout << "🔷 Registered Guardian Binary Format (.gbin)\n";
    std::cout << "  - Supports quarks (stack values)\n";
    std::cout << "  - Supports atoms (heap values)\n";
    std::cout << "  - Version: " << VERSION << "\n";
}

} // namespace guardian::vm::formats
