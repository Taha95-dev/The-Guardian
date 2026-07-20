#include "codegen.hpp"
#include <iostream>

namespace axiom {

void CodeGenerator::generate_atom(const guardian::Atom* atom) {
    // Check atom type and generate appropriate bytecode
    if (auto* int_atom = dynamic_cast<const guardian::IntAtom*>(atom)) {
        generate_int(int_atom);
    } else if (auto* str_atom = dynamic_cast<const guardian::StringAtom*>(atom)) {
        generate_string(str_atom);
    } else if (auto* mol = dynamic_cast<const guardian::Molecule*>(atom)) {
        generate_molecule(mol);
    } else {
        std::cerr << "Unknown atom type" << std::endl;
    }
}

void CodeGenerator::generate_int(const guardian::IntAtom* atom) {
    int val = atom->get();
    bytecode.push_back(guardian::vm::Instruction(
        guardian::vm::OpCode::PUSH,
        {static_cast<uint64_t>(val)}
    ));
}

void CodeGenerator::generate_string(const guardian::StringAtom* atom) {
    bytecode.push_back(guardian::vm::Instruction(guardian::vm::OpCode::PRINT));
}

void CodeGenerator::generate_molecule(const guardian::Molecule* mol) {
    for (const auto& atom : mol->get_atoms()) {
        generate_atom(atom.get());
    }
}

std::vector<guardian::vm::Instruction> CodeGenerator::generate(const guardian::Atom* atom) {
    bytecode.clear();
    generate_atom(atom);
    return bytecode;
}

} // namespace axiom
