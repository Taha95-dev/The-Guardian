# Changelog

All notable changes to The Guardian will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]

### Added
- *(Nothing yet — coming soon!)*

---

## [0.1.0] — 2026-07-24

### Added
- **Core Library** (`libguardian_core.a`)
  - Atoms — Immutable, type-safe data units
  - Molecules — Composable data structures with LUT
  - Quarks — 0-overhead primitive values

- **Memory System** (`libguardian_memory.a`)
  - Cache Manager — Persistent VM state
  - LUT (Lookup Table) — Pointer tracking for memory safety
  - Memory Manager — Allocation, deallocation, and stats
  - Memory Pool — Efficient block allocations

- **Parser Framework** (`libguardian_parser.a`)
  - Lexer Base — Tokenization
  - Parser Base — AST construction
  - AST Base — Abstract syntax tree nodes

- **Format Framework** (`libguardian_format.a`)
  - .gbin Format — Guardian Binary Format
  - Format Registry — Register custom formats
  - Serialization — Read/write binary data

- **VM** (`libguardian_vm.a`)
  - Bytecode execution
  - Stack operations (push, pop, peek)
  - Execution loop
  - Print and Println opcodes

- **CLI** (`guardianc`)
  - `--version` — Show version
  - `--help` — Show help

- **Documentation**
  - User Guide
  - Architecture documentation
  - Security model
  - Tutorial — Build your first language
  - API reference
  - Contributing guide
  - Changelog

- **System-wide Installation**
  - Installs to `/usr/local/`
  - Headers in `/usr/local/include/guardian/`
  - Libraries in `/usr/local/lib/`
  - Binary in `/usr/local/bin/`

- **License**
  - The Guardian License v1.0
  - Free for personal, educational, and open-source use
  - Commercial licensing with tiers

### Fixed
- All `TODO` items resolved
- Memory safety issues
- Build system fixes
- Permission issues in build directory

### Removed
- Old `atom.hpp.old` backup file
- Unused OmScript test (moved to separate project)

---

## [0.0.1] — 2026-07-19

### Added
- Initial project structure
- Basic CMake build system
- Core atom types (IntAtom, FloatAtom, StringAtom, BoolAtom, CharAtom)
- Basic molecule implementation
- Initial VM design

### Changed
- *(N/A — initial release)*

### Fixed
- *(N/A — initial release)*

---

## Upgrading from 0.0.1 to 0.1.0

### Breaking Changes
- None — the API is stable

### New Features
- Full memory safety with LUT
- Custom binary format support (.gbin)
- System-wide installation
- Complete documentation

---

## Future Releases

### 0.2.0 (Planned)
- [ ] JIT compilation
- [ ] More VM opcodes (arrays, dictionaries, control flow)
- [ ] ELF/PE/Mach-O format support
- [ ] Language server (LSP)
- [ ] Package manager

### 1.0.0 (Future)
- [ ] Stable API
- [ ] Production-ready
- [ ] Full documentation
- [ ] Community contributions
- [ ] Commercial licensing ready

---

## About This Project

The Guardian is built by **Taha (Taha95-dev)** — a 13-year-old developer building the future of code.

- **GitHub:** [Taha95-dev](https://github.com/Taha95-dev)
- **Repository:** [The-Guardian](https://github.com/Taha95-dev/The-Guardian)
- **License:** [The Guardian License v1.0](LICENSE.md)

---

*This changelog follows the principles of [Keep a Changelog](https://keepachangelog.com/).*
