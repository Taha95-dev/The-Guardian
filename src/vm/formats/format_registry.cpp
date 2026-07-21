#include "format_registry.hpp"
#include <iostream>

namespace guardian::vm::formats {

FormatRegistry& FormatRegistry::instance() {
    static FormatRegistry registry;
    return registry;
}

void FormatRegistry::registerFormat(const FormatInfo& format) {
    formats[format.name] = format;
    std::cout << "  📦 Registered format: " << format.name 
              << " (" << format.extension << ")\n";
}

bool FormatRegistry::hasFormat(const std::string& name) const {
    return formats.find(name) != formats.end();
}

const FormatInfo& FormatRegistry::getFormat(const std::string& name) const {
    return formats.at(name);
}

std::vector<std::string> FormatRegistry::listFormats() const {
    std::vector<std::string> names;
    for (const auto& [name, info] : formats) {
        names.push_back(name);
    }
    return names;
}

void FormatRegistry::printFormats() const {
    std::cout << "\n📦 Registered Formats:\n";
    for (const auto& [name, info] : formats) {
        std::cout << "  " << name << " (" << info.extension << ") - " 
                  << info.description << "\n";
    }
}

} // namespace guardian::vm::formats
