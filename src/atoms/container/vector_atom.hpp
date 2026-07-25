#pragma once
#include "../../core/atom.hpp"
#include <vector>
#include <cstdint>
#include <cstring>

namespace guardian {

class VectorAtom : public Atom {
public:
    VectorAtom() = default;
    explicit VectorAtom(const std::vector<uint8_t>& data) : data(data) {}
    
    void push_back(uint8_t byte) { data.push_back(byte); }
    size_t size() const override { return data.size(); }
    const std::vector<uint8_t>& get() const { return data; }
    
    std::vector<uint8_t> serialize() const override {
        std::vector<uint8_t> result;
        uint32_t len = data.size();
        result.push_back(len & 0xFF);
        result.push_back((len >> 8) & 0xFF);
        result.push_back((len >> 16) & 0xFF);
        result.push_back((len >> 24) & 0xFF);
        result.insert(result.end(), data.begin(), data.end());
        return result;
    }
    
    void deserialize(const std::vector<uint8_t>& data) override {
        if (data.size() >= 4) {
            uint32_t len = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
            this->data.clear();
            if (data.size() >= 4 + len) {
                this->data.insert(this->data.end(), data.begin() + 4, data.begin() + 4 + len);
            }
        }
    }
    
    const char* name() const override { return "VectorAtom"; }
    AtomType type() const override { return AtomType::CUSTOM; }
    
private:
    std::vector<uint8_t> data;
};

} // namespace guardian
