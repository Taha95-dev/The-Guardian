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
    
    void addAtom(std::shared_ptr<Atom> atom);
    void addAtom(const std::string& name, std::shared_ptr<Atom> atom);
    std::shared_ptr<Atom> getAtom(size_t index) const;
    std::shared_ptr<Atom> getAtom(const std::string& name) const;
    size_t getAtomCount() const { return atoms.size(); }
    
    bool registerPointer(void* ptr, const std::string& name = "");
    bool unregisterPointer(void* ptr);
    bool isValidPointer(void* ptr) const;
    
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
};

// ============================================
// FREE LIST ALLOCATOR — Simple and reliable
// ============================================
struct FreeBlock {
    size_t size;
    FreeBlock* next;
};

class FreeListAllocator {
public:
    FreeListAllocator(size_t size);
    ~FreeListAllocator();
    
    void* allocate(size_t size);
    void free(void* ptr);
    size_t used() const { return total_used; }
    size_t total() const { return total_size; }
    size_t freeCount() const;
    void printStats() const;
    void reset();
    
private:
    uint8_t* memory;
    size_t total_size;
    size_t total_used;
    FreeBlock* free_list;
    
    void initialize();
};

// ============================================
// STACK ALLOCATOR — Fast bump pointer
// ============================================
class StackAllocator {
public:
    StackAllocator(size_t size);
    ~StackAllocator();
    
    void* push(size_t size);
    void pop(size_t size);
    void* top() const { return stack_ptr; }
    size_t used() const { return stack_ptr - stack_base; }
    size_t total() const { return stack_size; }
    void printStats() const;
    void reset();
    
private:
    uint8_t* stack_base;
    uint8_t* stack_ptr;
    size_t stack_size;
};

// ============================================
// MEMORY MANAGER — Unified memory system
// ============================================
class MemoryManager {
public:
    MemoryManager(size_t stack_size = 1024 * 1024, size_t heap_size = 1024 * 1024);
    ~MemoryManager();
    
    // Stack operations
    void* stackPush(size_t size);
    void stackPop(size_t size);
    
    // Heap operations
    void* heapAllocate(size_t size);
    void heapFree(void* ptr);
    
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
    };
    Stats getStats() const;
    void printStats() const;
    void reset();
    
private:
    StackAllocator stack;
    FreeListAllocator heap;
    std::unordered_map<void*, size_t> lut;  // Pointer -> size
    std::unordered_map<void*, std::string> lut_names;  // Pointer -> name
};

} // namespace guardian::memory
