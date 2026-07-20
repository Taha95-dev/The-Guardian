#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace guardian {
namespace compiler {

// ============================================
// TOKEN TYPES
// ============================================
enum class TokenType {
    KEYWORD,
    IDENTIFIER,
    NUMBER,
    STRING,
    OPERATOR,
    PUNCTUATOR,
    COMMENT,
    END_OF_FILE,
    UNKNOWN,
};

// ============================================
// TOKEN
// ============================================
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    Token(TokenType t, const std::string& v, int l = 1, int c = 1)
        : type(t), value(v), line(l), column(c) {}
};

// ============================================
// LEXER
// ============================================
class Lexer {
private:
    std::string source;
    size_t pos = 0;
    int line = 1;
    int column = 1;
    
    static const std::unordered_map<std::string, TokenType> keywords;
    Token read_punctuator();
    
    char peek() const;
    char advance();
    void skip_whitespace();
    void skip_comment();
    Token read_identifier();
    Token read_number();
    Token read_string();
    Token read_operator();
    
public:
    Lexer(const std::string& src);
    std::vector<Token> tokenize();
    Token next_token();
};

} // namespace compiler
} // namespace guardian
