#pragma once
#include <cstdint>
#include <string>

namespace guardian {

enum class QuarkType : uint8_t {
    INT,
    UINT,
    INT64,
    UINT64,
    FLOAT,
    FLOAT64,
    BOOL,
    CHAR,
    POINTER,
    NULL_TYPE
};

struct Quark {
    QuarkType type;
    union {
        int32_t int_val;
        uint32_t uint_val;
        int64_t int64_val;
        uint64_t uint64_val;
        float float_val;
        double float64_val;
        bool bool_val;
        char char_val;
        void* ptr_val;
    };
    
    Quark() : type(QuarkType::NULL_TYPE), int_val(0) {}
    Quark(int v) : type(QuarkType::INT), int_val(v) {}
    Quark(unsigned int v) : type(QuarkType::UINT), uint_val(v) {}
    Quark(int64_t v) : type(QuarkType::INT64), int64_val(v) {}
    Quark(uint64_t v) : type(QuarkType::UINT64), uint64_val(v) {}
    Quark(float v) : type(QuarkType::FLOAT), float_val(v) {}
    Quark(double v) : type(QuarkType::FLOAT64), float64_val(v) {}
    Quark(bool v) : type(QuarkType::BOOL), bool_val(v) {}
    Quark(char v) : type(QuarkType::CHAR), char_val(v) {}
    Quark(void* v) : type(QuarkType::POINTER), ptr_val(v) {}
    
    size_t size() const {
        switch (type) {
            case QuarkType::INT: return sizeof(int32_t);
            case QuarkType::UINT: return sizeof(uint32_t);
            case QuarkType::INT64: return sizeof(int64_t);
            case QuarkType::UINT64: return sizeof(uint64_t);
            case QuarkType::FLOAT: return sizeof(float);
            case QuarkType::FLOAT64: return sizeof(double);
            case QuarkType::BOOL: return sizeof(bool);
            case QuarkType::CHAR: return sizeof(char);
            case QuarkType::POINTER: return sizeof(void*);
            default: return 0;
        }
    }
    
    std::string to_string() const {
        switch (type) {
            case QuarkType::INT: return std::to_string(int_val);
            case QuarkType::UINT: return std::to_string(uint_val);
            case QuarkType::INT64: return std::to_string(int64_val);
            case QuarkType::UINT64: return std::to_string(uint64_val);
            case QuarkType::FLOAT: return std::to_string(float_val);
            case QuarkType::FLOAT64: return std::to_string(float64_val);
            case QuarkType::BOOL: return bool_val ? "true" : "false";
            case QuarkType::CHAR: return std::string(1, char_val);
            case QuarkType::POINTER: return "ptr:" + std::to_string((uintptr_t)ptr_val);
            default: return "null";
        }
    }
};

} // namespace guardian
