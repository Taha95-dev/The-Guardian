#include "gbin_format.hpp"
#include <iostream>
#include <cstring>

namespace guardian::format {

GbinFormat::GbinFormat() {
    init_header();
}

GbinFormat::GbinFormat(const std::vector<uint8_t>& data) {
    init_header();
    this->data = data;
    header.data_size = data.size();
}

void GbinFormat::init_header() {
    header.magic = MAGIC;
    header.version = 1;
    header.flags = 0;
    header.entry_point = sizeof(FormatHeader);
    header.data_size = 0;
    memset(header.reserved, 0, 8);
}

std::vector<uint8_t> GbinFormat::serialize() {
    std::vector<uint8_t> result;
    
    // Write header
    const uint8_t* header_ptr = reinterpret_cast<const uint8_t*>(&header);
    result.insert(result.end(), header_ptr, header_ptr + sizeof(header));
    
    // Write data
    result.insert(result.end(), data.begin(), data.end());
    
    return result;
}

bool GbinFormat::deserialize(const std::vector<uint8_t>& bytes) {
    if (bytes.size() < sizeof(FormatHeader)) {
        std::cerr << "Error: Data too small for .gbin header\n";
        return false;
    }
    
    // Read header
    memcpy(&header, bytes.data(), sizeof(header));
    
    // Verify magic
    if (header.magic != MAGIC) {
        std::cerr << "Error: Invalid .gbin magic number (expected " 
                  << std::hex << MAGIC << ", got " << header.magic << ")\n";
        return false;
    }
    
    // Read data
    size_t data_size = header.data_size;
    if (bytes.size() >= sizeof(header) + data_size) {
        data.assign(bytes.begin() + sizeof(header), 
                    bytes.begin() + sizeof(header) + data_size);
        return true;
    }
    
    std::cerr << "Error: Incomplete .gbin data\n";
    return false;
}

void GbinFormat::push_byte(uint8_t byte) {
    data.push_back(byte);
    header.data_size = data.size();
}

void GbinFormat::push_bytes(const std::vector<uint8_t>& bytes) {
    data.insert(data.end(), bytes.begin(), bytes.end());
    header.data_size = data.size();
}

void GbinFormat::push_int(int value) {
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&value);
    data.insert(data.end(), ptr, ptr + sizeof(value));
    header.data_size = data.size();
}

void GbinFormat::push_float(float value) {
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&value);
    data.insert(data.end(), ptr, ptr + sizeof(value));
    header.data_size = data.size();
}

void GbinFormat::push_string(const std::string& str) {
    // Push length first
    uint32_t len = str.length();
    const uint8_t* len_ptr = reinterpret_cast<const uint8_t*>(&len);
    data.insert(data.end(), len_ptr, len_ptr + 4);
    
    // Push string data
    data.insert(data.end(), str.begin(), str.end());
    header.data_size = data.size();
}

} // namespace guardian::format
