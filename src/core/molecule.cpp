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
// BOOL STORAGE
// ============================================
void Molecule::add_bool(const std::string& name, bool value) {
    bool_lut[name] = value;
    total_size += sizeof(bool);
}

bool Molecule::get_bool(const std::string& name) const {
    auto it = bool_lut.find(name);
    if (it != bool_lut.end()) {
        return it->second;
    }
    return false;
}

bool Molecule::has_bool(const std::string& name) const {
    return bool_lut.find(name) != bool_lut.end();
}

// ============================================
// REMOVE
// ============================================
void Molecule::remove(const std::string& name) {
    // Remove from string LUT
    auto it_string = string_lut.find(name);
    if (it_string != string_lut.end()) {
        total_size -= it_string->second.size();
        string_lut.erase(it_string);
    }
    
    // Remove from number LUT
    auto it_number = number_lut.find(name);
    if (it_number != number_lut.end()) {
        total_size -= sizeof(double);
        number_lut.erase(it_number);
    }
    
    // Remove from bool LUT
    auto it_bool = bool_lut.find(name);
    if (it_bool != bool_lut.end()) {
        total_size -= sizeof(bool);
        bool_lut.erase(it_bool);
    }
}

// ============================================
// DUMP - FIXED
// ============================================
void Molecule::dump() const {
    std::cout << "Molecule Contents:\n";
    std::cout << "  Total entries: " << (string_lut.size() + number_lut.size() + bool_lut.size()) << "\n";
    std::cout << "  Total size: " << total_size << " bytes\n\n";
    
    // Dump strings
    if (!string_lut.empty()) {
        std::cout << "  Strings:\n";
        for (const auto& pair : string_lut) {
            std::cout << "    " << pair.first << " = \"" << pair.second << "\"\n";
        }
        std::cout << "\n";
    }
    
    // Dump numbers
    if (!number_lut.empty()) {
        std::cout << "  Numbers:\n";
        for (const auto& pair : number_lut) {
            std::cout << "    " << pair.first << " = " << pair.second << "\n";
        }
        std::cout << "\n";
    }
    
    // Dump bools
    if (!bool_lut.empty()) {
        std::cout << "  Bools:\n";
        for (const auto& pair : bool_lut) {
            std::cout << "    " << pair.first << " = " << (pair.second ? "true" : "false") << "\n";
        }
        std::cout << "\n";
    }
    
    // If nothing in molecule
    if (string_lut.empty() && number_lut.empty() && bool_lut.empty()) {
        std::cout << "  (empty molecule)\n";
    }
}

} // namespace guardian
