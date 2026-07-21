#include "vm.hpp"
#include <iostream>
#include <cstring>

namespace guardian::vm {

VM::VM() : running(false) {}

VM::~VM() = default;

void VM::load(const std::vector<uint8_t>& bytecode, size_t entry) {
    this->bytecode = bytecode;
    this->pc = entry;
    this->running = true;
    this->stack.clear();
    this->frames.clear();
}

void VM::run() {
    while (running && pc < bytecode.size()) {
        uint8_t opcode = bytecode[pc++];
        execute(opcode);
    }
}

void VM::execute(uint8_t opcode) {
    // For now, handle the basic opcodes
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
            stack.push_back(Value(value));
            break;
        }
        
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
            stack.push_back(Value(str));
            break;
        }
        
        case Opcode::PRINT: {
            if (stack.empty()) {
                std::cerr << "Error: PRINT with empty stack\n";
                running = false;
                return;
            }
            Value val = stack.back();
            stack.pop_back();
            std::cout << val.to_string();
            break;
        }
        
        case Opcode::PRINTLN: {
            if (stack.empty()) {
                std::cerr << "Error: PRINTLN with empty stack\n";
                running = false;
                return;
            }
            Value val = stack.back();
            stack.pop_back();
            std::cout << val.to_string() << "\n";
            break;
        }
        
        case Opcode::POP: {
            if (!stack.empty()) {
                stack.pop_back();
            }
            break;
        }
        
        case Opcode::DUP: {
            if (stack.empty()) {
                std::cerr << "Error: DUP with empty stack\n";
                running = false;
                return;
            }
            stack.push_back(stack.back());
            break;
        }
        
        default:
            std::cerr << "Error: Unknown opcode 0x" << std::hex << (int)opcode << "\n";
            running = false;
            break;
    }
}

void VM::reset() {
    running = false;
    stack.clear();
    frames.clear();
    pc = 0;
}

bool VM::is_running() const {
    return running;
}

} // namespace guardian::vm
