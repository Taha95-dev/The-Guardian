#include <iostream>
#include <guardian/core/molecule.hpp>

using namespace guardian;

int main() {
    std::cout << "🧪 Testing Nested Molecules\n";
    std::cout << "===========================\n\n";
    
    // Create parent molecule
    Molecule parent;
    parent.add_string("name", "Parent");
    parent.add_number("version", 1.0);
    
    // Create child molecule
    auto child = std::make_shared<Molecule>();
    child->add_string("name", "Child");
    child->add_number("value", 42);
    child->add_bool("active", true);
    
    // Store child in parent
    parent.add_molecule("child", child);
    
    // Dump parent (should show nested molecule)
    std::cout << "Parent molecule:\n";
    parent.dump();
    
    // Retrieve and verify child
    if (parent.has_molecule("child")) {
        auto retrieved = parent.get_molecule("child");
        if (retrieved) {
            std::cout << "\nRetrieved child molecule:\n";
            retrieved->dump();
            
            std::cout << "\n  Child name: " << retrieved->get_string("name") << "\n";
            std::cout << "  Child value: " << retrieved->get_number("value") << "\n";
            std::cout << "  Child active: " << (retrieved->get_bool("active") ? "true" : "false") << "\n";
        }
    }
    
    std::cout << "\n✅ Nested molecule test passed!\n";
    return 0;
}
