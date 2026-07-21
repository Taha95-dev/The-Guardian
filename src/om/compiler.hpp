#pragma once

#include "lexer.hpp"
#include "parser.hpp"
#include "codegen.hpp"
#include <string>
#include <vector>
#include <memory>

namespace om {

class Compiler {
public:
    Compiler();
    
    // Compile Om source to .gbin
    bool compile(const std::string& source, const std::string& output_path);
    
    // Compile from tokens (after parsing)
    bool compile(const std::vector<Token>& tokens, const std::string& output_path);
    
    // Run the compiled binary
    bool run(const std::string& binary_path);
    
private:
    std::unique_ptr<Parser> parser;
    std::unique_ptr<CodeGen> codegen;
};

} // namespace om
