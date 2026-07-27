# 📚 The Guardian Documentation

## 📝 Axiom Parser

Axiom now uses The Guardian's **complete parser API** with full support for:

- ✅ Variables (`let x = 5`)
- ✅ If/Else (`if (x > 5) { ... } else { ... }`)
- ✅ While loops (`while (x < 10) { ... }`)
- ✅ For loops (`for (i = 0; i < 10; i = i + 1) { ... }`)
- ✅ Functions (`func add(a, b) { return a + b }`)
- ✅ Print (`print("Hello")`, `println("World")`)
- ✅ Expressions (`x + y * 3`)
- ✅ Return statements (`return x + y`)

The parser is built on The Guardian's `parser.hpp` API and is fully extensible.

Welcome to The Guardian documentation — your complete guide to building languages with The Guardian.

## Quick Links

- [Getting Started](#getting-started) — Install and run your first language
- [User Guide](user-guide/) — Complete framework documentation
- [API Reference](api/) — Full C++ API documentation
- [Examples](examples/) — Example languages and usage
- [Architecture](architecture/) — How The Guardian works

## Getting Started

### Installation

```bash
git clone https://github.com/Taha95-dev/The-Guardian.git
cd The-Guardian
make build
sudo make install

Your First Language
cpp

#include <guardian/core/molecule.hpp>
#include <guardian/vm/vm.hpp>

using namespace guardian;

int main() {
    // Create a molecule
    Molecule mol;
    mol.add_string("greeting", "Hello from The Guardian!");
    
    // Run it on the VM
    vm::VM vm;
    vm.run();
    
    return 0;
}

Why The Guardian?
Feature	The Guardian
Memory Safe	LUT-based pointer tracking
No GC	Stack + heap management
Custom Formats	GBIN binary format
VM	Bytecode execution
Clean API	C++17, modern
0 Bloat	7,000 lines, focused
Documentation Sections

    User Guide — Complete usage guide

    API Reference — All classes and functions

    Examples — Real-world examples

    Architecture — Design and internals

Built by a 13-Year-Old

The Guardian was built on 4GB RAM with Arch Linux and Neovim. It's proof that great software doesn't require expensive hardware — just focus and passion.
License

    Personal/Educational: FREE

    Commercial: $20 one-time

    Enterprise: $99/year

    Buy a license



The Guardian — Safe by Design, Not by Choice.
