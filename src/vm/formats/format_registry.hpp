#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <unordered_map>
#include <functional>

namespace guardian::vm::formats {

// Forward declarations
struct GuardianHeader;
struct Instruction;

// Format registry
struct FormatInfo {
    std::string name;
    std::string extension;
    std::string description;
    uint16_t version;
    
    // Parse function
    std::function<std::vector<Instruction>(const std::vector<uint8_t>&)> parser;
    
    // Generate function
    std::function<std::vector<uint8_t>(const std::vector<Instruction>&)> generator;
};

class FormatRegistry {
public:
    static FormatRegistry& instance();
    
    void registerFormat(const FormatInfo& format);
    bool hasFormat(const std::string& name) const;
    const FormatInfo& getFormat(const std::string& name) const;
    std::vector<std::string> listFormats() const;
    void printFormats() const;
    
private:
    std::unordered_map<std::string, FormatInfo> formats;
};

} // namespace guardian::vm::formats
