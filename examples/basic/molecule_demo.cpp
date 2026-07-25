#include <guardian/core/molecule.hpp>
#include <iostream>

using namespace guardian;

int main() {
    Molecule mol;
    mol.add_string("name", "Guardian");
    mol.add_number("version", 1.0);
    mol.add_bool("active", true);
    mol.dump();
    return 0;
}
