#include "memory.hpp"
#include <iostream>
#include <cstring>
#include <filesystem>

namespace guardian::memory {

// ============================================
// CACHE MANAGER
// ============================================
CacheManager::CacheManager() {
    std::string home = getenv("HOME");
    cache_dir = home + "/.guardian/cache";
    std::filesystem::create_directories(cache_dir);
}

CacheManager::~CacheManager() = default;

std::string CacheManager::get_cache_dir() const {
    return cache_dir;
}

std::string CacheManager::get_program_cache_dir(const std::string& program_hash) const {
    return cache_dir + "/" + program_hash;
}

bool CacheManager::save_cache(const std::string& program_hash, const std::vector<uint8_t>& state) {
    std::string prog_dir = get_program_cache_dir(program_hash);
    std::filesystem::create_directories(prog_dir);
    
    std::string cache_file = prog_dir + "/state.bin";
    std::ofstream out(cache_file, std::ios::binary);
    if (!out.is_open()) {
        return false;
    }
    
    out.write(reinterpret_cast<const char*>(state.data()), state.size());
    out.close();
    
    std::cout << "  [CACHE] Saved state for: " << program_hash << "\n";
    return true;
}

bool CacheManager::load_cache(const std::string& program_hash, std::vector<uint8_t>& state) {
    std::string cache_file = get_program_cache_dir(program_hash) + "/state.bin";
    
    if (!std::filesystem::exists(cache_file)) {
        return false;
    }
    
    std::ifstream in(cache_file, std::ios::binary);
    if (!in.is_open()) {
        return false;
    }
    
    state = std::vector<uint8_t>(std::istreambuf_iterator<char>(in),
                                  std::istreambuf_iterator<char>());
    in.close();
    
    std::cout << "  [CACHE] Loaded state for: " << program_hash << "\n";
    return true;
}

bool CacheManager::has_cache(const std::string& program_hash) const {
    std::string cache_file = get_program_cache_dir(program_hash) + "/state.bin";
    return std::filesystem::exists(cache_file);
}

void CacheManager::clear_cache(const std::string& program_hash) {
    std::string prog_dir = get_program_cache_dir(program_hash);
    if (std::filesystem::exists(prog_dir)) {
        std::filesystem::remove_all(prog_dir);
        std::cout << "  [CACHE] Cleared: " << program_hash << "\n";
    }
}

void CacheManager::clear_all_cache() {
    if (std::filesystem::exists(cache_dir)) {
        std::filesystem::remove_all(cache_dir);
        std::filesystem::create_directories(cache_dir);
        std::cout << "  [CACHE] All cache cleared\n";
    }
}

// ============================================
// MEMORY MANAGER
// ============================================
MemoryManager::MemoryManager() 
    : total_allocated(0), total_freed(0), current_allocations(0) {}

MemoryManager::~MemoryManager() {
    reset();
}

void* MemoryManager::allocate(size_t size) {
    void* ptr = malloc(size);
    if (ptr) {
        total_allocated += size;
        current_allocations++;
        lut[ptr] = {size, "unknown", true};
    }
    return ptr;
}

void MemoryManager::deallocate(void* ptr) {
    if (!ptr) return;
    
    auto it = lut.find(ptr);
    if (it != lut.end()) {
        total_freed += it->second.size;
        current_allocations--;
        lut.erase(it);
    }
    free(ptr);
}

void MemoryManager::register_pointer(void* ptr, size_t size, const std::string& name) {
    if (ptr) {
        lut[ptr] = {size, name, true};
    }
}

void MemoryManager::unregister_pointer(void* ptr) {
    auto it = lut.find(ptr);
    if (it != lut.end()) {
        lut.erase(it);
    }
}

bool MemoryManager::is_valid_pointer(void* ptr) const {
    auto it = lut.find(ptr);
    return it != lut.end() && it->second.is_valid;
}

void MemoryManager::dump_lut() const {
    std::cout << "🔒 LUT (Lookup Table):\n";
    for (const auto& [ptr, alloc] : lut) {
        std::cout << "  " << ptr << " -> " << alloc.name 
                  << " (" << alloc.size << " bytes) [" 
                  << (alloc.is_valid ? "VALID" : "INVALID") << "]\n";
    }
}

MemoryManager::Stats MemoryManager::get_stats() const {
    Stats stats;
    stats.total_allocated = total_allocated;
    stats.total_freed = total_freed;
    stats.current_allocations = current_allocations;
    stats.lut_size = lut.size();
    return stats;
}

void MemoryManager::print_stats() const {
    auto stats = get_stats();
    std::cout << "📊 Memory Manager Stats:\n";
    std::cout << "  Total allocated: " << stats.total_allocated << " bytes\n";
    std::cout << "  Total freed: " << stats.total_freed << " bytes\n";
    std::cout << "  Current allocations: " << stats.current_allocations << "\n";
    std::cout << "  LUT entries: " << stats.lut_size << "\n";
}

void MemoryManager::reset() {
    for (auto& [ptr, alloc] : lut) {
        free(ptr);
    }
    lut.clear();
    total_allocated = 0;
    total_freed = 0;
    current_allocations = 0;
}

// ============================================
// MEMORY POOL
// ============================================
MemoryPool::MemoryPool(size_t block_size) 
    : head(nullptr), block_size(block_size), used_memory(0), total_memory(0) {}

MemoryPool::~MemoryPool() {
    reset();
}

void* MemoryPool::allocate(size_t size) {
    // Find free block
    Block* block = find_free_block(size);
    if (block) {
        block->free = false;
        used_memory += size;
        return block->data;
    }
    
    // Allocate new block
    add_block(size);
    block = head;
    while (block && block->next) {
        block = block->next;
    }
    if (block) {
        block->free = false;
        used_memory += size;
        return block->data;
    }
    
    return nullptr;
}

void MemoryPool::deallocate(void* ptr) {
    Block* current = head;
    while (current) {
        if (current->data == ptr) {
            current->free = true;
            used_memory -= current->size;
            return;
        }
        current = current->next;
    }
}

void MemoryPool::reset() {
    Block* current = head;
    while (current) {
        Block* next = current->next;
        delete[] current->data;
        delete current;
        current = next;
    }
    head = nullptr;
    used_memory = 0;
    total_memory = 0;
}

MemoryPool::Block* MemoryPool::find_free_block(size_t size) {
    Block* current = head;
    while (current) {
        if (current->free && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

void MemoryPool::add_block(size_t size) {
    Block* new_block = new Block();
    new_block->data = new uint8_t[size];
    new_block->size = size;
    new_block->free = true;
    new_block->next = nullptr;
    
    if (!head) {
        head = new_block;
    } else {
        Block* current = head;
        while (current->next) {
            current = current->next;
        }
        current->next = new_block;
    }
    
    total_memory += size;
}

} // namespace guardian::memory
