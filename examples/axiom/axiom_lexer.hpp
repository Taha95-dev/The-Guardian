#pragma once
#include <guardian/compiler/lexer.hpp>
#include <vector>
#include <string>

namespace axiom {

using namespace guardian::compiler;

class AxiomLexer : public Lexer {
public:
    AxiomLexer(const std::string& source) : Lexer(source) {}
    
    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        Token tok = next_token();
        while (tok.type != TokenType::END_OF_FILE) {
            tokens.push_back(tok);
            tok = next_token();
        }
        tokens.push_back(tok); // EOF
        return tokens;
    }
};

} // namespace axiom
