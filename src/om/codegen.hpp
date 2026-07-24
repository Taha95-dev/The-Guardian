#pragma once

#include "ast.hpp"
#include <vector>
#include <string>
#include <cstdint>
#include <memory>

namespace om {

struct GuardianHeader {
    uint32_t magic;
    uint16_t version;
    uint16_t flags;
    uint32_t entry_point;
    uint32_t instruction_count;
    uint8_t reserved[8];
};

class CodeGen {
public:
    CodeGen();
    bool generate(const std::unique_ptr<ProgramNode>& ast, const std::string& output_path);
    
private:
    uint32_t magic;
    uint16_t version;
    uint16_t flags;
    
    void generateStatement(const std::unique_ptr<ASTNode>& node, std::vector<uint8_t>& bytecode);
    void generateExpression(const std::unique_ptr<ASTNode>& node, std::vector<uint8_t>& bytecode);
    bool writeHeader(std::ofstream& out, uint32_t bytecode_size);
};

} // namespace om
