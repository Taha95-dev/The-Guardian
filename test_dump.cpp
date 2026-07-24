#include <iostream>
#include <guardian/core/molecule.hpp>

using namespace guardian;

int main() {
    std::cout << "Testing Molecule with dump()\n";
    std::cout << "============================\n\n";
    
    Molecule mol;
    mol.add_string("name", "Guardian");
    mol.add_number("version", 1.0);
    mol.add_number("count", 42);
    mol.add_bool("active", true);
    mol.add_bool("debug", false);
    
    mol.dump();
    
    std::cout << "\n✅ Dump test passed!\n";
    return 0;
}
