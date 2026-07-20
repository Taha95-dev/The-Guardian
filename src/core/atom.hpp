#pragma once
#include <cstddef>
#include <vector>
#include <cstdint>
#include <string>

namespace guardian {

// ============================================
// ATOM TYPE ENUM — Fast type checking (no RTTI!)
// ============================================
enum class AtomType : uint8_t {
    INT,
    FLOAT,
    STRING,
    BOOL,
    CHAR,
    ARRAY,
    NULL_TYPE,
    CUSTOM
};

// ============================================
// BASE ATOM CLASS
// ============================================
class Atom {
public:
    virtual ~Atom() = default;
    
    virtual size_t size() const = 0;
    virtual std::vector<uint8_t> serialize() const = 0;
    virtual void deserialize(const std::vector<uint8_t>& data) = 0;
    virtual const char* name() const = 0;
    
    // Fast type checking — NO dynamic_cast!
    virtual AtomType type() const = 0;
};

} // namespace guardian
