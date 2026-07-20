#include "compiler.hpp"
#include <iostream>
#include <string>
#include <vector>

void print_usage() {
    std::cout << "╔═══════════════════════════════════════════╗\n";
    std::cout << "║   🚀 AXIOM COMPILER (axc)               ║\n";
    std::cout << "║   Built on The Guardian Framework        ║\n";
    std::cout << "╚═══════════════════════════════════════════╝\n\n";
    std::cout << "Usage: axc [options] <file.ax>\n\n";
    std::cout << "Options:\n";
    std::cout << "  --help, -h          Show this help message\n";
    std::cout << "  --tokens, -t        Print tokens from lexer\n";
    std::cout << "  --ast, -a           Print AST from parser\n";
    std::cout << "  --debug, -d         Enable all debug output\n";
    std::cout << "  --output, -o <file> Output to custom file (default: <input>.axbin)\n";
    std::cout << "\n";
    std::cout << "Examples:\n";
    std::cout << "  axc program.ax\n";
    std::cout << "  axc -t program.ax\n";
    std::cout << "  axc -a program.ax\n";
    std::cout << "  axc -o output.bin program.ax\n";
}

int main(int argc, char* argv[]) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; i++) {
        args.push_back(argv[i]);
    }
    
    // Parse flags
    bool show_tokens = false;
    bool show_ast = false;
    bool show_help = false;
    std::string input_file;
    std::string output_file;
    
    for (size_t i = 0; i < args.size(); i++) {
        const std::string& arg = args[i];
        
        if (arg == "--help" || arg == "-h") {
            show_help = true;
        } else if (arg == "--tokens" || arg == "-t") {
            show_tokens = true;
        } else if (arg == "--ast" || arg == "-a") {
            show_ast = true;
        } else if (arg == "--debug" || arg == "-d") {
            show_tokens = true;
            show_ast = true;
        } else if (arg == "--output" || arg == "-o") {
            if (i + 1 < args.size()) {
                output_file = args[++i];
            } else {
                std::cerr << "Error: --output requires a file argument\n";
                return 1;
            }
        } else if (arg[0] != '-') {
            input_file = arg;
        } else {
            std::cerr << "Unknown option: " << arg << "\n";
            print_usage();
            return 1;
        }
    }
    
    if (show_help) {
        print_usage();
        return 0;
    }
    
    if (input_file.empty()) {
        print_usage();
        return 1;
    }
    
    // Set debug flags in compiler
    axiom::Compiler::set_debug_tokens(show_tokens);
    axiom::Compiler::set_debug_ast(show_ast);
    
    // Set output file
    if (!output_file.empty()) {
        axiom::Compiler::set_output_file(output_file);
    }
    
    // Compile
    axiom::Compiler::compile_file(input_file);
    
    return 0;
}
