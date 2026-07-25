#pragma once
#include "../../core/atom.hpp"
#include <cstdint>
#include <vector>
#include <cstring>

namespace guardian {

class Float32Atom : public Atom {
public:
    Float32Atom() : value(0.0f) {}
    explicit Float32Atom(float v) : value(v) {}
    
    float get() const { return value; }
    void set(float v) { value = v; }
    
    size_t size() const override { return sizeof(float); }
    std::vector<uint8_t> serialize() const override {
        std::vector<uint8_t> data(sizeof(float));
        std::memcpy(data.data(), &value, sizeof(float));
        return data;
    }
    void deserialize(const std::vector<uint8_t>& data) override {
        if (data.size() >= sizeof(float)) {
            std::memcpy(&value, data.data(), sizeof(float));
        }
    }
    const char* name() const override { return "Float32Atom"; }
    AtomType type() const override { return AtomType::FLOAT32; }
    
private:
    float value;
};

} // namespace guardian
