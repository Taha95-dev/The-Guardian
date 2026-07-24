#include <iostream>
#include <vector>
#include <string>

// Forward declarations of all test functions
void test_atoms();
void test_quarks();
void test_format();
void test_memory();
void test_runner();

int main() {
    std::cout << "🧪 Guardian Test Suite\n";
    std::cout << "======================\n\n";
    
    std::vector<std::pair<std::string, void(*)()>> tests = {
        {"Atoms", test_atoms},
        {"Quarks", test_quarks},
        {"Format", test_format},
        {"Memory", test_memory},
        {"Runner", test_runner}
    };
    
    int passed = 0;
    int failed = 0;
    
    for (const auto& test : tests) {
        std::cout << "Running " << test.first << " tests...\n";
        try {
            test.second();
            std::cout << "✅ " << test.first << " tests passed\n\n";
            passed++;
        } catch (const std::exception& e) {
            std::cout << "❌ " << test.first << " tests failed: " << e.what() << "\n\n";
            failed++;
        }
    }
    
    std::cout << "========================\n";
    std::cout << "Summary: " << passed << " passed, " << failed << " failed\n";
    
    return failed > 0 ? 1 : 0;
}
