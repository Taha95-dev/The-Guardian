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
    
    // String storage
    void add_string(const std::string& name, const std::string& value);
    std::string get_string(const std::string& name) const;
    bool has_string(const std::string& name) const;
    
    // Number storage (for VM)
    void add_number(const std::string& name, double value);
    double get_number(const std::string& name) const;
    bool has_number(const std::string& name) const;
    
    // Size
    size_t size() const { return total_size; }
    
    // Debug
    void dump() const;
    
private:
    std::unordered_map<std::string, std::string> string_lut;
    std::unordered_map<std::string, double> number_lut;
    size_t total_size;
};

} // namespace guardian
