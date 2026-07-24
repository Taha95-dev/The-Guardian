#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <fstream>
#include <memory>
#include <unordered_map>  // ← ADD THIS

namespace guardian::format {

// ============================================
// FORMAT — Base binary format
// ============================================
struct FormatHeader {
    uint32_t magic;
    uint16_t version;
    uint16_t flags;
    uint32_t entry_point;
    uint32_t data_size;
    uint8_t reserved[8];
};

class Format {
public:
    virtual ~Format() = default;
    
    // Serialize data to binary format
    virtual std::vector<uint8_t> serialize() = 0;
    
    // Deserialize from binary data
    virtual bool deserialize(const std::vector<uint8_t>& data) = 0;
    
    // Read from file
    virtual bool read(const std::string& path);
    
    // Write to file
    virtual bool write(const std::string& path) const;
    
protected:
    FormatHeader header;
    std::vector<uint8_t> data;
};

// ============================================
// FORMAT REGISTRY — Register custom formats
// ============================================
class FormatRegistry {
public:
    static FormatRegistry& instance();
    
    void register_format(const std::string& name, std::shared_ptr<Format> format);
    std::shared_ptr<Format> get_format(const std::string& name) const;
    bool has_format(const std::string& name) const;
    std::vector<std::string> list_formats() const;
    
private:
    std::unordered_map<std::string, std::shared_ptr<Format>> formats;
};

} // namespace guardian::format
