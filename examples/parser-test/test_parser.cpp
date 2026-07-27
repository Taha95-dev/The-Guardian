#include <guardian/parser/parser.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace guardian::parser;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file>\n";
        return 1;
    }
    
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file\n";
        return 1;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    std::cout << "🔤 Tokens: " << tokens.size() << "\n";
    for (const auto& token : tokens) {
        std::cout << "  " << token.value << "\n";
    }
    
    Parser parser(tokens);
    auto ast = parser.parse();
    
    if (ast) {
        std::cout << "✅ AST built successfully!\n";
    } else {
        std::cerr << "❌ Parsing failed!\n";
        return 1;
    }
    
    return 0;
}
