#pragma once
#include "atom.hpp"
#include <vector>
#include <memory>

namespace guardian {

class Molecule {
    std::vector<std::unique_ptr<Atom>> atoms;
    size_t total_size = 0;
    
public:
    void add_atom(std::unique_ptr<Atom> atom);
    size_t size() const;
    size_t atom_count() const;
    
    // NEW: Get all atoms (const reference)
    const std::vector<std::unique_ptr<Atom>>& get_atoms() const {
        return atoms;
    }
};

} // namespace guardian
