#include "vm.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

// Core atom includes
#include "../core/atom.hpp"  // For AtomType
#include "../atoms/primitive/int_atom.hpp"
#include "../atoms/primitive/float_atom.hpp"
#include "../atoms/text/string_atom.hpp"
#include "../core/molecule.hpp"

namespace guardian::vm {

void VM::push_value(const Value& value) {
    stack.push(value);
}

Value VM::pop_value() {
    if (stack.empty()) {
        throw std::runtime_error("Stack underflow");
    }
    Value top = stack.top();
    stack.pop();
    return top;
}

Value VM::peek_value() const {
    if (stack.empty()) {
        throw std::runtime_error("Stack empty");
    }
    return stack.top();
}

void VM::load_bytecode(const Bytecode& code) {
    bytecode = code;
    pc = 0;
    running = false;
}

void VM::load_bytecode(const Bytecode& code, const std::vector<Value>& consts) {
    load_bytecode(code);
    constants = consts;
}

void VM::run() {
    running = true;
    while (running && pc < bytecode.size()) {
        step();
    }
}

void VM::step() {
    if (pc >= bytecode.size() || !running) {
        running = false;
        return;
    }
    
    const Instruction& instr = bytecode[pc];
    pc++;
    
    switch (instr.opcode) {
        case OpCode::NOP:
            break;
            
        case OpCode::PUSH: {
            if (instr.operands.size() >= 1) {
                int64_t val = static_cast<int64_t>(instr.operands[0]);
                push_value(val);
            }
            break;
        }
            
        case OpCode::POP: {
            if (!stack.empty()) {
                stack.pop();
            }
            break;
        }
            
        case OpCode::DUP: {
            if (!stack.empty()) {
                Value top = stack.top();
                push_value(top);
            }
            break;
        }
            
        case OpCode::SWAP: {
            if (stack.size() >= 2) {
                Value a = pop_value();
                Value b = pop_value();
                push_value(a);
                push_value(b);
            }
            break;
        }
            
        case OpCode::ADD:
        case OpCode::SUB:
        case OpCode::MUL:
        case OpCode::DIV:
        case OpCode::MOD:
            binary_op(instr.opcode);
            break;
            
        case OpCode::CMP_EQ:
        case OpCode::CMP_NE:
        case OpCode::CMP_LT:
        case OpCode::CMP_GT:
        case OpCode::CMP_LE:
        case OpCode::CMP_GE:
            comparison_op(instr.opcode);
            break;
            
        case OpCode::PRINT: {
            if (!stack.empty()) {
                Value val = pop_value();
                std::cout << "PRINT: ";
                if (std::holds_alternative<int64_t>(val)) {
                    std::cout << std::get<int64_t>(val);
                } else if (std::holds_alternative<double>(val)) {
                    std::cout << std::get<double>(val);
                } else if (std::holds_alternative<bool>(val)) {
                    std::cout << (std::get<bool>(val) ? "true" : "false");
                } else if (std::holds_alternative<std::string>(val)) {
                    std::cout << std::get<std::string>(val);
                } else {
                    std::cout << "?";
                }
                std::cout << std::endl;
            }
            break;
        }
            
        case OpCode::HALT:
            running = false;
            break;
            
        default:
            std::cerr << "Unknown opcode: " << static_cast<int>(instr.opcode) << std::endl;
            running = false;
            break;
    }
}

void VM::binary_op(OpCode op) {
    if (stack.size() < 2) return;
    
    Value b = pop_value();
    Value a = pop_value();
    
    if (std::holds_alternative<int64_t>(a) && std::holds_alternative<int64_t>(b)) {
        int64_t ia = std::get<int64_t>(a);
        int64_t ib = std::get<int64_t>(b);
        int64_t result = 0;
        
        switch (op) {
            case OpCode::ADD: result = ia + ib; break;
            case OpCode::SUB: result = ia - ib; break;
            case OpCode::MUL: result = ia * ib; break;
            case OpCode::DIV: if (ib != 0) result = ia / ib; break;
            case OpCode::MOD: if (ib != 0) result = ia % ib; break;
            default: break;
        }
        
        push_value(result);
    }
}

void VM::comparison_op(OpCode op) {
    if (stack.size() < 2) return;
    
    Value b = pop_value();
    Value a = pop_value();
    
    if (std::holds_alternative<int64_t>(a) && std::holds_alternative<int64_t>(b)) {
        int64_t ia = std::get<int64_t>(a);
        int64_t ib = std::get<int64_t>(b);
        bool result = false;
        
        switch (op) {
            case OpCode::CMP_EQ: result = ia == ib; break;
            case OpCode::CMP_NE: result = ia != ib; break;
            case OpCode::CMP_LT: result = ia < ib; break;
            case OpCode::CMP_GT: result = ia > ib; break;
            case OpCode::CMP_LE: result = ia <= ib; break;
            case OpCode::CMP_GE: result = ia >= ib; break;
            default: break;
        }
        
        push_value(result);
    }
}

void VM::reset() {
    pc = 0;
    while (!stack.empty()) stack.pop();
    while (!frames.empty()) frames.pop();
    globals.clear();
    constants.clear();
    running = false;
    debug_mode = false;
}

void VM::print_stack() const {
    std::cout << "Stack (" << stack.size() << " items): ";
    std::cout << "size = " << stack.size() << std::endl;
}

void VM::print_globals() const {
    std::cout << "Globals: " << globals.size() << " variables" << std::endl;
    for (const auto& pair : globals) {
        std::cout << "  " << pair.first << " = ";
        if (std::holds_alternative<int64_t>(pair.second)) {
            std::cout << std::get<int64_t>(pair.second);
        } else if (std::holds_alternative<std::string>(pair.second)) {
            std::cout << "\"" << std::get<std::string>(pair.second) << "\"";
        } else {
            std::cout << "?";
        }
        std::cout << std::endl;
    }
}

std::string opcode_to_string(OpCode opcode) {
    switch (opcode) {
        case OpCode::NOP: return "NOP";
        case OpCode::PUSH: return "PUSH";
        case OpCode::POP: return "POP";
        case OpCode::DUP: return "DUP";
        case OpCode::SWAP: return "SWAP";
        case OpCode::ADD: return "ADD";
        case OpCode::SUB: return "SUB";
        case OpCode::MUL: return "MUL";
        case OpCode::DIV: return "DIV";
        case OpCode::MOD: return "MOD";
        case OpCode::CMP_EQ: return "CMP_EQ";
        case OpCode::CMP_NE: return "CMP_NE";
        case OpCode::CMP_LT: return "CMP_LT";
        case OpCode::CMP_GT: return "CMP_GT";
        case OpCode::CMP_LE: return "CMP_LE";
        case OpCode::CMP_GE: return "CMP_GE";
        case OpCode::PRINT: return "PRINT";
        case OpCode::HALT: return "HALT";
        default: return "UNKNOWN";
    }
}

OpCode string_to_opcode(const std::string& str) {
    if (str == "NOP") return OpCode::NOP;
    if (str == "PUSH") return OpCode::PUSH;
    if (str == "POP") return OpCode::POP;
    if (str == "DUP") return OpCode::DUP;
    if (str == "SWAP") return OpCode::SWAP;
    if (str == "ADD") return OpCode::ADD;
    if (str == "SUB") return OpCode::SUB;
    if (str == "MUL") return OpCode::MUL;
    if (str == "DIV") return OpCode::DIV;
    if (str == "MOD") return OpCode::MOD;
    if (str == "CMP_EQ") return OpCode::CMP_EQ;
    if (str == "CMP_NE") return OpCode::CMP_NE;
    if (str == "CMP_LT") return OpCode::CMP_LT;
    if (str == "CMP_GT") return OpCode::CMP_GT;
    if (str == "CMP_LE") return OpCode::CMP_LE;
    if (str == "CMP_GE") return OpCode::CMP_GE;
    if (str == "PRINT") return OpCode::PRINT;
    if (str == "HALT") return OpCode::HALT;
    return OpCode::NOP;
}

// ============================================
// MOLECULE → BYTECODE CONVERSION
// NO dynamic_cast! Uses AtomType enum (10x faster)
// ============================================

Bytecode VM::molecule_to_bytecode(const Molecule& molecule) {
    Bytecode bytecode;
    
    for (const auto& atom : molecule.get_atoms()) {
        switch (atom->type()) {
            case AtomType::INT: {
                auto* int_atom = static_cast<const IntAtom*>(atom.get());
                int val = int_atom->get();
                bytecode.push_back(Instruction(OpCode::PUSH, {static_cast<uint64_t>(val)}));
                bytecode.push_back(Instruction(OpCode::PRINT));
                break;
            }
            case AtomType::FLOAT: {
                auto* float_atom = static_cast<const FloatAtom*>(atom.get());
                float val = float_atom->get();
                bytecode.push_back(Instruction(OpCode::PUSH, {static_cast<uint64_t>(val)}));
                bytecode.push_back(Instruction(OpCode::PRINT));
                break;
            }
            case AtomType::STRING: {
                bytecode.push_back(Instruction(OpCode::PRINT));
                break;
            }
            default:
                bytecode.push_back(Instruction(OpCode::NOP));
                break;
        }
    }
    
    bytecode.push_back(Instruction(OpCode::HALT));
    return bytecode;
}

void VM::load_molecule(const Molecule& molecule) {
    reset();
    Bytecode code = molecule_to_bytecode(molecule);
    load_bytecode(code);
    running = false;
}

// ============================================
// BINARY FORMAT LOADING
// ============================================

bool VM::load_binary_file(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return false;
    }
    
