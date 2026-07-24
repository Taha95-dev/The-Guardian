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
        if (data.size() < 4) return;
        
        // Read element count
        uint32_t count;
        std::memcpy(&count, data.data(), 4);
        size_t pos = 4;
        
        // Clear existing elements
        elements.clear();
        
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
                    continue;
            }
            
            if (elem) {
                elements.push_back(std::move(elem));
            }
        }
    }
    
    const char* name() const override { return "ArrayAtom"; }
    
    void push_back(std::shared_ptr<Atom> atom) { value.push_back(atom); }
    std::shared_ptr<Atom> get(size_t index) const { return value[index]; }
    size_t length() const { return value.size(); }
};

} // namespace guardian
