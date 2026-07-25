#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <memory>

namespace guardian {

// ── Atom Types ──
enum class AtomType : uint8_t {
    // Numeric types
    INT8,
    INT16,
    INT32,
    INT64,
    UINT8,
    UINT16,
    UINT32,
    UINT64,
    FLOAT32,
    FLOAT64,
    
    // Basic types
    INT,        // Alias for INT32
    FLOAT,      // Alias for FLOAT64
    BOOL,
    CHAR,
    STRING,
    
    // Container types
    ARRAY,
    VECTOR,
    LIST,
    MAP,
    
    // Special types
    POINTER,
    NULL_TYPE,
    CUSTOM
};

// ── Atom Base Class ──
class Atom {
public:
    virtual ~Atom() = default;
    
    // Core methods
    virtual size_t size() const = 0;
    virtual std::vector<uint8_t> serialize() const = 0;
    virtual void deserialize(const std::vector<uint8_t>& data) = 0;
    virtual const char* name() const = 0;
    virtual AtomType type() const = 0;
    
    // Utility
    virtual std::string to_string() const {
        return std::string(name());
    }
};

} // namespace guardian
