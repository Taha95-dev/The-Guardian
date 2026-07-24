#pragma once
#include "../../core/atom.hpp"
#include "../../core/molecule.hpp"
#include <vector>
#include <memory>
#include <cstring>

namespace guardian {

class MoleculeAtom : public Atom {
    std::shared_ptr<Molecule> molecule;
    AtomType type() const override { return AtomType::CUSTOM; }
    
public:
    MoleculeAtom() : molecule(std::make_shared<Molecule>()) {}
    explicit MoleculeAtom(std::shared_ptr<Molecule> m) : molecule(m) {}
    
    size_t size() const override {
        return molecule ? molecule->size() : 0;
    }
    
    std::vector<uint8_t> serialize() const override {
        std::vector<uint8_t> data;
        
        if (!molecule) return data;
        
        // For now, we'll serialize the molecule's atom count
        // and each atom's type and data
        // This is a simplified version
        
        // Count atoms
        uint32_t count = 0;
        // We'd need to iterate over molecule's atoms
        // For now, store count as 0 and return
        data.insert(data.end(), 
                   reinterpret_cast<const uint8_t*>(&count),
                   reinterpret_cast<const uint8_t*>(&count) + 4);
        
        return data;
    }
    
    void deserialize(const std::vector<uint8_t>& data) override {
        if (data.size() < 4) return;
        
        molecule = std::make_shared<Molecule>();
        
        // Read atom count
        uint32_t count;
        std::memcpy(&count, data.data(), 4);
        size_t pos = 4;
        
        for (uint32_t i = 0; i < count && pos < data.size(); i++) {
            // Read atom type
            if (pos >= data.size()) break;
            uint8_t type_byte = data[pos++];
            
            // Read atom size
            if (pos + 4 > data.size()) break;
            uint32_t atom_size;
            std::memcpy(&atom_size, data.data() + pos, 4);
            pos += 4;
            
            // Read atom data
            if (pos + atom_size > data.size()) break;
            std::vector<uint8_t> atom_data(data.begin() + pos, data.begin() + pos + atom_size);
            pos += atom_size;
            
            // Create atom based on type
            std::unique_ptr<Atom> atom;
            switch (static_cast<AtomType>(type_byte)) {
                case AtomType::INT:
                    atom = std::make_unique<IntAtom>();
                    atom->deserialize(atom_data);
                    break;
                case AtomType::FLOAT:
                    atom = std::make_unique<FloatAtom>();
                    atom->deserialize(atom_data);
                    break;
                case AtomType::BOOL:
                    atom = std::make_unique<BoolAtom>();
                    atom->deserialize(atom_data);
                    break;
                case AtomType::CHAR:
                    atom = std::make_unique<CharAtom>();
                    atom->deserialize(atom_data);
                    break;
                case AtomType::STRING:
                    atom = std::make_unique<StringAtom>();
                    atom->deserialize(atom_data);
                    break;
                case AtomType::ARRAY:
                    atom = std::make_unique<ArrayAtom>();
                    atom->deserialize(atom_data);
                    break;
                default:
                    continue;
            }
            
            if (atom) {
                // Store atom in molecule using add_atom
                molecule->add_atom(std::move(atom));
            }
        }
    }
    
    const char* name() const override { return "MoleculeAtom"; }
    
    std::shared_ptr<Molecule> get_molecule() const { return molecule; }
    void set_molecule(std::shared_ptr<Molecule> m) { molecule = m; }
};

} // namespace guardian
