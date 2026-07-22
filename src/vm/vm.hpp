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
// VALUE — Can be Quark (stack) or Atom (heap) or Array
// ============================================
struct Value {
    bool is_quark;
    bool is_array;
    guardian::Quark quark_data;
    std::shared_ptr<guardian::Atom> atom_data;
    std::vector<Value> array_data;  // Store array elements directly
    
    Value() : is_quark(true), is_array(false) {}
    
    // Quark constructors
    Value(int v) : is_quark(true), is_array(false), quark_data(v) {}
    Value(unsigned int v) : is_quark(true), is_array(false), quark_data(v) {}
    Value(int64_t v) : is_quark(true), is_array(false), quark_data(v) {}
    Value(uint64_t v) : is_quark(true), is_array(false), quark_data(v) {}
    Value(float v) : is_quark(true), is_array(false), quark_data(v) {}
    Value(double v) : is_quark(true), is_array(false), quark_data(v) {}
    Value(bool v) : is_quark(true), is_array(false), quark_data(v) {}
    Value(char v) : is_quark(true), is_array(false), quark_data(v) {}
    
    // Atom constructor
    Value(std::shared_ptr<guardian::Atom> a) : is_quark(false), is_array(false), atom_data(a) {}
    
    // Array constructor
    Value(const std::vector<Value>& arr) : is_quark(false), is_array(true), array_data(arr) {}
    
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
        if (atom_data) {
            if (auto str_atom = std::dynamic_pointer_cast<guardian::StringAtom>(atom_data)) {
                std::string value = str_atom->get();
                if (value.rfind("char:", 0) == 0) {
                    return value.substr(5);
                }
                return value;
            }
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
    
    std::string readString();
};

} // namespace guardian::vm
