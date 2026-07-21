#pragma once
#include "../../core/atom.hpp"
#include <vector>
#include <memory>
#include <cstring>

namespace guardian {

class ArrayAtom : public Atom {
    std::vector<std::shared_ptr<Atom>> value;
    AtomType type() const override { return AtomType::ARRAY; }
    
public:
    ArrayAtom() {}
    explicit ArrayAtom(const std::vector<std::shared_ptr<Atom>>& val) : value(val) {}
    
    size_t size() const override {
        size_t total = 4; // count
        for (const auto& atom : value) {
            total += 4 + atom->size(); // type + data
        }
        return total;
    }
    
    std::vector<uint8_t> serialize() const override {
        std::vector<uint8_t> data;
        uint32_t count = static_cast<uint32_t>(value.size());
        uint8_t* count_ptr = reinterpret_cast<uint8_t*>(&count);
        data.insert(data.end(), count_ptr, count_ptr + 4);
        
        for (const auto& atom : value) {
            uint8_t type_byte = static_cast<uint8_t>(atom->type());
            data.push_back(type_byte);
            auto atom_data = atom->serialize();
            uint32_t len = static_cast<uint32_t>(atom_data.size());
            uint8_t* len_ptr = reinterpret_cast<uint8_t*>(&len);
            data.insert(data.end(), len_ptr, len_ptr + 4);
            data.insert(data.end(), atom_data.begin(), atom_data.end());
        }
        return data;
    }
    
    void deserialize(const std::vector<uint8_t>& data) override {
        // TODO: Implement
    }
    
    const char* name() const override { return "ArrayAtom"; }
    
    void push_back(std::shared_ptr<Atom> atom) { value.push_back(atom); }
    std::shared_ptr<Atom> get(size_t index) const { return value[index]; }
    size_t length() const { return value.size(); }
};

} // namespace guardian
