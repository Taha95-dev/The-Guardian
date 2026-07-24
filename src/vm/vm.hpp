#pragma once

#include <vector>
#include <string>
#include <memory>
#include <cstdint>
#include "../core/molecule.hpp"

namespace guardian::vm {

enum class Opcode : uint8_t {
    HALT = 0x00,
    PUSH_INT = 0x10,
    PUSH_STRING = 0x13,
    STORE = 0x60,
    LOAD = 0x61,
    PRINT = 0x80,
    PRINTLN = 0x81,
};

struct Value {
    bool is_quark;
    int int_val;
    float float_val;
    bool bool_val;
    std::string string_val;
    
    Value() : is_quark(true), int_val(0), float_val(0.0f), bool_val(false) {}
    Value(int v) : is_quark(true), int_val(v), float_val(0.0f), bool_val(false) {}
    Value(float v) : is_quark(true), int_val(0), float_val(v), bool_val(false) {}
    Value(double v) : is_quark(true), int_val(0), float_val(static_cast<float>(v)), bool_val(false) {}
    Value(bool v) : is_quark(true), int_val(0), float_val(0.0f), bool_val(v) {}
    Value(const std::string& v) : is_quark(false), string_val(v) {}
};

class VM {
public:
    VM();
    ~VM();
    
    void load(const std::vector<uint8_t>& bytecode, size_t entry = 0);
    void run();
    void reset();
    bool is_running() const;
    
    void push(const Value& val);
    Value pop();
    
private:
    std::vector<uint8_t> bytecode;
    size_t pc;
    std::vector<Value> stack;
    std::shared_ptr<guardian::Molecule> main_molecule;
    bool running;
    
    void execute(uint8_t opcode);
    void executePrint();
    void executePrintln();
    
    std::string readString();
    void storeVariable(const std::string& name, const Value& val);
    Value loadVariable(const std::string& name);
};

} // namespace guardian::vm
