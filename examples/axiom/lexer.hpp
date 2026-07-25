#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <optional>  // ADD THIS
#include <cctype>

namespace axiom {

// ── Token Types ──
enum class TokenType {
    // Keywords
    LET, PRINT, PRINTLN, IF, ELSE, WHILE, FUNC, RETURN,
    // Types
    INT, FLOAT, STRING, BOOL, CHAR,
    // Values
    IDENTIFIER, NUMBER, STRING_LITERAL, CHAR_LITERAL, BOOL_LITERAL,
    // Operators
    PLUS, MINUS, STAR, SLASH, EQUALS, EQUALS_EQUALS,
    GT, LT, GTE, LTE, NOT, NOT_EQUALS,
    // Symbols
    LPAREN, RPAREN, LBRACE, RBRACE, SEMICOLON, COMMA, COLON, DOT,
    // Comments
    COMMENT,
    // End of file
    EOF_TOKEN
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;

    Token(TokenType t, const std::string& v = "", int l = 0, int c = 0)
        : type(t), value(v), line(l), column(c) {}

    std::string to_string() const {
        static const std::unordered_map<TokenType, std::string> names = {
            {TokenType::LET, "LET"},
            {TokenType::PRINT, "PRINT"},
            {TokenType::PRINTLN, "PRINTLN"},
            {TokenType::IF, "IF"},
            {TokenType::ELSE, "ELSE"},
            {TokenType::WHILE, "WHILE"},
            {TokenType::FUNC, "FUNC"},
            {TokenType::RETURN, "RETURN"},
            {TokenType::INT, "INT"},
            {TokenType::FLOAT, "FLOAT"},
            {TokenType::STRING, "STRING"},
            {TokenType::BOOL, "BOOL"},
            {TokenType::CHAR, "CHAR"},
            {TokenType::IDENTIFIER, "IDENTIFIER(" + value + ")"},
            {TokenType::NUMBER, "NUMBER(" + value + ")"},
            {TokenType::STRING_LITERAL, "STRING_LITERAL(\"" + value + "\")"},
            {TokenType::CHAR_LITERAL, "CHAR_LITERAL('" + value + "')"},
            {TokenType::BOOL_LITERAL, "BOOL_LITERAL(" + value + ")"},
            {TokenType::PLUS, "PLUS"},
            {TokenType::MINUS, "MINUS"},
            {TokenType::STAR, "STAR"},
            {TokenType::SLASH, "SLASH"},
            {TokenType::EQUALS, "EQUALS"},
            {TokenType::EQUALS_EQUALS, "EQUALS_EQUALS"},
            {TokenType::GT, "GT"},
            {TokenType::LT, "LT"},
            {TokenType::GTE, "GTE"},
            {TokenType::LTE, "LTE"},
            {TokenType::NOT, "NOT"},
            {TokenType::NOT_EQUALS, "NOT_EQUALS"},
            {TokenType::LPAREN, "LPAREN"},
            {TokenType::RPAREN, "RPAREN"},
            {TokenType::LBRACE, "LBRACE"},
            {TokenType::RBRACE, "RBRACE"},
            {TokenType::SEMICOLON, "SEMICOLON"},
            {TokenType::COMMA, "COMMA"},
            {TokenType::COLON, "COLON"},
            {TokenType::DOT, "DOT"},
            {TokenType::COMMENT, "COMMENT(" + value + ")"},
            {TokenType::EOF_TOKEN, "EOF"}
        };
        auto it = names.find(type);
        if (it != names.end()) return it->second;
        return "UNKNOWN";
    }
};

// ── Lexer ──
class Lexer {
public:
    Lexer(const std::string& source) : source(source), pos(0), line(1), column(1) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        while (true) {
            char c = peek();
            if (c == '\0') { tokens.push_back(Token(TokenType::EOF_TOKEN)); break; }
            if (std::isspace(c)) { skipWhitespace(); continue; }
            if (c == '/' && peekNext() == '/') { tokens.push_back(scanComment()); continue; }
            if (c == '"') { tokens.push_back(scanString()); continue; }
            if (c == '\'') { tokens.push_back(scanChar()); continue; }
            if (std::isalpha(c) || c == '_') { tokens.push_back(scanIdentifier()); continue; }
            if (std::isdigit(c) || c == '.') { tokens.push_back(scanNumber()); continue; }
            if (auto tok = scanSymbol()) { tokens.push_back(*tok); continue; }
            advance(); // Skip unknown
        }
        return tokens;
    }

private:
    std::string source;
    size_t pos;
    int line, column;

