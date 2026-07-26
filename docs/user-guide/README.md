# 🧭 User Guide

The Guardian is a complete language framework. This guide will take you from zero to building your own language.

## Table of Contents

1. [Installation](#installation)
2. [Core Concepts](#core-concepts)
3. [Atoms](#atoms)
4. [Molecules](#molecules)
5. [Quarks](#quarks)
6. [Memory Management](#memory-management)
7. [Binary Formats](#binary-formats)
8. [The VM](#the-vm)
9. [Building a Language](#building-a-language)

## Installation

### From Source

```bash
git clone https://github.com/Taha95-dev/The-Guardian.git
cd The-Guardian
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install

Package Managers

Coming soon!
Core Concepts

The Guardian is built on three core concepts:
1. Quarks (Primitive Values)

Quarks are the building blocks — int, float, bool, string, char.
cpp

Quark q1(42);        // integer
Quark q2(3.14);      // float
Quark q3(true);      // bool
Quark q4("Hello");   // string

2. Atoms (Heap Values)

Atoms are heap-allocated data structures — arrays, dicts, custom types.
cpp

auto arr = std::make_shared<ArrayAtom>();
arr->push(42);
arr->push(3.14);

3. Molecules (Containers)

Molecules are containers that hold quarks and atoms.
cpp

Molecule mol;
mol.add_string("name", "Guardian");
mol.add_number("version", 1.0);
mol.add_bool("active", true);

Memory Safety

The Guardian uses LUT (Lookup Table) to track every pointer allocation. No dangling pointers. No use-after-free. No GC.
cpp

memory::MemoryManager mem;
void* ptr = mem.allocate(1024);
mem.register_pointer(ptr, 1024, "buffer");
// ... use ptr ...
mem.unregister_pointer(ptr);
mem.deallocate(ptr);

Building a Language

The Guardian makes it easy to build custom languages:

    Lexer: Tokenize source code

    Parser: Build AST

    Compiler: Generate bytecode

    VM: Execute bytecode

See Examples for complete language implementations.

The Guardian — Safe by Design, Not by Choice.
