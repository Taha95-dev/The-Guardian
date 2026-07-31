#include "file.hpp"
#include <sstream>

namespace guardian::io {

File::File(const std::string& path) {
    open(path);
}

File::~File() {
    close();
}

bool File::open(const std::string& path) {
    close();
    file_path = path;
    file.open(path, std::ios::in | std::ios::out | std::ios::binary);
    return file.is_open();
}

void File::close() {
    if (file.is_open()) {
        file.close();
    }
}

bool File::is_open() const {
    return file.is_open();
}

std::string File::read_all() {
    if (!is_open()) return "";
    file.seekg(0, std::ios::beg);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::vector<std::string> File::read_lines() {
    std::vector<std::string> lines;
    if (!is_open()) return lines;
    file.seekg(0, std::ios::beg);
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

std::string File::read_line() {
    if (!is_open()) return "";
    std::string line;
    std::getline(file, line);
    return line;
}

bool File::write(const std::string& content) {
    if (!is_open()) return false;
    file.seekg(0, std::ios::beg);
    file << content;
    file.flush();
    return true;
}

bool File::write_line(const std::string& line) {
    return write(line + "\n");
}

bool File::append(const std::string& content) {
    if (!is_open()) return false;
    file.seekg(0, std::ios::end);
    file << content;
    file.flush();
    return true;
}

size_t File::size() {
    if (!is_open()) return 0;
    auto current = file.tellg();
    file.seekg(0, std::ios::end);
    auto size = file.tellg();
    file.seekg(current);
    return static_cast<size_t>(size);
}

bool File::exists() const {
    return std::filesystem::exists(file_path);
}

bool File::exists(const std::string& path) {
    return std::filesystem::exists(path);
}

bool File::remove(const std::string& path) {
    return std::filesystem::remove(path);
}

bool File::rename(const std::string& old_path, const std::string& new_path) {
    try {
        std::filesystem::rename(old_path, new_path);
        return true;
    } catch (...) {
        return false;
    }
}

} // namespace guardian::io
