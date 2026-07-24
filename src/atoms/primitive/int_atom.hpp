#pragma once
#include "../../core/atom.hpp"
#include <cstring>  // ← ADD THIS

namespace guardian {

class IntAtom : public Atom {
    int value;
    AtomType type() const override { return AtomType::INT; }
    
public:
    IntAtom() : value(0) {}
    explicit IntAtom(int val) : value(val) {}
    
    size_t size() const override { return sizeof(int); }
    
    std::vector<uint8_t> serialize() const override {
        std::vector<uint8_t> bytes(sizeof(int));
        std::memcpy(bytes.data(), &value, sizeof(int));
        return bytes;
    }
    
    void deserialize(const std::vector<uint8_t>& data) override {
        if (data.size() >= sizeof(int)) {
            std::memcpy(&value, data.data(), sizeof(int));
        }
    }
    
    const char* name() const override { return "IntAtom"; }
    void release() override {}
    
    int get() const { return value; }
    void set(int val) { value = val; }
};

} // namespace guardian
