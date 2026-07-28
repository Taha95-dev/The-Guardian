#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <optional>

namespace guardian::parser {

// ── Token ──
/**
 * @brief Represents a single token from the source code.
 * 
 * Tokens are the basic building blocks of the lexer. They represent
 * identifiers, numbers, strings, keywords, operators, and symbols.
 */
struct Token {
    /**
     * @brief Enumeration of all possible token types.
     */
    enum class Type {
        // Literals
        IDENTIFIER,   ///< Variable or function name
        NUMBER,       ///< Integer or floating-point number
        STRING,       ///< String literal
        CHAR,         ///< Character literal
        
        // Keywords
        LET,          ///< Variable declaration
        FN,           ///< Function declaration
        IF,           ///< If statement
        ELSE,         ///< Else statement
        FOR,          ///< For loop
        WHILE,        ///< While loop
        RETURN,       ///< Return statement
        TRUE,         ///< Boolean true
        FALSE,        ///< Boolean false
        NULL_TOKEN,   ///< Null value
        
        // Operators
        PLUS,         ///< Addition
        MINUS,        ///< Subtraction
        STAR,         ///< Multiplication
        SLASH,        ///< Division
        MOD,          ///< Modulo
        ASSIGN,       ///< Assignment
        EQUAL,        ///< Equality comparison
        NOT_EQUAL,    ///< Inequality comparison
        LESS,         ///< Less than
        GREATER,      ///< Greater than
        LESS_EQUAL,   ///< Less than or equal
        GREATER_EQUAL,///< Greater than or equal
        AND,          ///< Logical AND
        OR,           ///< Logical OR
        NOT,          ///< Logical NOT
        
        // Symbols
        LPAREN,       ///< Left parenthesis
        RPAREN,       ///< Right parenthesis
        LBRACE,       ///< Left brace
        RBRACE,       ///< Right brace
        LBRACKET,     ///< Left bracket
        RBRACKET,     ///< Right bracket
        SEMICOLON,    ///< Semicolon
        COLON,        ///< Colon
        COMMA,        ///< Comma
        DOT,          ///< Dot
        ARROW,        ///< Arrow
        
        // Special
        COMMENT,      ///< Comment
        END_OF_FILE   ///< End of file
    };
    
    Type type;           ///< The token type
    std::string value;   ///< The token's string value
    int line;            ///< Line number in source
    int column;          ///< Column number in source
    
    Token() : type(Type::END_OF_FILE), line(0), column(0) {}
    Token(Type t, const std::string& v = "", int l = 0, int c = 0) 
        : type(t), value(v), line(l), column(c) {}
};

// ── AST Nodes ──
/**
 * @brief Base class for all Abstract Syntax Tree nodes.
 * 
 * AST nodes represent the structure of the parsed source code.
 * Each node type corresponds to a language construct.
 */
struct ASTNode {
    /**
     * @brief Enumeration of all AST node types.
     */
    enum class Type {
        PROGRAM,     ///< Root of the AST
        BLOCK,       ///< Block of statements
        LET,         ///< Variable declaration
        IF,          ///< If statement
        FOR,         ///< For loop
        WHILE,       ///< While loop
        RETURN,      ///< Return statement
        CALL,        ///< Function call
        LITERAL,     ///< Literal value
        BINARY_OP,   ///< Binary operation
        UNARY_OP,    ///< Unary operation
        IDENTIFIER,  ///< Identifier
        PRINT,       ///< Print statement
        CUSTOM       ///< Custom node
    };
    
    Type type;           ///< The node type
    int line;            ///< Line number in source
    int column;          ///< Column number in source
    virtual ~ASTNode() = default;
};

// ── Concrete AST Nodes ──
/**
 * @brief Represents a complete program (root of the AST).
 */
struct ProgramNode : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements; ///< Program statements
    ProgramNode() { type = Type::PROGRAM; }
};

/**
 * @brief Represents a block of statements.
 */
struct BlockNode : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements; ///< Block statements
    BlockNode() { type = Type::BLOCK; }
};

/**
 * @brief Represents a variable declaration.
 */
