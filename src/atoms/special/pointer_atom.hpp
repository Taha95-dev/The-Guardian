#pragma once
#include "../../core/atom.hpp"

namespace guardian {

template<typename T>
class PointerAtom : public Atom {
    T* ptr;
    
public:
    PointerAtom() : ptr(nullptr) {}
    explicit PointerAtom(T* p) : ptr(p) {}
    
    size_t size() const override { return sizeof(void*); }
    
    std::vector<uint8_t> serialize() const override {
        std::vector<uint8_t> bytes(sizeof(void*));
        std::memcpy(bytes.data(), &ptr, sizeof(void*));
        return bytes;
    }
    
    void deserialize(const std::vector<uint8_t>& data) override {
        if (data.size() >= sizeof(void*)) {
            std::memcpy(&ptr, data.data(), sizeof(void*));
        }
    }
    
    const char* name() const override { return "PointerAtom"; }
    
    T* get() const { return ptr; }
    void set(T* p) { ptr = p; }
};

} // namespace guardian
