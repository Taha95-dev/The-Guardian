#include "molecule.hpp"
#include <iostream>

namespace guardian {

// ============================================
// STRING LUT
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
// ATOM LUT
// ============================================
void Molecule::add_atom(const std::string& name, std::unique_ptr<guardian::Atom> atom) {
    if (atom) {
        total_size += atom->size();
        atom_lut[name] = std::move(atom);
    }
}

guardian::Atom* Molecule::get_atom(const std::string& name) const {
    auto it = atom_lut.find(name);
    if (it != atom_lut.end()) {
        return it->second.get();
    }
    return nullptr;
}

bool Molecule::has_atom(const std::string& name) const {
    return atom_lut.find(name) != atom_lut.end();
}

// ============================================
// NUMBER LUT
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
// BOOL LUT
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
// ARRAY LUT
// ============================================
void Molecule::add_array(const std::string& name, const std::vector<std::string>& value) {
    array_lut[name] = value;
    for (const auto& item : value) {
        total_size += item.size();
    }
}

std::vector<std::string> Molecule::get_array(const std::string& name) const {
    auto it = array_lut.find(name);
    if (it != array_lut.end()) {
        return it->second;
    }
    return {};
}

bool Molecule::has_array(const std::string& name) const {
    return array_lut.find(name) != array_lut.end();
}

// ============================================
// DICTIONARY LUT
// ============================================
void Molecule::add_dict(const std::string& name, 
                        const std::unordered_map<std::string, std::string>& value) {
    dict_lut[name] = value;
    for (const auto& [key, val] : value) {
        total_size += key.size() + val.size();
    }
}

std::unordered_map<std::string, std::string> Molecule::get_dict(const std::string& name) const {
    auto it = dict_lut.find(name);
    if (it != dict_lut.end()) {
        return it->second;
    }
    return {};
}

bool Molecule::has_dict(const std::string& name) const {
    return dict_lut.find(name) != dict_lut.end();
}

// ============================================
// TYPE DETECTION
// ============================================
Molecule::ValueType Molecule::get_type(const std::string& name) const {
    if (string_lut.find(name) != string_lut.end()) return ValueType::STRING;
    if (number_lut.find(name) != number_lut.end()) return ValueType::NUMBER;
    if (bool_lut.find(name) != bool_lut.end()) return ValueType::BOOL;
    if (array_lut.find(name) != array_lut.end()) return ValueType::ARRAY;
    if (dict_lut.find(name) != dict_lut.end()) return ValueType::DICT;
    if (atom_lut.find(name) != atom_lut.end()) return ValueType::ATOM;
    if (atom_lut_shared.find(name) != atom_lut_shared.end()) return ValueType::ATOM;
    return ValueType::NONE;
}

// ============================================
// RELEASE ALL
// ============================================
void Molecule::release_all() {
    for (auto& [name, atom] : atom_lut) {
        if (atom) {
            atom->release();
        }
    }
    atom_lut.clear();
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
    
    for (const auto& [name, value] : bool_lut) {
        std::cout << "  [bool] " << name << " = " << (value ? "true" : "false") << "\n";
    }
    
    for (const auto& [name, value] : array_lut) {
        std::cout << "  [array] " << name << " = [";
        for (size_t i = 0; i < value.size(); i++) {
            std::cout << "\"" << value[i] << "\"";
            if (i < value.size() - 1) std::cout << ", ";
        }
        std::cout << "]\n";
    }
    
    for (const auto& [name, value] : dict_lut) {
        std::cout << "  [dict] " << name << " = {";
        size_t i = 0;
        for (const auto& [key, val] : value) {
            std::cout << "\"" << key << "\": \"" << val << "\"";
            if (i < value.size() - 1) std::cout << ", ";
            i++;
        }
        std::cout << "}\n";
    }
}

// ============================================
// SHARED ATOM LUT
// ============================================
void Molecule::add_atom_shared(const std::string& name, std::shared_ptr<guardian::Atom> atom) {
    if (atom) {
        total_size += atom->size();
        atom_lut_shared[name] = atom;
    }
}

std::shared_ptr<guardian::Atom> Molecule::get_atom_shared(const std::string& name) const {
    auto it = atom_lut_shared.find(name);
    if (it != atom_lut_shared.end()) {
        return it->second;
    }
    return nullptr;
}

bool Molecule::has_atom_shared(const std::string& name) const {
    return atom_lut_shared.find(name) != atom_lut_shared.end();
}

} // namespace guardian
