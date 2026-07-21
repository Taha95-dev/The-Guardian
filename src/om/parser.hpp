#pragma once

#include "lexer.hpp"
#include <vector>
#include <memory>

namespace om {

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    
private:
    std::vector<Token> tokens;
    size_t pos;
};

} // namespace om
