#include "codegen.hpp"
#include "ast.hpp"
#include "opcodes.hpp"
#include <iostream>
#include <cstring>
#include <vector>
#include <unordered_map>

namespace om {

CodeGen::CodeGen() 
    : magic(0x44525547), version(1), flags(0) {}

bool CodeGen::generate(const std::unique_ptr<ProgramNode>& ast, const std::string& output_path) {
    std::cout << "  📦 Generating .gbin from AST: " << output_path << "\n";
    
    if (!ast) {
        std::cerr << "  ❌ AST is null!\n";
        return false;
    }
    
    std::cout << "  📊 AST has " << ast->statements.size() << " statements\n";
    
    std::vector<uint8_t> bytecode;
    
    // Generate bytecode for each statement
    for (const auto& stmt : ast->statements) {
        std::cout << "  📝 Generating code for statement\n";
        generateStatement(stmt, bytecode);
    }
    
    // Add HALT instruction
    bytecode.push_back(static_cast<uint8_t>(Opcode::HALT));
    
    std::cout << "  📊 Generated " << bytecode.size() << " bytes of bytecode\n";
    
    // Write to file
    std::ofstream out(output_path, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "Error: Could not write to " << output_path << "\n";
        return false;
    }
    
    // Write header
    if (!writeHeader(out, bytecode.size())) {
        return false;
    }
    
    // Write bytecode
    out.write(reinterpret_cast<const char*>(bytecode.data()), bytecode.size());
    if (!out.good()) {
        std::cerr << "Error: Failed to write bytecode\n";
        return false;
    }
    
    out.close();
    std::cout << "  ✅ Wrote " << bytecode.size() << " bytes of bytecode\n";
    return true;
}

void CodeGen::generateStatement(const std::unique_ptr<ASTNode>& node, std::vector<uint8_t>& bytecode) {
    if (!node) {
        std::cout << "  ⚠️ generateStatement: node is null\n";
        return;
    }
    
    std::cout << "  🔧 generateStatement: node type = " << (int)node->type << "\n";
    
    switch (node->type) {
        case NodeType::FUNCTION_DEF: {
            auto func = static_cast<FunctionDefNode*>(node.get());
            std::cout << "  📝 Function: " << func->name << "\n";
            if (func->body) {
                generateStatement(func->body, bytecode);
            } else {
                std::cout << "  ⚠️ Function body is null\n";
            }
            break;
        }
        
        case NodeType::BLOCK: {
            auto block = static_cast<BlockNode*>(node.get());
            std::cout << "  📝 Block with " << block->statements.size() << " statements\n";
            for (const auto& stmt : block->statements) {
                generateStatement(stmt, bytecode);
            }
            break;
        }
        
        case NodeType::VARIABLE_DEF: {
            auto var = static_cast<VariableDefNode*>(node.get());
            std::cout << "  📝 Variable: " << var->name << "\n";
            if (var->value) {
                generateExpression(var->value, bytecode);
            }
            break;
        }
        
        case NodeType::CALL: {
            auto call = static_cast<CallNode*>(node.get());
            std::cout << "  📝 Call: " << call->name << " with " << call->args.size() << " args\n";
            if (call->name == "println" || call->name == "print") {
                for (const auto& arg : call->args) {
                    generateExpression(arg, bytecode);
                }
                if (call->name == "println") {
                    bytecode.push_back(static_cast<uint8_t>(Opcode::PRINTLN));
                } else {
                    bytecode.push_back(static_cast<uint8_t>(Opcode::PRINT));
                }
            }
            break;
        }
        
        default:
            std::cout << "  ⚠️ Unknown statement type: " << (int)node->type << "\n";
            break;
    }
}

void CodeGen::generateExpression(const std::unique_ptr<ASTNode>& node, std::vector<uint8_t>& bytecode) {
    if (!node) {
        std::cout << "  ⚠️ generateExpression: node is null\n";
        return;
    }
    
    std::cout << "  🔧 generateExpression: node type = " << (int)node->type << "\n";
    
    switch (node->type) {
        case NodeType::LITERAL: {
            auto literal = static_cast<LiteralNode*>(node.get());
            std::cout << "  📝 Literal: " << literal->value << "\n";
            if (literal->literal_type == LiteralNode::NUMBER) {
                int value = std::stoi(literal->value);
                bytecode.push_back(static_cast<uint8_t>(Opcode::PUSH_INT));
                bytecode.insert(bytecode.end(), 
                              reinterpret_cast<uint8_t*>(&value),
                              reinterpret_cast<uint8_t*>(&value) + 4);
            } else if (literal->literal_type == LiteralNode::STRING) {
                bytecode.push_back(static_cast<uint8_t>(Opcode::PUSH_STRING));
                uint32_t len = literal->value.length();
                bytecode.insert(bytecode.end(),
                              reinterpret_cast<uint8_t*>(&len),
                              reinterpret_cast<uint8_t*>(&len) + 4);
                bytecode.insert(bytecode.end(), 
                              literal->value.begin(), 
                              literal->value.end());
            }
            break;
        }
        
        case NodeType::IDENTIFIER: {
            auto ident = static_cast<IdentifierNode*>(node.get());
            std::cout << "  📝 Identifier: " << ident->name << "\n";
            // TODO: Load variable from environment
            // For now, push placeholder value
            int placeholder = 0;
            bytecode.push_back(static_cast<uint8_t>(Opcode::PUSH_INT));
            bytecode.insert(bytecode.end(),
                          reinterpret_cast<uint8_t*>(&placeholder),
                          reinterpret_cast<uint8_t*>(&placeholder) + 4);
            break;
        }
        
        case NodeType::BINARY_OP: {
            auto binary = static_cast<BinaryOpNode*>(node.get());
            std::cout << "  📝 Binary op: " << binary->op << "\n";
            generateExpression(binary->left, bytecode);
            generateExpression(binary->right, bytecode);
            
            if (binary->op == "+") {
                bytecode.push_back(static_cast<uint8_t>(Opcode::ADD));
            } else if (binary->op == "-") {
                bytecode.push_back(static_cast<uint8_t>(Opcode::SUB));
            } else if (binary->op == "*") {
                bytecode.push_back(static_cast<uint8_t>(Opcode::MUL));
            } else if (binary->op == "/") {
                bytecode.push_back(static_cast<uint8_t>(Opcode::DIV));
            }
            break;
        }
        
        default:
            std::cout << "  ⚠️ Unknown expression type: " << (int)node->type << "\n";
            break;
    }
}

bool CodeGen::writeHeader(std::ofstream& out, uint32_t bytecode_size) {
    GuardianHeader header;
    header.magic = magic;
    header.version = version;
    header.flags = flags;
    header.entry_point = sizeof(GuardianHeader);
    header.instruction_count = bytecode_size;
    memset(header.reserved, 0, 8);
    
    out.write(reinterpret_cast<const char*>(&header), sizeof(header));
    return out.good();
}

} // namespace om
