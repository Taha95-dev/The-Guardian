#pragma once
#include "opcodes.hpp"
#include "../core/molecule.hpp"
#include "../core/quark.hpp"
#include "../core/atom.hpp"
#include "../atoms/text/string_atom.hpp"
#include "../atoms/primitive/int_atom.hpp"
#include "../atoms/primitive/float_atom.hpp"
#include "../atoms/primitive/bool_atom.hpp"
#include "../atoms/primitive/char_atom.hpp"
#include "formats/format_registry.hpp"
#include <stack>
#include <unordered_map>
#include <vector>
#include <string>
#include <variant>
#include <fstream>
#include <memory>

namespace guardian::vm {

// ============================================
// VALUE — Can be Quark, Atom, Array, Molecule, or Struct Pointer
// ============================================
struct Value {
    bool is_quark;
    bool is_array;
    bool is_molecule;
    bool is_struct_ptr;          // NEW: pointer to struct on heap
    bool is_dict;
    guardian::Quark quark_data;
    std::unordered_map<std::string, Value> dict_data;
    std::shared_ptr<guardian::Atom> atom_data;
    std::vector<Value> array_data;
    std::shared_ptr<guardian::Molecule> molecule_data;
    void* struct_ptr;            // NEW: raw pointer to struct on heap
    void* dict_ptr;
    
    Value() : is_quark(true), is_array(false), is_molecule(false), is_struct_ptr(false), struct_ptr(nullptr) {}

    // Quark constructors
    Value(int v) : is_quark(true), is_array(false), is_molecule(false), is_struct_ptr(false), quark_data(v), struct_ptr(nullptr) {}
    Value(unsigned int v) : is_quark(true), is_array(false), is_molecule(false), is_struct_ptr(false), quark_data(v), struct_ptr(nullptr) {}
    Value(int64_t v) : is_quark(true), is_array(false), is_molecule(false), is_struct_ptr(false), quark_data(v), struct_ptr(nullptr) {}
    Value(uint64_t v) : is_quark(true), is_array(false), is_molecule(false), is_struct_ptr(false), quark_data(v), struct_ptr(nullptr) {}
    Value(float v) : is_quark(true), is_array(false), is_molecule(false), is_struct_ptr(false), quark_data(v), struct_ptr(nullptr) {}
    Value(double v) : is_quark(true), is_array(false), is_molecule(false), is_struct_ptr(false), quark_data(v), struct_ptr(nullptr) {}
    Value(bool v) : is_quark(true), is_array(false), is_molecule(false), is_struct_ptr(false), quark_data(v), struct_ptr(nullptr) {}
    Value(char v) : is_quark(true), is_array(false), is_molecule(false), is_struct_ptr(false), quark_data(v), struct_ptr(nullptr) {}
    
    // Atom constructor
    Value(std::shared_ptr<guardian::Atom> a) 
        : is_quark(false), is_array(false), is_molecule(false), is_struct_ptr(false), atom_data(a), struct_ptr(nullptr) {}
    
    // Array constructor
    Value(const std::vector<Value>& arr) 
        : is_quark(false), is_array(true), is_molecule(false), is_struct_ptr(false), array_data(arr), struct_ptr(nullptr) {}
    
    // Molecule constructor
    Value(std::shared_ptr<guardian::Molecule> m) 
        : is_quark(false), is_array(false), is_molecule(true), is_struct_ptr(false), molecule_data(m), struct_ptr(nullptr) {} 

    // Dictionary constructor
    Value(const std::unordered_map<std::string, Value>& d) 
        : is_quark(false), is_array(false), is_molecule(false), is_struct_ptr(false), 
          is_dict(true), dict_data(d), dict_ptr(nullptr) {}
    
    

    // Pointer type enum (scoped, no leaking!)
    enum class PtrType {
        STRUCT,
        DICT
    };
    
