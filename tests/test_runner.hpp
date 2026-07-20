#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <cmath>
#include <memory>
#include <stdexcept>

namespace guardian::test {

// ============================================
// TEST MACROS
// ============================================

// Forward declaration of runner
class TestRunner;

// Global runner instance
extern TestRunner runner;

#define GUARDIAN_TEST(name) \
    void test_##name(); \
    struct test_##name##_registrar { \
        test_##name##_registrar() { \
            guardian::test::runner.add(#name, test_##name); \
        } \
    }; \
    test_##name##_registrar test_##name##_registrar_instance; \
    void test_##name()

#define ASSERT_EQ(a, b) \
    if ((a) != (b)) { \
        throw std::runtime_error("Assertion failed: " #a " == " #b); \
    }

#define ASSERT_FLOAT_EQ(a, b) \
    if (std::abs((a) - (b)) > 0.00001f) { \
        throw std::runtime_error("Assertion failed: " #a " == " #b); \
    }

#define RUN_TESTS() \
    runner.run()

// ============================================
// TEST RUNNER
// ============================================

struct Test {
    std::string name;
    std::function<void()> fn;
    bool passed = false;
    std::string error;
};

class TestRunner {
    std::vector<Test> tests;
    int passed = 0;
    int failed = 0;
    
public:
    void add(const std::string& name, std::function<void()> fn) {
        tests.push_back({name, fn});
    }
    
    void run() {
        std::cout << "🧪 Running " << tests.size() << " tests..." << std::endl;
        for (auto& test : tests) {
            try {
                test.fn();
                test.passed = true;
                passed++;
                std::cout << "  ✅ " << test.name << std::endl;
            } catch (const std::exception& e) {
                test.passed = false;
                test.error = e.what();
                failed++;
                std::cout << "  ❌ " << test.name << " → " << e.what() << std::endl;
            }
        }
        std::cout << "📊 Passed: " << passed << ", Failed: " << failed << std::endl;
    }
};

} // namespace guardian::test
