#include <guardian/memory/lazy.hpp>
#include <guardian/vm/vm.hpp>
#include <iostream>
#include <cassert>
#include <string>

using namespace guardian::memory;
using namespace guardian::vm;

// ── Test 1: Lazy Value ──
void test_lazy_value() {
    std::cout << "📦 Test 1: Lazy Value\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    Lazy<int> lazy_int;
    
    // Check initial state
    assert(!lazy_int.is_allocated());
    std::cout << "  ✅ Initial state: not allocated\n";
    
    // Allocate and set
    lazy_int.get() = 42;
    assert(lazy_int.is_allocated());
    assert(lazy_int.get() == 42);
    std::cout << "  ✅ Allocated and set: " << lazy_int.get() << "\n";
    
    // Reset
    lazy_int.reset();
    assert(!lazy_int.is_allocated());
    std::cout << "  ✅ Reset successful\n\n";
}

// ── Test 2: Lazy Stack ──
void test_lazy_stack() {
    std::cout << "📦 Test 2: Lazy Stack\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    LazyStack<int> stack;
    
    // Check initial state
    assert(stack.empty());
    assert(stack.size() == 0);
    std::cout << "  ✅ Initial state: empty, size=0\n";
    
    // Push values
    stack.push(10);
    stack.push(20);
    stack.push(30);
    
    assert(!stack.empty());
    assert(stack.size() == 3);
    std::cout << "  ✅ After push: size=" << stack.size() << "\n";
    
    // Pop values
    assert(stack.pop() == 30);
    assert(stack.pop() == 20);
    assert(stack.pop() == 10);
    assert(stack.empty());
    std::cout << "  ✅ After pop: empty\n\n";
}

// ── Test 3: Lazy String ──
void test_lazy_string() {
    std::cout << "📦 Test 3: Lazy String\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    Lazy<std::string> lazy_str;
    
    // Check initial state
    assert(!lazy_str.is_allocated());
    std::cout << "  ✅ Initial state: not allocated\n";
    
    // Allocate and set
    lazy_str.get() = "Hello, Lazy Allocation!";
    assert(lazy_str.is_allocated());
    assert(lazy_str.get() == "Hello, Lazy Allocation!");
    std::cout << "  ✅ Allocated and set: \"" << lazy_str.get() << "\"\n";
    
    // Reset
    lazy_str.reset();
    assert(!lazy_str.is_allocated());
    std::cout << "  ✅ Reset successful\n\n";
}

// ── Test 4: Lazy Stack with Complex Types ──
void test_lazy_stack_complex() {
    std::cout << "📦 Test 4: Lazy Stack with Complex Types\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    LazyStack<std::string> stack;
    
    stack.push("First");
    stack.push("Second");
    stack.push("Third");
    
    assert(stack.size() == 3);
    assert(stack.get_top() == "Third");
    std::cout << "  ✅ Top: " << stack.get_top() << "\n";
    
    assert(stack.pop() == "Third");
    assert(stack.pop() == "Second");
    assert(stack.pop() == "First");
    assert(stack.empty());
    std::cout << "  ✅ All popped\n\n";
}

// ── Test 5: VM with Lazy Stack ──
void test_vm_lazy_stack() {
    std::cout << "📦 Test 5: VM with Lazy Stack\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    
    VM vm;
    
    // VM should start with empty stack
    std::cout << "  ✅ VM created\n";
    
    // Push values (use std::string to avoid bool conversion)
    vm.push(Value(42));
    vm.push(Value(3.14));
    vm.push(Value(true));
    vm.push(Value(std::string("Hello")));  // Explicit std::string
    
    std::cout << "  ✅ Values pushed\n";
    
    // Pop and verify (LIFO order)
    auto v1 = vm.pop(); // "Hello" (last pushed)
    auto v2 = vm.pop(); // true
    auto v3 = vm.pop(); // 3.14
    auto v4 = vm.pop(); // 42 (first pushed)
    
    std::cout << "  v1: " << v1.to_string() << "\n";
    std::cout << "  v2: " << v2.to_string() << "\n";
    std::cout << "  v3: " << v3.to_string() << "\n";
    std::cout << "  v4: " << v4.to_string() << "\n";
    
    assert(v1.to_string() == "Hello");
    assert(v2.to_string() == "true");
    assert(v3.to_string() == "3.140000");
    assert(v4.to_string() == "42");
    
    std::cout << "  ✅ All values popped and verified\n\n";
}

// ── Main ──
int main() {
    std::cout << "🧪 Testing The Guardian's Lazy Allocator\n";
    std::cout << "=========================================\n\n";
    
    test_lazy_value();
    test_lazy_stack();
    test_lazy_string();
    test_lazy_stack_complex();
    test_vm_lazy_stack();
    
    std::cout << "✅ All tests passed! 🎉\n";
    std::cout << "\nThe Guardian's lazy allocator is working perfectly!\n";
    std::cout << "Memory is only allocated when needed.\n";
    
    return 0;
}
