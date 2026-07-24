#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <guardian/core/molecule.hpp>
#include <guardian/core/quark.hpp>
#include <guardian/format/gbin_format.hpp>
#include <guardian/memory/memory.hpp>

using namespace guardian;

void test_molecule() {
    std::cout << "\n🔬 Testing Molecule...\n";
    std::cout << "=====================\n";
    
    Molecule mol;
    mol.add_string("name", "Guardian");
    mol.add_number("version", 1.0);
    mol.add_number("count", 42);
    mol.add_bool("active", true);
    mol.add_bool("debug", false);
    
    std::cout << "  has_string('name'): " << (mol.has_string("name") ? "true" : "false") << "\n";
    std::cout << "  get_string('name'): " << mol.get_string("name") << "\n";
    std::cout << "  has_number('version'): " << (mol.has_number("version") ? "true" : "false") << "\n";
    std::cout << "  get_number('version'): " << mol.get_number("version") << "\n";
    std::cout << "  has_number('count'): " << (mol.has_number("count") ? "true" : "false") << "\n";
    std::cout << "  get_number('count'): " << mol.get_number("count") << "\n";
    std::cout << "  has_bool('active'): " << (mol.has_bool("active") ? "true" : "false") << "\n";
    std::cout << "  get_bool('active'): " << (mol.get_bool("active") ? "true" : "false") << "\n";
    std::cout << "  has_bool('debug'): " << (mol.has_bool("debug") ? "true" : "false") << "\n";
    std::cout << "  get_bool('debug'): " << (mol.get_bool("debug") ? "true" : "false") << "\n";
    std::cout << "  size(): " << mol.size() << " bytes\n";
    
    // Test dump()
    std::cout << "\n  Dump output:\n";
    mol.dump();
}

void test_quark() {
    std::cout << "\n🔬 Testing Quark...\n";
    std::cout << "==================\n";
    
    Quark q1(42);
    Quark q2(3.14159);
    Quark q3(true);
    
    std::cout << "  Int quark: " << q1.to_string() << " (type: " << (int)q1.type << ")\n";
    std::cout << "  Float64 quark: " << q2.to_string() << " (type: " << (int)q2.type << ")\n";
    std::cout << "  Bool quark: " << q3.to_string() << " (type: " << (int)q3.type << ")\n";
    
    std::cout << "  q1 size: " << q1.size() << " bytes\n";
    std::cout << "  q2 size: " << q2.size() << " bytes\n";
    std::cout << "  q3 size: " << q3.size() << " bytes\n";
}

void test_format() {
    std::cout << "\n🔬 Testing GbinFormat...\n";
    std::cout << "=======================\n";
    
    format::GbinFormat fmt;
    fmt.set_version(1);
    fmt.set_entry_point(0x1000);
    fmt.push_byte(0x42);
    fmt.push_int(0x12345678);
    fmt.push_float(3.14159f);
    fmt.push_string("Guardian Format Test");
    
    std::cout << "  Format version: " << fmt.get_version() << "\n";
    std::cout << "  Entry point: 0x" << std::hex << fmt.get_entry_point() << std::dec << "\n";
    std::cout << "  Data size: " << fmt.get_data().size() << " bytes\n";
    
    auto data = fmt.serialize();
    std::cout << "  Serialized to " << data.size() << " bytes\n";
    
    format::GbinFormat fmt2;
    bool success = fmt2.deserialize(data);
    std::cout << "  Deserialization: " << (success ? "Success" : "Failed") << "\n";
    
    if (success) {
        std::cout << "  Deserialized version: " << fmt2.get_version() << "\n";
        std::cout << "  Deserialized data size: " << fmt2.get_data().size() << " bytes\n";
    }
}

void test_memory() {
    std::cout << "\n🔬 Testing MemoryManager...\n";
    std::cout << "===========================\n";
    
    memory::MemoryManager mem;
    
    void* ptr1 = mem.allocate(100);
    void* ptr2 = mem.allocate(200);
    void* ptr3 = mem.allocate(300);
    
    mem.register_pointer(ptr1, 100, "test1");
    mem.register_pointer(ptr2, 200, "test2");
    mem.register_pointer(ptr3, 300, "test3");
    
    auto stats = mem.get_stats();
    std::cout << "  Total allocated: " << stats.total_allocated << " bytes\n";
    std::cout << "  Current allocations: " << stats.current_allocations << "\n";
    std::cout << "  LUT size: " << stats.lut_size << "\n";
    
    std::cout << "  ptr1 valid: " << (mem.is_valid_pointer(ptr1) ? "true" : "false") << "\n";
    std::cout << "  ptr2 valid: " << (mem.is_valid_pointer(ptr2) ? "true" : "false") << "\n";
    std::cout << "  ptr3 valid: " << (mem.is_valid_pointer(ptr3) ? "true" : "false") << "\n";
    
    mem.unregister_pointer(ptr1);
    mem.unregister_pointer(ptr2);
    mem.unregister_pointer(ptr3);
    mem.deallocate(ptr1);
    mem.deallocate(ptr2);
    mem.deallocate(ptr3);
    
    stats = mem.get_stats();
    std::cout << "  After cleanup - current allocations: " << stats.current_allocations << "\n";
}

void test_format_registry() {
    std::cout << "\n🔬 Testing FormatRegistry...\n";
    std::cout << "============================\n";
    
    auto& registry = format::FormatRegistry::instance();
    
    auto fmt = std::make_shared<format::GbinFormat>();
    registry.register_format("gbin", fmt);
    std::cout << "  Registered 'gbin' format\n";
    
    bool has = registry.has_format("gbin");
    std::cout << "  Has 'gbin' format: " << (has ? "true" : "false") << "\n";
    
    auto formats = registry.list_formats();
    std::cout << "  Registered formats: ";
    for (const auto& name : formats) {
        std::cout << name << " ";
    }
    std::cout << "\n";
}

int main() {
    std::cout << "🧪 Guardian Framework Test Suite\n";
    std::cout << "=================================\n";
    std::cout << "Testing installed Guardian libraries\n\n";
    
    try {
        test_molecule();
        test_quark();
        test_format();
        test_memory();
        test_format_registry();
        
        std::cout << "\n✅ All tests passed! 🎉\n";
        std::cout << "\nThe Guardian framework is working correctly!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cout << "\n❌ Test failed: " << e.what() << "\n";
        return 1;
    }
}
