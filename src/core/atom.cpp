#include "atom.hpp"
#include <cstring>

namespace guardian {

StringAtom::StringAtom(const std::string& val) : value(val) {}

size_t StringAtom::size() const {
    return value.size() + 1;  // +1 for null terminator
}

std::vector<uint8_t> StringAtom::serialize() const {
    std::vector<uint8_t> bytes(value.size() + 1);
    std::memcpy(bytes.data(), value.data(), value.size() + 1);
    return bytes;
}

void StringAtom::deserialize(const std::vector<uint8_t>& data) {
    if (data.empty()) return;
    value = std::string(reinterpret_cast<const char*>(data.data()));
}

const char* StringAtom::name() const {
    return "StringAtom";
}

const std::string& StringAtom::get() const {
    return value;
}

void StringAtom::set(const std::string& val) {
    value = val;
}

} // namespace guardian
