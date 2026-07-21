#pragma once
#include "../../core/atom.hpp"
#include <vector>
#include <cstring>

namespace guardian {

class BytesAtom : public Atom {
    std::vector<uint8_t> value;
    AtomType type() const override { return AtomType::ARRAY; }
    
public:
    BytesAtom() {}
    explicit BytesAtom(const std::vector<uint8_t>& val) : value(val) {}
    explicit BytesAtom(const uint8_t* data, size_t len) : value(data, data + len) {}
    
    size_t size() const override { return value.size(); }
    
    std::vector<uint8_t> serialize() const override {
        std::vector<uint8_t> data;
        uint32_t len = value.size();
        data.insert(data.end(), reinterpret_cast<uint8_t*>(&len), 
                   reinterpret_cast<uint8_t*>(&len) + 4);
        data.insert(data.end(), value.begin(), value.end());
        return data;
    }
    
    void deserialize(const std::vector<uint8_t>& data) override {
        if (data.size() < 4) return;
        uint32_t len;
        std::memcpy(&len, data.data(), 4);
        value.assign(data.begin() + 4, data.begin() + 4 + len);
    }
    
    const char* name() const override { return "BytesAtom"; }
    
    const std::vector<uint8_t>& get() const { return value; }
    void set(const std::vector<uint8_t>& val) { value = val; }
    size_t length() const { return value.size(); }
    uint8_t& operator[](size_t index) { return value[index]; }
};

} // namespace guardian
