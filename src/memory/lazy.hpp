#pragma once
#include <memory>
#include <vector>
#include <optional>
#include <iostream>
#include <typeinfo>

namespace guardian::memory {

template<typename T>
class Lazy {
public:
    Lazy() : value(nullptr) {}
    
    // Constructor with initializer
    Lazy(T&& val) : value(std::make_unique<T>(std::move(val))) {}
    
    // Check if allocated
    bool is_allocated() {
        return value != nullptr;
    }
    
    // Allocate if not already
    T& get() {
        if (!value) {
            value = std::make_unique<T>();
            std::cout << "🔷 Lazy allocation: " << typeid(T).name() << "\n";
        }
        return *value;
    }
    
    // Get without allocation
    T* get_or_null() {
        return value.get();
    }
    
    // Reset allocation
    void reset() {
        value.reset();
    }
    
    // Force allocation
    void allocate() {
        if (!value) {
            value = std::make_unique<T>();
        }
    }
    
    // Check if allocated (const version)
    bool is_allocated() const {
        return value != nullptr;
    }
    
    // Get const (no allocation)
    const T* get_or_null() const {
        return value.get();
    }

private:
    std::unique_ptr<T> value;
};

// ── Lazy Stack ──
template<typename T>
class LazyStack {
public:
    LazyStack() : allocated(false) {}
    
    void push(const T& val) {
        if (!allocated) {
            stack = std::make_unique<std::vector<T>>();
            allocated = true;
            std::cout << "🔷 LazyStack allocated\n";
        }
        stack->push_back(val);
    }
    
    T pop() {
        if (!allocated || stack->empty()) {
            return T();
        }
        T val = stack->back();
        stack->pop_back();
        return val;
    }
    
    bool empty() const {
        return !allocated || stack->empty();
    }
    
    size_t size() const {
        return allocated ? stack->size() : 0;
    }
    
    void reset() {
        if (allocated) {
            stack.reset();
            allocated = false;
        }
    }
    
    // Get top without popping
    T& get_top() {
        static T empty;
        if (!allocated || stack->empty()) {
            return empty;
        }
        return stack->back();
    }
    
    const T& get_top() const {
        static T empty;
        if (!allocated || stack->empty()) {
            return empty;
        }
        return stack->back();
    }

private:
    std::unique_ptr<std::vector<T>> stack;
    bool allocated;
};

} // namespace guardian::memory
