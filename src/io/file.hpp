#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>

namespace guardian::io {

class File {
public:
    File() = default;
    explicit File(const std::string& path);
    ~File();

    bool open(const std::string& path);
    void close();
    bool is_open() const;

    // Read operations
    std::string read_all();
    std::vector<std::string> read_lines();
    std::string read_line();

    // Write operations
    bool write(const std::string& content);
    bool write_line(const std::string& line);
    bool append(const std::string& content);

    // File info
    size_t size();  // Remove const
    bool exists() const;
    std::string path() const { return file_path; }

    // Static helpers
    static bool exists(const std::string& path);
    static bool remove(const std::string& path);
    static bool rename(const std::string& old_path, const std::string& new_path);

private:
    std::string file_path;
    mutable std::fstream file;  // Make mutable so const methods can use it
};

} // namespace guardian::io
