#include "molecule.hpp"
#include <memory>

namespace guardian {

void Molecule::add_atom(std::unique_ptr<Atom> atom) {
    if (atom) {
        total_size += atom->size();
        atoms.push_back(std::move(atom));
    }
}

size_t Molecule::size() const {
    return total_size;
}

size_t Molecule::atom_count() const {
    return atoms.size();
}

} // namespace guardian
