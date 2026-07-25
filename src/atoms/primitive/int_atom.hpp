#pragma once
#include "../../core/atom.hpp"
#include <cstdint>
#include <vector>
#include <cstring>

namespace guardian {

class IntAtom : public Atom {
public:
    IntAtom() : value(0) {}
    explicit IntAtom(int32_t v) : value(v) {}
    
    int32_t get() const { return value; }
    void set(int32_t v) { value = v; }
    
    size_t size() const override { return sizeof(int32_t); }
    std::vector<uint8_t> serialize() const override {
        std::vector<uint8_t> data(sizeof(int32_t));
        std::memcpy(data.data(), &value, sizeof(int32_t));
        return data;
    }
    void deserialize(const std::vector<uint8_t>& data) override {
        if (data.size() >= sizeof(int32_t)) {
            std::memcpy(&value, data.data(), sizeof(int32_t));
        }
    }
    const char* name() const override { return "IntAtom"; }
    AtomType type() const override { return AtomType::INT32; }
    
private:
    int32_t value;
};

} // namespace guardian
