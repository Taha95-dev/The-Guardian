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
        // Literals
        IDENTIFIER,
        NUMBER,
        STRING,
        
        // Keywords
        KEYWORD,
        IF,
        ELSE,
        FOR,
        WHILE,
        RETURN,
        LET,
        FN,
        TRUE,
        FALSE,
        NULL_TOKEN,
        STRUCT,
        DICT,
        ANY,
        
        // Symbols
        LPAREN,
        RPAREN,
        LBRACE,
        RBRACE,
        LBRACKET,
        RBRACKET,
        SEMICOLON,
        COLON,
        COMMA,
        DOT,
        ASSIGN,
        PLUS,
        MINUS,
        STAR,
        SLASH,
        MOD,
        EQUAL,
        NOT_EQUAL,
        LESS,
        GREATER,
        LESS_EQUAL,
        GREATER_EQUAL,
        AND,
        OR,
        NOT,
        ARROW,
        
        // Special
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
        FOR,
        WHILE,
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
// AST NODE TYPES
// ============================================
struct ProgramNode : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
    ProgramNode() { type = Type::PROGRAM; }
};

struct BlockNode : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
    BlockNode() { type = Type::BLOCK; }
};

struct IfNode : ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> then_branch;
    std::unique_ptr<ASTNode> else_branch;
    IfNode() { type = Type::IF; }
};

struct ForNode : ASTNode {
    std::unique_ptr<ASTNode> init;
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> increment;
    std::unique_ptr<ASTNode> body;
    ForNode() { type = Type::FOR; }
};

struct WhileNode : ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> body;
    WhileNode() { type = Type::WHILE; }
};

struct ReturnNode : ASTNode {
    std::unique_ptr<ASTNode> value;
    ReturnNode() { type = Type::RETURN; }
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
    
    // Extended parser methods
    std::unique_ptr<ASTNode> parseIf();
    std::unique_ptr<ASTNode> parseFor();
    std::unique_ptr<ASTNode> parseWhile();
    std::unique_ptr<ASTNode> parseReturn();
    
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
    
    // Virtual methods for language-specific parsing
    virtual std::unique_ptr<ASTNode> parseExpression() = 0;
    virtual std::unique_ptr<ASTNode> parseStatement() = 0;
    virtual std::unique_ptr<ASTNode> parseBlock() = 0;
};

} // namespace guardian::parser
