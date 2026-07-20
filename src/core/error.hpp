#pragma once
#include <exception>
#include <string>
#include <source_location>

namespace guardian {

// ============================================
// GUARDIAN ERROR SYSTEM
// ============================================

class Error : public std::exception {
    std::string message;
    std::string file;
    int line;
    
public:
    Error(const std::string& msg,
          const std::string& file = "",
          int line = 0) noexcept
        : message(msg), file(file), line(line) {}
    
    const char* what() const noexcept override {
        return message.c_str();
    }
    
    std::string full_message() const {
        if (!file.empty()) {
            return message + " (at " + file + ":" + std::to_string(line) + ")";
        }
        return message;
    }
};

// Helper macro for error reporting
#define GUARDIAN_ERROR(msg) \
    guardian::Error(msg, __FILE__, __LINE__)

#define GUARDIAN_THROW(msg) \
    throw GUARDIAN_ERROR(msg)

#define GUARDIAN_ASSERT(cond, msg) \
    if (!(cond)) { GUARDIAN_THROW(msg); }

} // namespace guardian
