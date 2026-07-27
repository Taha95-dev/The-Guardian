# 📖 API Reference — Parser

## Parser API

The Guardian's parser provides a complete foundation for building language parsers.

### Token

```cpp
struct Token {
    enum class Type {
        IDENTIFIER, NUMBER, STRING,
        IF, ELSE, FOR, WHILE, RETURN, LET, FN,
        TRUE, FALSE, NULL_TOKEN,
        LPAREN, RPAREN, LBRACE, RBRACE,
        SEMICOLON, COLON, COMMA, DOT, ASSIGN,
        PLUS, MINUS, STAR, SLASH, MOD,
        EQUAL, NOT_EQUAL, LESS, GREATER,
        LESS_EQUAL, GREATER_EQUAL, AND, OR, NOT,
        // ...
    };
    Type type;
    std::string value;
    int line;
    int column;
};

AST Nodes
Node	Purpose
ProgramNode	Root of the AST
BlockNode	Block of statements
IfNode	If/Else statement
ForNode	For loop
WhileNode	While loop
ReturnNode	Return statement
LiteralNode	Literal value
BinaryOpNode	Binary operation
VariableNode	Variable declaration
CallNode	Function call
Parser Base Class
cpp

class Parser {
public:
    virtual std::unique_ptr<ASTNode> parse() = 0;
    
protected:
    // Extended parsing methods
    std::unique_ptr<ASTNode> parseIf();
    std::unique_ptr<ASTNode> parseFor();
    std::unique_ptr<ASTNode> parseWhile();
    std::unique_ptr<ASTNode> parseReturn();
    
    // Virtual methods (must implement)
    virtual std::unique_ptr<ASTNode> parseExpression() = 0;
    virtual std::unique_ptr<ASTNode> parseStatement() = 0;
    virtual std::unique_ptr<ASTNode> parseBlock() = 0;
};

Building a Parser
cpp

class MyParser : public Parser {
public:
    std::unique_ptr<ASTNode> parseExpression() override {
        // Parse expressions
    }
    std::unique_ptr<ASTNode> parseStatement() override {
        // Parse statements
    }
    std::unique_ptr<ASTNode> parseBlock() override {
        // Parse blocks
    }
    std::unique_ptr<ASTNode> parse() override {
        // Parse program
    }
};

