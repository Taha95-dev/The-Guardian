API Reference
Atoms

    IntAtom(int value)

    StringAtom(const std::string& value)

    BoolAtom(bool value)

    NullAtom()

Molecules

    void add_atom(std::unique_ptr<Atom> atom)

    size_t size() const

    size_t atom_count() const

VM

    void load_bytecode(const Bytecode& code)

    void load_molecule(const Molecule& molecule)

    void run()

    bool load_binary_file(const std::string& path)

    bool save_binary_file(const std::string& path)

Opcodes

PUSH, POP, ADD, SUB, MUL, DIV, PRINT, HALT
