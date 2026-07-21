#include "memory.hpp"
#include <iostream>
#include <cstring>
#include <algorithm>

namespace guardian::memory {

// ============================================
// ATOM Implementation
// ============================================
Atom::Atom(Type type) : type(type), string_val("") {
    switch (type) {
        case Type::INT: int_val = 0; break;
        case Type::FLOAT: float_val = 0.0f; break;
        case Type::BOOL: bool_val = false; break;
        case Type::CHAR: char_val = '\0'; break;
        default: break;
    }
}

Atom::Atom(int value) : type(Type::INT), int_val(value), string_val("") {}
Atom::Atom(float value) : type(Type::FLOAT), float_val(value), string_val("") {}
Atom::Atom(bool value) : type(Type::BOOL), bool_val(value), string_val("") {}
Atom::Atom(const std::string& value) : type(Type::STRING), string_val(value) {}
Atom::Atom(char value) : type(Type::CHAR), char_val(value), string_val("") {}

size_t Atom::getSize() const {
    switch (type) {
        case Type::INT: return sizeof(int);
        case Type::FLOAT: return sizeof(float);
        case Type::BOOL: return sizeof(bool);
        case Type::CHAR: return sizeof(char);
        case Type::STRING: return string_val.size() + 1;
        case Type::NULL_TYPE: return 0;
    }
    return 0;
}

std::string Atom::toString() const {
    switch (type) {
        case Type::INT: return std::to_string(int_val);
        case Type::FLOAT: return std::to_string(float_val);
        case Type::BOOL: return bool_val ? "true" : "false";
        case Type::STRING: return string_val;
        case Type::CHAR: return std::string(1, char_val);
        case Type::NULL_TYPE: return "null";
    }
    return "unknown";
}

int Atom::asInt() const { return int_val; }
float Atom::asFloat() const { return float_val; }
bool Atom::asBool() const { return bool_val; }
std::string Atom::asString() const { return string_val; }
char Atom::asChar() const { return char_val; }

// ============================================
// MOLECULE Implementation
// ============================================
Molecule::Molecule() {}

void Molecule::addAtom(std::shared_ptr<Atom> atom) {
    AtomEntry entry;
    entry.name = "atom_" + std::to_string(atoms.size());
    entry.atom = atom;
    atoms.push_back(entry);
}

void Molecule::addAtom(const std::string& name, std::shared_ptr<Atom> atom) {
    AtomEntry entry;
    entry.name = name;
    entry.atom = atom;
    atoms.push_back(entry);
    name_map[name] = atoms.size() - 1;
}

std::shared_ptr<Atom> Molecule::getAtom(size_t index) const {
    if (index < atoms.size()) {
        return atoms[index].atom;
    }
    return nullptr;
}

std::shared_ptr<Atom> Molecule::getAtom(const std::string& name) const {
    auto it = name_map.find(name);
    if (it != name_map.end()) {
        return atoms[it->second].atom;
    }
    return nullptr;
}

bool Molecule::registerPointer(void* ptr, const std::string& name) {
    pointer_table[ptr] = name;
    return true;
}

bool Molecule::unregisterPointer(void* ptr) {
    return pointer_table.erase(ptr) > 0;
}

bool Molecule::isValidPointer(void* ptr) const {
    return pointer_table.find(ptr) != pointer_table.end();
}

std::vector<uint8_t> Molecule::serialize() const {
    std::vector<uint8_t> data;
    
    uint32_t count = static_cast<uint32_t>(atoms.size());
    data.resize(4 + count * (1 + 4));  // Type + length
    // Simplified serialization
    
    return data;
}

std::shared_ptr<Molecule> Molecule::deserialize(const std::vector<uint8_t>& data) {
    return std::make_shared<Molecule>();
}

// ============================================
// FREE LIST ALLOCATOR Implementation
// ============================================
FreeListAllocator::FreeListAllocator(size_t size) 
    : total_size(size), total_used(0) {
    memory = new uint8_t[size];
    initialize();
}

FreeListAllocator::~FreeListAllocator() {
    delete[] memory;
}

void FreeListAllocator::initialize() {
    free_list = reinterpret_cast<FreeBlock*>(memory);
    free_list->size = total_size - sizeof(FreeBlock);
    free_list->next = nullptr;
}

void* FreeListAllocator::allocate(size_t size) {
    if (size == 0) return nullptr;
    
    // Align size to 8 bytes
    size = (size + 7) & ~7;
    
    FreeBlock* current = free_list;
    FreeBlock* prev = nullptr;
    
    while (current) {
        if (current->size >= size + sizeof(FreeBlock)) {
            // Split the block
            FreeBlock* new_block = reinterpret_cast<FreeBlock*>(
                reinterpret_cast<uint8_t*>(current) + sizeof(FreeBlock) + size
            );
            new_block->size = current->size - size - sizeof(FreeBlock);
            new_block->next = current->next;
            
            if (prev) {
                prev->next = new_block;
            } else {
                free_list = new_block;
            }
            
            total_used += size;
            
            return reinterpret_cast<uint8_t*>(current) + sizeof(FreeBlock);
        }
        
        prev = current;
        current = current->next;
    }
    
    return nullptr;
}

void FreeListAllocator::free(void* ptr) {
    if (!ptr) return;
    
    uint8_t* block_ptr = reinterpret_cast<uint8_t*>(ptr) - sizeof(FreeBlock);
    FreeBlock* block = reinterpret_cast<FreeBlock*>(block_ptr);
    total_used -= block->size;
    
    // Add to free list
    block->next = free_list;
    free_list = block;
}

size_t FreeListAllocator::freeCount() const {
    size_t count = 0;
    FreeBlock* current = free_list;
    while (current) {
        count++;
        current = current->next;
    }
    return count;
}

void FreeListAllocator::printStats() const {
    std::cout << "  Free List Allocator:\n";
    std::cout << "    Total: " << total_size << " bytes\n";
    std::cout << "    Used: " << total_used << " bytes\n";
    std::cout << "    Free blocks: " << freeCount() << "\n";
}

void FreeListAllocator::reset() {
    total_used = 0;
    initialize();
}

// ============================================
// STACK ALLOCATOR Implementation
// ============================================
StackAllocator::StackAllocator(size_t size) : stack_size(size) {
    stack_base = new uint8_t[size];
    stack_ptr = stack_base;
}

StackAllocator::~StackAllocator() {
    delete[] stack_base;
}

void* StackAllocator::push(size_t size) {
    if (stack_ptr + size > stack_base + stack_size) {
        std::cerr << "Error: Stack overflow!\n";
        return nullptr;
    }
    void* ptr = stack_ptr;
    stack_ptr += size;
    return ptr;
}

void StackAllocator::pop(size_t size) {
    if (stack_ptr - size < stack_base) {
        std::cerr << "Error: Stack underflow!\n";
        return;
    }
    stack_ptr -= size;
}

void StackAllocator::printStats() const {
    std::cout << "  Stack Allocator:\n";
    std::cout << "    Total: " << stack_size << " bytes\n";
    std::cout << "    Used: " << used() << " bytes\n";
}

void StackAllocator::reset() {
    stack_ptr = stack_base;
}

// ============================================
// MEMORY MANAGER Implementation
// ============================================
MemoryManager::MemoryManager(size_t stack_size, size_t heap_size)
    : stack(stack_size), heap(heap_size) {}

MemoryManager::~MemoryManager() = default;

void* MemoryManager::stackPush(size_t size) {
    return stack.push(size);
}

void MemoryManager::stackPop(size_t size) {
    stack.pop(size);
}

void* MemoryManager::heapAllocate(size_t size) {
    void* ptr = heap.allocate(size);
    if (ptr) {
        lut[ptr] = size;
    }
    return ptr;
}

void MemoryManager::heapFree(void* ptr) {
    if (!ptr) return;
    lut.erase(ptr);
    heap.free(ptr);
}

bool MemoryManager::registerPointer(void* ptr, size_t size, const std::string& name) {
    if (!ptr) return false;
    lut[ptr] = size;
    lut_names[ptr] = name;
    return true;
}

bool MemoryManager::unregisterPointer(void* ptr) {
    lut.erase(ptr);
    lut_names.erase(ptr);
    return true;
}

bool MemoryManager::isValidPointer(void* ptr) const {
    return lut.find(ptr) != lut.end();
}

void MemoryManager::dumpPointerTable() const {
    std::cout << "Pointer Lookup Table (LUT):\n";
    for (const auto& [ptr, size] : lut) {
        auto it = lut_names.find(ptr);
        std::string name = (it != lut_names.end()) ? it->second : "unnamed";
        std::cout << "  " << ptr << " -> " << name << " (" << size << " bytes)\n";
    }
}

MemoryManager::Stats MemoryManager::getStats() const {
    Stats stats;
    stats.stack_used = stack.used();
    stats.stack_total = stack.total();
    stats.heap_used = heap.used();
    stats.heap_total = heap.total();
    stats.pointer_count = lut.size();
    return stats;
}

void MemoryManager::printStats() const {
    std::cout << "Memory Manager Stats:\n";
    stack.printStats();
    heap.printStats();
    std::cout << "  Pointers in LUT: " << lut.size() << "\n";
}

void MemoryManager::reset() {
    stack.reset();
    heap.reset();
    lut.clear();
    lut_names.clear();
}

} // namespace guardian::memory
