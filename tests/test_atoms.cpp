#include <iostream>
#include "../src/core/atom.hpp"
#include "../src/atoms/primitive/int_atom.hpp"
#include "../src/atoms/primitive/float_atom.hpp"
#include "../src/atoms/primitive/bool_atom.hpp"
#include "../src/atoms/primitive/char_atom.hpp"
#include "../src/atoms/text/string_atom.hpp"
#include "../src/core/molecule.hpp"

void test_atoms() {
    using namespace guardian;
    
    std::cout << "  Testing atoms...\n";
    
    IntAtom i(42);
    FloatAtom f(3.14f);
    StringAtom s("Hello");
    BoolAtom b(true);
    CharAtom c('A');
    
    std::cout << "  IntAtom: " << i.name() << " = " << i.get() << "\n";
    std::cout << "  FloatAtom: " << f.name() << " = " << f.get() << "\n";
    std::cout << "  StringAtom: " << s.name() << " = " << s.get() << "\n";
    std::cout << "  BoolAtom: " << b.name() << " = " << (b.get() ? "true" : "false") << "\n";
    std::cout << "  CharAtom: " << c.name() << " = " << c.get() << "\n";
}
