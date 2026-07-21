#pragma once
#include "../../core/atom.hpp"
#include <fstream>
#include <string>

namespace guardian {

class FileAtom : public Atom {
    std::string path;
    std::fstream file;
    bool is_open;
    AtomType type() const override { return AtomType::CUSTOM; }
    
public:
    FileAtom() : is_open(false) {}
    explicit FileAtom(const std::string& path, std::ios::openmode mode = std::ios::in)
        : path(path), is_open(false) {
        open(path, mode);
    }
    
    size_t size() const override { return path.size() + 1; }
    
    std::vector<uint8_t> serialize() const override {
        std::vector<uint8_t> data(path.begin(), path.end());
        data.push_back('\0');
        return data;
    }
    
    void deserialize(const std::vector<uint8_t>& data) override {
        path = std::string(data.begin(), data.end());
    }
    
    const char* name() const override { return "FileAtom"; }
    
    bool open(const std::string& p, std::ios::openmode mode) {
        if (is_open) file.close();
        path = p;
        file.open(path, mode);
        is_open = file.is_open();
        return is_open;
    }
    
    void close() {
        if (is_open) {
            file.close();
            is_open = false;
        }
    }
    
    std::string read() {
        if (!is_open) return "";
        std::string content;
        char ch;
        while (file.get(ch)) {
            content += ch;
        }
        return content;
    }
    
    void write(const std::string& data) {
        if (is_open) {
            file << data;
        }
    }
    
    bool eof() const { return file.eof(); }
};

} // namespace guardian