    char peek() const {
        if (pos >= source.length()) return '\0';
        return source[pos];
    }

    char peekNext() const {
        if (pos + 1 >= source.length()) return '\0';
        return source[pos + 1];
    }

    char advance() {
        char c = peek();
        pos++;
        if (c == '\n') { line++; column = 1; }
        else { column++; }
        return c;
    }

    void skipWhitespace() {
        while (std::isspace(peek())) advance();
    }

    Token scanComment() {
        advance(); advance(); // consume //
        std::string comment;
        while (peek() != '\n' && peek() != '\0') {
            comment += advance();
        }
        return Token(TokenType::COMMENT, comment, line, column);
    }

    Token scanString() {
        advance(); // consume "
        std::string str;
        while (peek() != '"' && peek() != '\0') {
            str += advance();
        }
        if (peek() == '"') advance();
        return Token(TokenType::STRING_LITERAL, str, line, column);
    }

    Token scanChar() {
        advance(); // consume '
        std::string ch;
        if (peek() != '\'') ch += advance();
        if (peek() == '\'') advance();
        return Token(TokenType::CHAR_LITERAL, ch, line, column);
    }

    Token scanIdentifier() {
        std::string ident;
        while (std::isalnum(peek()) || peek() == '_') {
            ident += advance();
        }

        // Keywords
        if (ident == "let") return Token(TokenType::LET, ident, line, column);
        if (ident == "print") return Token(TokenType::PRINT, ident, line, column);
        if (ident == "println") return Token(TokenType::PRINTLN, ident, line, column);
        if (ident == "if") return Token(TokenType::IF, ident, line, column);
        if (ident == "else") return Token(TokenType::ELSE, ident, line, column);
        if (ident == "while") return Token(TokenType::WHILE, ident, line, column);
        if (ident == "func") return Token(TokenType::FUNC, ident, line, column);
        if (ident == "return") return Token(TokenType::RETURN, ident, line, column);
        if (ident == "int") return Token(TokenType::INT, ident, line, column);
        if (ident == "float") return Token(TokenType::FLOAT, ident, line, column);
        if (ident == "string") return Token(TokenType::STRING, ident, line, column);
        if (ident == "bool") return Token(TokenType::BOOL, ident, line, column);
        if (ident == "char") return Token(TokenType::CHAR, ident, line, column);
        if (ident == "true" || ident == "false") 
            return Token(TokenType::BOOL_LITERAL, ident, line, column);

        return Token(TokenType::IDENTIFIER, ident, line, column);
    }

    Token scanNumber() {
        std::string num;
        bool hasDot = false;
        while (std::isdigit(peek()) || peek() == '.') {
            if (peek() == '.') {
                if (hasDot) break;
                hasDot = true;
            }
            num += advance();
        }
        return Token(TokenType::NUMBER, num, line, column);
    }

    std::optional<Token> scanSymbol() {
        char c = peek();
        advance();

        switch (c) {
            case '+': return Token(TokenType::PLUS, "+", line, column);
            case '-': return Token(TokenType::MINUS, "-", line, column);
            case '*': return Token(TokenType::STAR, "*", line, column);
            case '/': return Token(TokenType::SLASH, "/", line, column);
            case '(': return Token(TokenType::LPAREN, "(", line, column);
            case ')': return Token(TokenType::RPAREN, ")", line, column);
            case '{': return Token(TokenType::LBRACE, "{", line, column);
            case '}': return Token(TokenType::RBRACE, "}", line, column);
            case ';': return Token(TokenType::SEMICOLON, ";", line, column);
            case ',': return Token(TokenType::COMMA, ",", line, column);
            case ':': return Token(TokenType::COLON, ":", line, column);
            case '.': return Token(TokenType::DOT, ".", line, column);
            case '=':
                if (peek() == '=') { advance(); return Token(TokenType::EQUALS_EQUALS, "==", line, column); }
                return Token(TokenType::EQUALS, "=", line, column);
            case '>':
                if (peek() == '=') { advance(); return Token(TokenType::GTE, ">=", line, column); }
                return Token(TokenType::GT, ">", line, column);
            case '<':
                if (peek() == '=') { advance(); return Token(TokenType::LTE, "<=", line, column); }
                return Token(TokenType::LT, "<", line, column);
            case '!':
                if (peek() == '=') { advance(); return Token(TokenType::NOT_EQUALS, "!=", line, column); }
                return Token(TokenType::NOT, "!", line, column);
            default: return std::nullopt;
        }
    }
};

} // namespace axiom
