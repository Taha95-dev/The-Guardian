#include "molecule.hpp"
#include <memory>
#include <iostream>
#include <cstring>

namespace guardian {

void Molecule::add_atom(std::unique_ptr<Atom> atom) {
    if (atom) {
        total_size += atom->size();
        AtomEntry entry;
        entry.name = "atom_" + std::to_string(atoms.size());
        entry.atom = std::move(atom);
        atoms.push_back(std::move(entry));
    }
}

void Molecule::add_atom(const std::string& name, std::unique_ptr<Atom> atom) {
    if (atom) {
        total_size += atom->size();
        AtomEntry entry;
        entry.name = name;
        entry.atom = std::move(atom);
        name_index[name] = atoms.size();
        atoms.push_back(std::move(entry));
    }
}

Atom* Molecule::get_atom(size_t index) const {
    if (index < atoms.size()) {
        return atoms[index].atom.get();
    }
    return nullptr;
}

Atom* Molecule::get_atom(const std::string& name) const {
    auto it = name_index.find(name);
    if (it != name_index.end() && it->second < atoms.size()) {
        return atoms[it->second].atom.get();
    }
    return nullptr;
}

size_t Molecule::atom_count() const {
    return atoms.size();
}

size_t Molecule::size() const {
    return total_size;
}

void Molecule::register_pointer(void* ptr, const std::string& name) {
    pointer_table[ptr] = name;
}

void Molecule::unregister_pointer(void* ptr) {
    pointer_table.erase(ptr);
}

bool Molecule::is_valid_pointer(void* ptr) const {
    return pointer_table.find(ptr) != pointer_table.end();
}

std::vector<uint8_t> Molecule::serialize() const {
    std::vector<uint8_t> data;
    // TODO: Implement serialization
    return data;
}

void Molecule::deserialize(const std::vector<uint8_t>& data) {
    // TODO: Implement deserialization
}

} // namespace guardian
