#pragma once
#include "../../core/atom.hpp"
#include "../../core/molecule.hpp"
#include <memory>
#include <vector>
#include <cstring>

namespace guardian {

// An atom that wraps a molecule — allowing molecules to be treated as atoms
class MoleculeAtom : public Atom {
    std::unique_ptr<Molecule> molecule;
    
public:
    MoleculeAtom() : molecule(std::make_unique<Molecule>()) {}
    explicit MoleculeAtom(std::unique_ptr<Molecule> mol) : molecule(std::move(mol)) {}
    
    size_t size() const override { return molecule ? molecule->size() : 0; }
    
    std::vector<uint8_t> serialize() const override {
        if (!molecule) return {};
        // Molecule doesn't have serialize, so we'll just return the size
        std::vector<uint8_t> bytes;
        bytes.resize(sizeof(size_t));
        size_t s = molecule->size();
        std::memcpy(bytes.data(), &s, sizeof(size_t));
        return bytes;
    }
    
    void deserialize(const std::vector<uint8_t>& data) override {
        // TODO: Implement deserialization
    }
    
    const char* name() const override { return "MoleculeAtom"; }
    
    // NEW: Add the required type() method
    AtomType type() const override { return AtomType::CUSTOM; }
    
    Molecule* get_molecule() const { return molecule.get(); }
    void set_molecule(std::unique_ptr<Molecule> mol) { molecule = std::move(mol); }
};

} // namespace guardian
