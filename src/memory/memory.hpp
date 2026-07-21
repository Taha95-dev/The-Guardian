#pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

namespace guardian::memory {

// ============================================
// ATOM — Immutable, safe data unit
// ============================================
class Atom {
public:
    enum class Type {
        INT,
        FLOAT,
        BOOL,
        STRING,
        CHAR,
        NULL_TYPE
    };
    
    Atom(Type type);
    Atom(int value);
    Atom(float value);
    Atom(bool value);
    Atom(const std::string& value);
    Atom(char value);
    ~Atom() = default;
    
    Type getType() const { return type; }
    size_t getSize() const;
    std::string toString() const;
    bool isNull() const { return type == Type::NULL_TYPE; }
    
    // Value accessors
    int asInt() const;
    float asFloat() const;
    bool asBool() const;
    std::string asString() const;
    char asChar() const;
    
private:
    Type type;
    union {
        int int_val;
        float float_val;
        bool bool_val;
        char char_val;
    };
    std::string string_val;
};

// ============================================
// MOLECULE — Composable data with LUT
// ============================================
class Molecule {
public:
    Molecule();
    ~Molecule() = default;
    
    // Add atoms
    void addAtom(std::shared_ptr<Atom> atom);
    void addAtom(const std::string& name, std::shared_ptr<Atom> atom);
    
    // Get atoms
    std::shared_ptr<Atom> getAtom(size_t index) const;
    std::shared_ptr<Atom> getAtom(const std::string& name) const;
    size_t getAtomCount() const { return atoms.size(); }
    
    // Pointer tracking (LUT)
    bool registerPointer(void* ptr, const std::string& name = "");
    bool unregisterPointer(void* ptr);
    bool isValidPointer(void* ptr) const;
    const std::unordered_map<void*, std::string>& getPointerTable() const {
        return pointer_table;
    }
    
    // Serialization
    std::vector<uint8_t> serialize() const;
    static std::shared_ptr<Molecule> deserialize(const std::vector<uint8_t>& data);
    
private:
    struct AtomEntry {
        std::string name;
        std::shared_ptr<Atom> atom;
    };
    
    std::vector<AtomEntry> atoms;
    std::unordered_map<std::string, size_t> name_map;
    std::unordered_map<void*, std::string> pointer_table;
    std::vector<uint8_t> raw_data;
};

// ============================================
// MEMORY MANAGER — Stack + Heap with LUT
// ============================================
class MemoryManager {
public:
    MemoryManager(size_t stack_size = 1024 * 1024, size_t heap_size = 1024 * 1024);
    ~MemoryManager();
    
    // Stack operations
    void* stackPush(size_t size);
    void stackPop(size_t size);
    void* stackTop() const { return stack_ptr; }
    size_t stackUsed() const { return stack_ptr - stack_base; }
    
    // Heap operations
    void* heapAllocate(size_t size);
    void heapFree(void* ptr);
    size_t heapUsed() const;
    size_t heapTotal() const { return heap_size; }
    
    // Pointer validation (LUT)
    bool registerPointer(void* ptr, size_t size, const std::string& name = "");
    bool unregisterPointer(void* ptr);
    bool isValidPointer(void* ptr) const;
    void dumpPointerTable() const;
    
    // Stats
    struct Stats {
        size_t stack_used;
        size_t stack_total;
        size_t heap_used;
        size_t heap_total;
        size_t pointer_count;
        size_t total_atoms;
        size_t total_molecules;
    };
    Stats getStats() const;
    void printStats() const;
    
    // Reset
    void reset();
    
private:
    // Stack
    uint8_t* stack_base;
    uint8_t* stack_ptr;
    size_t stack_size;
    
    // Heap
    struct HeapBlock {
        size_t size;
        bool free;
        void* ptr;
    };
    std::vector<uint8_t> heap_memory;
    size_t heap_size;
    std::vector<HeapBlock> heap_blocks;
    
    // Pointer Lookup Table (LUT)
    struct PointerEntry {
        size_t size;
        std::string name;
    };
    std::unordered_map<void*, PointerEntry> lut;
    
    // Stats
    size_t total_atoms;
    size_t total_molecules;
};

} // namespace guardian::memory
