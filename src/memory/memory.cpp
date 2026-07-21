#include "memory.hpp"
#include <iostream>
#include <cstring>
#include <algorithm>
#include <sstream>

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
    
    // Serialize atom count
    uint32_t count = atoms.size();
    data.insert(data.end(), 
                reinterpret_cast<const uint8_t*>(&count),
                reinterpret_cast<const uint8_t*>(&count) + 4);
    
    // Serialize each atom
    for (const auto& entry : atoms) {
        uint8_t type = static_cast<uint8_t>(entry.atom->getType());
        data.push_back(type);
        
        auto value = entry.atom->toString();
        uint32_t len = value.length();
        data.insert(data.end(),
                    reinterpret_cast<const uint8_t*>(&len),
                    reinterpret_cast<const uint8_t*>(&len) + 4);
        data.insert(data.end(), value.begin(), value.end());
    }
    
    return data;
}

std::shared_ptr<Molecule> Molecule::deserialize(const std::vector<uint8_t>& data) {
    auto molecule = std::make_shared<Molecule>();
    size_t pos = 0;
    
    if (data.size() < 4) return molecule;
    
    uint32_t count;
    memcpy(&count, data.data(), 4);
    pos += 4;
    
    for (uint32_t i = 0; i < count && pos < data.size(); i++) {
        if (pos >= data.size()) break;
        
        uint8_t type = data[pos++];
        if (pos + 4 > data.size()) break;
        
        uint32_t len;
        memcpy(&len, data.data() + pos, 4);
        pos += 4;
        
        if (pos + len > data.size()) break;
        
        std::string value(data.begin() + pos, data.begin() + pos + len);
        pos += len;
        
        std::shared_ptr<Atom> atom;
        switch (static_cast<Atom::Type>(type)) {
            case Atom::Type::INT:
                atom = std::make_shared<Atom>(std::stoi(value));
                break;
            case Atom::Type::FLOAT:
                atom = std::make_shared<Atom>(std::stof(value));
                break;
            case Atom::Type::BOOL:
                atom = std::make_shared<Atom>(value == "true");
                break;
            case Atom::Type::STRING:
                atom = std::make_shared<Atom>(value);
                break;
            case Atom::Type::CHAR:
                atom = std::make_shared<Atom>(value.empty() ? '\0' : value[0]);
                break;
            default:
                atom = std::make_shared<Atom>(Atom::Type::NULL_TYPE);
                break;
        }
        molecule->addAtom(atom);
    }
    
    return molecule;
}

// ============================================
// MEMORY MANAGER Implementation
// ============================================
MemoryManager::MemoryManager(size_t stack_size, size_t heap_size)
    : stack_size(stack_size), heap_size(heap_size), total_atoms(0), total_molecules(0) {
    
    // Allocate stack
    stack_base = new uint8_t[stack_size];
    stack_ptr = stack_base;
    
    // Allocate heap
    heap_memory.resize(heap_size);
    heap_blocks.push_back({heap_size, true, heap_memory.data()});
}

MemoryManager::~MemoryManager() {
    delete[] stack_base;
}

void* MemoryManager::stackPush(size_t size) {
    if (stack_ptr + size > stack_base + stack_size) {
        std::cerr << "Error: Stack overflow!\n";
        return nullptr;
    }
    void* ptr = stack_ptr;
    stack_ptr += size;
    return ptr;
}

void MemoryManager::stackPop(size_t size) {
    if (stack_ptr - size < stack_base) {
        std::cerr << "Error: Stack underflow!\n";
        return;
    }
    stack_ptr -= size;
}

void* MemoryManager::heapAllocate(size_t size) {
    // Simple first-fit allocation
    for (auto& block : heap_blocks) {
        if (block.free && block.size >= size) {
            if (block.size > size) {
                // Split the block
                HeapBlock new_block;
                new_block.size = block.size - size;
                new_block.free = true;
                new_block.ptr = static_cast<uint8_t*>(block.ptr) + size;
                heap_blocks.insert(heap_blocks.begin() + (&block - heap_blocks.data()) + 1, new_block);
                block.size = size;
            }
            block.free = false;
            return block.ptr;
        }
    }
    std::cerr << "Error: Heap allocation failed!\n";
    return nullptr;
}

void MemoryManager::heapFree(void* ptr) {
    for (auto& block : heap_blocks) {
        if (block.ptr == ptr) {
            block.free = true;
            // Coalesce adjacent free blocks
            break;
        }
    }
}

bool MemoryManager::registerPointer(void* ptr, size_t size, const std::string& name) {
    if (ptr == nullptr) return false;
    PointerEntry entry;
    entry.size = size;
    entry.name = name;
    lut[ptr] = entry;
    return true;
}

bool MemoryManager::unregisterPointer(void* ptr) {
    return lut.erase(ptr) > 0;
}

bool MemoryManager::isValidPointer(void* ptr) const {
    return lut.find(ptr) != lut.end();
}

void MemoryManager::dumpPointerTable() const {
    std::cout << "Pointer Lookup Table (LUT):\n";
    for (const auto& [ptr, entry] : lut) {
        std::cout << "  " << ptr << " -> " << entry.name 
                  << " (" << entry.size << " bytes)\n";
    }
}

MemoryManager::Stats MemoryManager::getStats() const {
    Stats stats;
    stats.stack_used = stackUsed();
    stats.stack_total = stack_size;
    stats.heap_used = heapUsed();
    stats.heap_total = heap_size;
    stats.pointer_count = lut.size();
    stats.total_atoms = total_atoms;
    stats.total_molecules = total_molecules;
    return stats;
}

void MemoryManager::printStats() const {
    auto stats = getStats();
    std::cout << "Memory Manager Stats:\n";
    std::cout << "  Stack: " << stats.stack_used << " / " << stats.stack_total << " bytes\n";
    std::cout << "  Heap:  " << stats.heap_used << " / " << stats.heap_total << " bytes\n";
    std::cout << "  Pointers: " << stats.pointer_count << " in LUT\n";
    std::cout << "  Atoms: " << stats.total_atoms << ", Molecules: " << stats.total_molecules << "\n";
}

void MemoryManager::reset() {
    stack_ptr = stack_base;
    heap_blocks.clear();
    heap_blocks.push_back({heap_size, true, heap_memory.data()});
    lut.clear();
    total_atoms = 0;
    total_molecules = 0;
}

size_t MemoryManager::heapUsed() const {
    size_t used = 0;
    for (const auto& block : heap_blocks) {
        if (!block.free) {
            used += block.size;
        }
    }
    return used;
}

} // namespace guardian::memory
