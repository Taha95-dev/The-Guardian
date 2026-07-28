#include <guardian/parser/parser.hpp>
#include <iostream>

using namespace guardian::parser;

int main() {
    std::string source = "let x = 42;";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    for (const auto& token : tokens) {
        std::cout << token.value << " ";
    }
    std::cout << "\n";
    return 0;
}
