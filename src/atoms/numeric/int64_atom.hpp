#pragma once
#include "../../core/atom.hpp"
#include <cstdint>
#include <vector>
#include <cstring>

namespace guardian {

class Int64Atom : public Atom {
public:
    Int64Atom() : value(0) {}
    explicit Int64Atom(int64_t v) : value(v) {}
    
    int64_t get() const { return value; }
    void set(int64_t v) { value = v; }
    
    size_t size() const override { return sizeof(int64_t); }
    std::vector<uint8_t> serialize() const override {
        std::vector<uint8_t> data(sizeof(int64_t));
        std::memcpy(data.data(), &value, sizeof(int64_t));
        return data;
    }
    void deserialize(const std::vector<uint8_t>& data) override {
        if (data.size() >= sizeof(int64_t)) {
            std::memcpy(&value, data.data(), sizeof(int64_t));
        }
    }
    const char* name() const override { return "Int64Atom"; }
    AtomType type() const override { return AtomType::INT64; }
    
private:
    int64_t value;
};

} // namespace guardian
