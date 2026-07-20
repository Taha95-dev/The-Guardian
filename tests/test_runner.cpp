#include "test_runner.hpp"
#include <iostream>

namespace guardian::test {

// Define the global runner
TestRunner runner;

} // namespace guardian::test

int main() {
    std::cout << "🧪 The Guardian Test Suite" << std::endl;
    std::cout << "═══════════════════════════" << std::endl;
    
    // The runner is already populated by the registrars
    guardian::test::RUN_TESTS();
    
    return 0;
}
