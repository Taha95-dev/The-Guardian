#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <optional>
#include <iostream>

namespace guardian::parser {

// ── Token ──
struct Token {
    enum class Type {
        IDENTIFIER, NUMBER, STRING, CHAR,
        LET, FN, IF, ELSE, FOR, WHILE, RETURN, TRUE, FALSE, NULL_TOKEN,
        PLUS, MINUS, STAR, SLASH, MOD, ASSIGN, EQUAL, NOT_EQUAL,
        LESS, GREATER, LESS_EQUAL, GREATER_EQUAL, AND, OR, NOT,
        LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET,
        SEMICOLON, COLON, COMMA, DOT, ARROW,
        COMMENT, END_OF_FILE
    };
    
    Type type;
    std::string value;
    int line;
    int column;
    
    Token() : type(Type::END_OF_FILE), line(0), column(0) {}
    Token(Type t, const std::string& v = "", int l = 0, int c = 0) 
        : type(t), value(v), line(l), column(c) {}
};

// ── AST Nodes ──
struct ASTNode {
    enum class Type {
        PROGRAM, BLOCK, LET, ASSIGNMENT, IF, FOR, WHILE, RETURN,
        FUNCTION, CALL, LITERAL, BINARY_OP, UNARY_OP, IDENTIFIER,
        PRINT, CUSTOM
    };
    Type type;
    int line;
    int column;
    virtual ~ASTNode() = default;
};

struct ProgramNode : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
    ProgramNode() { type = Type::PROGRAM; }
};

struct BlockNode : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
    BlockNode() { type = Type::BLOCK; }
};

struct LetNode : ASTNode {
    std::string name;
    std::unique_ptr<ASTNode> value;
    LetNode() { type = Type::LET; }
};

struct IfNode : ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> then_branch;
    std::unique_ptr<ASTNode> else_branch;
    IfNode() { type = Type::IF; }
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

struct LiteralNode : ASTNode {
    enum LiteralType { LIT_INT, LIT_FLOAT, LIT_STRING, LIT_BOOL, LIT_CHAR };
    LiteralType literal_type;
    std::string value;
    LiteralNode() { type = Type::LITERAL; literal_type = LIT_INT; }
};

struct BinaryOpNode : ASTNode {
    std::string op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    BinaryOpNode() { type = Type::BINARY_OP; }
};

struct IdentifierNode : ASTNode {
    std::string name;
    IdentifierNode() { type = Type::IDENTIFIER; }
};

struct PrintNode : ASTNode {
    std::unique_ptr<ASTNode> value;
    bool newline;
    PrintNode(bool nl = true) : newline(nl) { type = Type::PRINT; }
};

// ── Lexer ──
class Lexer {
public:
    Lexer(const std::string& source);
    ~Lexer() = default;
    
    std::vector<Token> tokenize();
    
private:
    std::string source;
    size_t pos;
    int line;
    int column;
    
    char peek() const;
    char peekNext() const;
    char advance();
    void skipWhitespace();
    bool isAtEnd() const;
    void error(const std::string& msg);
    Token scanIdentifier();
    Token scanNumber();
    Token scanString();
    Token scanChar();
    std::optional<Token> scanSymbol();
};

// ── Parser ──
class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    virtual ~Parser() = default;
    
    std::unique_ptr<ASTNode> parse();
    
protected:
    std::vector<Token> tokens;
    size_t pos;
    int error_count;
    
    Token peek() const;
    Token advance();
    bool match(Token::Type type);
    bool isAtEnd() const;
    void error(const std::string& msg);
    void synchronize();
    
    std::unique_ptr<ASTNode> parseProgram();
    std::unique_ptr<ASTNode> parseBlock();
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseBinary(int min_precedence = 0);
    std::unique_ptr<ASTNode> parsePrimary();
    std::unique_ptr<ASTNode> parseLet();
    std::unique_ptr<ASTNode> parseIf();
    std::unique_ptr<ASTNode> parseWhile();
    std::unique_ptr<ASTNode> parseReturn();
    std::unique_ptr<ASTNode> parsePrint(bool newline);
    
    int getPrecedence(Token::Type type);
};

} // namespace guardian::parser
