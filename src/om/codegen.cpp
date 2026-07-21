#include "codegen.hpp"
#include "ast.hpp"
#include "opcodes.hpp"
#include <iostream>
#include <cstring>
#include <vector>

namespace om {

CodeGen::CodeGen() 
    : magic(0x44525547), version(1), flags(0) {}

bool CodeGen::generate(const std::unique_ptr<ProgramNode>& ast, const std::string& output_path) {
    std::cout << "  📦 Generating .gbin from AST: " << output_path << "\n";
    
    if (!ast) {
        std::cerr << "  ❌ AST is null!\n";
        return false;
    }
    
    std::vector<uint8_t> bytecode;
    
    for (const auto& stmt : ast->statements) {
        generateStatement(stmt, bytecode);
    }
    
    bytecode.push_back(static_cast<uint8_t>(Opcode::HALT));
    
    std::cout << "  📊 Generated " << bytecode.size() << " bytes of bytecode\n";
    
    std::ofstream out(output_path, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "Error: Could not write to " << output_path << "\n";
        return false;
    }
    
    if (!writeHeader(out, bytecode.size())) {
        return false;
    }
    
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
    if (!node) return;
    
    switch (node->type) {
        case NodeType::FUNCTION_DEF: {
            auto func = static_cast<FunctionDefNode*>(node.get());
            if (func->body) {
                generateStatement(func->body, bytecode);
            }
            break;
        }
        
        case NodeType::BLOCK: {
            auto block = static_cast<BlockNode*>(node.get());
            for (const auto& stmt : block->statements) {
                generateStatement(stmt, bytecode);
            }
            break;
        }
        
        case NodeType::VARIABLE_DEF: {
            auto var = static_cast<VariableDefNode*>(node.get());
            if (var->value) {
                // Generate code for the value (pushes onto stack)
                generateExpression(var->value, bytecode);
                // Store it in main molecule
                bytecode.push_back(static_cast<uint8_t>(Opcode::STORE));
                uint32_t name_len = var->name.length();
                bytecode.insert(bytecode.end(),
                              reinterpret_cast<uint8_t*>(&name_len),
                              reinterpret_cast<uint8_t*>(&name_len) + 4);
                bytecode.insert(bytecode.end(), var->name.begin(), var->name.end());
            }
            break;
        }
        
        case NodeType::CALL: {
            auto call = static_cast<CallNode*>(node.get());
            if (call->name == "println" || call->name == "print") {
                for (const auto& arg : call->args) {
                    generateExpression(arg, bytecode);
                }
                bytecode.push_back(static_cast<uint8_t>(
                    call->name == "println" ? Opcode::PRINTLN : Opcode::PRINT
                ));
            }
            break;
        }
        
        default:
            break;
    }
}

void CodeGen::generateExpression(const std::unique_ptr<ASTNode>& node, std::vector<uint8_t>& bytecode) {
    if (!node) return;
    
    switch (node->type) {
        case NodeType::LITERAL: {
            auto literal = static_cast<LiteralNode*>(node.get());
            
            if (literal->literal_type == LiteralNode::NUMBER) {
                if (literal->value.find('.') != std::string::npos) {
                    float value = std::stof(literal->value);
                    bytecode.push_back(static_cast<uint8_t>(Opcode::PUSH_FLOAT));
                    bytecode.insert(bytecode.end(), 
                                  reinterpret_cast<uint8_t*>(&value),
                                  reinterpret_cast<uint8_t*>(&value) + 4);
                } else {
                    int value = std::stoi(literal->value);
                    bytecode.push_back(static_cast<uint8_t>(Opcode::PUSH_INT));
                    bytecode.insert(bytecode.end(), 
                                  reinterpret_cast<uint8_t*>(&value),
                                  reinterpret_cast<uint8_t*>(&value) + 4);
                }
            } else if (literal->literal_type == LiteralNode::CHAR) {
                // CHAR literal - push as integer (ASCII value)
                char value = literal->value[0];
                bytecode.push_back(static_cast<uint8_t>(Opcode::PUSH_INT));
                int int_val = static_cast<int>(value);
                bytecode.insert(bytecode.end(),
                              reinterpret_cast<uint8_t*>(&int_val),
                              reinterpret_cast<uint8_t*>(&int_val) + 4);
            } else if (literal->literal_type == LiteralNode::BOOLEAN) {
                bool value = literal->value == "true";
                bytecode.push_back(static_cast<uint8_t>(Opcode::PUSH_BOOL));
                bytecode.push_back(value ? 1 : 0);
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
            // Load from main molecule
            bytecode.push_back(static_cast<uint8_t>(Opcode::LOAD));
            uint32_t name_len = ident->name.length();
            bytecode.insert(bytecode.end(),
                          reinterpret_cast<uint8_t*>(&name_len),
                          reinterpret_cast<uint8_t*>(&name_len) + 4);
            bytecode.insert(bytecode.end(), ident->name.begin(), ident->name.end());
            break;
        }
        
        case NodeType::BINARY_OP: {
            auto binary = static_cast<BinaryOpNode*>(node.get());
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
