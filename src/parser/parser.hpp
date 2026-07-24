#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

namespace guardian::parser {

// ============================================
// TOKEN — Base token type
// ============================================
struct Token {
    enum class Type {
        IDENTIFIER,
        NUMBER,
        STRING,
        KEYWORD,
        SYMBOL,
        END_OF_FILE
    };
    
    Type type;
    std::string value;
    int line;
    int column;
    
    Token() : type(Type::END_OF_FILE), line(0), column(0) {}
    Token(Type t, const std::string& v, int l, int c) 
        : type(t), value(v), line(l), column(c) {}
};

// ============================================
// AST NODE — Base abstract syntax tree node
// ============================================
struct ASTNode {
    enum class Type {
        PROGRAM,
        FUNCTION,
        VARIABLE,
        EXPRESSION,
        LITERAL,
        BINARY_OP,
        UNARY_OP,
        CALL,
        IF,
        LOOP,
        RETURN,
        BLOCK,
        CUSTOM
    };
    
    Type type;
    int line;
    int column;
    virtual ~ASTNode() = default;
};

// ============================================
// LEXER — Base lexer
// ============================================
class Lexer {
public:
    Lexer(const std::string& source);
    virtual ~Lexer() = default;
    
    virtual std::vector<Token> tokenize() = 0;
    
protected:
    std::string source;
    size_t pos;
    int line;
    int column;
    
    char peek() const;
    char advance();
    void skipWhitespace();
    bool isAtEnd() const;
};

// ============================================
// PARSER — Base parser
// ============================================
class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    virtual ~Parser() = default;
    
    virtual std::unique_ptr<ASTNode> parse() = 0;
    
protected:
    std::vector<Token> tokens;
    size_t pos;
    int error_count;
    int max_errors;
    
    Token peek() const;
    Token advance();
    bool match(Token::Type type);
    bool isAtEnd() const;
    void error(const std::string& msg);
};

} // namespace guardian::parser
