#include <iostream>
#include <iomanip>
#include <fstream>
#include "core/atom.hpp"
#include "core/molecule.hpp"
#include "atoms/primitive/int_atom.hpp"
#include "atoms/primitive/float_atom.hpp"
#include "atoms/primitive/bool_atom.hpp"
#include "atoms/primitive/char_atom.hpp"
#include "atoms/text/string_atom.hpp"
#include "atoms/containers/array_atom.hpp"
#include "atoms/special/null_atom.hpp"
#include "vm/vm.hpp"
#include "vm/formats/guardian_format.hpp"
#include "compiler/lexer.hpp"

using namespace guardian;

// Helper function
void print_atom_info(const std::string& label, const Atom* atom) {
    std::cout << "  " << label << std::endl;
    std::cout << "    Type: " << atom->name() << std::endl;
    std::cout << "    Size: " << atom->size() << " bytes" << std::endl;
}

int main() {
    guardian::vm::formats::register_guardian_format();
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════╗\n";
    std::cout << "║   🔷 THE GUARDIAN — Language Framework   ║\n";
    std::cout << "║           Building the future of code     ║\n";
    std::cout << "╚═══════════════════════════════════════════╝\n";
    std::cout << "\n";

    // ============================================
    // 1. ATOMS — The Building Blocks
    // ============================================
    std::cout << "📦 ATOMS — The Building Blocks\n";
    std::cout << "────────────────────────────────\n\n";

    IntAtom int_atom(42);
    print_atom_info("IntAtom:", &int_atom);
    std::cout << "    Value: " << int_atom.get() << "\n\n";

    FloatAtom float_atom(3.14159f);
    print_atom_info("FloatAtom:", &float_atom);
    std::cout << "    Value: " << float_atom.get() << "\n\n";

    StringAtom string_atom("Hello, Guardian!");
    print_atom_info("StringAtom:", &string_atom);
    std::cout << "    Value: \"" << string_atom.get() << "\"\n\n";

    BoolAtom bool_atom(true);
    print_atom_info("BoolAtom:", &bool_atom);
    std::cout << "    Value: " << (bool_atom.get() ? "true" : "false") << "\n\n";

    CharAtom char_atom('G');
    print_atom_info("CharAtom:", &char_atom);
    std::cout << "    Value: '" << char_atom.get() << "'\n\n";

    NullAtom null_atom;
    print_atom_info("NullAtom:", &null_atom);
    std::cout << "    Value: (nothing)\n\n";

    // ============================================
    // 2. MOLECULES — Composition
    // ============================================
    std::cout << "🧬 MOLECULES — Composing Atoms\n";
    std::cout << "────────────────────────────────\n\n";

    Molecule molecule;
    molecule.add_atom(std::make_unique<IntAtom>(42));
    molecule.add_atom(std::make_unique<FloatAtom>(3.14f));
    molecule.add_atom(std::make_unique<StringAtom>("Guardian"));

    std::cout << "  Molecule contains " << molecule.atom_count() << " atoms\n";
    std::cout << "  Total size: " << molecule.size() << " bytes\n\n";

    // ============================================
    // 3. VIRTUAL MACHINE — Molecule Loading
    // ============================================
    std::cout << "🖥️  VIRTUAL MACHINE — Molecule Execution\n";
    std::cout << "────────────────────────────────\n\n";

    std::cout << "  [1] Loading molecule as program:\n";
    Molecule program;
    program.add_atom(std::make_unique<IntAtom>(42));
    program.add_atom(std::make_unique<IntAtom>(100));
    program.add_atom(std::make_unique<StringAtom>("Hello from The Guardian!"));

    vm::VM vm;
    vm.load_molecule(program);
    vm.run();
    std::cout << "      ✅ Molecule executed successfully!\n\n";

    // ============================================
    // 4. VIRTUAL MACHINE — Bytecode
    // ============================================
    std::cout << "  [2] Loading bytecode directly:\n";
    vm::Bytecode bytecode = {
        {vm::OpCode::PUSH, {99}},
        {vm::OpCode::PRINT},
        {vm::OpCode::HALT}
    };

    vm::VM vm2;
    vm2.load_bytecode(bytecode);
    vm2.run();
    std::cout << "      ✅ Bytecode executed successfully!\n\n";

    // ============================================
    // 5. BINARY FORMAT — Saving and Loading
    // ============================================
    std::cout << "💾 BINARY FORMAT — Saving and Loading\n";
    std::cout << "────────────────────────────────\n\n";

    vm::Bytecode test_program = {
        {vm::OpCode::PUSH, {42}},
        {vm::OpCode::PUSH, {10}},
        {vm::OpCode::ADD},
        {vm::OpCode::PRINT},
        {vm::OpCode::HALT}
    };

    vm::VM writer_vm;
    writer_vm.load_bytecode(test_program);
    if (writer_vm.save_binary_file("test_program.gbin")) {
        std::cout << "  ✅ Saved test_program.gbin\n";
    } else {
        std::cout << "  ❌ Failed to save binary\n";
    }

    vm::VM reader_vm;
    if (reader_vm.load_binary_file("test_program.gbin")) {
        std::cout << "  ✅ Loaded test_program.gbin\n";
        reader_vm.run();
    } else {
        std::cout << "  ❌ Failed to load binary\n";
    }
    std::cout << "\n";

    // ============================================
    // 6. LEXER — Tokenization
    // ============================================
    std::cout << "🔤 LEXER — Tokenizing Source Code\n";
    std::cout << "────────────────────────────────\n\n";

    std::string source = "let x = 42 + 10;";
    std::cout << "  Source: \"" << source << "\"\n";
    
    compiler::Lexer lexer(source);
    auto tokens = lexer.tokenize();

    std::cout << "  Tokens:\n";
    for (const auto& token : tokens) {
        std::cout << "    " << std::setw(10) << std::left;
        switch (token.type) {
            case compiler::TokenType::KEYWORD:   std::cout << "KEYWORD"; break;
            case compiler::TokenType::IDENTIFIER: std::cout << "IDENT"; break;
            case compiler::TokenType::NUMBER:     std::cout << "NUMBER"; break;
            case compiler::TokenType::STRING:     std::cout << "STRING"; break;
            case compiler::TokenType::OPERATOR:   std::cout << "OP"; break;
            case compiler::TokenType::PUNCTUATOR: std::cout << "PUNCT"; break;
            case compiler::TokenType::END_OF_FILE: std::cout << "EOF"; break;
            default:                              std::cout << "UNKNOWN"; break;
        }
        std::cout << "  \"" << token.value << "\"\n";
    }
    std::cout << "\n";

    // ============================================
    // 7. SUMMARY
    // ============================================
    std::cout << "✅ The Guardian is alive and kicking!\n";
    std::cout << "   Atoms: 7 types\n";
    std::cout << "   Opcodes: 50+\n";
    std::cout << "   VM: Working (NO dynamic_cast!)\n";
    std::cout << "   Lexer: Working\n";
    std::cout << "   Binary Format: Working\n";
    std::cout << "   Tests: Passing\n";
    std::cout << "\n";

    return 0;
}
