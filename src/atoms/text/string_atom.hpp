#pragma once
#include "../../core/atom.hpp"
#include <string>
#include <cstring>  // ← ADD THIS

namespace guardian {

class StringAtom : public Atom {
    std::string value;
    AtomType type() const override { return AtomType::STRING; }
    
public:
    StringAtom() = default;
    explicit StringAtom(const std::string& val) : value(val) {}
    
    size_t size() const override { return value.size() + 1; }
    
    std::vector<uint8_t> serialize() const override {
        std::vector<uint8_t> bytes(value.size() + 1);
        std::memcpy(bytes.data(), value.data(), value.size() + 1);
        return bytes;
    }
    
    void deserialize(const std::vector<uint8_t>& data) override {
        if (data.empty()) return;
        value = std::string(reinterpret_cast<const char*>(data.data()));
    }
    
    const char* name() const override { return "StringAtom"; }
    
    const std::string& get() const { return value; }
    void set(const std::string& val) { value = val; }
};

} // namespace guardian
