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
    ARRAY_DECL,
    ARRAY_ACCESS,
    ARRAY_LITERAL,
    ARRAY_ASSIGN,
    TYPED_ARRAY_LITERAL,
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

// Array assignment: arr[0] = 42;
struct ArrayAssignNode : ASTNode {
    std::unique_ptr<ASTNode> access;
    std::unique_ptr<ASTNode> value;
    ArrayAssignNode() { type = NodeType::ARRAY_ASSIGN; }
};

// Identifier
struct IdentifierNode : ASTNode {
    std::string name;
    IdentifierNode() { type = NodeType::IDENTIFIER; }
};

// Literal
struct LiteralNode : ASTNode {
    enum Type { STRING, NUMBER, BOOLEAN, NULL_TOKEN, CHAR, };
    Type literal_type;
    std::string value;
    LiteralNode() { type = NodeType::LITERAL; }
};

// Array declaration: let arr: int[10];
struct ArrayDeclNode : ASTNode {
    std::string name;
    std::string element_type;
    std::unique_ptr<ASTNode> size;
    ArrayDeclNode() { type = NodeType::ARRAY_DECL; }
};

// Array access: arr[0]
struct ArrayAccessNode : ASTNode {
    std::string name;
    std::unique_ptr<ASTNode> index;
    ArrayAccessNode() { type = NodeType::ARRAY_ACCESS; }
};

// Array literal: [1, 2, 3]
struct ArrayLiteralNode : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> elements;
    ArrayLiteralNode() { type = NodeType::ARRAY_LITERAL; }
};

// Typed array literal: [int: 1, 2, 3]
struct TypedArrayLiteralNode : ASTNode {
    std::string element_type;
    std::vector<std::unique_ptr<ASTNode>> elements;
    TypedArrayLiteralNode() { type = NodeType::TYPED_ARRAY_LITERAL; }
};

} // namespace om
