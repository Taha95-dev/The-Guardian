#include "compiler.hpp"
#include "../compiler/lexer.hpp"   // ← The Guardian's lexer!
#include "parser.hpp"
#include "codegen.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>

namespace axiom {

// Static member initialization
bool Compiler::debug_tokens = false;
bool Compiler::debug_ast = false;
std::string Compiler::output_file;

std::vector<guardian::vm::Instruction> Compiler::compile(const std::string& source) {
    // 1. Lex using Guardian's lexer
    guardian::compiler::Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    // Debug: Print tokens
    if (debug_tokens) {
        std::cout << "\n📝 TOKENS:\n";
        std::cout << "─────────────\n";
        for (const auto& token : tokens) {
            std::cout << "  " << std::setw(12) << std::left << token.value 
                      << " [type: " << static_cast<int>(token.type) << "]\n";
        }
        std::cout << "\n";
    }
    
    // 2. Parse to AST
    Parser parser(tokens);
    auto program = parser.parse();
    if (!program) {
        std::cerr << "Failed to parse program" << std::endl;
        return {};
    }
    
    if (debug_ast) {
        std::cout << "\n🌳 AST:\n";
        std::cout << "─────────────\n";
        for (const auto& func : program->functions) {
            // Cast to FunctionDef* to access name and body
            if (auto* fd = dynamic_cast<FunctionDef*>(func.get())) {
                std::cout << "  Function: " << fd->name << "\n";
                std::cout << "    Statements: " << fd->body.size() << "\n";
            }
        }
        std::cout << "\n";
    }
    
    // 3. Convert AST to Guardian atoms
    auto atom = program->to_atom();
    if (!atom) {
        std::cerr << "Failed to convert to atom" << std::endl;
        return {};
    }
    
    // 4. Generate bytecode
    CodeGenerator codegen;
    return codegen.generate(atom.get());
}

void Compiler::compile_file(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return;
    }
    
    std::string source((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    auto bytecode = compile(source);
    
    std::string output_path = output_file.empty() 
        ? path.substr(0, path.find_last_of('.')) + ".axbin"
        : output_file;
    
    std::ofstream out(output_path, std::ios::binary);
    if (!out) {
        std::cerr << "Failed to create output file: " << output_path << std::endl;
        return;
    }
    
    for (const auto& instr : bytecode) {
        uint8_t op = static_cast<uint8_t>(instr.opcode);
        out.write(reinterpret_cast<const char*>(&op), 1);
        for (uint64_t operand : instr.operands) {
            out.write(reinterpret_cast<const char*>(&operand), sizeof(uint64_t));
        }
    }
    
    std::cout << "✅ Compiled to: " << output_path << std::endl;
}

} // namespace axiom
