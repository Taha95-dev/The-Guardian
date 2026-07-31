#include <guardian/memory/lazy.hpp>
#include <iostream>

using namespace guardian::memory;

int main() {
    std::cout << "🧪 Testing Lazy Allocation\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
    
    // ── Lazy Value ──
    {
        std::cout << "📦 Lazy<int>:\n";
        Lazy<int> lazy_int;
        
        std::cout << "  Before allocation: " << (lazy_int.is_allocated() ? "allocated" : "not allocated") << "\n";
        
        int& val = lazy_int.get();
        val = 42;
        
        std::cout << "  After allocation: " << (lazy_int.is_allocated() ? "allocated" : "not allocated") << "\n";
        std::cout << "  Value: " << lazy_int.get() << "\n";
        std::cout << "\n";
    }
    
    // ── Lazy Stack ──
    {
        std::cout << "📦 LazyStack<int>:\n";
        LazyStack<int> lazy_stack;
        
        std::cout << "  Before push: size = " << lazy_stack.size() << "\n";
        
        lazy_stack.push(1);
        lazy_stack.push(2);
        lazy_stack.push(3);
        
        std::cout << "  After push: size = " << lazy_stack.size() << "\n";
        std::cout << "  Pop: " << lazy_stack.pop() << "\n";
        std::cout << "  Pop: " << lazy_stack.pop() << "\n";
        std::cout << "  Pop: " << lazy_stack.pop() << "\n";
        
        std::cout << "\n";
    }
    
    // ── Lazy String ──
    {
        std::cout << "📦 Lazy<std::string>:\n";
        Lazy<std::string> lazy_str;
        
        std::cout << "  Before allocation: " << (lazy_str.is_allocated() ? "allocated" : "not allocated") << "\n";
        
        lazy_str.get() = "Hello, Lazy Allocation!";
        
        std::cout << "  After allocation: " << (lazy_str.is_allocated() ? "allocated" : "not allocated") << "\n";
        std::cout << "  Value: " << lazy_str.get() << "\n";
        std::cout << "\n";
    }
    
    std::cout << "✅ Lazy allocation tests passed!\n";
    return 0;
}
