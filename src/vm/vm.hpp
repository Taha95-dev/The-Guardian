#pragma once
#include "opcodes.hpp"
#include "../core/molecule.hpp"
#include "formats/format_registry.hpp"
#include <stack>
#include <unordered_map>
#include <vector>
#include <string>
#include <variant>
#include <fstream>

namespace guardian::vm {

// Forward declaration for formats namespace
namespace formats {
    void register_guardian_format();
}

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
    bool binary_loaded = false;
    std::string loaded_format;
    FormatRegistry registry;
    
    void push_value(const Value& value);
    Value pop_value();
    Value peek_value() const;
    void binary_op(OpCode op);
    void comparison_op(OpCode op);
    Bytecode molecule_to_bytecode(const Molecule& molecule);
    
public:
    VM() {
        formats::register_guardian_format();  // Now this works!
    }
    
    void load_bytecode(const Bytecode& code);
    void load_bytecode(const Bytecode& code, const std::vector<Value>& consts);
    void load_molecule(const Molecule& molecule);
    
    void run();
    void step();
    
    void reset();
    void set_debug(bool debug) { debug_mode = debug; }
    
    bool is_running() const { return running; }
    size_t get_pc() const { return pc; }
    size_t stack_size() const { return stack.size(); }
    bool is_binary_loaded() const { return binary_loaded; }
    std::string get_loaded_format() const { return loaded_format; }
    
    // Binary format loading — PUBLIC!
    bool load_binary_file(const std::string& path);
    bool load_binary(const std::vector<uint8_t>& data);
    bool load_binary(const std::vector<uint8_t>& data, const std::string& format_name);
    bool save_binary_file(const std::string& path, const std::string& format_name = "Guardian Binary");
    
    void print_stack() const;
    void print_globals() const;
};

} // namespace guardian::vm
