#include <iostream>
#include <cassert>
#include <guardian/core/quark.hpp>

using namespace guardian;

void test_quarks() {
    std::cout << "  Testing quarks...\n";
    
    // Test integer quark
    {
        std::cout << "    Integer quark... ";
        Quark q(42);
        assert(q.type == QuarkType::INT);
        assert(q.int_val == 42);
        assert(q.to_string() == "42");
        std::cout << "✅\n";
    }
    
    // Test float quark
    {
        std::cout << "    Float quark... ";
        Quark q(3.14159);
        assert(q.type == QuarkType::FLOAT64);
        assert(q.float64_val == 3.14159);
        std::cout << "✅\n";
    }
    
    // Test bool quark
    {
        std::cout << "    Bool quark... ";
        Quark q(true);
        assert(q.type == QuarkType::BOOL);
        assert(q.bool_val == true);
        assert(q.to_string() == "true");
        std::cout << "✅\n";
    }
    
    // Test string quark (pointer)
    {
        std::cout << "    String quark... ";
        std::string str = "Hello, Quark!";
        Quark q((void*)str.c_str());
        assert(q.type == QuarkType::POINTER);
        assert(q.to_string().find("ptr:") != std::string::npos);
        std::cout << "✅\n";
    }
    
    // Test quark size
    {
        std::cout << "    Quark size... ";
        Quark q(42);
        assert(q.size() == sizeof(int32_t));
        Quark q2(3.14);
        assert(q2.size() == sizeof(double));
        std::cout << "✅\n";
    }
    
    std::cout << "  ✅ All quark tests passed!\n";
}
