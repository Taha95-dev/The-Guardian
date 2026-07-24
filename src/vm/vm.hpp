#pragma once

#include <cmath>
#include <vector>
#include <string>
#include <memory>
#include <cstdint>
#include <cstring>
#include <unordered_map>
#include "../core/molecule.hpp"

namespace guardian::vm {

// ============================================
// OPCODES
// ============================================
enum class Opcode : uint8_t {
    HALT = 0x00,
    NOP = 0x01,
    
    // Stack operations
    PUSH_INT = 0x10,
    PUSH_FLOAT = 0x11,
    PUSH_BOOL = 0x12,
    PUSH_STRING = 0x13,
    PUSH_NULL = 0x14,
    POP = 0x15,
    DUP = 0x16,
    SWAP = 0x17,
    ROT = 0x18,
    
    // Arithmetic
    ADD = 0x20,
    SUB = 0x21,
    MUL = 0x22,
    DIV = 0x23,
    MOD = 0x24,
    NEG = 0x25,
    INC = 0x26,
    DEC = 0x27,
    
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
    STORE = 0x60,
    LOAD = 0x61,
    STORE_GLOBAL = 0x62,
    LOAD_GLOBAL = 0x63,
    
    // Functions
    DEFINE_FN = 0x70,
    CALL_FN = 0x71,
    RETURN = 0x72,
    
    // Print
    PRINT = 0x80,
    PRINTLN = 0x81,
    
    // Arrays
    MAKE_ARRAY = 0x90,
    ARRAY_GET = 0x91,
    ARRAY_SET = 0x92,
    ARRAY_PUSH = 0x93,
    ARRAY_POP = 0x94,
    ARRAY_LEN = 0x95,
    
    // Dictionaries
    MAKE_DICT = 0xA0,
    DICT_GET = 0xA1,
    DICT_SET = 0xA2,
    DICT_KEYS = 0xA3,
    DICT_VALUES = 0xA4,
    
    // Molecules
    MAKE_MOLECULE = 0xB0,
    STORE_ATOM = 0xB1,
    GET_ATOM = 0xB2,
    REMOVE_ATOM = 0xB3,  // ADD THIS
};

// ============================================
// VALUE
// ============================================
struct Value {
    bool is_quark;
    bool is_atom;
    
    // Quark data (primitive values)
    struct QuarkData {
        enum Type { INT, FLOAT, BOOL, STRING, NONE } type;
        union {
            int int_val;
            double float_val;
            bool bool_val;
        };
        std::string string_val;
        
        QuarkData() : type(NONE), int_val(0) {}
        QuarkData(int v) : type(INT), int_val(v) {}
        QuarkData(double v) : type(FLOAT), float_val(v) {}
        QuarkData(bool v) : type(BOOL), bool_val(v) {}
        QuarkData(const std::string& v) : type(STRING), string_val(v) {}
    } quark_data;
    
    // Atom data (heap-allocated)
    struct AtomData {
        enum Type { ARRAY, DICT, MOLECULE, CUSTOM } type;
        
        // For arrays
        std::vector<Value> array_data;
        
        // For dictionaries
        std::unordered_map<std::string, Value> dict_data;
        
        // For molecules (container of atoms)
        std::shared_ptr<guardian::Molecule> molecule_data;
        
        // For custom atoms
        void* custom_data;
        
        AtomData() : type(ARRAY), custom_data(nullptr) {}
        AtomData(const std::vector<Value>& arr) : type(ARRAY), array_data(arr), custom_data(nullptr) {}
        AtomData(const std::unordered_map<std::string, Value>& d) : type(DICT), dict_data(d), custom_data(nullptr) {}
        AtomData(std::shared_ptr<guardian::Molecule> m) : type(MOLECULE), molecule_data(m), custom_data(nullptr) {}
    };
    std::shared_ptr<AtomData> atom_data;
    
    Value() : is_quark(true), is_atom(false) {}
    
    // Quark constructors
    Value(int v) : is_quark(true), is_atom(false), quark_data(v) {}
    Value(double v) : is_quark(true), is_atom(false), quark_data(v) {}
    Value(bool v) : is_quark(true), is_atom(false), quark_data(v) {}
    Value(const std::string& v) : is_quark(true), is_atom(false), quark_data(v) {}
    
    // Atom constructors (heap allocated)
    Value(const std::vector<Value>& arr) 
        : is_quark(false), is_atom(true), atom_data(std::make_shared<AtomData>(arr)) {}
    
    Value(const std::unordered_map<std::string, Value>& d) 
        : is_quark(false), is_atom(true), atom_data(std::make_shared<AtomData>(d)) {}
    
    Value(std::shared_ptr<guardian::Molecule> m) 
        : is_quark(false), is_atom(true), atom_data(std::make_shared<AtomData>(m)) {}
    
    std::string to_string() const {
        if (is_quark) {
            switch (quark_data.type) {
                case QuarkData::INT: return std::to_string(quark_data.int_val);
                case QuarkData::FLOAT: return std::to_string(quark_data.float_val);
                case QuarkData::BOOL: return quark_data.bool_val ? "true" : "false";
                case QuarkData::STRING: return quark_data.string_val;
                default: return "null";
            }
        }
        
        if (is_atom && atom_data) {
            switch (atom_data->type) {
                case AtomData::ARRAY: {
                    std::string result = "[";
                    for (size_t i = 0; i < atom_data->array_data.size(); i++) {
                        result += atom_data->array_data[i].to_string();
                        if (i < atom_data->array_data.size() - 1) result += ", ";
                    }
                    result += "]";
                    return result;
                }
                case AtomData::DICT: {
                    std::string result = "{";
                    bool first = true;
                    for (const auto& [key, val] : atom_data->dict_data) {
                        if (!first) result += ", ";
                        result += key + ": " + val.to_string();
                        first = false;
                    }
                    result += "}";
                    return result;
                }
                case AtomData::MOLECULE:
                    return "Molecule{...}";
                default:
                    return "Atom{...}";
            }
        }
        
        return "null";
    }
};

// ============================================
// VIRTUAL MACHINE
// ============================================
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
    Value peek() const;
    
    void setVariable(const std::string& name, const Value& value);
    Value getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    
private:
    std::vector<uint8_t> bytecode;
    size_t pc;
    std::vector<Value> stack;
    bool running;
    std::shared_ptr<guardian::Molecule> main_molecule;
    
    void execute(uint8_t opcode);
    void executeAdd();
    void executeSub();
    void executeMul();
    void executeDiv();
    void executeMod();
    void executePrint();
    void executePrintln();
    
    // Control flow
    void executeJmp();
    void executeJmpIf();
    void executeJmpIfNot();
    
    // Arrays
    void executeMakeArray();
    void executeArrayGet();
    void executeArraySet();
    void executeArrayPush();
    void executeArrayPop();
    void executeArrayLen();
    
    // Dictionaries
    void executeMakeDict();
    void executeDictGet();
    void executeDictSet();
    void executeDictKeys();
    void executeDictValues();
    
    // Molecules
    void executeMakeMolecule();
    void executeStoreAtom();
    void executeGetAtom();
    void executeRemoveAtom();  // ADD THIS
    
    std::string readString();
};

} // namespace guardian::vm
