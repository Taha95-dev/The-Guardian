#include "molecule.hpp"
#include <iostream>

namespace guardian {

// ============================================
// STRING STORAGE
// ============================================
void Molecule::add_string(const std::string& name, const std::string& value) {
    string_lut[name] = value;
    total_size += value.size();
}

std::string Molecule::get_string(const std::string& name) const {
    auto it = string_lut.find(name);
    if (it != string_lut.end()) {
        return it->second;
    }
    return "";
}

bool Molecule::has_string(const std::string& name) const {
    return string_lut.find(name) != string_lut.end();
}

// ============================================
// NUMBER STORAGE
// ============================================
void Molecule::add_number(const std::string& name, double value) {
    number_lut[name] = value;
    total_size += sizeof(double);
}

double Molecule::get_number(const std::string& name) const {
    auto it = number_lut.find(name);
    if (it != number_lut.end()) {
        return it->second;
    }
    return 0.0;
}

bool Molecule::has_number(const std::string& name) const {
    return number_lut.find(name) != number_lut.end();
}

// ============================================
// DUMP
// ============================================
void Molecule::dump() const {
    std::cout << "Molecule Contents:\n";
    for (const auto& [name, value] : string_lut) {
        std::cout << "  [string] " << name << " = \"" << value << "\"\n";
    }
    for (const auto& [name, value] : number_lut) {
        std::cout << "  [number] " << name << " = " << value << "\n";
    }
}

} // namespace guardian
