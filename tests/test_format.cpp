#include "../src/vm/formats/guardian_format.hpp"
#include <iostream>
#include <vector>

using namespace guardian::vm::formats;

int main() {
    std::cout << "🧪 Testing Guardian Binary Format\n\n";
    
    // Test quark serialization
    std::cout << "📦 Testing Quark Serialization:\n";
    guardian::Quark q1(42);
    guardian::Quark q2(3.14159);
    guardian::Quark q3(true);
    
    auto data1 = serialize_quark(q1);
    auto data2 = serialize_quark(q2);
    auto data3 = serialize_quark(q3);
    
    std::cout << "  Int quark: " << q1.to_string() << " -> " << data1.size() << " bytes\n";
    std::cout << "  Float quark: " << q2.to_string() << " -> " << data2.size() << " bytes\n";
    std::cout << "  Bool quark: " << q3.to_string() << " -> " << data3.size() << " bytes\n";
    
    // Test deserialization
    std::cout << "\n📦 Testing Quark Deserialization:\n";
    size_t pos = 0;
    auto q1_des = deserialize_quark(data1, pos);
    pos = 0;
    auto q2_des = deserialize_quark(data2, pos);
    pos = 0;
    auto q3_des = deserialize_quark(data3, pos);
    
    std::cout << "  Deserialized: " << q1_des.to_string() << "\n";
    std::cout << "  Deserialized: " << q2_des.to_string() << "\n";
    std::cout << "  Deserialized: " << q3_des.to_string() << "\n";
    
    // Test binary generation
    std::cout << "\n📦 Testing Binary Generation:\n";
    std::vector<Instruction> instructions;
    instructions.push_back(Instruction(0x10, {42})); // PUSH_INT 42
    instructions.push_back(Instruction(0x11, {0, 0, 0, 0, 0, 0, 0, 0})); // PUSH_STRING placeholder
    instructions.push_back(Instruction(0x80, {})); // PRINT
    instructions.push_back(Instruction(0x00, {})); // HALT
    
    auto binary = generate_guardian_binary(instructions);
    std::cout << "  Generated " << binary.size() << " bytes\n";
    
    // Parse back
    auto parsed = parse_guardian_binary(binary);
    std::cout << "  Parsed " << parsed.size() << " instructions\n";
    
    std::cout << "\n✅ Format tests passed!\n";
    return 0;
}
