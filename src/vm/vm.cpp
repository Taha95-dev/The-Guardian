#include "vm.hpp"
#include "../atoms/text/string_atom.hpp"
#include <iostream>
#include <cstring>

namespace guardian::vm {

VM::VM() : running(false) {
    main_molecule = std::make_shared<guardian::Molecule>();
}

VM::~VM() = default;

void VM::load(const std::vector<uint8_t>& bytecode, size_t entry) {
    this->bytecode = bytecode;
    this->pc = entry;
    this->running = true;
    this->stack.clear();
    this->main_molecule = std::make_shared<guardian::Molecule>();
}

void VM::run() {
    while (running && pc < bytecode.size()) {
        uint8_t opcode = bytecode[pc++];
        execute(opcode);
    }
}

void VM::push(const Value& val) {
    stack.push_back(val);
}

Value VM::pop() {
    if (stack.empty()) {
        std::cerr << "Error: Stack underflow\n";
        return Value();
    }
    Value val = stack.back();
    stack.pop_back();
    return val;
}

Value VM::peek() const {
    if (stack.empty()) {
        return Value();
    }
    return stack.back();
}

std::string VM::readString() {
    if (pc + 4 > bytecode.size()) {
        std::cerr << "Error: Invalid string length\n";
        return "";
    }
    uint32_t len;
    memcpy(&len, &bytecode[pc], 4);
    pc += 4;
    if (pc + len > bytecode.size()) {
        std::cerr << "Error: String extends beyond bytecode\n";
        return "";
    }
    std::string str(bytecode.begin() + pc, bytecode.begin() + pc + len);
    pc += len;
    return str;
}

void VM::setVariable(const std::string& name, const Value& value) {
    if (value.is_quark) {
        switch (value.quark_data.type) {
            case guardian::QuarkType::INT:
                main_molecule->add_atom(name, std::make_unique<guardian::IntAtom>(value.quark_data.int_val));
                break;
            case guardian::QuarkType::FLOAT:
                main_molecule->add_atom(name, std::make_unique<guardian::FloatAtom>(value.quark_data.float_val));
                break;
            case guardian::QuarkType::FLOAT64:
                main_molecule->add_atom(name, std::make_unique<guardian::FloatAtom>(static_cast<float>(value.quark_data.float64_val)));
                break;
            case guardian::QuarkType::BOOL:
                main_molecule->add_atom(name, std::make_unique<guardian::BoolAtom>(value.quark_data.bool_val));
                break;
            case guardian::QuarkType::CHAR:
                main_molecule->add_atom(name, std::make_unique<guardian::CharAtom>(value.quark_data.char_val));
                break;
            default:
                break;
        }
    } else if (value.atom_data) {
        // For strings and other atoms
    }
}

Value VM::getVariable(const std::string& name) const {
    auto atom = main_molecule->get_atom(name);
    if (atom) {
        return Value(std::shared_ptr<guardian::Atom>(atom, [](guardian::Atom*){}));
    }
    return Value(0);
}

bool VM::hasVariable(const std::string& name) const {
    return main_molecule->get_atom(name) != nullptr;
}

void VM::execute(uint8_t opcode) {
    switch (static_cast<Opcode>(opcode)) {
        case Opcode::HALT:
            running = false;
            break;
            
        case Opcode::PUSH_INT: {
            if (pc + 4 > bytecode.size()) {
                std::cerr << "Error: Invalid PUSH_INT instruction\n";
                running = false;
                return;
            }
            int32_t value;
            memcpy(&value, &bytecode[pc], 4);
            pc += 4;
            push(Value(value));
            break;
        }
        
        case Opcode::PUSH_FLOAT: {
            if (pc + 4 > bytecode.size()) {
                std::cerr << "Error: Invalid PUSH_FLOAT instruction\n";
                running = false;
                return;
            }
            float value;
            memcpy(&value, &bytecode[pc], 4);
            pc += 4;
            push(Value(value));
            break;
        }
        
        case Opcode::PUSH_BOOL: {
            if (pc >= bytecode.size()) {
                std::cerr << "Error: Invalid PUSH_BOOL instruction\n";
                running = false;
                return;
            }
            bool value = bytecode[pc++] != 0;
            push(Value(value));
            break;
        }
       
        case Opcode::MOD:
            executeMod();
            break;

        case Opcode::PUSH_STRING: {
            if (pc + 4 > bytecode.size()) {
                std::cerr << "Error: Invalid PUSH_STRING instruction\n";
                running = false;
                return;
            }
            uint32_t len;
            memcpy(&len, &bytecode[pc], 4);
            pc += 4;
            if (pc + len > bytecode.size()) {
                std::cerr << "Error: String extends beyond bytecode\n";
                running = false;
                return;
            }
            std::string str(bytecode.begin() + pc, bytecode.begin() + pc + len);
            pc += len;
            auto atom = std::make_shared<guardian::StringAtom>(str);
            push(Value(atom));
            break;
        }
        
        case Opcode::STORE: {
            std::string name = readString();
            Value val = pop();
            setVariable(name, val);
            break;
        }
        
        case Opcode::LOAD: {
            std::string name = readString();
            push(getVariable(name));
            break;
        }
        
        case Opcode::ADD:
            executeAdd();
            break;
            
        case Opcode::SUB:
            executeSub();
            break;
            
        case Opcode::MUL:
            executeMul();
            break;
            
        case Opcode::DIV:
            executeDiv();
            break; 
            
        case Opcode::PRINT:
            executePrint();
            break;
            
        case Opcode::PRINTLN:
            executePrintln();
            break;
            
        case Opcode::POP:
            pop();
            break;
            
        case Opcode::DUP: {
            if (stack.empty()) {
                std::cerr << "Error: DUP with empty stack\n";
                running = false;
                return;
            }
            push(stack.back());
            break;
        }
        
        default:
            std::cerr << "Error: Unknown opcode 0x" << std::hex << (int)opcode << "\n";
            running = false;
            break;
    }
}

void VM::executeAdd() {
    if (stack.size() < 2) {
        std::cerr << "Error: ADD requires 2 operands\n";
        running = false;
        return;
    }
    Value right = pop();
    Value left = pop();
    
    if (left.is_quark && right.is_quark) {
        if (left.quark_data.type == guardian::QuarkType::INT && 
            right.quark_data.type == guardian::QuarkType::INT) {
            int result = left.quark_data.int_val + right.quark_data.int_val;
            push(Value(result));
        } else if (left.quark_data.type == guardian::QuarkType::FLOAT64 && 
                   right.quark_data.type == guardian::QuarkType::FLOAT64) {
            double result = left.quark_data.float64_val + right.quark_data.float64_val;
            push(Value(result));
        } else if (left.quark_data.type == guardian::QuarkType::INT && 
                   right.quark_data.type == guardian::QuarkType::FLOAT64) {
            double result = (double)left.quark_data.int_val + right.quark_data.float64_val;
            push(Value(result));
        } else if (left.quark_data.type == guardian::QuarkType::FLOAT64 && 
                   right.quark_data.type == guardian::QuarkType::INT) {
            double result = left.quark_data.float64_val + (double)right.quark_data.int_val;
            push(Value(result));
        } else {
            std::cerr << "Error: Unsupported ADD types\n";
            running = false;
        }
    } else {
        std::cerr << "Error: ADD currently only supports quarks\n";
        running = false;
    }
}

void VM::executeSub() {
    if (stack.size() < 2) {
        std::cerr << "Error: SUB requires 2 operands\n";
        running = false;
        return;
    }
    Value right = pop();
    Value left = pop();
    
    if (left.is_quark && right.is_quark) {
        if (left.quark_data.type == guardian::QuarkType::INT && 
            right.quark_data.type == guardian::QuarkType::INT) {
            int result = left.quark_data.int_val - right.quark_data.int_val;
            push(Value(result));
        } else if (left.quark_data.type == guardian::QuarkType::FLOAT64 && 
                   right.quark_data.type == guardian::QuarkType::FLOAT64) {
            double result = left.quark_data.float64_val - right.quark_data.float64_val;
            push(Value(result));
        } else {
            std::cerr << "Error: Unsupported SUB types\n";
            running = false;
        }
    } else {
        std::cerr << "Error: SUB currently only supports quarks\n";
        running = false;
    }
}

void VM::executeMul() {
    if (stack.size() < 2) {
        std::cerr << "Error: MUL requires 2 operands\n";
        running = false;
        return;
    }
    Value right = pop();
    Value left = pop();
    
    if (left.is_quark && right.is_quark) {
        if (left.quark_data.type == guardian::QuarkType::INT && 
            right.quark_data.type == guardian::QuarkType::INT) {
            int result = left.quark_data.int_val * right.quark_data.int_val;
            push(Value(result));
        } else if (left.quark_data.type == guardian::QuarkType::FLOAT64 && 
                   right.quark_data.type == guardian::QuarkType::FLOAT64) {
            double result = left.quark_data.float64_val * right.quark_data.float64_val;
            push(Value(result));
        } else {
            std::cerr << "Error: Unsupported MUL types\n";
            running = false;
        }
    } else {
        std::cerr << "Error: MUL currently only supports quarks\n";
        running = false;
    }
}

void VM::executeDiv() {
    if (stack.size() < 2) {
        std::cerr << "Error: DIV requires 2 operands\n";
        running = false;
        return;
    }
    Value right = pop();
    Value left = pop();
    
    if (right.is_quark && right.quark_data.type == guardian::QuarkType::INT && 
        right.quark_data.int_val == 0) {
        std::cerr << "Error: Division by zero\n";
        running = false;
        return;
    }
    
    if (left.is_quark && right.is_quark) {
        if (left.quark_data.type == guardian::QuarkType::INT && 
            right.quark_data.type == guardian::QuarkType::INT) {
            int result = left.quark_data.int_val / right.quark_data.int_val;
            push(Value(result));
        } else if (left.quark_data.type == guardian::QuarkType::FLOAT64 && 
                   right.quark_data.type == guardian::QuarkType::FLOAT64) {
            double result = left.quark_data.float64_val / right.quark_data.float64_val;
            push(Value(result));
        } else {
            std::cerr << "Error: Unsupported DIV types\n";
            running = false;
        }
    } else {
        std::cerr << "Error: DIV currently only supports quarks\n";
        running = false;
    }
}

void VM::executeMod() {
    if (stack.size() < 2) {
        std::cerr << "Error: MOD requires 2 operands\n";
        running = false;
        return;
    }
    Value right = pop();
    Value left = pop();
    
    if (left.is_quark && right.is_quark) {
        if (left.quark_data.type == guardian::QuarkType::INT && 
            right.quark_data.type == guardian::QuarkType::INT) {
            int result = left.quark_data.int_val % right.quark_data.int_val;
            push(Value(result));
        } else {
            std::cerr << "Error: MOD only supports integers\n";
            running = false;
        }
    } else {
        std::cerr << "Error: MOD currently only supports quarks\n";
        running = false;
    }
}

void VM::executePrint() {
    if (stack.empty()) {
        std::cerr << "Error: PRINT with empty stack\n";
        running = false;
        return;
    }
    Value val = pop();
    std::cout << val.to_string();
}

void VM::executePrintln() {
    if (stack.empty()) {
        std::cerr << "Error: PRINTLN with empty stack\n";
        running = false;
        return;
    }
    Value val = pop();
    std::cout << val.to_string() << "\n";
}

void VM::reset() {
    running = false;
    stack.clear();
    pc = 0;
    main_molecule = std::make_shared<guardian::Molecule>();
}

bool VM::is_running() const {
    return running;
}

} // namespace guardian::vm
