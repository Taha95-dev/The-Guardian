#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <fstream>

namespace om {

// Guardian Binary Format (.gbin)
struct GuardianHeader {
    uint32_t magic;      // 'GURD'
    uint16_t version;
    uint16_t flags;
    uint32_t entry_point;
    uint32_t instruction_count;
    uint8_t reserved[8];
};

class CodeGen {
public:
    CodeGen();
    ~CodeGen() = default;
    
    // Generate .gbin from Om AST (to be implemented)
    bool generate(const std::vector<std::string>& instructions, 
                  const std::string& output_path);
    
    // Helper to write header
    bool writeHeader(std::ofstream& out, uint32_t instruction_count);
    
    // Helper to write instructions
    bool writeInstructions(std::ofstream& out, 
                          const std::vector<std::string>& instructions);
    
private:
    uint32_t magic;
    uint16_t version;
    uint16_t flags;
};

} // namespace om
