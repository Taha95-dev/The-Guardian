#include <iostream>
#include <cassert>
#include <guardian/format/gbin_format.hpp>

using namespace guardian;

void test_format() {
    std::cout << "  Testing format...\n";
    
    // Test GbinFormat creation
    {
        std::cout << "    Creating GbinFormat... ";
        format::GbinFormat fmt;
        assert(fmt.get_magic() == format::GbinFormat::MAGIC);
        std::cout << "✅\n";
    }
    
    // Test push operations
    {
        std::cout << "    Push operations... ";
        format::GbinFormat fmt;
        fmt.push_byte(0x42);
        fmt.push_int(0x12345678);
        fmt.push_float(3.14f);
        fmt.push_string("Test");
        assert(fmt.get_data().size() > 0);
        std::cout << "✅\n";
    }
    
    // Test serialization
    {
        std::cout << "    Serialization... ";
        format::GbinFormat fmt;
        fmt.push_string("Hello");
        fmt.push_int(42);
        auto data = fmt.serialize();
        assert(data.size() > 0);
        std::cout << "✅ (" << data.size() << " bytes)\n";
    }
    
    // Test deserialization
    {
        std::cout << "    Deserialization... ";
        format::GbinFormat fmt1;
        fmt1.push_string("Hello");
        fmt1.push_int(42);
        auto data = fmt1.serialize();
        
        format::GbinFormat fmt2;
        bool success = fmt2.deserialize(data);
        assert(success);
        std::cout << "✅\n";
    }
    
    // Test version
    {
        std::cout << "    Version handling... ";
        format::GbinFormat fmt;
        fmt.set_version(5);
        assert(fmt.get_version() == 5);
        std::cout << "✅\n";
    }
    
    std::cout << "  ✅ All format tests passed!\n";
}
