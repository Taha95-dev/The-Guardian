#pragma once
#include "atom.hpp"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace guardian {

class Molecule {
public:
    Molecule() : total_size(0) {}
    ~Molecule() = default;
    
    // String LUT
    void add_string(const std::string& name, const std::string& value);
    std::string get_string(const std::string& name) const;
    bool has_string(const std::string& name) const;

    // Atom LUT (for struct fields)
    void add_atom(const std::string& name, std::unique_ptr<guardian::Atom> atom);
    guardian::Atom* get_atom(const std::string& name) const;
    bool has_atom(const std::string& name) const;
    
    // Number LUT
    void add_number(const std::string& name, double value);
    void add_atom_shared(const std::string& name, std::shared_ptr<guardian::Atom> atom);
    std::shared_ptr<guardian::Atom> get_atom_shared(const std::string& name) const;
    bool has_atom_shared(const std::string& name) const;
    std::unordered_map<std::string, std::shared_ptr<guardian::Atom>> atom_lut_shared;
    double get_number(const std::string& name) const;
    bool has_number(const std::string& name) const;
    
    // Bool LUT
    void add_bool(const std::string& name, bool value);
    bool get_bool(const std::string& name) const;
    bool has_bool(const std::string& name) const;
    
    // Array LUT
    void add_array(const std::string& name, const std::vector<std::string>& value);
    std::vector<std::string> get_array(const std::string& name) const;
    bool has_array(const std::string& name) const;
    
    // Dictionary LUT
    void add_dict(const std::string& name, const std::unordered_map<std::string, std::string>& value);
    std::unordered_map<std::string, std::string> get_dict(const std::string& name) const;
    bool has_dict(const std::string& name) const;
    
    // Generic getter (for VM)
    enum class ValueType { STRING, NUMBER, BOOL, ARRAY, DICT, ATOM, NONE };
    ValueType get_type(const std::string& name) const;
    
    // Size
    size_t size() const { return total_size; }
    
    // Debug
    void dump() const;
    
    // Release all atoms
    void release_all();
    
private:
    std::unordered_map<std::string, std::string> string_lut;
    std::unordered_map<std::string, double> number_lut;
    std::unordered_map<std::string, bool> bool_lut;
    std::unordered_map<std::string, std::vector<std::string>> array_lut;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> dict_lut;
    std::unordered_map<std::string, std::unique_ptr<guardian::Atom>> atom_lut;
    
    size_t total_size;
};

} // namespace guardian