struct LetNode : ASTNode {
    std::string name;                         ///< Variable name
    std::unique_ptr<ASTNode> value;           ///< Initial value
    LetNode() { type = Type::LET; }
};

/**
 * @brief Represents an if/else statement.
 */
struct IfNode : ASTNode {
    std::unique_ptr<ASTNode> condition;       ///< Condition expression
    std::unique_ptr<ASTNode> then_branch;     ///< Then branch
    std::unique_ptr<ASTNode> else_branch;     ///< Else branch (optional)
    IfNode() { type = Type::IF; }
};

/**
 * @brief Represents a while loop.
 */
struct WhileNode : ASTNode {
    std::unique_ptr<ASTNode> condition;       ///< Loop condition
    std::unique_ptr<ASTNode> body;            ///< Loop body
    WhileNode() { type = Type::WHILE; }
};

/**
 * @brief Represents a return statement.
 */
struct ReturnNode : ASTNode {
    std::unique_ptr<ASTNode> value;           ///< Return value
    ReturnNode() { type = Type::RETURN; }
};

/**
 * @brief Represents a literal value (number, string, bool, etc.).
 */
struct LiteralNode : ASTNode {
    /**
     * @brief Enumeration of literal types.
     */
    enum LiteralType { LIT_INT, LIT_FLOAT, LIT_STRING, LIT_BOOL, LIT_CHAR };
    LiteralType literal_type; ///< The literal type
    std::string value;        ///< The literal value as string
    LiteralNode() { type = Type::LITERAL; literal_type = LIT_INT; }
};

/**
 * @brief Represents a binary operation (e.g., x + y).
 */
struct BinaryOpNode : ASTNode {
    std::string op;                           ///< Operator string
    std::unique_ptr<ASTNode> left;            ///< Left operand
    std::unique_ptr<ASTNode> right;           ///< Right operand
    BinaryOpNode() { type = Type::BINARY_OP; }
};

/**
 * @brief Represents an identifier (variable or function name).
 */
struct IdentifierNode : ASTNode {
    std::string name;                         ///< Identifier name
    IdentifierNode() { type = Type::IDENTIFIER; }
};

/**
 * @brief Represents a print statement.
 */
struct PrintNode : ASTNode {
    std::unique_ptr<ASTNode> value;           ///< Value to print
    bool newline;                             ///< Whether to add a newline
    PrintNode(bool nl = true) : newline(nl) { type = Type::PRINT; }
};

// ── Lexer ──
/**
 * @brief Lexer for tokenizing source code.
 * 
 * The Lexer takes a string of source code and produces a stream of tokens.
 * It handles whitespace, comments, identifiers, numbers, strings, and symbols.
 * 
 * Example:
 * @code
 * Lexer lexer("let x = 42;");
 * auto tokens = lexer.tokenize();
 * @endcode
 */
class Lexer {
public:
    /**
     * @brief Construct a Lexer with the given source code.
     * @param source The source code string to tokenize.
     */
    Lexer(const std::string& source);
    ~Lexer() = default;
    
    /**
     * @brief Tokenize the source code.
     * @return A vector of tokens.
     */
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
/**
 * @brief Parser for building an Abstract Syntax Tree from tokens.
 * 
 * The Parser takes a vector of tokens and builds an AST that represents
 * the structure of the source code. It supports variables, if/else,
 * while loops, functions, return statements, and more.
 * 
 * Example:
 * @code
 * Parser parser(tokens);
 * auto ast = parser.parse();
 * @endcode
 */
class Parser {
public:
    /**
     * @brief Construct a Parser with the given tokens.
     * @param tokens The vector of tokens from the lexer.
     */
    Parser(const std::vector<Token>& tokens);
    ~Parser() = default;
    
    /**
     * @brief Parse the tokens and build an AST.
     * @return A unique pointer to the root AST node.
     */
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
    
    // ── Parsing functions ──
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
    
    /**
     * @brief Get the precedence of an operator token.
     * @param type The token type.
     * @return The precedence level (higher = tighter binding).
     */
    int getPrecedence(Token::Type type);
};

} // namespace guardian::parser
