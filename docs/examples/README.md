# 💻 Examples

## Axiom — A Complete Demo Language

Axiom is a small language built with The Guardian to demonstrate the framework.

### Features

- Variables (`let x = 42`)
- Print (`println("Hello")`)
- Strings, numbers, booleans
- Compiled to bytecode
- Runs on The Guardian VM

### Example

```javascript
let x = 42
let name = "Axiom"
let active = true

println(x)
println(name)
println(active)

Output
text

42
Axiom
true

Code

Check out examples/axiom/ in the source tree.
Basic Usage Examples
Molecule
cpp

#include <guardian/core/molecule.hpp>

using namespace guardian;

int main() {
    Molecule mol;
    mol.add_string("name", "Guardian");
    mol.add_number("version", 1.0);
    mol.add_bool("active", true);
    
    mol.dump();
    // Output:
    // Molecule Contents:
    //   Strings:
    //     name = "Guardian"
    //   Numbers:
    //     version = 1
    //   Bools:
    //     active = true
    
    return 0;
}

Memory Manager
cpp

#include <guardian/memory/memory.hpp>

using namespace guardian;

int main() {
    memory::MemoryManager mem;
    
    void* ptr = mem.allocate(1024);
    mem.register_pointer(ptr, 1024, "buffer");
    
    // ... use ptr ...
    
    mem.unregister_pointer(ptr);
    mem.deallocate(ptr);
    
    return 0;
}

Binary Format
cpp

#include <guardian/format/gbin_format.hpp>

using namespace guardian;

int main() {
    format::GbinFormat fmt;
    fmt.push_string("Hello, World!");
    fmt.push_int(42);
    fmt.push_float(3.14);
    
    auto data = fmt.serialize();
    fmt.write("output.gbin");
    
    return 0;
}

