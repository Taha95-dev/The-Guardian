#pragma once
#include "../../core/atom.hpp"
#include <vector>
#include <memory>
#include <string>
#include <cstdint>

namespace guardian {

class ArrayAtom : public Atom {
    std::vector<std::unique_ptr<Atom>> elements;
    AtomType type() const override { return AtomType::ARRAY; }
    
public:
    ArrayAtom() = default;
    
    void add_element(std::unique_ptr<Atom> element) {
        elements.push_back(std::move(element));
    }
    
    size_t size() const override {
        size_t total = 4;
        for (const auto& elem : elements) {
            total += elem->size();
        }
        return total;
    }
    
    std::vector<uint8_t> serialize() const override {
        std::vector<uint8_t> bytes;
        uint32_t count = static_cast<uint32_t>(elements.size());
        uint8_t* count_ptr = reinterpret_cast<uint8_t*>(&count);
        bytes.insert(bytes.end(), count_ptr, count_ptr + 4);
        
        for (const auto& elem : elements) {
            auto elem_bytes = elem->serialize();
            bytes.insert(bytes.end(), elem_bytes.begin(), elem_bytes.end());
        }
        return bytes;
    }
    
    void deserialize(const std::vector<uint8_t>& data) override {
        // TODO: Implement array deserialization
        // Need type information to reconstruct atoms
    }
    
    const char* name() const override { return "ArrayAtom"; }
    
    size_t count() const { return elements.size(); }
    
    const std::vector<std::unique_ptr<Atom>>& get_elements() const {
        return elements;
    }
};

} // namespace guardian
