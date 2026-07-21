#include "lexer.hpp"
#include <cctype>
#include <iostream>

namespace om {

Lexer::Lexer(const std::string& source)
    : source(source), pos(0), line(1), column(1) {}

char Lexer::peek() const {
    if (pos >= source.length()) return '\0';
    return source[pos];
}

char Lexer::advance() {
    char c = peek();
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    pos++;
    return c;
}

void Lexer::skipWhitespace() {
    while (std::isspace(peek())) {
        advance();
    }
}

void Lexer::skipComment() {
    if (peek() == '/' && pos + 1 < source.length() && source[pos + 1] == '/') {
        while (peek() != '\n' && peek() != '\0') {
            advance();
        }
    }
}

void Lexer::skipMultilineComment() {
    if (peek() == '/' && pos + 1 < source.length() && source[pos + 1] == '*') {
        advance(); advance(); // consume /*
        while (!(peek() == '*' && pos + 1 < source.length() && source[pos + 1] == '/')) {
            if (peek() == '\0') {
                std::cerr << "Error: Unterminated multiline comment at line " << line << "\n";
                return;
            }
            advance();
        }
        advance(); advance(); // consume */
    }
}

Token Lexer::readIdentifier() {
    std::string value;
    int start_line = line;
    int start_col = column;
    
    while (std::isalnum(peek()) || peek() == '_') {
        value += advance();
    }
    
    auto it = keywords.find(value);
    if (it != keywords.end()) {
        return Token(it->second, value, start_line, start_col);
    }
    return Token(TokenType::IDENTIFIER, value, start_line, start_col);
}

Token Lexer::readNumber() {
    std::string value;
    int start_line = line;
    int start_col = column;
    
    while (std::isdigit(peek()) || peek() == '.') {
        value += advance();
    }
    
    return Token(TokenType::NUMBER, value, start_line, start_col);
}

Token Lexer::readString() {
    std::string value;
    int start_line = line;
    int start_col = column;
    
    advance(); // consume opening quote
    while (peek() != '"' && peek() != '\0') {
        if (peek() == '\\') {
            value += advance(); // backslash
            value += advance(); // escaped char
        } else {
            value += advance();
        }
    }
    advance(); // consume closing quote
    
    return Token(TokenType::STRING, value, start_line, start_col);
}

Token Lexer::readSymbol() {
    int start_line = line;
    int start_col = column;
    char c = advance();
    
    switch (c) {
        case '+': return Token(TokenType::PLUS, "+", start_line, start_col);
        case '-': return Token(TokenType::MINUS, "-", start_line, start_col);
        case '*': return Token(TokenType::STAR, "*", start_line, start_col);
        case '/': return Token(TokenType::SLASH, "/", start_line, start_col);
        case '=': 
            if (peek() == '=') {
                advance();
                return Token(TokenType::EQUAL, "==", start_line, start_col);
            }
            return Token(TokenType::ASSIGN, "=", start_line, start_col);
        case '!':
            if (peek() == '=') {
                advance();
                return Token(TokenType::NOT_EQUAL, "!=", start_line, start_col);
            }
            return Token(TokenType::NOT, "!", start_line, start_col);
        case '<':
            if (peek() == '=') {
                advance();
                return Token(TokenType::LESS_EQUAL, "<=", start_line, start_col);
            }
            if (peek() == '<') {
                advance();
                return Token(TokenType::ARROW, "<<", start_line, start_col);
            }
            return Token(TokenType::LESS, "<", start_line, start_col);
        case '>':
            if (peek() == '=') {
                advance();
                return Token(TokenType::GREATER_EQUAL, ">=", start_line, start_col);
            }
            if (peek() == '>') {
                advance();
                return Token(TokenType::ARROW, ">>", start_line, start_col);
            }
            return Token(TokenType::GREATER, ">", start_line, start_col);
        case '(': return Token(TokenType::LPAREN, "(", start_line, start_col);
        case ')': return Token(TokenType::RPAREN, ")", start_line, start_col);
        case '{': return Token(TokenType::LBRACE, "{", start_line, start_col);
        case '}': return Token(TokenType::RBRACE, "}", start_line, start_col);
        case '[': return Token(TokenType::LBRACKET, "[", start_line, start_col);
        case ']': return Token(TokenType::RBRACKET, "]", start_line, start_col);
        case ';': return Token(TokenType::SEMICOLON, ";", start_line, start_col);
        case ':': return Token(TokenType::COLON, ":", start_line, start_col);
        case ',': return Token(TokenType::COMMA, ",", start_line, start_col);
        case '.': return Token(TokenType::DOT, ".", start_line, start_col);
        case '&':
            if (peek() == '&') {
                advance();
                return Token(TokenType::AND, "&&", start_line, start_col);
            }
            return Token(TokenType::AND, "&", start_line, start_col);
        case '|':
            if (peek() == '|') {
                advance();
                return Token(TokenType::OR, "||", start_line, start_col);
            }
            return Token(TokenType::OR, "|", start_line, start_col);
        default:
            return Token(TokenType::END_OF_FILE, "", start_line, start_col);
    }
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    
    while (true) {
        char c = peek();
        if (c == '\0') {
            tokens.push_back(Token(TokenType::END_OF_FILE, "", line, column));
            break;
        }
        
        if (std::isspace(c)) {
            skipWhitespace();
            continue;
        }
        
        if (c == '/' && pos + 1 < source.length()) {
            if (source[pos + 1] == '/') {
                skipComment();
                continue;
            }
            if (source[pos + 1] == '*') {
                skipMultilineComment();
                continue;
            }
        }
        
        if (std::isalpha(c) || c == '_') {
            tokens.push_back(readIdentifier());
            continue;
        }
        
        if (std::isdigit(c)) {
            tokens.push_back(readNumber());
            continue;
        }
        
        if (c == '"') {
            tokens.push_back(readString());
            continue;
        }
        
        tokens.push_back(readSymbol());
    }
    
    return tokens;
}

} // namespace om
