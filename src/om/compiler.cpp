#include "compiler.hpp"
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
    : parser(nullptr), codegen(std::make_unique<CodeGen>()) {}

bool Compiler::compile(const std::string& source, const std::string& output_path) {
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    return compile(tokens, output_path);
}

bool Compiler::compile(const std::vector<Token>& tokens, const std::string& output_path) {
    // TODO: Parse tokens into AST
    // For now, generate placeholder instructions
    std::vector<std::string> instructions = {
        "PUSH_STRING Hello from Om!",
        "PRINTLN",
        "PUSH_INT 42",
        "PRINTLN",
        "HALT"
    };
    
    return codegen->generate(instructions, output_path);
}

bool Compiler::run(const std::string& binary_path) {
    std::cout << "  ▶️ Executing: " << binary_path << "\n";
    
    // Load the .gbin file
    std::ifstream in(binary_path, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "Error: Could not open " << binary_path << "\n";
        return false;
    }
    
    // Read header
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
    
    // Read bytecode
    std::vector<uint8_t> bytecode(header.instruction_count);
    in.seekg(header.entry_point);
    in.read(reinterpret_cast<char*>(bytecode.data()), header.instruction_count);
    in.close();
    
    std::cout << "  🚀 Executing bytecode via Guardian VM...\n\n";
    
    // Create VM and execute
    guardian::vm::VM vm;
    vm.load(bytecode, 0);  // Start from beginning
    vm.run();
    
    std::cout << "\n  ✅ Execution complete!\n";
    return true;
}

} // namespace om
