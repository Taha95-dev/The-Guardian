#include <iostream>
#include <string>
#include "license/license.hpp"

void print_version() {
    std::cout << "The Guardian v0.1.0\n";
    std::cout << "Built on " << __DATE__ << " at " << __TIME__ << "\n";
    std::cout << "\n";
    std::cout << "Components:\n";
    std::cout << "  - Core Library (atoms, molecules)\n";
    std::cout << "  - VM (bytecode execution)\n";
    std::cout << "  - Compiler (lexer, parser)\n";
    std::cout << "  - License System\n";
    std::cout << "\n";
    std::cout << "Commands:\n";
    std::cout << "  guardianc --version   - Show version\n";
    std::cout << "  guardianc --license   - Show license status\n";
    std::cout << "  guardianc --help      - Show help\n";
}

void print_help() {
    std::cout << R"(
╔═══════════════════════════════════════════╗
║   🔷 THE GUARDIAN — Language Framework    ║
║           Building the future of code     ║
╚═══════════════════════════════════════════╝

Usage: guardianc [options]

Options:
  --version, -v    Show version information
  --license, -l    Show license status
  --help, -h       Show this help message

The Guardian is a framework for building languages with:
  - Custom binary formats
  - Memory safety (atoms + molecules + LUT)
  - VM execution
  - Commercial licensing

For commercial use, purchase a license:
  https://buy.polar.sh/polar_cl_czZ7LW2XcJR1zNjUoiGbBYU4MNOI4ShcvcR6l00HLU9

Built by a 13-year-old developer on 4GB RAM.
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
        if (arg == "--license" || arg == "-l") {
            guardian::license::showLicenseStatus();
            return 0;
        }
        if (arg == "--help" || arg == "-h") {
            print_help();
            return 0;
        }
    }
    
    // No arguments or unknown arguments - show license status
    guardian::license::showLicenseStatus();
    print_help();
    return 0;
}
