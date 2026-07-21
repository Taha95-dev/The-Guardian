#include "lexer.hpp"
#include "parser.hpp"
#include "compiler.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

void print_version() {
    std::cout << "Om Compiler v0.1.0\n";
    std::cout << "Built on " << __DATE__ << " at " << __TIME__ << "\n";
    std::cout << "\n";
    std::cout << "Part of The Guardian Framework\n";
    std::cout << "  - Custom binary formats (.gbin)\n";
    std::cout << "  - VM execution\n";
    std::cout << "  - Memory safety\n";
}

void print_usage(const char* program) {
    std::cout << "Om Compiler — The Guardian Language\n\n";
    std::cout << "Usage:\n";
    std::cout << "  " << program << " <file.om>           - Tokenize and parse\n";
    std::cout << "  " << program << " -c <file.om>        - Compile to .gbin\n";
    std::cout << "  " << program << " -r <file.om>        - Compile and run\n";
    std::cout << "  " << program << " -e <file.gbin>      - Execute .gbin\n";
    std::cout << "  " << program << " --version, -v       - Show version\n";
    std::cout << "  " << program << " --help, -h          - Show this help\n";
    std::cout << "\nOutput format: .gbin (Guardian Binary Format)\n";
}

int main(int argc, char* argv[]) {
    // Check for flags first
    if (argc < 2) {
        print_usage(argv[0]);
        return 0;
    }
    
    std::string arg1 = argv[1];
    
    // Version
    if (arg1 == "--version" || arg1 == "-v") {
        print_version();
        return 0;
    }
    
    // Help
    if (arg1 == "--help" || arg1 == "-h") {
        print_usage(argv[0]);
        return 0;
    }
    
    std::string mode = "tokenize";
    std::string filepath;
    
    // Parse command
    if (argc >= 3) {
        if (arg1 == "-c") {
            mode = "compile";
            filepath = argv[2];
        } else if (arg1 == "-r") {
            mode = "run";
            filepath = argv[2];
        } else if (arg1 == "-e") {
            mode = "execute";
            filepath = argv[2];
        } else {
            filepath = argv[1];
        }
    } else {
        filepath = argv[1];
    }
    
    // Execute mode (run .gbin directly)
    if (mode == "execute") {
        om::Compiler compiler;
        if (compiler.run(filepath)) {
            std::cout << "\n✅ Execution complete!\n";
        } else {
            std::cerr << "\n❌ Execution failed!\n";
            return 1;
        }
        return 0;
    }
    
    // Read source file
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filepath << "\n";
        return 1;
    }
    
    std::string source((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    // Tokenize
    om::Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    std::cout << "Tokens:\n";
    for (const auto& token : tokens) {
        std::cout << "  " << token.value << " (line " << token.line << ")\n";
    }
    
    if (mode == "compile") {
        // Compile to .gbin
        std::string output = filepath + ".gbin";
        om::Compiler compiler;
        if (compiler.compile(tokens, output)) {
            std::cout << "\n✅ Compiled to: " << output << "\n";
            std::cout << "   Run with: " << argv[0] << " -e " << output << "\n";
        } else {
            std::cerr << "\n❌ Compilation failed!\n";
            return 1;
        }
    } else if (mode == "run") {
        // Compile and run
        std::string output = filepath + ".gbin";
        om::Compiler compiler;
        if (compiler.compile(tokens, output)) {
            std::cout << "\n✅ Compiled to: " << output << "\n";
            std::cout << "▶️ Running...\n\n";
            if (compiler.run(output)) {
                std::cout << "\n✅ Execution complete!\n";
            } else {
                std::cerr << "\n❌ Execution failed!\n";
                return 1;
            }
        } else {
            std::cerr << "\n❌ Compilation failed!\n";
            return 1;
        }
    }
    
    std::cout << "\n✅ Om compilation successful!\n";
    return 0;
}
