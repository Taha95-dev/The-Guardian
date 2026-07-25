#pragma once
#include <vector>
#include <string>
#include <memory>
#include <cstdint>
#include <unordered_map>
#include "../core/molecule.hpp"

namespace guardian::vm {

// ── Opcodes ──
enum class Opcode : uint8_t {
    HALT = 0xFF,
    NOP = 0x00,
    PUSH_INT = 0x10,
    PUSH_FLOAT = 0x11,
    PUSH_BOOL = 0x12,
    PUSH_STRING = 0x13,
    PUSH_NULL = 0x14,
    POP = 0x15,
    DUP = 0x16,
    SWAP = 0x17,
    ADD = 0x20,
    SUB = 0x21,
    MUL = 0x22,
    DIV = 0x23,
    MOD = 0x24,
    PRINT = 0x80,
    NEWLINE = 0x83,
    SPACE = 0x82,
    STORE = 0x60,
    LOAD = 0x61,
    JMP = 0x50,
    JMP_IF = 0x51,
    JMP_IF_NOT = 0x52,
};

// ── Value ──
struct Value {
    bool is_quark;
    bool is_atom;
    
    struct QuarkData {
        enum Type { INT, FLOAT, BOOL, STRING, NONE } type;
        union {
            int int_val;
            double float_val;
            bool bool_val;
        };
        std::string string_val;  // Outside union!
        
        QuarkData() : type(NONE), int_val(0) {}
        QuarkData(int v) : type(INT), int_val(v) {}
        QuarkData(double v) : type(FLOAT), float_val(v) {}
        QuarkData(bool v) : type(BOOL), bool_val(v) {}
        QuarkData(const std::string& v) : type(STRING), string_val(v) {}
    } quark_data;
    
    struct AtomData {
        enum Type { ARRAY, DICT, MOLECULE, CUSTOM } type;
        std::vector<Value> array_data;
        std::unordered_map<std::string, Value> dict_data;
        std::shared_ptr<guardian::Molecule> molecule_data;
        void* custom_data;
        
        AtomData() : type(ARRAY), custom_data(nullptr) {}
        AtomData(const std::vector<Value>& arr) : type(ARRAY), array_data(arr), custom_data(nullptr) {}
        AtomData(const std::unordered_map<std::string, Value>& d) : type(DICT), dict_data(d), custom_data(nullptr) {}
        AtomData(std::shared_ptr<guardian::Molecule> m) : type(MOLECULE), molecule_data(m), custom_data(nullptr) {}
    };
    std::shared_ptr<AtomData> atom_data;
    
    Value() : is_quark(true), is_atom(false) {}
    Value(int v) : is_quark(true), is_atom(false), quark_data(v) {}
    Value(double v) : is_quark(true), is_atom(false), quark_data(v) {}
    Value(bool v) : is_quark(true), is_atom(false), quark_data(v) {}
    Value(const std::string& v) : is_quark(true), is_atom(false), quark_data(v) {}
    Value(const std::vector<Value>& arr) : is_quark(false), is_atom(true), atom_data(std::make_shared<AtomData>(arr)) {}
    Value(const std::unordered_map<std::string, Value>& d) : is_quark(false), is_atom(true), atom_data(std::make_shared<AtomData>(d)) {}
    Value(std::shared_ptr<guardian::Molecule> m) : is_quark(false), is_atom(true), atom_data(std::make_shared<AtomData>(m)) {}
    
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
                    for (const auto& pair : atom_data->dict_data) {
                        if (!first) result += ", ";
                        result += pair.first + ": " + pair.second.to_string();
                        first = false;
                    }
                    result += "}";
                    return result;
                }
                case AtomData::MOLECULE: return "Molecule{...}";
                default: return "Atom{...}";
            }
        }
        return "null";
    }
};

// ── VM ──
class VM {
public:
    VM();
    ~VM();
    
    void load(const std::vector<uint8_t>& bytecode, size_t entry = 0);
    void run();
    void reset();
    bool isRunning() const;
    
    void push(const Value& val);
    Value pop();
    Value peek() const;
    
private:
    std::vector<uint8_t> bytecode;
    size_t pc;
    std::vector<Value> stack;
    bool running;
    std::shared_ptr<guardian::Molecule> main_molecule;
    
    // ── Opcode handlers ──
    void handleHalt();
    void handleNop();
    void handlePushInt();
    void handlePushFloat();
    void handlePushBool();
    void handlePushString();
    void handlePushNull();
    void handlePop();
    void handleDup();
    void handleSwap();
    void handleAdd();
    void handleSub();
    void handleMul();
    void handleDiv();
    void handleMod();
    void handlePrint();
    void handlePrintln();
    void handleSpace();
    void handleNewline();
    void handleStore();
    void handleLoad();
    void handleJmp();
    void handleJmpIf();
    void handleJmpIfNot();
    
    // ── Helpers ──
    uint32_t readUint32();
    std::string readString();
    bool popBool();
    double popFloat();
    int popInt();
    Value popValue();
    
    void dispatch(uint8_t opcode);
};

} // namespace guardian::vm
