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
        
        case NodeType::STRUCT_INSTANCE: {
            auto inst = static_cast<StructInstanceNode*>(node.get());
            
            // Create a molecule
            bytecode.push_back(static_cast<uint8_t>(Opcode::MAKE_MOLECULE));
            
            // Store each field as an atom in the molecule
            for (const auto& [field_name, field_value] : inst->fields) {
                generateExpression(field_value, bytecode, variables);
                bytecode.push_back(static_cast<uint8_t>(Opcode::STORE_ATOM));
                uint32_t name_len = field_name.length();
                bytecode.insert(bytecode.end(),
                               reinterpret_cast<uint8_t*>(&name_len),
                               reinterpret_cast<uint8_t*>(&name_len) + 4);
                bytecode.insert(bytecode.end(), field_name.begin(), field_name.end());
            }
            break;
        }

        case NodeType::DICT_LITERAL: {
            auto dict = static_cast<DictLiteralNode*>(node.get());
            
            // Push key count
            int count = dict->pairs.size();
            bytecode.push_back(static_cast<uint8_t>(Opcode::PUSH_INT));
            bytecode.insert(bytecode.end(),
                           reinterpret_cast<uint8_t*>(&count),
                           reinterpret_cast<uint8_t*>(&count) + 4);
            
            // Push each key-value pair
            for (const auto& [key, value] : dict->pairs) {
                // Push key
                bytecode.push_back(static_cast<uint8_t>(Opcode::PUSH_STRING));
                uint32_t key_len = key.length();
                bytecode.insert(bytecode.end(),
                               reinterpret_cast<uint8_t*>(&key_len),
                               reinterpret_cast<uint8_t*>(&key_len) + 4);
                bytecode.insert(bytecode.end(), key.begin(), key.end());
                
                // Push value
                generateExpression(value, bytecode, variables);
            }
            
            // Make dictionary
            bytecode.push_back(static_cast<uint8_t>(Opcode::MAKE_DICT));
            break;
        }

        case NodeType::DICT_ACCESS: {
            auto access = static_cast<DictAccessNode*>(node.get());
            
            // Load the dictionary
            bytecode.push_back(static_cast<uint8_t>(Opcode::LOAD));
            uint32_t name_len = access->name.length();
            bytecode.insert(bytecode.end(),
                           reinterpret_cast<uint8_t*>(&name_len),
                           reinterpret_cast<uint8_t*>(&name_len) + 4);
            bytecode.insert(bytecode.end(), access->name.begin(), access->name.end());
            
            // Push the key
            generateExpression(access->key, bytecode, variables);
            
            // Get from dictionary
            bytecode.push_back(static_cast<uint8_t>(Opcode::DICT_GET));
            break;
        }

        case NodeType::FIELD_ACCESS: {
            auto access = static_cast<FieldAccessNode*>(node.get());
            
            if (access->is_array_field_access && access->array_access) {
                // Handle arr[index]:field
                // First, get the array element (which is a struct pointer)
                generateExpression(access->array_access, bytecode, variables);
                // Then get the field from the struct
                bytecode.push_back(static_cast<uint8_t>(Opcode::GET_ATOM));
                uint32_t field_len = access->field_name.length();
                bytecode.insert(bytecode.end(),
                               reinterpret_cast<uint8_t*>(&field_len),
                               reinterpret_cast<uint8_t*>(&field_len) + 4);
                bytecode.insert(bytecode.end(), access->field_name.begin(), access->field_name.end());
            } else {
                // Regular field access: struct:field
                bytecode.push_back(static_cast<uint8_t>(Opcode::LOAD));
                uint32_t name_len = access->struct_name.length();
                bytecode.insert(bytecode.end(),
                               reinterpret_cast<uint8_t*>(&name_len),
                               reinterpret_cast<uint8_t*>(&name_len) + 4);
                bytecode.insert(bytecode.end(), access->struct_name.begin(), access->struct_name.end());
                
                bytecode.push_back(static_cast<uint8_t>(Opcode::GET_ATOM));
                uint32_t field_len = access->field_name.length();
                bytecode.insert(bytecode.end(),
                               reinterpret_cast<uint8_t*>(&field_len),
                               reinterpret_cast<uint8_t*>(&field_len) + 4);
                bytecode.insert(bytecode.end(), access->field_name.begin(), access->field_name.end());
            }
            break;
        }
        
        case NodeType::ARRAY_DECL: {
            auto decl = static_cast<ArrayDeclNode*>(node.get());
            
            generateExpression(decl->size, bytecode, variables);
            
            uint8_t type_code = 0;
            if (decl->element_type == "int") type_code = 1;
            else if (decl->element_type == "float") type_code = 2;
            else if (decl->element_type == "bool") type_code = 3;
            else if (decl->element_type == "char") type_code = 4;
            else if (decl->element_type == "string") type_code = 5;
            else type_code = 0;
            
            bytecode.push_back(static_cast<uint8_t>(Opcode::MAKE_ARRAY));
            bytecode.push_back(type_code);
            
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
            
            generateExpression(assign->value, bytecode, variables);
            generateExpression(assign->access, bytecode, variables);
            
            bytecode.push_back(static_cast<uint8_t>(Opcode::ARRAY_SET));
            break;
        }
        
        case NodeType::CALL: {
            auto call = static_cast<CallNode*>(node.get());
            std::cout << "  [DEBUG] Codegen: CALL " << call->name << " with " << call->args.size() << " args\n";
            
            if (call->name == "println" || call->name == "print") {
                for (const auto& arg : call->args) {
                    std::cout << "  [DEBUG] Codegen: generating arg of type " << (int)arg->type << "\n";
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
        
        case NodeType::STRUCT_INSTANCE: {
            auto inst = static_cast<StructInstanceNode*>(node.get());
            
            bytecode.push_back(static_cast<uint8_t>(Opcode::MAKE_MOLECULE));
            
            for (const auto& [field_name, field_value] : inst->fields) {
                generateExpression(field_value, bytecode, variables);
                bytecode.push_back(static_cast<uint8_t>(Opcode::STORE_ATOM));
                uint32_t name_len = field_name.length();
                bytecode.insert(bytecode.end(),
                               reinterpret_cast<uint8_t*>(&name_len),
                               reinterpret_cast<uint8_t*>(&name_len) + 4);
                bytecode.insert(bytecode.end(), field_name.begin(), field_name.end());
            }
            break;
        } 
        
        case NodeType::ARRAY_ACCESS: {
            auto access = static_cast<ArrayAccessNode*>(node.get());
            
            bytecode.push_back(static_cast<uint8_t>(Opcode::LOAD));
            uint32_t name_len = access->name.length();
            bytecode.insert(bytecode.end(),
                           reinterpret_cast<uint8_t*>(&name_len),
                           reinterpret_cast<uint8_t*>(&name_len) + 4);
            bytecode.insert(bytecode.end(), access->name.begin(), access->name.end());
            
            generateExpression(access->index, bytecode, variables);
            
            bytecode.push_back(static_cast<uint8_t>(Opcode::ARRAY_GET));
            break;
        }
        
        case NodeType::TYPED_ARRAY_LITERAL: {
            auto arr = static_cast<TypedArrayLiteralNode*>(node.get());
            
            for (const auto& element : arr->elements) {
                generateExpression(element, bytecode, variables);
            }
            
            int length = arr->elements.size();
            bytecode.push_back(static_cast<uint8_t>(Opcode::PUSH_INT));
            bytecode.insert(bytecode.end(),
                           reinterpret_cast<uint8_t*>(&length),
                           reinterpret_cast<uint8_t*>(&length) + 4);
            
            uint8_t type_code = 0;
            if (arr->element_type == "int") type_code = 1;
            else if (arr->element_type == "float") type_code = 2;
            else if (arr->element_type == "bool") type_code = 3;
            else if (arr->element_type == "char") type_code = 4;
            else if (arr->element_type == "string") type_code = 5;
            else type_code = 0;
            
            bytecode.push_back(static_cast<uint8_t>(Opcode::MAKE_ARRAY));
            bytecode.push_back(type_code);
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
        

        case NodeType::FIELD_ACCESS: {
            auto access = static_cast<FieldAccessNode*>(node.get());
            std::cout << "  [DEBUG] Codegen: FIELD_ACCESS " << access->struct_name << ":" << access->field_name << "\n";
            
            // Load the struct
            bytecode.push_back(static_cast<uint8_t>(Opcode::LOAD));
            uint32_t name_len = access->struct_name.length();
            bytecode.insert(bytecode.end(),
                           reinterpret_cast<uint8_t*>(&name_len),
                           reinterpret_cast<uint8_t*>(&name_len) + 4);
            bytecode.insert(bytecode.end(), access->struct_name.begin(), access->struct_name.end());
            
            // Get the field
            bytecode.push_back(static_cast<uint8_t>(Opcode::GET_ATOM));
            uint32_t field_len = access->field_name.length();
            bytecode.insert(bytecode.end(),
                           reinterpret_cast<uint8_t*>(&field_len),
                           reinterpret_cast<uint8_t*>(&field_len) + 4);
            bytecode.insert(bytecode.end(), access->field_name.begin(), access->field_name.end());
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
