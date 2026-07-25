#pragma once
#include "../../core/atom.hpp"
#include <cstdint>
#include <vector>
#include <cstring>

namespace guardian {

class CharAtom : public Atom {
public:
    CharAtom() : value('\0') {}
    explicit CharAtom(char v) : value(v) {}
    
    char get() const { return value; }
    void set(char v) { value = v; }
    
    size_t size() const override { return sizeof(char); }
    std::vector<uint8_t> serialize() const override {
        return std::vector<uint8_t>{static_cast<uint8_t>(value)};
    }
    void deserialize(const std::vector<uint8_t>& data) override {
        if (!data.empty()) {
            value = static_cast<char>(data[0]);
        }
    }
    const char* name() const override { return "CharAtom"; }
    AtomType type() const override { return AtomType::CHAR; }
    
private:
    char value;
};

} // namespace guardian
