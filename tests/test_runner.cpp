#include <iostream>

// Forward declarations
void test_atoms();
void test_quarks();
void test_format();
void test_memory();

int main() {
    std::cout << "🧪 Running Guardian Tests\n";
    std::cout << "==========================\n\n";
    
    int passed = 0;
    int failed = 0;
    
    auto run_test = [&](const char* name, void (*test_func)()) {
        std::cout << "Running " << name << "...\n";
        try {
            test_func();
            std::cout << "✅ " << name << " passed\n";
            passed++;
        } catch (const std::exception& e) {
            std::cout << "❌ " << name << " failed: " << e.what() << "\n";
            failed++;
        }
        std::cout << "\n";
    };
    
    run_test("atoms", test_atoms);
    run_test("quarks", test_quarks);
    run_test("format", test_format);
    run_test("memory", test_memory);
    
    std::cout << "==========================\n";
    std::cout << "Results: " << passed << " passed, " << failed << " failed\n";
    
    return failed > 0 ? 1 : 0;
}
