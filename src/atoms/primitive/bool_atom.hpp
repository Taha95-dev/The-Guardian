#pragma once
#include "../../core/atom.hpp"
#include <cstdint>
#include <vector>
#include <cstring>

namespace guardian {

class BoolAtom : public Atom {
public:
    BoolAtom() : value(false) {}
    explicit BoolAtom(bool v) : value(v) {}
    
    bool get() const { return value; }
    void set(bool v) { value = v; }
    
    size_t size() const override { return sizeof(bool); }
    std::vector<uint8_t> serialize() const override {
        return std::vector<uint8_t>{static_cast<uint8_t>(value ? 1 : 0)};
    }
    void deserialize(const std::vector<uint8_t>& data) override {
        if (!data.empty()) {
            value = data[0] != 0;
        }
    }
    const char* name() const override { return "BoolAtom"; }
    AtomType type() const override { return AtomType::BOOL; }
    
private:
    bool value;
};

} // namespace guardian
