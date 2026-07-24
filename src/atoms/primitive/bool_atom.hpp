#pragma once
#include "../../core/atom.hpp"
#include <cstring>  // ← ADD THIS

namespace guardian {

class BoolAtom : public Atom {
    bool value;
    AtomType type() const override { return AtomType::BOOL; }
    
public:
    BoolAtom() : value(false) {}
    explicit BoolAtom(bool val) : value(val) {}
    
    size_t size() const override { return 1; }
    
    std::vector<uint8_t> serialize() const override {
        return std::vector<uint8_t>{static_cast<uint8_t>(value ? 1 : 0)};
    }
    
    void deserialize(const std::vector<uint8_t>& data) override {
        if (!data.empty()) {
            value = data[0] != 0;
        }
    }
    
    const char* name() const override { return "BoolAtom"; }
    void release() override {};
    
    bool get() const { return value; }
    void set(bool val) { value = val; }
};

} // namespace guardian
