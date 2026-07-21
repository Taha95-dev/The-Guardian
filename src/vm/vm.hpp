#pragma once

#include <vector>
#include <string>
#include <stack>
#include <variant>
#include <cstdint>

namespace guardian::vm {

// Opcodes for the VM
enum class Opcode : uint8_t {
    HALT = 0x00,
    NOP = 0x01,
    
    // Stack operations
    PUSH_INT = 0x10,
    PUSH_STRING = 0x11,
    PUSH_BOOL = 0x12,
    PUSH_NULL = 0x13,
    POP = 0x14,
    DUP = 0x15,
    
    // Arithmetic
    ADD = 0x20,
    SUB = 0x21,
    MUL = 0x22,
    DIV = 0x23,
    MOD = 0x24,
    
    // Comparison
    EQ = 0x30,
    NEQ = 0x31,
    LT = 0x32,
    GT = 0x33,
    LTE = 0x34,
    GTE = 0x35,
    
    // Logic
    AND = 0x40,
    OR = 0x41,
    NOT = 0x42,
    
    // Control flow
    JMP = 0x50,
    JMP_IF = 0x51,
    JMP_IF_NOT = 0x52,
    CALL = 0x53,
    RET = 0x54,
    
    // Variables
    LOAD = 0x60,
    STORE = 0x61,
    LOAD_GLOBAL = 0x62,
    STORE_GLOBAL = 0x63,
    
    // Functions
    DEFINE_FN = 0x70,
    CALL_FN = 0x71,
    RETURN = 0x72,
    
    // Print
    PRINT = 0x80,
    PRINTLN = 0x81,
};

// Values on the stack
struct Value {
    std::variant<int, float, bool, std::string> data;
    
    Value() : data(0) {}
    Value(int v) : data(v) {}
    Value(float v) : data(v) {}
    Value(bool v) : data(v) {}
    Value(const std::string& v) : data(v) {}
    Value(const char* v) : data(std::string(v)) {}
    
    std::string to_string() const {
        if (std::holds_alternative<int>(data)) {
            return std::to_string(std::get<int>(data));
        }
        if (std::holds_alternative<float>(data)) {
            return std::to_string(std::get<float>(data));
        }
        if (std::holds_alternative<bool>(data)) {
            return std::get<bool>(data) ? "true" : "false";
        }
        if (std::holds_alternative<std::string>(data)) {
            return std::get<std::string>(data);
        }
        return "null";
    }
};

struct Frame {
    size_t return_address;
    std::vector<Value> locals;
    
    Frame(size_t ret) : return_address(ret) {}
};

class VM {
public:
    VM();
    ~VM();
    
    // Load bytecode
    void load(const std::vector<uint8_t>& bytecode, size_t entry = 0);
    
    // Run the program
    void run();
    
    // Reset state
    void reset();
    
    // Check if running
    bool is_running() const;
    
private:
    std::vector<uint8_t> bytecode;
    size_t pc;
    std::vector<Value> stack;
    std::vector<Frame> frames;
    bool running;
    
    void execute(uint8_t opcode);
};

} // namespace guardian::vm
