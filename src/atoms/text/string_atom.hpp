#pragma once
#include "../../core/atom.hpp"
#include <string>
#include <vector>
#include <cstring>

namespace guardian {

class StringAtom : public Atom {
public:
    StringAtom() : value("") {}
    explicit StringAtom(const std::string& v) : value(v) {}
    explicit StringAtom(const char* v) : value(v ? v : "") {}
    
    std::string get() const { return value; }
    void set(const std::string& v) { value = v; }
    
    size_t size() const override { return value.size(); }
    std::vector<uint8_t> serialize() const override {
        std::vector<uint8_t> data;
        uint32_t len = value.size();
        data.push_back(len & 0xFF);
        data.push_back((len >> 8) & 0xFF);
        data.push_back((len >> 16) & 0xFF);
        data.push_back((len >> 24) & 0xFF);
        data.insert(data.end(), value.begin(), value.end());
        return data;
    }
    void deserialize(const std::vector<uint8_t>& data) override {
        if (data.size() >= 4) {
            uint32_t len = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
            if (data.size() >= 4 + len) {
                value.assign(data.begin() + 4, data.begin() + 4 + len);
            }
        }
    }
    const char* name() const override { return "StringAtom"; }
    AtomType type() const override { return AtomType::STRING; }
    
private:
    std::string value;
};

} // namespace guardian
