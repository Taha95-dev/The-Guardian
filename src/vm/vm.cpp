#include "vm.hpp"
#include <iostream>
#include <cstring>

namespace guardian::vm {

VM::VM() : running(false) {
    main_molecule = std::make_shared<guardian::Molecule>();
}

VM::~VM() = default;

void VM::reset() {
    running = false;
    stack.clear();
    pc = 0;
    main_molecule = std::make_shared<guardian::Molecule>();
}

void VM::load(const std::vector<uint8_t>& bytecode, size_t entry) {
    this->bytecode = bytecode;
    this->pc = entry;
    this->running = true;
    this->main_molecule = std::make_shared<guardian::Molecule>();
}

void VM::push(const Value& val) {
    stack.push_back(val);
}

Value VM::pop() {
    if (stack.empty()) {
        return Value();
    }
    Value val = stack.back();
    stack.pop_back();
    return val;
}

std::string VM::readString() {
    uint32_t len;
    memcpy(&len, &bytecode[pc], 4);
    pc += 4;
    std::string str(bytecode.begin() + pc, bytecode.begin() + pc + len);
    pc += len;
    return str;
}

void VM::storeVariable(const std::string& name, const Value& val) {
    if (val.is_quark) {
        main_molecule->add_number(name, val.int_val);
    } else {
        main_molecule->add_string(name, val.string_val);
    }
}

Value VM::loadVariable(const std::string& name) {
    if (main_molecule->has_number(name)) {
        return Value(main_molecule->get_number(name));
    }
    if (main_molecule->has_string(name)) {
        return Value(main_molecule->get_string(name));
    }
    return Value(0);
}

void VM::run() {
    while (running && pc < bytecode.size()) {
        uint8_t opcode = bytecode[pc++];
        execute(opcode);
    }
}

void VM::execute(uint8_t opcode) {
    switch (static_cast<Opcode>(opcode)) {
        case Opcode::HALT:
            running = false;
            break;
            
        case Opcode::PUSH_INT: {
            int32_t val;
            memcpy(&val, &bytecode[pc], 4);
            pc += 4;
            push(Value(val));
            break;
        }
        
        case Opcode::PUSH_STRING: {
            std::string str = readString();
            push(Value(str));
            break;
        }
        
        case Opcode::STORE: {
            std::string name = readString();
            Value val = pop();
            storeVariable(name, val);
            break;
        }
        
        case Opcode::LOAD: {
            std::string name = readString();
            push(loadVariable(name));
            break;
        }
        
        case Opcode::PRINT:
            executePrint();
            break;
        case Opcode::PRINTLN:
            executePrintln();
            break;
            
        default:
            std::cerr << "Error: Unknown opcode 0x" << std::hex << (int)opcode << "\n";
            running = false;
            break;
    }
}

void VM::executePrint() {
    Value val = pop();
    if (val.is_quark) {
        std::cout << val.int_val;
    } else {
        std::cout << val.string_val;
    }
}

void VM::executePrintln() {
    Value val = pop();
    if (val.is_quark) {
        std::cout << val.int_val << "\n";
    } else {
        std::cout << val.string_val << "\n";
    }
}

bool VM::is_running() const {
    return running;
}

} // namespace guardian::vm
