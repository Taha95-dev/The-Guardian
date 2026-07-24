#pragma once
#include "../../core/atom.hpp"
#include <vector>
#include <memory>
#include <string>
#include <cstdint>
#include <cstring>

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
   
    void release() override {
        for (auto& elem : elements) {
            if (elem) {
                elem->release();
            }
        }
        elements.clear();
        elements.shrink_to_fit();
    }

    void deserialize(const std::vector<uint8_t>& data) override {
        if (data.size() < 4) return;
        
        size_t pos = 0;
        uint32_t count;
        std::memcpy(&count, data.data(), 4);
        pos += 4;
        
        for (uint32_t i = 0; i < count && pos < data.size(); i++) {
            // Read element type
            if (pos >= data.size()) break;
            uint8_t type_byte = data[pos++];
            
            // Read element size
            if (pos + 4 > data.size()) break;
            uint32_t elem_size;
            std::memcpy(&elem_size, data.data() + pos, 4);
            pos += 4;
            
            // Read element data
            if (pos + elem_size > data.size()) break;
            std::vector<uint8_t> elem_data(data.begin() + pos, data.begin() + pos + elem_size);
            pos += elem_size;
            
            // Create atom based on type
            std::unique_ptr<Atom> elem;
            switch (static_cast<AtomType>(type_byte)) {
                case AtomType::INT:
                    elem = std::make_unique<IntAtom>();
                    elem->deserialize(elem_data);
                    break;
                case AtomType::FLOAT:
                    elem = std::make_unique<FloatAtom>();
                    elem->deserialize(elem_data);
                    break;
                case AtomType::BOOL:
                    elem = std::make_unique<BoolAtom>();
                    elem->deserialize(elem_data);
                    break;
                case AtomType::CHAR:
                    elem = std::make_unique<CharAtom>();
                    elem->deserialize(elem_data);
                    break;
                case AtomType::STRING:
                    elem = std::make_unique<StringAtom>();
                    elem->deserialize(elem_data);
                    break;
                case AtomType::ARRAY:
                    elem = std::make_unique<ArrayAtom>();
                    elem->deserialize(elem_data);
                    break;
                default:
                    // Skip unknown type
                    continue;
            }
            
            if (elem) {
                elements.push_back(std::move(elem));
            }
        }
    }
    
    const char* name() const override { return "ArrayAtom"; }
    
    size_t count() const { return elements.size(); }
    
    const std::vector<std::unique_ptr<Atom>>& get_elements() const {
        return elements;
    }
};

} // namespace guardian
