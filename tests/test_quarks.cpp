#include "../src/quark/quark.hpp"
#include <iostream>
#include <chrono>

using namespace guardian;

int main() {
    std::cout << "🔬 Testing Quarks (Ultra-lightweight values)\n\n";
    
    // Test creation
    std::cout << "📦 Creating quarks:\n";
    Quark i(42);
    Quark f(3.14159);
    Quark b(true);
    Quark c('A');
    Quark p(nullptr);
    
    std::cout << "  Int: " << i.to_string() << " (" << i.size() << " bytes)\n";
    std::cout << "  Float64: " << f.to_string() << " (" << f.size() << " bytes)\n";
    std::cout << "  Bool: " << b.to_string() << " (" << b.size() << " bytes)\n";
    std::cout << "  Char: " << c.to_string() << " (" << c.size() << " bytes)\n";
    std::cout << "  Pointer: " << p.to_string() << " (" << p.size() << " bytes)\n";
    
    // Test arithmetic
    std::cout << "\n🧮 Arithmetic:\n";
    Quark a(10);
    Quark d(3.5);
    
    Quark add = a + d;
    Quark sub = a - d;
    Quark mul = a * d;
    Quark div = a / d;
    
    std::cout << "  10 + 3.5 = " << add.to_string() << "\n";
    std::cout << "  10 - 3.5 = " << sub.to_string() << "\n";
    std::cout << "  10 * 3.5 = " << mul.to_string() << "\n";
    std::cout << "  10 / 3.5 = " << div.to_string() << "\n";
    
    // Performance test
    std::cout << "\n⚡ Performance Test (10 million operations):\n";
    
    auto start = std::chrono::high_resolution_clock::now();
    
    int sum = 0;
    for (int i = 0; i < 10000000; i++) {
        sum += i;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "  C++ int: " << duration.count() << "ms (sum=" << sum << ")\n";
    
    // Quark performance
    start = std::chrono::high_resolution_clock::now();
    
    Quark qsum(0);
    for (int i = 0; i < 10000000; i++) {
        qsum = qsum + Quark(i);
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "  Quark: " << duration.count() << "ms (sum=" << qsum.to_string() << ")\n";
    
    std::cout << "\n✅ Quarks working!\n";
    return 0;
}
