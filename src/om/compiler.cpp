#include "compiler.hpp"
#include "parser.hpp"
#include "codegen.hpp"
#include "opcodes.hpp"
#include "../vm/vm.hpp"
#include <iostream>
#include <fstream>

struct GuardianHeader {
    uint32_t magic;
    uint16_t version;
    uint16_t flags;
    uint32_t entry_point;
    uint32_t instruction_count;
    uint8_t reserved[8];
};

namespace om {

Compiler::Compiler() 
    : codegen(std::make_unique<CodeGen>()) {}

bool Compiler::compile(const std::string& source, const std::string& output_path) {
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    return compile(tokens, output_path);
}

bool Compiler::compile(const std::vector<Token>& tokens, const std::string& output_path) {
    // Parse tokens into AST
    Parser parser(tokens);
    auto ast = parser.parse();
    
    if (!ast) {
        std::cerr << "Error: Failed to parse AST\n";
        return false;
    }
    
    // Generate bytecode from AST
    return codegen->generate(ast, output_path);
}

bool Compiler::run(const std::string& binary_path) {
    std::cout << "  ▶️ Executing: " << binary_path << "\n";
    
    std::ifstream in(binary_path, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "Error: Could not open " << binary_path << "\n";
        return false;
    }
    
    GuardianHeader header;
    in.read(reinterpret_cast<char*>(&header), sizeof(header));
    
    if (header.magic != 0x44525547) {
        std::cerr << "Error: Invalid magic number\n";
        return false;
    }
    
    std::cout << "  📖 Magic: GURD\n";
    std::cout << "  📖 Version: " << header.version << "\n";
    std::cout << "  📖 Bytecode size: " << header.instruction_count << " bytes\n";
    std::cout << "  📖 Entry point: " << header.entry_point << "\n";
    
    std::vector<uint8_t> bytecode(header.instruction_count);
    in.seekg(header.entry_point);
    in.read(reinterpret_cast<char*>(bytecode.data()), header.instruction_count);
    in.close();
    
    std::cout << "  🚀 Executing bytecode via Guardian VM...\n\n";
    
    guardian::vm::VM vm;
    vm.load(bytecode, 0);
    vm.run();
    
    std::cout << "\n  ✅ Execution complete!\n";
    return true;
}

} // namespace om
