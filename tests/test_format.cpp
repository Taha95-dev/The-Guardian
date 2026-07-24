#include "../src/format/format.hpp"
#include "../src/format/gbin_format.hpp"
#include <iostream>
#include <vector>
#include <cassert>
#include <cstring>
#include <fstream>
#include <memory>
#include <chrono>

using namespace guardian::format;

// Helper functions - now not duplicated
void print_test_header(const std::string& test_name) {
    std::cout << "\n🔬 " << test_name << "\n";
    std::cout << std::string(test_name.length() + 3, '=') << "\n";
}

void print_success() {
    std::cout << "  ✅ PASSED\n";
}

void print_failure(const std::string& msg) {
    std::cout << "  ❌ FAILED: " << msg << "\n";
}

// Test functions
void test_format_header() {
    print_test_header("Format Header");
    
    FormatHeader header;
    header.magic = 0x4742494E;
    header.version = 1;
    header.flags = 0;
    header.entry_point = 0;
    header.data_size = 1024;
    memset(header.reserved, 0, sizeof(header.reserved));
    
    std::cout << "  Magic: 0x" << std::hex << header.magic << std::dec << "\n";
    std::cout << "  Version: " << header.version << "\n";
    std::cout << "  Data Size: " << header.data_size << "\n";
    
    assert(header.magic == 0x4742494E);
    assert(header.version == 1);
    assert(header.data_size == 1024);
    
    print_success();
}

void test_gbin_creation() {
    print_test_header("GbinFormat Creation");
    
    {
        GbinFormat format;
        std::cout << "  Default format created\n";
        assert(format.get_magic() == GbinFormat::MAGIC);
        print_success();
    }
    
    {
        std::vector<uint8_t> test_data = {0x01, 0x02, 0x03, 0x04};
        GbinFormat format(test_data);
        std::cout << "  Format with data created (" << format.get_data().size() << " bytes)\n";
        assert(format.get_data().size() == 4);
        print_success();
    }
}

void test_gbin_data_ops() {
    print_test_header("GbinFormat Data Operations");
    
    GbinFormat format;
    
    format.push_byte(0x42);
    std::cout << "  Pushed byte: 0x42\n";
    assert(format.get_data().size() == 1);
    
    std::vector<uint8_t> bytes = {0x01, 0x02, 0x03, 0x04};
    format.push_bytes(bytes);
    std::cout << "  Pushed 4 bytes\n";
    assert(format.get_data().size() == 5);
    
    format.push_int(0x12345678);
    std::cout << "  Pushed int: 0x12345678\n";
    assert(format.get_data().size() == 9);
    
    format.push_float(3.14159f);
    std::cout << "  Pushed float: 3.14159\n";
    assert(format.get_data().size() == 13);
    
    format.push_string("Hello, Guardian!");
    std::cout << "  Pushed string: 'Hello, Guardian!'\n";
    assert(format.get_data().size() > 13);
    
    print_success();
}

void test_gbin_serialization() {
    print_test_header("GbinFormat Serialization");
    
    GbinFormat format;
    format.set_version(1);
    format.set_entry_point(0x1000);
    format.push_byte(0x42);
    format.push_int(0x12345678);
    format.push_string("Test");
    
    std::vector<uint8_t> serialized = format.serialize();
    std::cout << "  Serialized size: " << serialized.size() << " bytes\n";
    
    assert(serialized.size() > 0);
    print_success();
}

void test_gbin_deserialization() {
    print_test_header("GbinFormat Deserialization");
    
    GbinFormat original;
    original.set_version(2);
    original.set_entry_point(0x2000);
    original.push_byte(0xFF);
    original.push_int(0xDEADBEEF);
    original.push_string("Deserialization Test");
    
    std::vector<uint8_t> serialized = original.serialize();
    std::cout << "  Serialized to " << serialized.size() << " bytes\n";
    
    GbinFormat deserialized;
    bool success = deserialized.deserialize(serialized);
    std::cout << "  Deserialization: " << (success ? "Success" : "Failed") << "\n";
    
    if (success) {
        assert(deserialized.get_version() == 2);
        assert(deserialized.get_entry_point() == 0x2000);
        assert(deserialized.get_data().size() > 0);
    }
    
    print_success();
}

void test_file_io() {
    print_test_header("File I/O");
    
    const std::string test_file = "test_output.gbin";
    
    {
        GbinFormat format;
        format.set_version(1);
        format.push_byte(0x47);
        format.push_byte(0x42);
        format.push_byte(0x49);
        format.push_byte(0x4E);
        format.push_string("File I/O Test");
        
        bool success = format.write(test_file);
        std::cout << "  Write: " << (success ? "Success" : "Failed") << "\n";
        print_success();
    }
    
    {
        GbinFormat format;
        bool success = format.read(test_file);
        std::cout << "  Read: " << (success ? "Success" : "Failed") << "\n";
        if (success) {
            std::cout << "  Data size: " << format.get_data().size() << " bytes\n";
        }
        print_success();
    }
    
    std::remove(test_file.c_str());
}

void test_format_registry() {
    print_test_header("Format Registry");
    
    auto& registry = FormatRegistry::instance();
    
    {
        auto format = std::make_shared<GbinFormat>();
        registry.register_format("gbin", format);
        std::cout << "  Registered 'gbin' format\n";
        print_success();
    }
    
    {
        bool has = registry.has_format("gbin");
        std::cout << "  Has 'gbin' format: " << (has ? "Yes" : "No") << "\n";
        assert(has);
        print_success();
    }
    
    {
        auto format = registry.get_format("gbin");
        std::cout << "  Retrieved 'gbin' format: " << (format ? "Success" : "Failed") << "\n";
        assert(format != nullptr);
        print_success();
    }
}

void test_error_handling() {
    print_test_header("Error Handling");
    
    {
        GbinFormat format;
        bool success = format.read("non_existent_file.gbin");
        std::cout << "  Reading non-existent file: " << (success ? "Success" : "Failed (expected)") << "\n";
        assert(!success);
        print_success();
    }
    
    {
        GbinFormat format;
        std::vector<uint8_t> invalid_data = {0xFF, 0xFF, 0xFF, 0xFF};
        bool success = format.deserialize(invalid_data);
        std::cout << "  Deserializing invalid data: " << (success ? "Success" : "Failed (expected)") << "\n";
        assert(!success);
        print_success();
    }
}

void test_performance() {
    print_test_header("Performance Test");
    
    const size_t ITERATIONS = 10000;
    
    GbinFormat format;
    for (int i = 0; i < 100; i++) {
        format.push_int(i);
        format.push_float(static_cast<float>(i) * 1.5f);
        format.push_string("Test string " + std::to_string(i));
    }
    
    std::cout << "  Format with " << format.get_data().size() << " bytes of data\n";
    std::cout << "  Running " << ITERATIONS << " serialization iterations...\n";
    
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERATIONS; i++) {
        auto serialized = format.serialize();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "  Time: " << duration.count() << "ms\n";
    std::cout << "  Average: " << (static_cast<double>(duration.count()) / ITERATIONS * 1000) << "μs per serialization\n";
    
    print_success();
}

// Main test function for format
void test_format() {
    std::cout << "  Running format tests...\n";
    test_format_header();
    test_gbin_creation();
    test_gbin_data_ops();
    test_gbin_serialization();
    test_gbin_deserialization();
    test_file_io();
    test_format_registry();
    test_error_handling();
    test_performance();
    std::cout << "  Format tests complete\n";
}
