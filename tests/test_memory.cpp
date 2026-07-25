#include <iostream>
#include <cassert>
#include <vector>
#include <guardian/memory/memory.hpp>

using namespace guardian;

void test_memory() {
    std::cout << "  Testing memory...\n";
    
    // Test MemoryManager allocation
    {
        std::cout << "    Allocation... ";
        memory::MemoryManager mem;
        void* ptr = mem.allocate(1024);
        assert(ptr != nullptr);
        mem.deallocate(ptr);
        std::cout << "✅\n";
    }
    
    // Test MemoryManager pointer registration
    {
        std::cout << "    Pointer registration... ";
        memory::MemoryManager mem;
        void* ptr = mem.allocate(512);
        mem.register_pointer(ptr, 512, "test_buffer");
        assert(mem.is_valid_pointer(ptr));
        mem.unregister_pointer(ptr);
        mem.deallocate(ptr);
        std::cout << "✅\n";
    }
    
    // Test MemoryManager stats
    {
        std::cout << "    Stats tracking... ";
        memory::MemoryManager mem;
        void* p1 = mem.allocate(100);
        void* p2 = mem.allocate(200);
        void* p3 = mem.allocate(300);
        
        auto stats = mem.get_stats();
        assert(stats.total_allocated == 600);
        assert(stats.current_allocations == 3);
        
        mem.deallocate(p1);
        mem.deallocate(p2);
        mem.deallocate(p3);
        std::cout << "✅\n";
    }
    
    // Test MemoryPool
    {
        std::cout << "    MemoryPool... ";
        memory::MemoryPool pool(4096);
        void* ptr = pool.allocate(128);
        assert(ptr != nullptr);
        assert(pool.used() > 0);
        pool.deallocate(ptr);
        pool.reset();
        assert(pool.used() == 0);
        std::cout << "✅\n";
    }
    
    // Test LUT validity
    {
        std::cout << "    Pointer validity... ";
        memory::MemoryManager mem;
        void* ptr = mem.allocate(256);
        mem.register_pointer(ptr, 256, "valid_test");
        assert(mem.is_valid_pointer(ptr));
        
        void* invalid = (void*)0xDEADBEEF;
        assert(!mem.is_valid_pointer(invalid));
        
        mem.unregister_pointer(ptr);
        mem.deallocate(ptr);
        std::cout << "✅\n";
    }
    
    std::cout << "  ✅ All memory tests passed!\n";
}
