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
    
    std::vector<uint8_t> bytecode;
    std::unordered_map<std::string, bool> variables;
    
    for (const auto& stmt : ast->statements) {
        generateStatement(stmt, bytecode, variables);
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

void CodeGen::generateStatement(const std::unique_ptr<ASTNode>& node, 
                                std::vector<uint8_t>& bytecode,
                                std::unordered_map<std::string, bool>& variables) {
    if (!node) return;
    
    switch (node->type) {
        case NodeType::FUNCTION_DEF: {
            auto func = static_cast<FunctionDefNode*>(node.get());
            if (func->body) {
                generateStatement(func->body, bytecode, variables);
            }
            break;
        }
        
        case NodeType::BLOCK: {
            auto block = static_cast<BlockNode*>(node.get());
            for (const auto& stmt : block->statements) {
                generateStatement(stmt, bytecode, variables);
            }
            break;
        }
        
        case NodeType::VARIABLE_DEF: {
            auto var = static_cast<VariableDefNode*>(node.get());
            if (var->value) {
                generateExpression(var->value, bytecode, variables);
                bytecode.push_back(static_cast<uint8_t>(Opcode::STORE));
                uint32_t name_len = var->name.length();
                bytecode.insert(bytecode.end(),
                              reinterpret_cast<uint8_t*>(&name_len),
                              reinterpret_cast<uint8_t*>(&name_len) + 4);
                bytecode.insert(bytecode.end(), var->name.begin(), var->name.end());
                variables[var->name] = true;
            }
            break;
        }
        
        case NodeType::ARRAY_DECL: {
            auto decl = static_cast<ArrayDeclNode*>(node.get());
            
            // Generate size (pushes size onto stack)
            generateExpression(decl->size, bytecode, variables);
            
            // Type code as a separate byte in the instruction stream (not on stack)
            uint8_t type_code = 0;
            if (decl->element_type == "int") type_code = 1;
            else if (decl->element_type == "float") type_code = 2;
            else if (decl->element_type == "bool") type_code = 3;
            else if (decl->element_type == "char") type_code = 4;
            else if (decl->element_type == "string") type_code = 5;
            else type_code = 0; // any
            
            // MAKE_ARRAY instruction takes: [size, type_code]
            // Format: MAKE_ARRAY <type_code>
            bytecode.push_back(static_cast<uint8_t>(Opcode::MAKE_ARRAY));
            bytecode.push_back(type_code);  // Type code as operand
            
            // Store array
            bytecode.push_back(static_cast<uint8_t>(Opcode::STORE));
            uint32_t name_len = decl->name.length();
            bytecode.insert(bytecode.end(),
                           reinterpret_cast<uint8_t*>(&name_len),
                           reinterpret_cast<uint8_t*>(&name_len) + 4);
            bytecode.insert(bytecode.end(), decl->name.begin(), decl->name.end());
            
            variables[decl->name] = true;
            break;
        }
            
        case NodeType::ARRAY_ASSIGN: {
            auto assign = static_cast<ArrayAssignNode*>(node.get());
            
            // Generate value
            generateExpression(assign->value, bytecode, variables);
            
            // Generate array access (this pushes array and index)
            generateExpression(assign->access, bytecode, variables);
            
            // Use ARRAY_SET
            bytecode.push_back(static_cast<uint8_t>(Opcode::ARRAY_SET));
            break;
        }
        
        case NodeType::CALL: {
            auto call = static_cast<CallNode*>(node.get());
            if (call->name == "println" || call->name == "print") {
                for (const auto& arg : call->args) {
                    generateExpression(arg, bytecode, variables);
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

void CodeGen::generateExpression(const std::unique_ptr<ASTNode>& node, 
                                 std::vector<uint8_t>& bytecode,
                                 std::unordered_map<std::string, bool>& variables) {
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
            } else if (literal->literal_type == LiteralNode::BOOLEAN) {
                bool value = literal->value == "true";
                bytecode.push_back(static_cast<uint8_t>(Opcode::PUSH_BOOL));
                bytecode.push_back(value ? 1 : 0);
            } else if (literal->literal_type == LiteralNode::CHAR) {
                std::string value = "char:" + literal->value;
                bytecode.push_back(static_cast<uint8_t>(Opcode::PUSH_STRING));
                uint32_t len = value.length();
                bytecode.insert(bytecode.end(),
                              reinterpret_cast<uint8_t*>(&len),
                              reinterpret_cast<uint8_t*>(&len) + 4);
                bytecode.insert(bytecode.end(), value.begin(), value.end());
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
            bytecode.push_back(static_cast<uint8_t>(Opcode::LOAD));
            uint32_t name_len = ident->name.length();
            bytecode.insert(bytecode.end(),
                          reinterpret_cast<uint8_t*>(&name_len),
                          reinterpret_cast<uint8_t*>(&name_len) + 4);
            bytecode.insert(bytecode.end(), ident->name.begin(), ident->name.end());
            break;
        }
        
        case NodeType::ARRAY_ACCESS: {
            auto access = static_cast<ArrayAccessNode*>(node.get());
            
            // Load array
            bytecode.push_back(static_cast<uint8_t>(Opcode::LOAD));
            uint32_t name_len = access->name.length();
            bytecode.insert(bytecode.end(),
                           reinterpret_cast<uint8_t*>(&name_len),
                           reinterpret_cast<uint8_t*>(&name_len) + 4);
            bytecode.insert(bytecode.end(), access->name.begin(), access->name.end());
            
            // Push index
            generateExpression(access->index, bytecode, variables);
            
            // Get array element
            bytecode.push_back(static_cast<uint8_t>(Opcode::ARRAY_GET));
            break;
        }
        
        case NodeType::TYPED_ARRAY_LITERAL: {
            auto arr = static_cast<TypedArrayLiteralNode*>(node.get());
            
            // Push each element
            for (const auto& element : arr->elements) {
                generateExpression(element, bytecode, variables);
            }
            
            // Push length
            int length = arr->elements.size();
            bytecode.push_back(static_cast<uint8_t>(Opcode::PUSH_INT));
            bytecode.insert(bytecode.end(),
                           reinterpret_cast<uint8_t*>(&length),
                           reinterpret_cast<uint8_t*>(&length) + 4);
            
            // Type code as OPERAND (not on stack)
            uint8_t type_code = 0;
            if (arr->element_type == "int") type_code = 1;
            else if (arr->element_type == "float") type_code = 2;
            else if (arr->element_type == "bool") type_code = 3;
            else if (arr->element_type == "char") type_code = 4;
            else if (arr->element_type == "string") type_code = 5;
            else type_code = 0; // any
            
            // MAKE_ARRAY with type code as operand
            bytecode.push_back(static_cast<uint8_t>(Opcode::MAKE_ARRAY));
            bytecode.push_back(type_code);  // Operand
            break;
        }

        case NodeType::BINARY_OP: {
            auto binary = static_cast<BinaryOpNode*>(node.get());
            generateExpression(binary->left, bytecode, variables);
            generateExpression(binary->right, bytecode, variables);
            
            if (binary->op == "+") {
                bytecode.push_back(static_cast<uint8_t>(Opcode::ADD));
            } else if (binary->op == "-") {
                bytecode.push_back(static_cast<uint8_t>(Opcode::SUB));
            } else if (binary->op == "*") {
                bytecode.push_back(static_cast<uint8_t>(Opcode::MUL));
            } else if (binary->op == "/") {
                bytecode.push_back(static_cast<uint8_t>(Opcode::DIV));
            } else if (binary->op == "%") {
                bytecode.push_back(static_cast<uint8_t>(Opcode::MOD));
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
