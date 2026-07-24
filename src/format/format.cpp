#include "format.hpp"
#include <iostream>
#include <cstring>

namespace guardian::format {

bool Format::read(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in.is_open()) {
        return false;
    }
    
    // Read header
    in.read(reinterpret_cast<char*>(&header), sizeof(header));
    if (!in.good()) {
        return false;
    }
    
    // Read data
    data.resize(header.data_size);
    in.read(reinterpret_cast<char*>(data.data()), header.data_size);
    
    return in.good() || in.eof();
}

bool Format::write(const std::string& path) const {
    std::ofstream out(path, std::ios::binary);
    if (!out.is_open()) {
        return false;
    }
    
    // Write header
    out.write(reinterpret_cast<const char*>(&header), sizeof(header));
    if (!out.good()) {
        return false;
    }
    
    // Write data
    out.write(reinterpret_cast<const char*>(data.data()), data.size());
    
    return out.good();
}

// ============================================
// FORMAT REGISTRY
// ============================================
FormatRegistry& FormatRegistry::instance() {
    static FormatRegistry registry;
    return registry;
}

void FormatRegistry::register_format(const std::string& name, std::shared_ptr<Format> format) {
    formats[name] = format;
    std::cout << "  📦 Registered format: " << name << "\n";
}

std::shared_ptr<Format> FormatRegistry::get_format(const std::string& name) const {
    auto it = formats.find(name);
    if (it != formats.end()) {
        return it->second;
    }
    return nullptr;
}

bool FormatRegistry::has_format(const std::string& name) const {
    return formats.find(name) != formats.end();
}

std::vector<std::string> FormatRegistry::list_formats() const {
    std::vector<std::string> names;
    for (const auto& [name, _] : formats) {
        names.push_back(name);
    }
    return names;
}

} // namespace guardian::format
