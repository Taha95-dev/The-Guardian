#include <iostream>
#include "core/atom.hpp"
#include "core/molecule.hpp"
#include "vm/vm.hpp"
#include "compiler/lexer.hpp"

using namespace guardian;

// Helper to print atom types
void print_atom_info(const Atom* atom) {
    std::cout << "   Type: " << atom->name() << std::endl;
    std::cout << "   Size: " << atom->size() << " bytes" << std::endl;
}

int main() {
    std::cout << "🔷 THE GUARDIAN — v0.5" << std::endl;
    std::cout << "════════════════════════" << std::endl;
    std::cout << std::endl;
    
    // ============================================
    // TEST 1: ATOMS
    // ============================================
    std::cout << "📦 Testing Atoms..." << std::endl;
    
    // Int
    PrimitiveAtom<int> intAtom(42);
    std::cout << "🔢 IntAtom: " << intAtom.get() << std::endl;
    print_atom_info(&intAtom);
    
    // Float
    PrimitiveAtom<float> floatAtom(3.14159f);
    std::cout << "🔢 FloatAtom: " << floatAtom.get() << std::endl;
    print_atom_info(&floatAtom);
    
    // String
    StringAtom strAtom("Hello, Guardian!");
    std::cout << "📝 StringAtom: " << strAtom.get() << std::endl;
    print_atom_info(&strAtom);
    
    // Bool
    BoolAtom boolAtom(true);
    std::cout << "🔘 BoolAtom: " << (boolAtom.get() ? "true" : "false") << std::endl;
    print_atom_info(&boolAtom);
    
    // Char
    CharAtom charAtom('G');
    std::cout << "🔤 CharAtom: '" << charAtom.get() << "'" << std::endl;
    print_atom_info(&charAtom);
    
    // Null
    NullAtom nullAtom;
    std::cout << "🚫 NullAtom: nothing here" << std::endl;
    print_atom_info(&nullAtom);
    std::cout << std::endl;
    
    // ============================================
    // TEST 2: MOLECULE
    // ============================================
    std::cout << "🧬 Testing Molecule..." << std::endl;
    
    Molecule mol;
    mol.add_atom(std::make_unique<PrimitiveAtom<int>>(42));
    mol.add_atom(std::make_unique<PrimitiveAtom<float>>(3.14f));
    mol.add_atom(std::make_unique<StringAtom>("Guardian"));
    
    std::cout << "   Molecule has " << mol.atom_count() << " atoms" << std::endl;
    std::cout << "   Total size: " << mol.size() << " bytes" << std::endl;
    std::cout << std::endl;
    
    // ============================================
    // TEST 3: VM WITH MOLECULE
    // ============================================
    std::cout << "🖥️  Testing VM with Molecule loading..." << std::endl;
    
    // Create a molecule with atoms to execute
    Molecule program;
    program.add_atom(std::make_unique<PrimitiveAtom<int>>(42));
    program.add_atom(std::make_unique<PrimitiveAtom<int>>(100));
    program.add_atom(std::make_unique<StringAtom>("Hello from The Guardian!"));
    
    // Load and run
    vm::VM vm;
    vm.load_molecule(program);
    vm.run();
    
    std::cout << "   ✅ VM executed molecule successfully!" << std::endl;
    std::cout << std::endl;
    
    // ============================================
    // TEST 4: VM WITH BYTECODE (Direct)
    // ============================================
    std::cout << "🖥️  Testing VM with Bytecode..." << std::endl;
    
    // Manual bytecode: PUSH 99, PRINT, HALT
    vm::Bytecode bytecode = {
        {vm::OpCode::PUSH, {99}},
        {vm::OpCode::PRINT},
        {vm::OpCode::HALT}
    };
    
    vm::VM vm2;
    vm2.load_bytecode(bytecode);
    vm2.run();
    
    std::cout << "   ✅ Bytecode executed successfully!" << std::endl;
    std::cout << std::endl;
    
    // ============================================
    // TEST 5: LEXER (Compiler Frontend)
    // ============================================
    std::cout << "🔤 Testing Lexer..." << std::endl;
    std::string code = "let x = 42 + 10;";
    compiler::Lexer lexer(code);
    auto tokens = lexer.tokenize();
    
    std::cout << "   Tokens:" << std::endl;
    for (const auto& token : tokens) {
        std::cout << "      " << static_cast<int>(token.type) << ": " << token.value << std::endl;
    }
    std::cout << std::endl;
    
    // ============================================
    // FINAL
    // ============================================
    std::cout << "✅ The Guardian is alive and kicking!" << std::endl;
    
    return 0;
}
