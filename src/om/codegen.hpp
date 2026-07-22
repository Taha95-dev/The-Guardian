#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <fstream>
#include <memory>
#include <unordered_map>

namespace om {

struct ASTNode;
struct ProgramNode;

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
    
    bool generate(const std::unique_ptr<ProgramNode>& ast, const std::string& output_path);
    
private:
    uint32_t magic;
    uint16_t version;
    uint16_t flags;
    
    void generateStatement(const std::unique_ptr<ASTNode>& node, 
                          std::vector<uint8_t>& bytecode,
                          std::unordered_map<std::string, bool>& variables);
    
    void generateExpression(const std::unique_ptr<ASTNode>& node, 
                           std::vector<uint8_t>& bytecode,
                           std::unordered_map<std::string, bool>& variables);
    
    bool writeHeader(std::ofstream& out, uint32_t bytecode_size);
};

} // namespace om
