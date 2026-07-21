#pragma once
#include "atom.hpp"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace guardian {

// ============================================
// MOLECULE — Composable atoms with LUT
// ============================================
class Molecule {
public:
    struct AtomEntry {
        std::string name;
        std::unique_ptr<Atom> atom;
    };
    
    Molecule() : total_size(0) {}
    ~Molecule() = default;
    
    // Add an atom (takes ownership)
    void add_atom(std::unique_ptr<Atom> atom);
    void add_atom(const std::string& name, std::unique_ptr<Atom> atom);
    
    // Get an atom by index
    Atom* get_atom(size_t index) const;
    
    // Get an atom by name
    Atom* get_atom(const std::string& name) const;
    
    // Get all atoms
    const std::vector<AtomEntry>& get_atoms() const { return atoms; }
    size_t atom_count() const;
    size_t size() const;
    
    // Pointer tracking (LUT)
    void register_pointer(void* ptr, const std::string& name = "");
    void unregister_pointer(void* ptr);
    bool is_valid_pointer(void* ptr) const;
    
    // Serialization
    std::vector<uint8_t> serialize() const;
    void deserialize(const std::vector<uint8_t>& data);
    
private:
    std::vector<AtomEntry> atoms;
    std::unordered_map<std::string, size_t> name_index;
    std::unordered_map<void*, std::string> pointer_table;
    size_t total_size;
};

} // namespace guardian