    std::vector<uint8_t> data(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );
    
    return load_binary(data);
}

bool VM::load_binary(const std::vector<uint8_t>& data) {
    auto* format = registry.get_format_by_magic(data);
    if (!format) {
        std::cerr << "Unknown binary format" << std::endl;
        return false;
    }
    
    return load_binary(data, format->name);
}

bool VM::load_binary(const std::vector<uint8_t>& data, const std::string& format_name) {
    auto* format = registry.get_format(format_name);
    if (!format) {
        std::cerr << "Format not found: " << format_name << std::endl;
        return false;
    }
    
    try {
        Bytecode code = format->parser(data);
        load_bytecode(code);
        binary_loaded = true;
        loaded_format = format_name;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse binary: " << e.what() << std::endl;
        return false;
    }
}

bool VM::save_binary_file(const std::string& path, const std::string& format_name) {
    auto* format = registry.get_format(format_name);
    if (!format) {
        std::cerr << "Format not found: " << format_name << std::endl;
        return false;
    }
    
    try {
        std::vector<uint8_t> data = format->generator(bytecode);
        
        std::ofstream file(path, std::ios::binary);
        if (!file) {
            std::cerr << "Failed to open file for writing: " << path << std::endl;
            return false;
        }
        
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
        file.close();
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to generate binary: " << e.what() << std::endl;
        return false;
    }
}

} // namespace guardian::vm
