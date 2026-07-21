#pragma once
#include "../../core/atom.hpp"
#include <cstring>

namespace guardian {

class Float64Atom : public Atom {
    double value;
    AtomType type() const override { return AtomType::FLOAT; }
    
public:
    Float64Atom() : value(0.0) {}
    explicit Float64Atom(double val) : value(val) {}
    
    size_t size() const override { return sizeof(double); }
    
    std::vector<uint8_t> serialize() const override {
        std::vector<uint8_t> bytes(sizeof(double));
        std::memcpy(bytes.data(), &value, sizeof(double));
        return bytes;
    }
    
    void deserialize(const std::vector<uint8_t>& data) override {
        if (data.size() >= sizeof(double)) {
            std::memcpy(&value, data.data(), sizeof(double));
        }
    }
    
    const char* name() const override { return "Float64Atom"; }
    
    double get() const { return value; }
    void set(double val) { value = val; }
};

} // namespace guardian
