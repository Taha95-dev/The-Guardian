#pragma once

#include "format.hpp"
#include <vector>
#include <string>
#include <cstring>

namespace guardian::format {

// ============================================
// GBIN FORMAT — Guardian Binary Format
// ============================================
class GbinFormat : public Format {
public:
    // Magic number for .gbin files
    static constexpr uint32_t MAGIC = 0x4742494E;  // "GBIN"
    
    GbinFormat();
    explicit GbinFormat(const std::vector<uint8_t>& data);
    
    // Format methods
    std::vector<uint8_t> serialize() override;
    bool deserialize(const std::vector<uint8_t>& data) override;
    
    // Gbin-specific methods
    void set_magic(uint32_t magic) { header.magic = magic; }
    uint32_t get_magic() const { return header.magic; }
    
    void set_version(uint16_t version) { header.version = version; }
    uint16_t get_version() const { return header.version; }
    
    void set_entry_point(uint32_t entry) { header.entry_point = entry; }
    uint32_t get_entry_point() const { return header.entry_point; }
    
    void set_data(const std::vector<uint8_t>& data) { this->data = data; header.data_size = data.size(); }
    const std::vector<uint8_t>& get_data() const { return data; }
    
    // Convenience methods
    void push_byte(uint8_t byte);
    void push_bytes(const std::vector<uint8_t>& bytes);
    void push_int(int value);
    void push_float(float value);
    void push_string(const std::string& str);
    
private:
    void init_header();
};

} // namespace guardian::format
