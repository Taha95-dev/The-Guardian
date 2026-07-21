#include <iostream>
#include <string>

void print_version() {
    std::cout << "The Guardian v0.1.0\n";
    std::cout << "Built on " << __DATE__ << " at " << __TIME__ << "\n";
    std::cout << "\n";
    std::cout << "Components:\n";
    std::cout << "  - Core Library (atoms, molecules)\n";
    std::cout << "  - VM (bytecode execution)\n";
    std::cout << "  - Compiler (lexer, parser)\n";
    std::cout << "  - Om Language\n";
    std::cout << "\n";
    std::cout << "Commands:\n";
    std::cout << "  guardianc --version  - Show version\n";
    std::cout << "  omc -c file.om       - Compile Om to .gbin\n";
    std::cout << "  omc -r file.om       - Compile and run Om\n";
    std::cout << "  omc -e file.gbin     - Execute .gbin\n";
}

void print_help() {
    std::cout << R"(
╔═══════════════════════════════════════════╗
║   🔷 THE GUARDIAN — Language Framework   ║
║           Building the future of code     ║
╚═══════════════════════════════════════════╝

Usage: guardianc [options]

Options:
  --version, -v    Show version information
  --help, -h       Show this help message

The Guardian is a framework for building languages with:
  - Custom binary formats
  - Memory safety (atoms + molecules + LUT)
  - VM execution
  - Om language

For Om compiler, use: omc --help
)";
}

int main(int argc, char* argv[]) {
    // Parse arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--version" || arg == "-v") {
            print_version();
            return 0;
        }
        if (arg == "--help" || arg == "-h") {
            print_help();
            return 0;
        }
    }
    
    // No arguments or unknown arguments - show help
    print_help();
    return 0;
}
