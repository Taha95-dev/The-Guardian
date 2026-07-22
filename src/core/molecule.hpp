#pragma once
#include "atom.hpp"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace guardian {

// ============================================
// TYPE-TAGGED STORAGE WITH MULTIPLE LUTS
// ============================================
class Molecule {
public:
    Molecule() : total_size(0) {}
    ~Molecule() = default;
    
    // String LUT (for strings and characters)
    void add_string(const std::string& name, const std::string& value);
    std::string get_string(const std::string& name) const;
    bool has_string(const std::string& name) const;
    
    // Number LUT (for ints and floats)
    void add_number(const std::string& name, double value);
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
    enum class ValueType { STRING, NUMBER, BOOL, ARRAY, DICT, NONE };
    ValueType get_type(const std::string& name) const;
    
    // Size
    size_t size() const { return total_size; }
    
    // Debug
    void dump() const;
    
private:
    // Type-tagged storage
    std::unordered_map<std::string, std::string> string_lut;
    std::unordered_map<std::string, double> number_lut;
    std::unordered_map<std::string, bool> bool_lut;
    std::unordered_map<std::string, std::vector<std::string>> array_lut;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> dict_lut;
    
    size_t total_size;
};

} // namespace guardian
