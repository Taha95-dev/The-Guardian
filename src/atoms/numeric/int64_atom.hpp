#pragma once
#include "../../core/atom.hpp"
#include <cstring>

namespace guardian {

class Int64Atom : public Atom {
    int64_t value;
    AtomType type() const override { return AtomType::INT; }
    
public:
    Int64Atom() : value(0) {}
    explicit Int64Atom(int64_t val) : value(val) {}
    
    size_t size() const override { return sizeof(int64_t); }
    
    std::vector<uint8_t> serialize() const override {
        std::vector<uint8_t> bytes(sizeof(int64_t));
        std::memcpy(bytes.data(), &value, sizeof(int64_t));
        return bytes;
    }
    
    void deserialize(const std::vector<uint8_t>& data) override {
        if (data.size() >= sizeof(int64_t)) {
            std::memcpy(&value, data.data(), sizeof(int64_t));
        }
    }
    
    const char* name() const override { return "Int64Atom"; }
    
    int64_t get() const { return value; }
    void set(int64_t val) { value = val; }
};

} // namespace guardian
