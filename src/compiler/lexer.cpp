#include "lexer.hpp"
#include <cctype>
#include <cstring>
#include <iostream>

namespace guardian::compiler {

const std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"let", TokenType::KEYWORD},
    {"fn", TokenType::KEYWORD},
    {"if", TokenType::KEYWORD},
    {"else", TokenType::KEYWORD},
    {"while", TokenType::KEYWORD},
    {"return", TokenType::KEYWORD},
    {"true", TokenType::KEYWORD},
    {"false", TokenType::KEYWORD},
};

Lexer::Lexer(const std::string& src) : source(src) {}

char Lexer::peek() const {
    if (pos >= source.size()) return '\0';
    return source[pos];
}

char Lexer::advance() {
    char c = peek();
    if (c == '\n') { line++; column = 1; }
    else { column++; }
    pos++;
    return c;
}

void Lexer::skip_whitespace() {
    while (std::isspace(peek())) {
        advance();
    }
}

void Lexer::skip_comment() {
    if (peek() == '/' && pos + 1 < source.size() && source[pos + 1] == '/') {
        while (peek() != '\n' && peek() != '\0') {
            advance();
        }
    }
}

Token Lexer::read_identifier() {
    std::string result;
    while (std::isalnum(peek()) || peek() == '_') {
        result += advance();
    }
    
    auto it = keywords.find(result);
    if (it != keywords.end()) {
        return Token(it->second, result, line, column);
    }
    return Token(TokenType::IDENTIFIER, result, line, column);
}

Token Lexer::read_number() {
    std::string result;
    while (std::isdigit(peek()) || peek() == '.') {
        result += advance();
    }
    return Token(TokenType::NUMBER, result, line, column);
}

Token Lexer::read_string() {
    advance(); // Skip opening quote
    std::string result;
    while (peek() != '"' && peek() != '\0') {
        result += advance();
    }
    advance(); // Skip closing quote
    return Token(TokenType::STRING, result, line, column);
}

Token Lexer::read_operator() {
    char c = advance();
    std::string op(1, c);
    
    // Multi-character operators
    if (c == '=' && peek() == '=') { op += advance(); return Token(TokenType::OPERATOR, op); }
    if (c == '!' && peek() == '=') { op += advance(); return Token(TokenType::OPERATOR, op); }
    if (c == '<' && peek() == '=') { op += advance(); return Token(TokenType::OPERATOR, op); }
    if (c == '>' && peek() == '=') { op += advance(); return Token(TokenType::OPERATOR, op); }
    if (c == '+' && peek() == '=') { op += advance(); return Token(TokenType::OPERATOR, op); }
    if (c == '&' && peek() == '&') { op += advance(); return Token(TokenType::OPERATOR, op); }
    if (c == '|' && peek() == '|') { op += advance(); return Token(TokenType::OPERATOR, op); }
    
    // Single-character operators
    switch (c) {
        case '+': return Token(TokenType::OPERATOR, op);
        case '-': return Token(TokenType::OPERATOR, op);
        case '*': return Token(TokenType::OPERATOR, op);
        case '/': return Token(TokenType::OPERATOR, op);
        case '=': return Token(TokenType::OPERATOR, op);
        case '<': return Token(TokenType::OPERATOR, op);
        case '>': return Token(TokenType::OPERATOR, op);
        case '!': return Token(TokenType::OPERATOR, op);
        default:  return Token(TokenType::UNKNOWN, op);
    }
}

Token Lexer::read_punctuator() {
    char c = advance();
    std::string p(1, c);
    
    switch (c) {
        case '(': return Token(TokenType::PUNCTUATOR, p);
        case ')': return Token(TokenType::PUNCTUATOR, p);
        case '{': return Token(TokenType::PUNCTUATOR, p);
        case '}': return Token(TokenType::PUNCTUATOR, p);
        case ';': return Token(TokenType::PUNCTUATOR, p);
        case ',': return Token(TokenType::PUNCTUATOR, p);
        default:  return Token(TokenType::UNKNOWN, p);
    }
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    Token token = next_token();
    while (token.type != TokenType::END_OF_FILE) {
        tokens.push_back(token);
        token = next_token();
    }
    tokens.push_back(token);
    return tokens;
}

Token Lexer::next_token() {
    skip_whitespace();
    skip_comment();
    
    char c = peek();
    
    if (c == '\0') {
        return Token(TokenType::END_OF_FILE, "EOF", line, column);
    }
    
    if (std::isalpha(c) || c == '_') {
        return read_identifier();
    }
    
    if (std::isdigit(c)) {
        return read_number();
    }
    
    if (c == '"') {
        return read_string();
    }
    
    if (strchr("+-*/=<>!&|", c)) {
        return read_operator();
    }
    
    if (strchr("(){};,", c)) {
        return read_punctuator();
    }
    
    advance();
    return Token(TokenType::UNKNOWN, std::string(1, c), line, column);
}

} // namespace guardian::compiler
