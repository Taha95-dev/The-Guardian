#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <filesystem>

namespace guardian::memory {

// ============================================
// CACHE MANAGER — Persistent VM state
// ============================================
class CacheManager {
public:
    CacheManager();
    ~CacheManager();
    
    // Cache operations
    bool save_cache(const std::string& program_hash, const std::vector<uint8_t>& state);
    bool load_cache(const std::string& program_hash, std::vector<uint8_t>& state);
    bool has_cache(const std::string& program_hash) const;
    void clear_cache(const std::string& program_hash);
    void clear_all_cache();
    
    // Get cache directory
    std::string get_cache_dir() const;
    
private:
    std::string cache_dir;
    std::string get_program_cache_dir(const std::string& program_hash) const;
};

// ============================================
// MEMORY MANAGER — Controls VM memory
// ============================================
class MemoryManager {
public:
    MemoryManager();
    ~MemoryManager();
    
    // Allocation
    void* allocate(size_t size);
    void deallocate(void* ptr);
    
    // LUT (Lookup Table) — Memory safety
    void register_pointer(void* ptr, size_t size, const std::string& name = "");
    void unregister_pointer(void* ptr);
    bool is_valid_pointer(void* ptr) const;
    void dump_lut() const;
    
    // Stats
    struct Stats {
        size_t total_allocated;
        size_t total_freed;
        size_t current_allocations;
        size_t lut_size;
    };
    Stats get_stats() const;
    void print_stats() const;
    
    // Reset
    void reset();
    
private:
    struct Allocation {
        size_t size;
        std::string name;
        bool is_valid;
    };
    
    std::unordered_map<void*, Allocation> lut;
    size_t total_allocated;
    size_t total_freed;
    size_t current_allocations;
};

// ============================================
// MEMORY POOL — Efficient allocations
// ============================================
class MemoryPool {
public:
    MemoryPool(size_t block_size = 4096);
    ~MemoryPool();
    
    void* allocate(size_t size);
    void deallocate(void* ptr);
    void reset();
    
    size_t used() const { return used_memory; }
    size_t total() const { return total_memory; }
    
private:
    struct Block {
        uint8_t* data;
        size_t size;
        bool free;
        Block* next;
    };
    
    Block* head;
    size_t block_size;
    size_t used_memory;
    size_t total_memory;
    
    Block* find_free_block(size_t size);
    void add_block(size_t size);
};

} // namespace guardian::memory
