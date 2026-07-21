#pragma once

#include <string>
#include <vector>
#include <memory>
#include <variant>

namespace om {

// Forward declarations
struct ProgramNode;
struct FunctionDefNode;
struct VariableDefNode;
struct AssignmentNode;
struct IfStatementNode;
struct ForLoopNode;
struct WhileLoopNode;
struct ReturnStatementNode;
struct BlockNode;
struct BinaryOpNode;
struct UnaryOpNode;
struct CallNode;
struct IdentifierNode;
struct LiteralNode;

// AST Node types
enum class NodeType {
    PROGRAM,
    FUNCTION_DEF,
    VARIABLE_DEF,
    ASSIGNMENT,
    IF_STATEMENT,
    FOR_LOOP,
    WHILE_LOOP,
    RETURN_STATEMENT,
    BLOCK,
    BINARY_OP,
    UNARY_OP,
    CALL,
    IDENTIFIER,
    LITERAL
};

// Base AST Node
struct ASTNode {
    NodeType type;
    int line;
    int column;
    virtual ~ASTNode() = default;
};

// Program (root node)
struct ProgramNode : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
    ProgramNode() { type = NodeType::PROGRAM; }
};

// Function definition
struct FunctionDefNode : ASTNode {
    std::string name;
    std::vector<std::string> params;
    std::unique_ptr<ASTNode> body;
    FunctionDefNode() { type = NodeType::FUNCTION_DEF; }
};

// Variable definition
struct VariableDefNode : ASTNode {
    std::string name;
    std::unique_ptr<ASTNode> value;
    bool is_const;
    VariableDefNode() : is_const(false) { type = NodeType::VARIABLE_DEF; }
};

// Assignment
struct AssignmentNode : ASTNode {
    std::string name;
    std::unique_ptr<ASTNode> value;
    AssignmentNode() { type = NodeType::ASSIGNMENT; }
};

// If statement
struct IfStatementNode : ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> then_branch;
    std::unique_ptr<ASTNode> else_branch;
    IfStatementNode() { type = NodeType::IF_STATEMENT; }
};

// For loop
struct ForLoopNode : ASTNode {
    std::unique_ptr<ASTNode> init;
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> increment;
    std::unique_ptr<ASTNode> body;
    ForLoopNode() { type = NodeType::FOR_LOOP; }
};

// While loop
struct WhileLoopNode : ASTNode {
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> body;
    WhileLoopNode() { type = NodeType::WHILE_LOOP; }
};

// Return statement
struct ReturnStatementNode : ASTNode {
    std::unique_ptr<ASTNode> value;
    ReturnStatementNode() { type = NodeType::RETURN_STATEMENT; }
};

// Block (scope)
struct BlockNode : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
    BlockNode() { type = NodeType::BLOCK; }
};

// Binary operation
struct BinaryOpNode : ASTNode {
    std::string op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    BinaryOpNode() { type = NodeType::BINARY_OP; }
};

// Unary operation
struct UnaryOpNode : ASTNode {
    std::string op;
    std::unique_ptr<ASTNode> operand;
    UnaryOpNode() { type = NodeType::UNARY_OP; }
};

// Function call
struct CallNode : ASTNode {
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> args;
    CallNode() { type = NodeType::CALL; }
};

// Identifier
struct IdentifierNode : ASTNode {
    std::string name;
    IdentifierNode() { type = NodeType::IDENTIFIER; }
};

// Literal
struct LiteralNode : ASTNode {
    enum Type { STRING, NUMBER, BOOLEAN, NULL_TOKEN };
    Type literal_type;
    std::string value;
    LiteralNode() { type = NodeType::LITERAL; }
};

} // namespace om
