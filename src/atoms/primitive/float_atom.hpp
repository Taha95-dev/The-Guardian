#pragma once
#include "../../core/atom.hpp"
#include <cstring>  // ← ADD THIS

namespace guardian {

class FloatAtom : public Atom {
    float value;
    AtomType type() const override { return AtomType::FLOAT; }
    
public:
    FloatAtom() : value(0.0f) {}
    explicit FloatAtom(float val) : value(val) {}
    
    size_t size() const override { return sizeof(float); }
    
    std::vector<uint8_t> serialize() const override {
        std::vector<uint8_t> bytes(sizeof(float));
        std::memcpy(bytes.data(), &value, sizeof(float));
        return bytes;
    }
    
    void deserialize(const std::vector<uint8_t>& data) override {
        if (data.size() >= sizeof(float)) {
            std::memcpy(&value, data.data(), sizeof(float));
        }
    }
    
    const char* name() const override { return "FloatAtom"; }
    
    float get() const { return value; }
    void set(float val) { value = val; }
};

} // namespace guardian
