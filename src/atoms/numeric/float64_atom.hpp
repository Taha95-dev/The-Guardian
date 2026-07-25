#pragma once
#include "../../core/atom.hpp"
#include <cstdint>
#include <vector>
#include <cstring>

namespace guardian {

class Float64Atom : public Atom {
public:
    Float64Atom() : value(0.0) {}
    explicit Float64Atom(double v) : value(v) {}
    
    double get() const { return value; }
    void set(double v) { value = v; }
    
    size_t size() const override { return sizeof(double); }
    std::vector<uint8_t> serialize() const override {
        std::vector<uint8_t> data(sizeof(double));
        std::memcpy(data.data(), &value, sizeof(double));
        return data;
    }
    void deserialize(const std::vector<uint8_t>& data) override {
        if (data.size() >= sizeof(double)) {
            std::memcpy(&value, data.data(), sizeof(double));
        }
    }
    const char* name() const override { return "Float64Atom"; }
    AtomType type() const override { return AtomType::FLOAT64; }
    
private:
    double value;
};

} // namespace guardian
