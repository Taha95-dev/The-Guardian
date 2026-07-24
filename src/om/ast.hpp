#pragma once

#include <string>
#include <vector>
#include <memory>

namespace om {

enum class NodeType {
    PROGRAM,
    FUNCTION_DEF,
    VARIABLE_DEF,
    BLOCK,
    BINARY_OP,
    LITERAL,
    IDENTIFIER,
    CALL,
};

struct ASTNode {
    NodeType type;
    int line;
    int column;
    virtual ~ASTNode() = default;
};

struct ProgramNode : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
    ProgramNode() { type = NodeType::PROGRAM; }
};

struct FunctionDefNode : ASTNode {
    std::string name;
    std::vector<std::string> params;
    std::unique_ptr<ASTNode> body;
    FunctionDefNode() { type = NodeType::FUNCTION_DEF; }
};

struct VariableDefNode : ASTNode {
    std::string name;
    std::unique_ptr<ASTNode> value;
    bool is_const;
    VariableDefNode() : is_const(false) { type = NodeType::VARIABLE_DEF; }
};

struct BlockNode : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> statements;
    BlockNode() { type = NodeType::BLOCK; }
};

struct BinaryOpNode : ASTNode {
    std::string op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    BinaryOpNode() { type = NodeType::BINARY_OP; }
};

struct LiteralNode : ASTNode {
    enum Type { NUMBER, STRING, BOOLEAN };
    Type literal_type;
    std::string value;
    LiteralNode() { type = NodeType::LITERAL; }
};

struct IdentifierNode : ASTNode {
    std::string name;
    IdentifierNode() { type = NodeType::IDENTIFIER; }
};

struct CallNode : ASTNode {
    std::string name;
    std::vector<std::unique_ptr<ASTNode>> args;
    CallNode() { type = NodeType::CALL; }
};

} // namespace om
