#include <iostream>
#include <cassert>
#include <memory>
#include "../src/atoms/numeric/int64_atom.hpp"
#include "../src/atoms/numeric/float64_atom.hpp"
#include "../src/atoms/numeric/float32_atom.hpp"
#include "../src/atoms/numeric/uint_atom.hpp"
#include "../src/atoms/primitive/bool_atom.hpp"
#include "../src/atoms/primitive/char_atom.hpp"
#include "../src/atoms/primitive/uint8_atom.hpp"
#include "../src/atoms/primitive/int16_atom.hpp"
#include "../src/atoms/text/string_atom.hpp"
#include "../src/atoms/container/vector_atom.hpp"

using namespace guardian;

void test_atoms() {
    std::cout << "  Testing atoms...\n";
    int passed = 0;
    int total = 0;
    
    auto run_test = [&](const char* name, auto fn) {
        total++;
        std::cout << "    " << name << "... ";
        try {
            fn();
            std::cout << "✅\n";
            passed++;
        } catch (const std::exception& e) {
            std::cout << "❌ " << e.what() << "\n";
        }
    };
    
    run_test("Int64Atom", []() {
        auto atom = std::make_shared<Int64Atom>(42);
        assert(atom->get() == 42);
        assert(atom->type() == AtomType::INT64);
    });
    
    run_test("Float64Atom", []() {
        auto atom = std::make_shared<Float64Atom>(3.14159);
        assert(atom->get() == 3.14159);
        assert(atom->type() == AtomType::FLOAT64);
    });
    
    run_test("Float32Atom", []() {
        auto atom = std::make_shared<Float32Atom>(2.718f);
        assert(atom->get() == 2.718f);
        assert(atom->type() == AtomType::FLOAT32);
    });
    
    run_test("UIntAtom", []() {
        auto atom = std::make_shared<UIntAtom>(42u);
        assert(atom->get() == 42u);
        assert(atom->type() == AtomType::UINT64);
    });
    
    run_test("UInt8Atom", []() {
        auto atom = std::make_shared<UInt8Atom>(255);
        assert(atom->get() == 255);
        assert(atom->type() == AtomType::UINT8);
    });
    
    run_test("Int16Atom", []() {
        auto atom = std::make_shared<Int16Atom>(32767);
        assert(atom->get() == 32767);
        assert(atom->type() == AtomType::INT16);
    });
    
    run_test("BoolAtom", []() {
        auto atom = std::make_shared<BoolAtom>(true);
        assert(atom->get() == true);
        assert(atom->type() == AtomType::BOOL);
    });
    
    run_test("CharAtom", []() {
        auto atom = std::make_shared<CharAtom>('Z');
        assert(atom->get() == 'Z');
        assert(atom->type() == AtomType::CHAR);
    });
    
    run_test("StringAtom", []() {
        auto atom = std::make_shared<StringAtom>("Hello, Guardian!");
        assert(atom->get() == "Hello, Guardian!");
        assert(atom->type() == AtomType::STRING);
    });
    
    run_test("VectorAtom", []() {
        auto atom = std::make_shared<VectorAtom>();
        atom->push_back(0x01);
        atom->push_back(0x02);
        atom->push_back(0x03);
        assert(atom->size() == 3);
        assert(atom->type() == AtomType::VECTOR);
    });
    
    run_test("Serialization", []() {
        auto atom = std::make_shared<Int64Atom>(42);
        auto data = atom->serialize();
        assert(data.size() == sizeof(int64_t));
    });
    
    run_test("Deserialization", []() {
        auto atom = std::make_shared<Int64Atom>(42);
        auto data = atom->serialize();
        auto atom2 = std::make_shared<Int64Atom>();
        atom2->deserialize(data);
        assert(atom2->get() == 42);
    });
    
    std::cout << "  ✅ " << passed << "/" << total << " atom tests passed!\n";
}
