#include "../src/memory/memory.hpp"
#include <iostream>
#include <cassert>
#include <memory>
#include <cstring>

using namespace guardian::memory;

// Forward declarations
void test_memory_manager_basic();
void test_memory_stats();
void test_memory_lut();
void test_memory_reset();
void test_cache_manager();
void test_memory_pool();
void test_stress();

// Main test function for memory
void test_memory() {
    std::cout << "  Running memory tests...\n";
    test_memory_manager_basic();
    test_memory_stats();
    test_memory_lut();
    test_memory_reset();
    test_cache_manager();
    test_memory_pool();
    test_stress();
    std::cout << "  Memory tests complete\n";
}

void test_memory_manager_basic() {
    std::cout << "    MemoryManager Basic Operations...\n";
    MemoryManager mem;
    
    void* ptr = mem.allocate(64);
    assert(ptr != nullptr);
    mem.deallocate(ptr);
    
    std::cout << "    ✅ PASSED\n";
}

void test_memory_stats() {
    std::cout << "    MemoryManager Stats...\n";
    MemoryManager mem;
    
    auto stats = mem.get_stats();
    assert(stats.total_allocated == 0);
    assert(stats.current_allocations == 0);
    
    void* ptr1 = mem.allocate(100);
    void* ptr2 = mem.allocate(200);
    
    stats = mem.get_stats();
    assert(stats.total_allocated == 300);
    assert(stats.current_allocations == 2);
    
    mem.deallocate(ptr1);
    mem.deallocate(ptr2);
    
    std::cout << "    ✅ PASSED\n";
}

void test_memory_lut() {
    std::cout << "    MemoryManager LUT...\n";
    MemoryManager mem;
    
    void* ptr = mem.allocate(512);
    mem.register_pointer(ptr, 512, "lut_test");
    
    assert(mem.is_valid_pointer(ptr));
    assert(!mem.is_valid_pointer((void*)0xDEADBEEF));
    
    mem.unregister_pointer(ptr);
    mem.deallocate(ptr);
    
    std::cout << "    ✅ PASSED\n";
}

void test_memory_reset() {
    std::cout << "    MemoryManager Reset...\n";
    MemoryManager mem;
    
    void* ptr1 = mem.allocate(100);
    void* ptr2 = mem.allocate(200);
    mem.register_pointer(ptr1, 100, "reset_test1");
    mem.register_pointer(ptr2, 200, "reset_test2");
    
    auto stats = mem.get_stats();
    assert(stats.current_allocations == 2);
    assert(stats.lut_size == 2);
    
    mem.reset();
    
    stats = mem.get_stats();
    assert(stats.current_allocations == 0);
    assert(stats.lut_size == 0);
    
    std::cout << "    ✅ PASSED\n";
}

void test_cache_manager() {
    std::cout << "    CacheManager...\n";
    CacheManager cache;
    
    std::string cache_dir = cache.get_cache_dir();
    std::cout << "      Cache directory: " << cache_dir << "\n";
    
    std::string hash = "test_hash_12345";
    std::vector<uint8_t> test_data = {0x01, 0x02, 0x03, 0x04, 0x05};
    
    bool saved = cache.save_cache(hash, test_data);
    if (saved) {
        assert(cache.has_cache(hash));
        
        std::vector<uint8_t> loaded_data;
        bool loaded = cache.load_cache(hash, loaded_data);
        assert(loaded);
        assert(loaded_data.size() == test_data.size());
        
        cache.clear_cache(hash);
        assert(!cache.has_cache(hash));
    }
    
    std::cout << "    ✅ PASSED\n";
}

void test_memory_pool() {
    std::cout << "    MemoryPool...\n";
    
    MemoryPool pool(4096);
    assert(pool.used() == 0);
    
    void* ptr1 = pool.allocate(128);
    assert(ptr1 != nullptr);
    assert(pool.used() > 0);
    
    void* ptr2 = pool.allocate(512);
    assert(ptr2 != nullptr);
    
    pool.deallocate(ptr2);
    pool.deallocate(ptr1);
    
    pool.reset();
    assert(pool.used() == 0);
    
    std::cout << "    ✅ PASSED\n";
}

void test_stress() {
    std::cout << "    Stress Test...\n";
    
    MemoryManager mem;
    const int NUM_ALLOCATIONS = 1000;
    std::vector<void*> pointers;
    
    for (int i = 0; i < NUM_ALLOCATIONS; i++) {
        size_t size = (i % 64) + 1;
        void* ptr = mem.allocate(size);
        pointers.push_back(ptr);
        mem.register_pointer(ptr, size, "stress_" + std::to_string(i));
    }
    
    auto stats = mem.get_stats();
    assert(stats.current_allocations == NUM_ALLOCATIONS);
    
    for (int i = 0; i < NUM_ALLOCATIONS; i++) {
        mem.unregister_pointer(pointers[i]);
        mem.deallocate(pointers[i]);
    }
    
    stats = mem.get_stats();
    assert(stats.current_allocations == 0);
    
    std::cout << "    ✅ PASSED\n";
}
