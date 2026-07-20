#pragma once
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include "../opcodes.hpp"

namespace guardian::vm {

// ============================================
// BINARY FORMAT — Custom binary layout
// ============================================
struct BinaryFormat {
    std::string name;
    std::string magic;             // File signature (e.g., "GURD")
    uint16_t version;
    uint16_t flags;
    
    // Callback to parse the binary
    std::function<Bytecode(const std::vector<uint8_t>&)> parser;
    
    // Callback to generate binary from bytecode
    std::function<std::vector<uint8_t>(const Bytecode&)> generator;
};

// ============================================
// FORMAT REGISTRY — Register custom formats
// ============================================
class FormatRegistry {
private:
    std::unordered_map<std::string, BinaryFormat> formats;
    std::unordered_map<std::string, BinaryFormat> formats_by_magic;
    
public:
    // Register a new binary format
    void register_format(const BinaryFormat& format) {
        formats[format.name] = format;
        if (!format.magic.empty()) {
            formats_by_magic[format.magic] = format;
        }
    }
    
    // Get a format by name
    BinaryFormat* get_format(const std::string& name) {
        auto it = formats.find(name);
        return it != formats.end() ? &it->second : nullptr;
    }
    
    // Get a format by magic bytes
    BinaryFormat* get_format_by_magic(const std::vector<uint8_t>& data) {
        for (const auto& [magic, format] : formats_by_magic) {
            if (data.size() >= magic.size()) {
                if (std::equal(magic.begin(), magic.end(), data.begin())) {
                    return const_cast<BinaryFormat*>(&format);
                }
            }
        }
        return nullptr;
    }
    
    // List all registered formats
    std::vector<std::string> list_formats() const {
        std::vector<std::string> names;
        for (const auto& [name, _] : formats) {
            names.push_back(name);
        }
        return names;
    }
};

// Global registry instance
extern FormatRegistry registry;

} // namespace guardian::vm
