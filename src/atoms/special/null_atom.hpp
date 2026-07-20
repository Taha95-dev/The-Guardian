#pragma once
#include "../../core/atom.hpp"

namespace guardian {

class NullAtom : public Atom {
    AtomType type() const override { return AtomType::NULL_TYPE; }
public:
    size_t size() const override { return 0; }
    
    std::vector<uint8_t> serialize() const override {
        return {};
    }
    
    void deserialize(const std::vector<uint8_t>& data) override {
        // Nothing to deserialize
    }
    
    const char* name() const override { return "NullAtom"; }
};

} // namespace guardian
