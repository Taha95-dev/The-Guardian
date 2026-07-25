#pragma once
#include "../../core/atom.hpp"
#include <cstdint>
#include <vector>
#include <cstring>

namespace guardian {

class UIntAtom : public Atom {
public:
    UIntAtom() : value(0) {}
    explicit UIntAtom(uint64_t v) : value(v) {}
    
    uint64_t get() const { return value; }
    void set(uint64_t v) { value = v; }
    
    size_t size() const override { return sizeof(uint64_t); }
    std::vector<uint8_t> serialize() const override {
        std::vector<uint8_t> data(sizeof(uint64_t));
        std::memcpy(data.data(), &value, sizeof(uint64_t));
        return data;
    }
    void deserialize(const std::vector<uint8_t>& data) override {
        if (data.size() >= sizeof(uint64_t)) {
            std::memcpy(&value, data.data(), sizeof(uint64_t));
        }
    }
    const char* name() const override { return "UIntAtom"; }
    AtomType type() const override { return AtomType::UINT64; }
    
private:
    uint64_t value;
};

} // namespace guardian
