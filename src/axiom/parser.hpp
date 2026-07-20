#pragma once
#include "../compiler/lexer.hpp"
#include "../core/atom.hpp"
#include <vector>
#include <memory>
#include <string>

namespace axiom {

// ============================================
// BUILTIN FUNCTIONS
// ============================================
enum class Builtin {
    PRINT,
    PRINTLN,
    LEN,
    NONE
};

// AST nodes
struct ASTNode {
    virtual ~ASTNode() = default;
    virtual std::unique_ptr<guardian::Atom> to_atom() const = 0;
};

// Add BuiltinCall node
struct BuiltinCall : ASTNode {
    Builtin name;
    std::vector<std::unique_ptr<ASTNode>> args;
    std::unique_ptr<guardian::Atom> to_atom() const override;
};

struct Program : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> functions;
    std::unique_ptr<guardian::Atom> to_atom() const override;
};

struct FunctionDef : ASTNode {
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> body;
    std::unique_ptr<guardian::Atom> to_atom() const override;
};

struct LetStmt : ASTNode {
    std::string name;
    std::unique_ptr<ASTNode> value;
    std::unique_ptr<guardian::Atom> to_atom() const override;
};

struct BinaryExpr : ASTNode {
    guardian::compiler::TokenType op; // Reuse Guardian's token types
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    std::unique_ptr<guardian::Atom> to_atom() const override;
};

struct LiteralExpr : ASTNode {
    guardian::compiler::TokenType type;
    std::string value;
    std::unique_ptr<guardian::Atom> to_atom() const override;
};

struct CallExpr : ASTNode {
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> args;
    std::unique_ptr<guardian::Atom> to_atom() const override;
};

struct ReturnStmt : ASTNode {
    std::unique_ptr<ASTNode> value;
    std::unique_ptr<guardian::Atom> to_atom() const override;
};

class Parser {
private:
    std::vector<guardian::compiler::Token> tokens;
    size_t pos = 0;
    
    guardian::compiler::Token peek() const;
    guardian::compiler::Token advance();
    bool match(guardian::compiler::TokenType type);
    bool expect(guardian::compiler::TokenType type);
    void synchronize();
    
    std::unique_ptr<Program> parse_program();
    std::unique_ptr<FunctionDef> parse_function();
    std::unique_ptr<ASTNode> parse_statement();
    std::unique_ptr<ASTNode> parse_let();
    std::unique_ptr<ASTNode> parse_return();
    std::unique_ptr<ASTNode> parse_expression();
    std::unique_ptr<ASTNode> parse_primary();
    std::unique_ptr<ASTNode> parse_call();
    
public:
    Parser(const std::vector<guardian::compiler::Token>& tokens);
    std::unique_ptr<Program> parse();
};

} // namespace axiom
