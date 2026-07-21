#include "test_memory.hpp"
#include "../src/memory/memory.hpp"
#include <iostream>

using namespace guardian::memory;

void test_memory() {
    std::cout << "  Testing Guardian Memory System...\n";
    
    // Test Atoms
    std::cout << "  📦 Atoms:\n";
    auto int_atom = std::make_shared<Atom>(42);
    auto float_atom = std::make_shared<Atom>(3.14f);
    auto bool_atom = std::make_shared<Atom>(true);
    auto string_atom = std::make_shared<Atom>("Hello, Guardian!");
    
    std::cout << "    Int: " << int_atom->toString() << "\n";
    std::cout << "    Float: " << float_atom->toString() << "\n";
    std::cout << "    Bool: " << bool_atom->toString() << "\n";
    std::cout << "    String: " << string_atom->toString() << "\n";
    
    // Test Molecules
    std::cout << "  🧬 Molecules:\n";
    auto molecule = std::make_shared<Molecule>();
    molecule->addAtom("x", int_atom);
    molecule->addAtom("y", float_atom);
    molecule->addAtom("flag", bool_atom);
    molecule->addAtom("msg", string_atom);
    
    std::cout << "    Molecule has " << molecule->getAtomCount() << " atoms\n";
    
    // Test LUT (Pointer tracking)
    std::cout << "  📋 LUT (Pointer Tracking):\n";
    int* ptr = new int(42);
    molecule->registerPointer(ptr, "test_ptr");
    std::cout << "    Pointer valid: " << (molecule->isValidPointer(ptr) ? "yes" : "no") << "\n";
    molecule->unregisterPointer(ptr);
    std::cout << "    After unregister: " << (molecule->isValidPointer(ptr) ? "yes" : "no") << "\n";
    delete ptr;
    
    // Test Memory Manager
    std::cout << "  💾 Memory Manager:\n";
    MemoryManager mem(1024, 1024);
    mem.printStats();
    
    // Test stack
    std::cout << "    Stack:\n";
    void* stack_ptr = mem.stackPush(4);
    if (stack_ptr) {
        *(int*)stack_ptr = 42;
        std::cout << "      Pushed: " << *(int*)stack_ptr << "\n";
        mem.stackPop(4);
    }
    
    // Test heap
    std::cout << "    Heap:\n";
    void* heap_ptr = mem.heapAllocate(8);
    if (heap_ptr) {
        mem.registerPointer(heap_ptr, 8, "test_heap");
        std::cout << "      Allocated 8 bytes at " << heap_ptr << "\n";
        mem.unregisterPointer(heap_ptr);
        mem.heapFree(heap_ptr);
    }
    
    // Test serialization
    std::cout << "  💾 Serialization:\n";
    auto data = molecule->serialize();
    std::cout << "    Serialized " << data.size() << " bytes\n";
    
    auto new_molecule = Molecule::deserialize(data);
    if (new_molecule) {
        std::cout << "    Deserialized: " << new_molecule->getAtomCount() << " atoms\n";
        
        auto x_atom = new_molecule->getAtom("x");
        auto msg_atom = new_molecule->getAtom("msg");
        
        if (x_atom) {
            std::cout << "    x = " << x_atom->toString() << "\n";
        }
        if (msg_atom) {
            std::cout << "    msg = " << msg_atom->toString() << "\n";
        }
    } else {
        std::cout << "    Deserialization failed!\n";
    }
}