    // Single pointer constructor with type flag
    Value(void* ptr, PtrType type) 
        : is_quark(false), is_array(false), is_molecule(false), 
          is_struct_ptr(type == PtrType::STRUCT), 
          is_dict(type == PtrType::DICT),
          struct_ptr(type == PtrType::STRUCT ? ptr : nullptr), 
          dict_ptr(type == PtrType::DICT ? ptr : nullptr) {}

    std::string to_string() const {
        if (is_quark) {
            return quark_data.to_string();
        }
        if (is_array) {
            std::string result = "[";
            for (size_t i = 0; i < array_data.size(); i++) {
                result += array_data[i].to_string();
                if (i < array_data.size() - 1) result += ", ";
            }
            result += "]";
            return result;
        }
        if (is_molecule) {
            return "Molecule{...}";
        }
        if (is_struct_ptr) {
            return "Struct*(" + std::to_string(reinterpret_cast<uintptr_t>(struct_ptr)) + ")";
        }
        if (atom_data) {
            // Check for StringAtom (strings and characters)
            if (auto str_atom = std::dynamic_pointer_cast<guardian::StringAtom>(atom_data)) {
                std::string value = str_atom->get();
                if (value.rfind("char:", 0) == 0) {
                    return value.substr(5);  // Return just the character
                }
                return value;  // Return the string
            }
            // Check for IntAtom
            if (auto int_atom = std::dynamic_pointer_cast<guardian::IntAtom>(atom_data)) {
                return std::to_string(int_atom->get());
            }
            // Check for FloatAtom
            if (auto float_atom = std::dynamic_pointer_cast<guardian::FloatAtom>(atom_data)) {
                return std::to_string(float_atom->get());
            }
            // Check for BoolAtom
            if (auto bool_atom = std::dynamic_pointer_cast<guardian::BoolAtom>(atom_data)) {
                return bool_atom->get() ? "true" : "false";
            }
            // Check for CharAtom
            if (auto char_atom = std::dynamic_pointer_cast<guardian::CharAtom>(atom_data)) {
                return std::string(1, char_atom->get());
            }
            // Fallback
            return std::string(atom_data->name());
        }
        return "null";
    }
    
    size_t size() const {
        if (is_quark) {
            return quark_data.size();
        }
        if (is_array) {
            size_t total = 0;
            for (const auto& val : array_data) {
                total += val.size();
            }
            return total;
        }
        if (is_molecule) {
            return molecule_data ? molecule_data->size() : 0;
        }
        if (is_struct_ptr) {
            return sizeof(void*);
        }
        return atom_data ? atom_data->size() : 0;
    }
    
    std::shared_ptr<guardian::Atom> toAtom() const {
        if (is_quark) {
            switch (quark_data.type) {
                case guardian::QuarkType::INT:
                    return std::make_shared<guardian::IntAtom>(quark_data.int_val);
                case guardian::QuarkType::FLOAT:
                    return std::make_shared<guardian::FloatAtom>(quark_data.float_val);
                case guardian::QuarkType::FLOAT64:
                    return std::make_shared<guardian::FloatAtom>(static_cast<float>(quark_data.float64_val));
                case guardian::QuarkType::BOOL:
                    return std::make_shared<guardian::BoolAtom>(quark_data.bool_val);
                case guardian::QuarkType::CHAR:
                    return std::make_shared<guardian::CharAtom>(quark_data.char_val);
                default:
                    return nullptr;
            }
        }
        if (is_molecule) {
            return nullptr;
        }
        if (is_struct_ptr) {
            return nullptr;
        }
        return atom_data;
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
    
    // Array operations
    void executeMakeArray();
    void executeArrayGet();
    void executeArraySet();
    
    // Molecule operations
    void executeMakeMolecule();
    void executeStoreAtom();
    void executeGetAtom();
    
    // Dictionary operations
    void executeMakeDict();
    void executeDictGet();
    
    std::string readString();
};

} // namespace guardian::vm
