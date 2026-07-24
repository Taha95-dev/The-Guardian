#include "compiler.hpp"
#include "parser.hpp"
#include "codegen.hpp"
#include "../vm/vm.hpp"
#include <iostream>
#include <fstream>

namespace om {

Compiler::Compiler() 
    : codegen(std::make_unique<CodeGen>()) {}

bool Compiler::compile(const std::string& source, const std::string& output_path) {
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    return compile(tokens, output_path);
}

bool Compiler::compile(const std::vector<Token>& tokens, const std::string& output_path) {
    Parser parser(tokens);
    auto ast = parser.parse();
    
    if (!ast) {
        std::cerr << "Error: Failed to parse AST\n";
        return false;
    }
    
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
    
    std::vector<uint8_t> bytecode(header.instruction_count);
    in.seekg(header.entry_point);
    in.read(reinterpret_cast<char*>(bytecode.data()), header.instruction_count);
    in.close();
    
    guardian::vm::VM vm;
    vm.reset();
    vm.load(bytecode, 0);
    vm.run();
    
    return true;
}

} // namespace om
