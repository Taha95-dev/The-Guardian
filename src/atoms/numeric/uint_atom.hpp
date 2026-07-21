#pragma once
#include "../../core/atom.hpp"
#include <cstring>

namespace guardian {

class UIntAtom : public Atom {
    uint32_t value;
    AtomType type() const override { return AtomType::INT; }
    
public:
    UIntAtom() : value(0) {}
    explicit UIntAtom(uint32_t val) : value(val) {}
    
    size_t size() const override { return sizeof(uint32_t); }
    
    std::vector<uint8_t> serialize() const override {
        std::vector<uint8_t> bytes(sizeof(uint32_t));
        std::memcpy(bytes.data(), &value, sizeof(uint32_t));
        return bytes;
    }
    
    void deserialize(const std::vector<uint8_t>& data) override {
        if (data.size() >= sizeof(uint32_t)) {
            std::memcpy(&value, data.data(), sizeof(uint32_t));
        }
    }
    
    const char* name() const override { return "UIntAtom"; }
    
    uint32_t get() const { return value; }
    void set(uint32_t val) { value = val; }
};

} // namespace guardian
