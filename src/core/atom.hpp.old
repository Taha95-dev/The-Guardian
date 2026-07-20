#pragma once
#include <cstddef>
#include <vector>
#include <cstdint>
#include <cstring>
#include <string>
#include <typeinfo>
#include <memory>

namespace guardian {

// ============================================
// BASE ATOM
// ============================================
class Atom {
public:
    virtual ~Atom() = default;
    
    virtual size_t size() const = 0;
    virtual std::vector<uint8_t> serialize() const = 0;
    virtual void deserialize(const std::vector<uint8_t>& data) = 0;
    virtual const char* name() const = 0;
};

// ============================================
// PRIMITIVE ATOM (int, float, bool, etc.)
// ============================================
template<typename T>
class PrimitiveAtom : public Atom {
    static_assert(std::is_arithmetic_v<T>, "T must be arithmetic (int, float, etc.)");
    
    T value;
    
public:
    PrimitiveAtom() : value(0) {}
    explicit PrimitiveAtom(T val) : value(val) {}
    
    size_t size() const override {
        return sizeof(T);
    }
    
    std::vector<uint8_t> serialize() const override {
        std::vector<uint8_t> bytes(sizeof(T));
        std::memcpy(bytes.data(), &value, sizeof(T));
        return bytes;
    }
    
    void deserialize(const std::vector<uint8_t>& data) override {
        if (data.size() >= sizeof(T)) {
            std::memcpy(&value, data.data(), sizeof(T));
        }
    }
    
    const char* name() const override {
        return typeid(T).name();
    }
    
    T get() const { return value; }
    void set(T val) { value = val; }
};

// ============================================
// STRING ATOM
// ============================================
class StringAtom : public Atom {
    std::string value;
    
public:
    StringAtom() = default;
    explicit StringAtom(const std::string& val);
    
    size_t size() const override;
    std::vector<uint8_t> serialize() const override;
    void deserialize(const std::vector<uint8_t>& data) override;
    const char* name() const override;
    
    const std::string& get() const;
    void set(const std::string& val);
};

// ============================================
// BOOL ATOM
// ============================================
class BoolAtom : public Atom {
    bool value;
    
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
    
    bool get() const { return value; }
    void set(bool val) { value = val; }
};

// ============================================
// CHAR ATOM
// ============================================
class CharAtom : public Atom {
    char value;
    
public:
    CharAtom() : value(0) {}
    explicit CharAtom(char val) : value(val) {}
    
    size_t size() const override { return 1; }
    
    std::vector<uint8_t> serialize() const override {
        return std::vector<uint8_t>{static_cast<uint8_t>(value)};
    }
    
    void deserialize(const std::vector<uint8_t>& data) override {
        if (!data.empty()) {
            value = static_cast<char>(data[0]);
        }
    }
    
    const char* name() const override { return "CharAtom"; }
    
    char get() const { return value; }
    void set(char val) { value = val; }
};

// ============================================
// ARRAY ATOM (Collection of atoms)
// ============================================
class ArrayAtom : public Atom {
    std::vector<std::unique_ptr<Atom>> elements;
    
public:
    ArrayAtom() = default;
    
    void add_element(std::unique_ptr<Atom> element) {
        elements.push_back(std::move(element));
    }
    
    size_t size() const override {
        size_t total = 4;  // 4 bytes for count
        for (const auto& elem : elements) {
            total += elem->size();
        }
        return total;
    }
    
    std::vector<uint8_t> serialize() const override {
        std::vector<uint8_t> bytes;
        
        // Write number of elements (4 bytes)
        uint32_t count = static_cast<uint32_t>(elements.size());
        uint8_t* count_ptr = reinterpret_cast<uint8_t*>(&count);
        bytes.insert(bytes.end(), count_ptr, count_ptr + 4);
        
        // Write each element
        for (const auto& elem : elements) {
            auto elem_bytes = elem->serialize();
            bytes.insert(bytes.end(), elem_bytes.begin(), elem_bytes.end());
        }
        
        return bytes;
    }
    
    void deserialize(const std::vector<uint8_t>& data) override {
        // TODO: Implement array deserialization
        // This is more complex and requires type info
    }
    
    const char* name() const override { return "ArrayAtom"; }

    const std::vector<std::unique_ptr<Atom>>& get_elements() const {
        return elements;
    } 
    size_t count() const { return elements.size(); }
};

// ============================================
// NULL ATOM (Nothing / Empty)
// ============================================
class NullAtom : public Atom {
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
