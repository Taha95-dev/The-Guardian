#include "../src/core/atom.hpp"
#include "../src/atoms/primitive/int_atom.hpp"
#include "../src/atoms/primitive/float_atom.hpp"
#include "../src/atoms/text/string_atom.hpp"
#include "../src/atoms/special/null_atom.hpp"
#include "test_runner.hpp"

using namespace guardian;

GUARDIAN_TEST(IntAtomTest) {
    IntAtom atom(42);
    ASSERT_EQ(atom.get(), 42);
    ASSERT_EQ(atom.size(), sizeof(int));
    
    auto data = atom.serialize();
    IntAtom restored;
    restored.deserialize(data);
    ASSERT_EQ(restored.get(), 42);
}

GUARDIAN_TEST(FloatAtomTest) {
    FloatAtom atom(3.14f);
    ASSERT_FLOAT_EQ(atom.get(), 3.14f);
    ASSERT_EQ(atom.size(), sizeof(float));
    
    auto data = atom.serialize();
    FloatAtom restored;
    restored.deserialize(data);
    ASSERT_FLOAT_EQ(restored.get(), 3.14f);
}

GUARDIAN_TEST(StringAtomTest) {
    StringAtom atom("Hello, Guardian!");
    ASSERT_EQ(atom.get(), "Hello, Guardian!");
    
    auto data = atom.serialize();
    StringAtom restored;
    restored.deserialize(data);
    ASSERT_EQ(restored.get(), "Hello, Guardian!");
}

GUARDIAN_TEST(NullAtomTest) {
    NullAtom atom;
    ASSERT_EQ(atom.size(), 0);
    auto data = atom.serialize();
    ASSERT_EQ(data.size(), 0);
}
