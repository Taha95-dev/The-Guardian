#include <iostream>
#include "../src/atoms/numeric/uint_atom.hpp"
#include "../src/atoms/numeric/int64_atom.hpp"
#include "../src/atoms/numeric/float64_atom.hpp"
#include "../src/atoms/container/bytes_atom.hpp"
#include "../src/atoms/container/array_atom.hpp"
#include "../src/atoms/primitive/int_atom.hpp"
#include "../src/atoms/primitive/float_atom.hpp"
#include "../src/atoms/text/string_atom.hpp"

using namespace guardian;

int main() {
    std::cout << "Testing New Atom Types...\n\n";
    
    // UIntAtom
    UIntAtom u(42);
    std::cout << "UIntAtom: " << u.get() << "\n";
    
    // Int64Atom
    Int64Atom i64(9223372036854775807LL);
    std::cout << "Int64Atom: " << i64.get() << "\n";
    
    // Float64Atom
    Float64Atom f64(3.14159265359);
    std::cout << "Float64Atom: " << f64.get() << "\n";
    
    // BytesAtom
    std::vector<uint8_t> bytes = {0x48, 0x65, 0x6C, 0x6C, 0x6F};
    BytesAtom b(bytes);
    std::cout << "BytesAtom: " << b.length() << " bytes\n";
    
    // ArrayAtom
    ArrayAtom arr;
    arr.push_back(std::make_shared<IntAtom>(1));
    arr.push_back(std::make_shared<FloatAtom>(2.5f));
    arr.push_back(std::make_shared<StringAtom>("three"));
    std::cout << "ArrayAtom: " << arr.length() << " elements\n";
    
    std::cout << "\n✅ New atom types working!\n";
    return 0;
}
