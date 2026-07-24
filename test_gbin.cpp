#include "src/format/gbin_format.hpp"
#include <iostream>
#include <vector>
#include <fstream>

using namespace guardian::format;

int main() {
    std::cout << "🧪 Testing .gbin Format\n\n";
    
    // Create a .gbin file
    GbinFormat gbin;
    gbin.set_version(2);
    gbin.set_entry_point(0x1000);
    
    // Add some data
    gbin.push_int(42);
    gbin.push_float(3.14159f);
    gbin.push_string("Hello, Guardian!");
    gbin.push_byte(0xFF);
    
    std::cout << "📦 Created .gbin with " << gbin.get_data().size() << " bytes of data\n";
    std::cout << "  Version: " << gbin.get_version() << "\n";
    std::cout << "  Entry point: 0x" << std::hex << gbin.get_entry_point() << "\n";
    std::cout << "  Magic: 0x" << gbin.get_magic() << "\n\n";
    
    // Serialize to bytes
    auto bytes = gbin.serialize();
    std::cout << "📦 Serialized to " << bytes.size() << " bytes\n\n";
    
    // Write to file
    std::ofstream out("test.gbin", std::ios::binary);
    out.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
    out.close();
    std::cout << "💾 Written to test.gbin\n\n";
    
    // Read back
    std::ifstream in("test.gbin", std::ios::binary);
    std::vector<uint8_t> read_bytes((std::istreambuf_iterator<char>(in)),
                                     std::istreambuf_iterator<char>());
    in.close();
    
    // Deserialize
    GbinFormat read_gbin;
    if (read_gbin.deserialize(read_bytes)) {
        std::cout << "📖 Read back .gbin:\n";
        std::cout << "  Version: " << read_gbin.get_version() << "\n";
        std::cout << "  Entry point: 0x" << std::hex << read_gbin.get_entry_point() << "\n";
        std::cout << "  Data size: " << read_gbin.get_data().size() << " bytes\n";
        
        // Show hexdump of first 32 bytes
        std::cout << "\n📋 Hexdump (first 32 bytes):\n";
        auto data = read_gbin.get_data();
        for (size_t i = 0; i < std::min(data.size(), size_t(32)); i++) {
            printf("%02x ", data[i]);
            if ((i + 1) % 16 == 0) printf("\n");
        }
        printf("\n");
    }
    
    std::cout << "\n✅ .gbin format test complete!\n";
    return 0;
}
