#pragma once
#include <string>
#include <vector>

// We're using The Guardian's lexer, not Axiom's
namespace guardian::vm {
    struct Instruction;
}

namespace axiom {

class Compiler {
private:
    static bool debug_tokens;
    static bool debug_ast;
    static std::string output_file;
    
public:
    static void set_debug_tokens(bool enable) { debug_tokens = enable; }
    static void set_debug_ast(bool enable) { debug_ast = enable; }
    static void set_output_file(const std::string& path) { output_file = path; }
    
    static std::vector<guardian::vm::Instruction> compile(const std::string& source);
    static void compile_file(const std::string& path);
};

} // namespace axiom
