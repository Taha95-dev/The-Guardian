#include "vm.hpp"
#include <iostream>
#include <cstring>
#include <memory>

namespace guardian::vm {

// ── Constructor/Destructor ──
VM::VM() : pc(0), running(false), main_molecule(std::make_shared<guardian::Molecule>()) {}
VM::~VM() {}

// ── Load / Run / Reset ──
void VM::load(const std::vector<uint8_t>& code, size_t entry) {
    bytecode = code;
    pc = 0;
    running = true;
    stack.clear();
}

void VM::run() {
    while (running && pc < bytecode.size()) {
        uint8_t opcode = bytecode[pc++];
        dispatch(opcode);
    }
}

void VM::reset() {
    pc = 0;
    stack.clear();
    running = false;
    main_molecule = std::make_shared<guardian::Molecule>();
}

bool VM::isRunning() const {
    return running;
}

// ── Stack ──
void VM::push(const Value& val) {
    stack.push_back(val);
}

Value VM::pop() {
    if (stack.empty()) return Value();
    Value val = stack.back();
    stack.pop_back();
    return val;
}

Value VM::peek() const {
    if (stack.empty()) return Value();
    return stack.back();
}

// ── Helpers ──
uint32_t VM::readUint32() {
    if (pc + 4 > bytecode.size()) return 0;
    uint32_t val = bytecode[pc] | (bytecode[pc+1] << 8) | (bytecode[pc+2] << 16) | (bytecode[pc+3] << 24);
    pc += 4;
    return val;
}

std::string VM::readString() {
    if (pc + 2 > bytecode.size()) return "";
    uint16_t len = bytecode[pc] | (bytecode[pc+1] << 8);
    pc += 2;
    if (pc + len > bytecode.size()) return "";
    std::string result(bytecode.begin() + pc, bytecode.begin() + pc + len);
    pc += len;
    return result;
}

bool VM::popBool() {
    Value v = pop();
    if (v.is_quark && v.quark_data.type == Value::QuarkData::BOOL) {
        return v.quark_data.bool_val;
    }
    return false;
}

int VM::popInt() {
    Value v = pop();
    if (v.is_quark && v.quark_data.type == Value::QuarkData::INT) {
        return v.quark_data.int_val;
    }
    return 0;
}

double VM::popFloat() {
    Value v = pop();
    if (v.is_quark && v.quark_data.type == Value::QuarkData::FLOAT) {
        return v.quark_data.float_val;
    }
    return 0.0;
}

Value VM::popValue() {
    return pop();
}

// ── Dispatch ──
void VM::dispatch(uint8_t opcode) {
    switch (static_cast<Opcode>(opcode)) {
        case Opcode::HALT:      handleHalt(); break;
        case Opcode::NOP:       handleNop(); break;
        case Opcode::PUSH_INT:  handlePushInt(); break;
        case Opcode::PUSH_FLOAT: handlePushFloat(); break;
        case Opcode::PUSH_BOOL: handlePushBool(); break;
        case Opcode::PUSH_STRING: handlePushString(); break;
        case Opcode::PUSH_NULL: handlePushNull(); break;
        case Opcode::POP:       handlePop(); break;
        case Opcode::DUP:       handleDup(); break;
        case Opcode::SWAP:      handleSwap(); break;
        case Opcode::ADD:       handleAdd(); break;
        case Opcode::SUB:       handleSub(); break;
        case Opcode::MUL:       handleMul(); break;
        case Opcode::DIV:       handleDiv(); break;
        case Opcode::MOD:       handleMod(); break;
        case Opcode::PRINT:     handlePrint(); break;
        case Opcode::NEWLINE:   handleNewline(); break;
        case Opcode::SPACE:     handleSpace(); break;
        case Opcode::STORE:     handleStore(); break;
        case Opcode::LOAD:      handleLoad(); break;
        case Opcode::JMP:       handleJmp(); break;
        case Opcode::JMP_IF:    handleJmpIf(); break;
        case Opcode::JMP_IF_NOT: handleJmpIfNot(); break;
        case Opcode::EQ:    handleEq(); break;
        case Opcode::NEQ:   handleNeq(); break;
        case Opcode::LT:    handleLt(); break;
        case Opcode::GT:    handleGt(); break;
        case Opcode::LTE:   handleLte(); break;
        case Opcode::GTE:   handleGte(); break;
        default:
            std::cerr << "ERROR: Unknown opcode 0x" << std::hex << (int)opcode << std::dec << "\n";
            running = false;
            break;
    }
}

// ── Handlers ──
void VM::handleHalt() { running = false; }
void VM::handleNop() {}

void VM::handlePushInt() {
    push(Value(static_cast<int>(readUint32())));
}

void VM::handlePushFloat() {
    if (pc + 8 <= bytecode.size()) {
        double val;
        memcpy(&val, &bytecode[pc], sizeof(double));
        pc += 8;
        push(Value(val));
    }
}

// ── Comparison Handlers ──
void VM::handleEq() {
    if (stack.size() < 2) return;
    Value b = pop();
    Value a = pop();
    
    bool result = false;
    if (a.is_quark && b.is_quark) {
        if (a.quark_data.type == Value::QuarkData::INT && 
            b.quark_data.type == Value::QuarkData::INT) {
            result = (a.quark_data.int_val == b.quark_data.int_val);
        } else if (a.quark_data.type == Value::QuarkData::FLOAT || 
                   b.quark_data.type == Value::QuarkData::FLOAT) {
            result = (a.quark_data.float_val == b.quark_data.float_val);
        } else if (a.quark_data.type == Value::QuarkData::STRING && 
                   b.quark_data.type == Value::QuarkData::STRING) {
            result = (a.quark_data.string_val == b.quark_data.string_val);
        } else if (a.quark_data.type == Value::QuarkData::BOOL && 
                   b.quark_data.type == Value::QuarkData::BOOL) {
            result = (a.quark_data.bool_val == b.quark_data.bool_val);
        }
    }
    push(Value(result));
}

void VM::handleNeq() {
    if (stack.size() < 2) return;
    Value b = pop();
    Value a = pop();
    
    bool result = true;
    if (a.is_quark && b.is_quark) {
        if (a.quark_data.type == Value::QuarkData::INT && 
            b.quark_data.type == Value::QuarkData::INT) {
            result = (a.quark_data.int_val != b.quark_data.int_val);
        } else if (a.quark_data.type == Value::QuarkData::FLOAT || 
                   b.quark_data.type == Value::QuarkData::FLOAT) {
            result = (a.quark_data.float_val != b.quark_data.float_val);
        } else if (a.quark_data.type == Value::QuarkData::STRING && 
                   b.quark_data.type == Value::QuarkData::STRING) {
            result = (a.quark_data.string_val != b.quark_data.string_val);
        } else if (a.quark_data.type == Value::QuarkData::BOOL && 
                   b.quark_data.type == Value::QuarkData::BOOL) {
            result = (a.quark_data.bool_val != b.quark_data.bool_val);
        }
    }
    push(Value(result));
}

void VM::handleLt() {
    if (stack.size() < 2) return;
    Value b = pop();
    Value a = pop();
    
    bool result = false;
    if (a.is_quark && b.is_quark) {
        if (a.quark_data.type == Value::QuarkData::INT && 
            b.quark_data.type == Value::QuarkData::INT) {
            result = (a.quark_data.int_val < b.quark_data.int_val);
        } else if (a.quark_data.type == Value::QuarkData::FLOAT || 
                   b.quark_data.type == Value::QuarkData::FLOAT) {
            result = (a.quark_data.float_val < b.quark_data.float_val);
        }
    }
    push(Value(result));
}

void VM::handleGt() {
    if (stack.size() < 2) return;
    Value b = pop();
    Value a = pop();
    
    bool result = false;
    if (a.is_quark && b.is_quark) {
        if (a.quark_data.type == Value::QuarkData::INT && 
            b.quark_data.type == Value::QuarkData::INT) {
            result = (a.quark_data.int_val > b.quark_data.int_val);
        } else if (a.quark_data.type == Value::QuarkData::FLOAT || 
                   b.quark_data.type == Value::QuarkData::FLOAT) {
            result = (a.quark_data.float_val > b.quark_data.float_val);
        }
    }
    push(Value(result));
}

void VM::handleLte() {
    if (stack.size() < 2) return;
    Value b = pop();
    Value a = pop();
    
    bool result = false;
    if (a.is_quark && b.is_quark) {
        if (a.quark_data.type == Value::QuarkData::INT && 
            b.quark_data.type == Value::QuarkData::INT) {
            result = (a.quark_data.int_val <= b.quark_data.int_val);
        } else if (a.quark_data.type == Value::QuarkData::FLOAT || 
                   b.quark_data.type == Value::QuarkData::FLOAT) {
            result = (a.quark_data.float_val <= b.quark_data.float_val);
        }
    }
    push(Value(result));
}

void VM::handleGte() {
    if (stack.size() < 2) return;
    Value b = pop();
    Value a = pop();
    
    bool result = false;
    if (a.is_quark && b.is_quark) {
        if (a.quark_data.type == Value::QuarkData::INT && 
            b.quark_data.type == Value::QuarkData::INT) {
            result = (a.quark_data.int_val >= b.quark_data.int_val);
        } else if (a.quark_data.type == Value::QuarkData::FLOAT || 
                   b.quark_data.type == Value::QuarkData::FLOAT) {
            result = (a.quark_data.float_val >= b.quark_data.float_val);
        }
    }
    push(Value(result));
}

void VM::handlePushBool() {
    if (pc < bytecode.size()) {
        push(Value(bytecode[pc++] != 0));
    }
}

void VM::handlePushString() {
    std::string str = readString();
    push(Value(str));
}

void VM::handlePushNull() {
    push(Value());
}

void VM::handlePop() {
    if (!stack.empty()) stack.pop_back();
}

void VM::handleDup() {
    if (!stack.empty()) push(stack.back());
}

void VM::handleSwap() {
    if (stack.size() >= 2) {
        Value a = pop();
        Value b = pop();
        push(a);
        push(b);
    }
}

void VM::handleAdd() {
    if (stack.size() < 2) return;
    Value b = pop();
    Value a = pop();
    if (a.is_quark && b.is_quark) {
        if (a.quark_data.type == Value::QuarkData::INT && b.quark_data.type == Value::QuarkData::INT) {
            push(Value(a.quark_data.int_val + b.quark_data.int_val));
        } else {
            push(Value(a.quark_data.float_val + b.quark_data.float_val));
        }
    }
}

void VM::handleSub() {
    if (stack.size() < 2) return;
    Value b = pop();
    Value a = pop();
    if (a.is_quark && b.is_quark) {
        push(Value(a.quark_data.float_val - b.quark_data.float_val));
    }
}

void VM::handleMul() {
    if (stack.size() < 2) return;
    Value b = pop();
    Value a = pop();
    if (a.is_quark && b.is_quark) {
        push(Value(a.quark_data.float_val * b.quark_data.float_val));
    }
}

void VM::handleDiv() {
    if (stack.size() < 2) return;
    Value b = pop();
    Value a = pop();
    if (a.is_quark && b.is_quark && b.quark_data.float_val != 0.0) {
        push(Value(a.quark_data.float_val / b.quark_data.float_val));
    }
}

void VM::handleMod() {
    if (stack.size() < 2) return;
    int b = popInt();
    int a = popInt();
    if (b != 0) push(Value(a % b));
}

void VM::handlePrint() {
    if (stack.empty()) return;
    Value val = pop();
    std::cout << val.to_string();
    std::cout.flush();
}

void VM::handlePrintln() {
    if (stack.empty()) return;
    Value val = pop();
    std::cout << val.to_string() << std::endl;
}

void VM::handleSpace() {
    std::cout << ' ';
    std::cout.flush();
}

void VM::handleNewline() {
    std::cout << '\n';
    std::cout.flush();
}

void VM::handleStore() {
    uint32_t idx = readUint32();
    Value val = pop();
    // Store in molecule (simplified)
    main_molecule->add_string("var_" + std::to_string(idx), val.to_string());
}

void VM::handleLoad() {
    uint32_t idx = readUint32();
    std::string key = "var_" + std::to_string(idx);
    if (main_molecule->has_string(key)) {
        push(Value(main_molecule->get_string(key)));
    } else {
        push(Value());
    }
}

void VM::handleJmp() {
    int32_t offset = static_cast<int32_t>(readUint32());
    pc = static_cast<size_t>(static_cast<int>(pc) + offset - 4);
}

void VM::handleJmpIf() {
    bool cond = popBool();
    if (cond) {
        handleJmp();
    } else {
        pc += 4;
    }
}

void VM::handleJmpIfNot() {
    bool cond = popBool();
    if (!cond) {
        handleJmp();
    } else {
        pc += 4;
    }
}

} // namespace guardian::vm
