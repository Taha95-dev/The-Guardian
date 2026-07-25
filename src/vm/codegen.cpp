#include "codegen.hpp"
#include "opcodes.hpp"
#include <iostream>
#include <iomanip>

namespace guardian::vm {

CodeGen::CodeGen() {}

void CodeGen::emitUint32(uint32_t value) {
    bytecode.push_back(value & 0xFF);
    bytecode.push_back((value >> 8) & 0xFF);
    bytecode.push_back((value >> 16) & 0xFF);
    bytecode.push_back((value >> 24) & 0xFF);
}

void CodeGen::emit(uint8_t opcode) {
    bytecode.push_back(opcode);
}

void CodeGen::emit(uint8_t opcode, uint32_t operand) {
    bytecode.push_back(opcode);
    emitUint32(operand);
}

void CodeGen::emitString(const std::string& str) {
    emit(static_cast<uint8_t>(Opcode::PUSH_STRING));
    uint32_t idx = nextStringIndex++;
    stringTable[idx] = str;
    emitUint32(idx);
}

uint32_t CodeGen::declareVariable(const std::string& name) {
    uint32_t idx = nextVariableIndex++;
    variableMap[name] = idx;
    return idx;
}

uint32_t CodeGen::getVariableIndex(const std::string& name) const {
    auto it = variableMap.find(name);
    if (it != variableMap.end()) {
        return it->second;
    }
    return 0xFFFFFFFF;
}

void CodeGen::storeVariable(uint32_t index) {
    emit(static_cast<uint8_t>(Opcode::STORE), index);
}

void CodeGen::loadVariable(uint32_t index) {
    emit(static_cast<uint8_t>(Opcode::LOAD), index);
}

void CodeGen::print() {
    emit(static_cast<uint8_t>(Opcode::PRINT));
}

void CodeGen::println() {
    emit(static_cast<uint8_t>(Opcode::PRINT));
    emit(static_cast<uint8_t>(Opcode::NEWLINE));
}

void CodeGen::space() {
    emit(static_cast<uint8_t>(Opcode::SPACE));
}

void CodeGen::newline() {
    emit(static_cast<uint8_t>(Opcode::NEWLINE));
}

void CodeGen::pushInt(int32_t value) {
    emit(static_cast<uint8_t>(Opcode::PUSH_INT), static_cast<uint32_t>(value));
}

void CodeGen::pushFloat(double value) {
    // FIXME: Proper float encoding
    emit(static_cast<uint8_t>(Opcode::PUSH_FLOAT));
    // Need to encode float properly
}

void CodeGen::pushBool(bool value) {
    emit(static_cast<uint8_t>(Opcode::PUSH_BOOL), value ? 1 : 0);
}

void CodeGen::pushString(const std::string& value) {
    emitString(value);
}

void CodeGen::pushNull() {
    emit(static_cast<uint8_t>(Opcode::PUSH_NULL));
}

void CodeGen::halt() {
    emit(static_cast<uint8_t>(Opcode::HALT));
}

void CodeGen::jump(uint32_t address) {
    emit(static_cast<uint8_t>(Opcode::JMP), address);
}

void CodeGen::jumpIf(uint32_t address) {
    emit(static_cast<uint8_t>(Opcode::JMP_IF), address);
}

void CodeGen::jumpIfNot(uint32_t address) {
    emit(static_cast<uint8_t>(Opcode::JMP_IF_NOT), address);
}

std::vector<uint8_t> CodeGen::getBytecode() const {
    return bytecode;
}

void CodeGen::dump() const {
    std::cout << "📜 Bytecode (" << bytecode.size() << " bytes):\n";
    size_t i = 0;
    while (i < bytecode.size()) {
        uint8_t op = bytecode[i++];
        std::cout << "  " << std::hex << std::setw(4) << std::setfill('0') << (i-1) << ": ";
        
        switch (static_cast<Opcode>(op)) {
            case Opcode::HALT: std::cout << "HALT"; break;
            case Opcode::PUSH_INT: 
                std::cout << "PUSH_INT";
                if (i + 4 <= bytecode.size()) {
                    uint32_t val = bytecode[i] | (bytecode[i+1] << 8) | (bytecode[i+2] << 16) | (bytecode[i+3] << 24);
                    std::cout << " " << val;
                    i += 4;
                }
                break;
            case Opcode::PUSH_STRING:
                std::cout << "PUSH_STRING";
                if (i + 4 <= bytecode.size()) {
                    uint32_t idx = bytecode[i] | (bytecode[i+1] << 8) | (bytecode[i+2] << 16) | (bytecode[i+3] << 24);
                    auto it = stringTable.find(idx);
                    if (it != stringTable.end()) {
                        std::cout << " \"" << it->second << "\"";
                    } else {
                        std::cout << " <str " << idx << ">";
                    }
                    i += 4;
                }
                break;
            case Opcode::STORE:
                std::cout << "STORE";
                if (i + 4 <= bytecode.size()) {
                    uint32_t idx = bytecode[i] | (bytecode[i+1] << 8) | (bytecode[i+2] << 16) | (bytecode[i+3] << 24);
                    std::cout << " var[" << idx << "]";
                    i += 4;
                }
                break;
            case Opcode::LOAD:
                std::cout << "LOAD";
                if (i + 4 <= bytecode.size()) {
                    uint32_t idx = bytecode[i] | (bytecode[i+1] << 8) | (bytecode[i+2] << 16) | (bytecode[i+3] << 24);
                    std::cout << " var[" << idx << "]";
                    i += 4;
                }
                break;
            case Opcode::PRINT: std::cout << "PRINT"; break;
            case Opcode::NEWLINE: std::cout << "NEWLINE"; break;
            case Opcode::SPACE: std::cout << "SPACE"; break;
            default: std::cout << "UNKNOWN(0x" << std::hex << (int)op << ")"; break;
        }
        std::cout << std::dec << "\n";
    }
}

} // namespace guardian::vm
