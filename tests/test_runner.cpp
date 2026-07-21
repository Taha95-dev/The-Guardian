#include <iostream>

// Declare test functions
void test_atoms();
void test_memory();

int main() {
    std::cout << "🧪 Running Guardian Tests...\n\n";
    
    std::cout << "=== Atom Tests ===\n";
    test_atoms();
    
    std::cout << "\n=== Memory Tests ===\n";
    test_memory();
    
    std::cout << "\n✅ All tests passed!\n";
    return 0;
}
