#pragma once
#include "../../core/atom.hpp"
#include <cstdint>
#include <vector>
#include <cstring>

namespace guardian {

class Int16Atom : public Atom {
public:
    Int16Atom() : value(0) {}
    explicit Int16Atom(int16_t v) : value(v) {}
    
    int16_t get() const { return value; }
    void set(int16_t v) { value = v; }
    
    size_t size() const override { return sizeof(int16_t); }
    std::vector<uint8_t> serialize() const override {
        std::vector<uint8_t> data(sizeof(int16_t));
        std::memcpy(data.data(), &value, sizeof(int16_t));
        return data;
    }
    void deserialize(const std::vector<uint8_t>& data) override {
        if (data.size() >= sizeof(int16_t)) {
            std::memcpy(&value, data.data(), sizeof(int16_t));
        }
    }
    const char* name() const override { return "Int16Atom"; }
    AtomType type() const override { return AtomType::INT16; }
    
private:
    int16_t value;
};

} // namespace guardian
