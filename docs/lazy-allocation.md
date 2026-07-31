# 🧠 Lazy Allocation in The Guardian

Lazy allocation is a memory optimization technique where memory is **only allocated when it's actually used**. This makes The Guardian more efficient, faster, and memory-safe.

## Why Lazy Allocation?

### The Problem

Without Lazy Allocation:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
❌ Stack allocated at VM creation (even if empty)
❌ Memory wasted for empty VMs
❌ Slower startup
❌ More memory pressure
text


### The Solution

With Lazy Allocation:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
✅ Stack allocated only on first use
✅ Memory used only when needed
✅ Faster startup
✅ Less memory usage
✅ More efficient
text


## How It Works

### Lazy<T> — Lazy Value

```cpp
#include <guardian/memory/lazy.hpp>

using namespace guardian::memory;

Lazy<int> lazy_int;

// Before: No memory allocated
std::cout << lazy_int.is_allocated(); // false

// After get(): Memory is allocated
int& value = lazy_int.get(); // 🔷 Lazy allocation: i
value = 42;

std::cout << lazy_int.is_allocated(); // true
std::cout << lazy_int.get(); // 42

LazyStack<T> — Lazy Stack
cpp

#include <guardian/memory/lazy.hpp>

using namespace guardian::memory;

LazyStack<int> stack;

// Before: No memory allocated
std::cout << stack.size(); // 0
std::cout << stack.empty(); // true

// After first push: Memory is allocated
stack.push(1); // 🔷 LazyStack allocated
stack.push(2);
stack.push(3);

std::cout << stack.size(); // 3
std::cout << stack.pop(); // 3

API Reference
Lazy<T>
Method	Description
bool is_allocated()	Check if memory is allocated
T& get()	Get value (allocates if needed)
T* get_or_null()	Get pointer (no allocation)
void reset()	Free allocated memory
void allocate()	Force allocation
LazyStack<T>
Method	Description
void push(const T& val)	Push value (allocates on first push)
T pop()	Pop value (returns T() if empty)
T& get_top()	Peek at top (no pop)
bool empty()	Check if stack is empty
size_t size()	Get stack size
void reset()	Free allocated memory
Example: Lazy VM Stack
cpp

#include <guardian/vm/vm.hpp>
#include <guardian/memory/lazy.hpp>

class VM {
private:
    LazyStack<Value> stack;
    
public:
    void push(const Value& val) {
        stack.push(val); // Allocates on first push
    }
    
    Value pop() {
        return stack.pop();
    }
};

Performance Benefits
Operation	Without Lazy	With Lazy	Improvement
Empty VM	~24 bytes	0 bytes	100%
Startup	Allocates stack	No allocation	Faster
1 value	~24 bytes	~24 bytes	Same
10 values	~104 bytes	~104 bytes	Same
Memory Savings
text

Memory Usage:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Empty VM:    0 bytes  (was 24 bytes)
1 value:     ~24 bytes (same)
10 values:   ~104 bytes (same)
100 values:  ~824 bytes (same)

Savings: Up to 24 bytes per empty VM

When to Use Lazy Allocation
✅ Use Lazy Allocation When:

    Memory might not be used

    Startup performance matters

    Resources are limited

    You want to be memory-efficient

❌ Don't Use Lazy Allocation When:

    Memory will definitely be used

    Overhead of checking is too high

    Simplicity is more important than efficiency

The Guardian's Lazy Implementation

The Guardian uses lazy allocation for:

    VM stack (only allocated when values are pushed)

    Future: Bytecode loading

    Future: Symbol tables

    Future: String tables

Benefits for Language Developers
text

For Language Developers:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
✅ Less memory usage
✅ Faster startup
✅ More efficient VMs
✅ Better performance
✅ Clean API
✅ Easy to use

See It In Action
bash

# Run the lazy allocation test
cd ~/Documents/The-Guardian
./test_lazy

# Output shows when allocation happens:
# 🔷 Lazy allocation: i
# 🔷 LazyStack allocated

Next Steps

    User Guide

    API Reference

    Examples

The Guardian — Safe by Design, Not by Choice.
