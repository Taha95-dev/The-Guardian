#pragma once
#include "../../core/atom.hpp"
#include <cstring>  // ← ADD THIS

namespace guardian {

class CharAtom : public Atom {
    char value;
    AtomType type() const override { return AtomType::CHAR; }
    
public:
    CharAtom() : value(0) {}
    explicit CharAtom(char val) : value(val) {}
    
    size_t size() const override { return 1; }
    
    std::vector<uint8_t> serialize() const override {
        return std::vector<uint8_t>{static_cast<uint8_t>(value)};
    }
    
    void deserialize(const std::vector<uint8_t>& data) override {
        if (!data.empty()) {
            value = static_cast<char>(data[0]);
        }
    }
    
    const char* name() const override { return "CharAtom"; }
    
    char get() const { return value; }
    void set(char val) { value = val; }
};

} // namespace guardian
