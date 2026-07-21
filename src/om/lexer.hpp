#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace om {

enum class TokenType {
    // Keywords
    FN, LET, CONST, IF, ELSE, FOR, WHILE, RETURN, STRUCT,
    TRUE, FALSE, NULL_TOKEN,
    SHORTCUT,
    
    // Symbols
    IDENTIFIER, NUMBER, STRING, CHAR,
    PLUS, MINUS, STAR, SLASH, MOD, ASSIGN, EQUAL, NOT_EQUAL,
    LESS, GREATER, LESS_EQUAL, GREATER_EQUAL,
    AND, OR, NOT,
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET,
    SEMICOLON, COLON, COMMA, DOT, ARROW,
    
    // Comments
    COMMENT, MULTILINE_COMMENT,
    
    // End of file
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    Token(TokenType type, const std::string& value, int line, int column)
        : type(type), value(value), line(line), column(column) {}
};

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();
    
private:
    std::string source;
    size_t pos;
    int line;
    int column;
    int error_count = 0;
    
    char peek() const;
    char advance();
    void skipWhitespace();
    void skipComment();
    void skipMultilineComment();
    Token readIdentifier();
    Token readNumber();
    Token readString();
    Token readChar();
    Token readSymbol();
    
    std::unordered_map<std::string, TokenType> keywords = {
        {"fn", TokenType::FN},
        {"let", TokenType::LET},
        {"const", TokenType::CONST},
        {"if", TokenType::IF},
        {"else", TokenType::ELSE},
        {"for", TokenType::FOR},
        {"while", TokenType::WHILE},
        {"return", TokenType::RETURN},
        {"struct", TokenType::STRUCT},
        {"true", TokenType::TRUE},
        {"false", TokenType::FALSE},
        {"null", TokenType::NULL_TOKEN},
        {"shortcut", TokenType::SHORTCUT}
    };
};

} // namespace om
