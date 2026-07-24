# The Guardian Architecture

## Overview

The Guardian is a layered language framework:

┌─────────────────────────────────────────────────────────────┐
│ The Guardian Framework │
│ │
│ ┌─────────────────────────────────────────────────────┐ │
│ │ CLI (guardianc) │ │
│ └─────────────────────────────────────────────────────┘ │
│ │
│ ┌─────────────────────────────────────────────────────┐ │
│ │ VM │ │
│ │ ┌─────────────┐ ┌─────────────┐ ┌───────────┐ │ │
│ │ │ Bytecode │ │ Stack │ │ Execution │ │ │
│ │ │ Execution │ │ Operations │ │ Loop │ │ │
│ │ └─────────────┘ └─────────────┘ └───────────┘ │ │
│ └─────────────────────────────────────────────────────┘ │
│ │
│ ┌─────────────────────────────────────────────────────┐ │
│ │ Format │ │
│ │ ┌─────────────┐ ┌─────────────┐ ┌───────────┐ │ │
│ │ │ .gbin │ │ Format │ │ Custom │ │ │
│ │ │ Format │ │ Registry │ │ Formats │ │ │
│ │ └─────────────┘ └─────────────┘ └───────────┘ │ │
│ └─────────────────────────────────────────────────────┘ │
│ │
│ ┌─────────────────────────────────────────────────────┐ │
│ │ Parser │ │
│ │ ┌─────────────┐ ┌─────────────┐ ┌───────────┐ │ │
│ │ │ Lexer │ │ Parser │ │ AST │ │ │
│ │ │ Base │ │ Base │ │ Base │ │ │
│ │ └─────────────┘ └─────────────┘ └───────────┘ │ │
│ └─────────────────────────────────────────────────────┘ │
│ │
│ ┌─────────────────────────────────────────────────────┐ │
│ │ Memory │ │
│ │ ┌─────────────┐ ┌─────────────┐ ┌───────────┐ │ │
│ │ │ Cache │ │ LUT │ │ Memory │ │ │
│ │ │ Manager │ │ (Lookup │ │ Manager │ │ │
│ │ └─────────────┘ └─────────────┘ └───────────┘ │ │
│ └─────────────────────────────────────────────────────┘ │
│ │
│ ┌─────────────────────────────────────────────────────┐ │
│ │ Core │ │
│ │ ┌─────────────┐ ┌─────────────┐ ┌───────────┐ │ │
│ │ │ Atoms │ │ Molecules │ │ Quarks │ │ │
│ │ └─────────────┘ └─────────────┘ └───────────┘ │ │
│ └─────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
text


## Component Details

### Core

The Core provides the fundamental data types:

- **Atoms**: Immutable, type-safe data units
- **Molecules**: Composable data structures with LUT
- **Quarks**: 0-overhead primitive values

### Memory

The Memory layer provides safety and persistence:

- **Cache Manager**: Persistent VM state
- **LUT (Lookup Table)**: Pointer tracking for memory safety
- **Memory Manager**: Allocation, deallocation, stats

### Parser

The Parser layer provides language parsing:

- **Lexer Base**: Tokenization
- **Parser Base**: AST construction
- **AST Base**: Abstract syntax tree nodes

### Format

The Format layer provides binary formats:

- **.gbin Format**: Guardian Binary Format
- **Format Registry**: Register custom formats
- **Serialization**: Read/write binary data

### VM

The VM layer provides bytecode execution:

- **Bytecode Execution**: Run bytecode
- **Stack Operations**: Push, pop, peek
- **Execution Loop**: Main VM loop

## Libraries

| Library | Purpose |
|---------|---------|
| `libguardian_core.a` | Core types |
| `libguardian_memory.a` | Memory management |
| `libguardian_parser.a` | Parsing |
| `libguardian_format.a` | Binary formats |
| `libguardian_vm.a` | VM execution |

## System-wide Installation

The Guardian installs to:

/usr/local/
├── bin/
│ └── guardianc
├── lib/
│ ├── libguardian_core.a
│ ├── libguardian_memory.a
│ ├── libguardian_parser.a
│ ├── libguardian_format.a
│ └── libguardian_vm.a
└── include/
└── guardian/
├── core/
├── memory/
├── parser/
├── format/
└── vm/ EOF
