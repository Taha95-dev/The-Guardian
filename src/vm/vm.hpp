#pragma once
#include "opcodes.hpp"
#include "../core/molecule.hpp"  // ← ADDED
#include <stack>
#include <unordered_map>
#include <vector>
#include <string>
#include <variant>

namespace guardian::vm {

// ============================================
// FRAME
// ============================================
struct Frame {
    size_t return_address;
    std::unordered_map<std::string, Value> locals;
    
    Frame(size_t ret_addr) : return_address(ret_addr) {}
};

// ============================================
// VIRTUAL MACHINE
// ============================================
class VM {
private:
    size_t pc = 0;
    Bytecode bytecode;
    std::stack<Value> stack;
    std::stack<Frame> frames;
    std::unordered_map<std::string, Value> globals;
    std::vector<Value> constants;
    bool running = false;
    bool debug_mode = false;
    
    void push_value(const Value& value);
    Value pop_value();
    Value peek_value() const;
    void binary_op(OpCode op);
    void comparison_op(OpCode op);
    
    // Helper: Convert molecule to bytecode
    Bytecode molecule_to_bytecode(const Molecule& molecule);
    
public:
    VM() = default;
    
    void load_bytecode(const Bytecode& code);
    void load_bytecode(const Bytecode& code, const std::vector<Value>& consts);
    
    // Load a molecule directly (converts it to bytecode automatically)
    void load_molecule(const Molecule& molecule);
    
    void run();
    void step();
    
    void reset();
    void set_debug(bool debug) { debug_mode = debug; }
    
    bool is_running() const { return running; }
    size_t get_pc() const { return pc; }
    size_t stack_size() const { return stack.size(); }
    
    void print_stack() const;
    void print_globals() const;
};

} // namespace guardian::vm
