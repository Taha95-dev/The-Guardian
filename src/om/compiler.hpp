#pragma once

#include "lexer.hpp"
#include "parser.hpp"
#include "codegen.hpp"
#include <vector>
#include <string>
#include <memory>

namespace om {

class Compiler {
public:
    Compiler();
    
    bool compile(const std::string& source, const std::string& output_path);
    bool compile(const std::vector<Token>& tokens, const std::string& output_path);
    bool run(const std::string& binary_path);
    
private:
    std::unique_ptr<CodeGen> codegen;
};

} // namespace om
