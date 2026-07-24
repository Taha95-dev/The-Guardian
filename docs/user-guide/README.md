# The Guardian User Guide

## Table of Contents

1. [Installation](installation.md)
2. [Core Concepts](core-concepts.md)
3. [Building a Language](building-a-language.md)
4. [Custom Binary Formats](custom-formats.md)
5. [VM Execution](vm-execution.md)
6. [Memory Safety](memory-safety.md)
7. [Examples](examples.md)

## Installation

### From Source

```bash
git clone https://github.com/yourusername/the-guardian
cd the-guardian
make build
sudo make install

System-wide

After installation, The Guardian is available system-wide:
bash

guardianc --version

Using in Your Project
cmake

find_library(GUARDIAN_CORE guardian_core REQUIRED)
find_library(GUARDIAN_MEMORY guardian_memory REQUIRED)
find_library(GUARDIAN_PARSER guardian_parser REQUIRED)
find_library(GUARDIAN_FORMAT guardian_format REQUIRED)
find_library(GUARDIAN_VM guardian_vm REQUIRED)

target_link_libraries(your_language
    ${GUARDIAN_CORE}
    ${GUARDIAN_MEMORY}
    ${GUARDIAN_PARSER}
    ${GUARDIAN_FORMAT}
    ${GUARDIAN_VM}
)

Core Concepts
Atoms

Atoms are immutable, type-safe data units:
cpp

IntAtom i(42);
FloatAtom f(3.14);
StringAtom s("Hello");
BoolAtom b(true);

Molecules

Molecules are composable data structures with LUT:
cpp

Molecule mol;
mol.add_atom("age", std::make_unique<IntAtom>(13));
mol.add_atom("name", std::make_unique<StringAtom>("Taha"));

Quarks

Quarks are 0-overhead primitive values:
cpp

Quark q(42);  // Stack-allocated, fast

Building a Language

See Building a Language for a complete tutorial.
