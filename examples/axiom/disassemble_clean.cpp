#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <cstdint>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file.axbin>\n";
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file\n";
        return 1;
    }

    std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());

    std::cout << "📜 Axiom Bytecode Disassembler\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    std::cout << "File: " << argv[1] << "\n";
    std::cout << "Size: " << data.size() << " bytes\n\n";

    // Skip GBIN header
    size_t offset = 0;
    if (data.size() >= 4 && data[0] == 0x4E && data[1] == 0x49 && data[2] == 0x42 && data[3] == 0x47) {
        std::cout << "📦 GBIN format detected (header skipped)\n\n";
        offset = 0x20; // 32-byte header
    }

    std::cout << "Bytecode:\n";
    std::cout << "──────────\n";

    size_t i = offset;
    int inst_count = 0;

    while (i < data.size()) {
        uint8_t op = data[i];
        
        // Skip null bytes (padding)
        if (op == 0x00) {
            // Check if it's part of a multi-byte instruction or just padding
            bool is_padding = true;
            // Look ahead to see if this is part of a valid instruction
            if (i + 1 < data.size()) {
                uint8_t next = data[i+1];
                // If next byte looks like an opcode, this might be padding
                if (next == 0x10 || next == 0x12 || next == 0x13 || next == 0x60 || next == 0x61 || next == 0x80) {
                    // It's padding before a new instruction
                    std::cout << std::setw(4) << i << ": (padding)\n";
                    i++;
                    continue;
                }
            }
            // If we get here, it's actual HALT
            std::cout << std::setw(4) << i << ": HALT";
            i++;
            inst_count++;
            std::cout << "\n";
            continue;
        }

        // Print address
        std::cout << std::setw(4) << i << ": ";

        switch (op) {
            case 0x10: {
                std::cout << "PUSH_INT ";
                if (i + 4 < data.size()) {
                    int32_t val = data[i+1] | (data[i+2] << 8) | (data[i+3] << 16) | (data[i+4] << 24);
                    std::cout << val;
                    i += 5;
                } else {
                    i++;
                }
                break;
            }
            case 0x12: {
                std::cout << "PUSH_BOOL ";
                if (i + 1 < data.size()) {
                    std::cout << (data[i+1] ? "true" : "false");
                    i += 2;
                } else {
                    i++;
                }
                break;
            }
            case 0x13: {
                std::cout << "PUSH_STRING ";
                if (i + 2 < data.size()) {
                    uint16_t len = data[i+1] | (data[i+2] << 8);
                    i += 3;
                    std::cout << "\"";
                    for (int j = 0; j < len && i + j < data.size(); j++) {
                        char c = data[i+j];
                        if (c >= 32 && c <= 126) std::cout << c;
                        else std::cout << "\\x" << std::hex << std::setw(2) << (int)(unsigned char)c << std::dec;
                    }
                    std::cout << "\"";
                    i += len;
                } else {
                    i++;
                }
                break;
            }
            case 0x60: {
                std::cout << "STORE var[";
                if (i + 4 < data.size()) {
                    uint32_t idx = data[i+1] | (data[i+2] << 8) | (data[i+3] << 16) | (data[i+4] << 24);
                    std::cout << idx << "]";
                    i += 5;
                } else {
                    i++;
                }
                break;
            }
            case 0x61: {
                std::cout << "LOAD var[";
                if (i + 4 < data.size()) {
                    uint32_t idx = data[i+1] | (data[i+2] << 8) | (data[i+3] << 16) | (data[i+4] << 24);
                    std::cout << idx << "]";
                    i += 5;
                } else {
                    i++;
                }
                break;
            }
            case 0x80: {
                std::cout << "PRINT";
                i++;
                break;
            }
            default: {
                std::cout << "UNKNOWN(0x" << std::hex << (int)op << std::dec << ")";
                i++;
                break;
            }
        }
        std::cout << "\n";
        inst_count++;
    }

    std::cout << "\nTotal instructions: " << inst_count << "\n";
    return 0;
}
