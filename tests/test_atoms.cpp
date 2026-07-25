#include <iostream>
#include <cassert>
#include <memory>
#include "../src/atoms/numeric/int64_atom.hpp"
#include "../src/atoms/numeric/float64_atom.hpp"
#include "../src/atoms/numeric/uint_atom.hpp"
#include "../src/atoms/primitive/bool_atom.hpp"
#include "../src/atoms/primitive/char_atom.hpp"
#include "../src/atoms/text/string_atom.hpp"

using namespace guardian;

void test_atoms() {
    std::cout << "  Testing atoms...\n";
    
    // Test Int64Atom
    {
        std::cout << "    Int64Atom... ";
        auto atom = std::make_shared<Int64Atom>(42);
        assert(atom->get() == 42);
        std::cout << "✅\n";
    }
    
    // Test Float64Atom
    {
        std::cout << "    Float64Atom... ";
        auto atom = std::make_shared<Float64Atom>(3.14159);
        assert(atom->get() == 3.14159);
        std::cout << "✅\n";
    }
    
    // Test UIntAtom
    {
        std::cout << "    UIntAtom... ";
        auto atom = std::make_shared<UIntAtom>(42u);
        assert(atom->get() == 42u);
        std::cout << "✅\n";
    }
    
    // Test BoolAtom
    {
        std::cout << "    BoolAtom... ";
        auto atom = std::make_shared<BoolAtom>(true);
        assert(atom->get() == true);
        std::cout << "✅\n";
    }
    
    // Test CharAtom
    {
        std::cout << "    CharAtom... ";
        auto atom = std::make_shared<CharAtom>('A');
        assert(atom->get() == 'A');
        std::cout << "✅\n";
    }
    
    // Test StringAtom
    {
        std::cout << "    StringAtom... ";
        auto atom = std::make_shared<StringAtom>("Hello, Guardian!");
        assert(atom->get() == "Hello, Guardian!");
        std::cout << "✅\n";
    }
    
    std::cout << "  ✅ All atom tests passed!\n";
}
