#pragma once
#include "../../core/atom.hpp"
#include <cstdint>
#include <vector>
#include <cstring>

namespace guardian {

class UInt8Atom : public Atom {
public:
    UInt8Atom() : value(0) {}
    explicit UInt8Atom(uint8_t v) : value(v) {}
    
    uint8_t get() const { return value; }
    void set(uint8_t v) { value = v; }
    
    size_t size() const override { return sizeof(uint8_t); }
    std::vector<uint8_t> serialize() const override {
        return std::vector<uint8_t>{value};
    }
    void deserialize(const std::vector<uint8_t>& data) override {
        if (!data.empty()) {
            value = data[0];
        }
    }
    const char* name() const override { return "UInt8Atom"; }
    AtomType type() const override { return AtomType::UINT8; }
    
private:
    uint8_t value;
};

} // namespace guardian
