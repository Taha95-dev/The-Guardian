#include "codegen.hpp"
#include "opcodes.hpp"
#include <iostream>
#include <cstring>
#include <vector>

namespace om {

CodeGen::CodeGen() 
    : magic(0x44525547),  // 'GURD' in little-endian
      version(1),
      flags(0) {}

bool CodeGen::generate(const std::vector<std::string>& instructions, 
                       const std::string& output_path) {
    std::cout << "  📦 Generating .gbin: " << output_path << "\n";
    
    // Convert string instructions to bytecode
    std::vector<uint8_t> bytecode;
    
    for (const auto& inst : instructions) {
        if (inst == "HALT") {
            bytecode.push_back(static_cast<uint8_t>(Opcode::HALT));
        } else if (inst.rfind("PUSH_STRING ", 0) == 0) {
            bytecode.push_back(static_cast<uint8_t>(Opcode::PUSH_STRING));
            std::string str = inst.substr(12);
            uint32_t len = str.length();
            bytecode.insert(bytecode.end(), 
                          reinterpret_cast<uint8_t*>(&len),
                          reinterpret_cast<uint8_t*>(&len) + 4);
            bytecode.insert(bytecode.end(), str.begin(), str.end());
        } else if (inst.rfind("PUSH_INT ", 0) == 0) {
            bytecode.push_back(static_cast<uint8_t>(Opcode::PUSH_INT));
            int val = std::stoi(inst.substr(9));
            bytecode.insert(bytecode.end(),
                          reinterpret_cast<uint8_t*>(&val),
                          reinterpret_cast<uint8_t*>(&val) + 4);
        } else if (inst == "PRINT") {
            bytecode.push_back(static_cast<uint8_t>(Opcode::PRINT));
        } else if (inst == "PRINTLN") {
            bytecode.push_back(static_cast<uint8_t>(Opcode::PRINTLN));
        } else {
            std::cerr << "Warning: Unknown instruction: " << inst << "\n";
        }
    }
    
    // Write the bytecode
    std::ofstream out(output_path, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "Error: Could not write to " << output_path << "\n";
        return false;
    }
    
    // Write header
    if (!writeHeader(out, bytecode.size())) {
        return false;
    }
    
    // Write bytecode
    out.write(reinterpret_cast<const char*>(bytecode.data()), bytecode.size());
    if (!out.good()) {
        std::cerr << "Error: Failed to write bytecode\n";
        return false;
    }
    
    out.close();
    std::cout << "  ✅ Wrote " << bytecode.size() << " bytes of bytecode\n";
    return true;
}

bool CodeGen::writeHeader(std::ofstream& out, uint32_t bytecode_size) {
    GuardianHeader header;
    header.magic = magic;
    header.version = version;
    header.flags = flags;
    header.entry_point = sizeof(GuardianHeader);  // Start after header
    header.instruction_count = bytecode_size;
    memset(header.reserved, 0, 8);
    
    out.write(reinterpret_cast<const char*>(&header), sizeof(header));
    if (!out.good()) {
        std::cerr << "Error: Failed to write header\n";
        return false;
    }
    
    return true;
}

} // namespace om
