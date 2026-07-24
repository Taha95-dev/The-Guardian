#include "src/vm/vm.hpp"
#include "src/core/molecule.hpp"
#include <iostream>
#include <vector>

int main() {
    std::cout << "🧪 Testing Guardian VM Directly\n\n";
    
    // Create VM
    guardian::vm::VM vm;
    
    // Create a simple program: PUSH_INT 42, PRINTLN, HALT
    std::vector<uint8_t> bytecode = {
        0x10, 0x2A, 0x00, 0x00, 0x00,  // PUSH_INT 42
        0x81,                           // PRINTLN
        0x00                            // HALT
    };
    
    std::cout << "📦 Loading bytecode...\n";
    vm.load(bytecode, 0);
    
    std::cout << "▶️ Running VM...\n";
    vm.run();
    
    std::cout << "\n✅ Test complete!\n";
    return 0;
}
