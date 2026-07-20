#pragma once
#include "../vm/opcodes.hpp"
#include "../core/atom.hpp"
#include "../core/molecule.hpp"           // ← ADD THIS
#include "../atoms/primitive/int_atom.hpp"  // ← ADD THIS
#include "../atoms/text/string_atom.hpp"    // ← ADD THIS
#include <vector>
#include <memory>

namespace axiom {

class CodeGenerator {
private:
    std::vector<guardian::vm::Instruction> bytecode;
    
    void generate_atom(const guardian::Atom* atom);
    void generate_int(const guardian::IntAtom* atom);
    void generate_string(const guardian::StringAtom* atom);
    void generate_molecule(const guardian::Molecule* mol);
    
public:
    std::vector<guardian::vm::Instruction> generate(const guardian::Atom* atom);
};

} // namespace axiom
