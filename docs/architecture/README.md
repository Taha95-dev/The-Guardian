# 🏗️ Architecture

The Guardian is built as a layered, modular framework.

## Layer Diagram

┌─────────────────────────────────────────────────────────┐
│                       User Language                     │
│           (Your language built with The Guardian)       │
├─────────────────────────────────────────────────────────┤
│                      VM Layer                           │
│   ┌─────────────┐ ┌─────────────┐ ┌─────────────┐       │
│   │ Opcodes     │ │ Executor    │ │ CodeGen     │       │
│   └─────────────┘ └─────────────┘ └─────────────┘       │
├─────────────────────────────────────────────────────────┤
│                   Format Layer                          │
│   ┌─────────────┐ ┌─────────────┐ ┌─────────────┐       │
│   │ GBIN        │ │ Registry    │ │ Custom      │       │
│   └─────────────┘ └─────────────┘ └─────────────┘       │
├─────────────────────────────────────────────────────────┤
│                   Memory Layer                          │
│   ┌─────────────┐ ┌─────────────┐ ┌─────────────┐       │
│   │ LUT         │ │ Manager     │ │ Pool        │       │
│   └─────────────┘ └─────────────┘ └─────────────┘       │
├─────────────────────────────────────────────────────────┤
│                   Core Layer                            │
│   ┌─────────────┐ ┌─────────────┐ ┌─────────────┐       │
│   │ Atoms       │ │ Molecules   │ │ Quarks      │       │
│   └─────────────┘ └─────────────┘ └─────────────┘       │
└─────────────────────────────────────────────────────────┘


## Core Principles

### 1. Safe by Design, Not by Choice

Safety is built into the architecture, not added on.

- Every pointer is tracked
- Every allocation is validated
- No undefined behavior
- No GC needed

### 2. No Bloat

The Guardian is 7,000 lines of focused code.

- No unnecessary dependencies
- No over-engineering
- Every line has a purpose

### 3. Built for Language Developers

The Guardian is designed specifically for building languages.

- Clean APIs
- Flexible architecture
- Easy to extend

## Data Flow

Source Code
↓
[Lexer] → Tokens
↓
[Parser] → AST
↓
[Compiler] → Bytecode
↓
[VM] → Execution
text


## Memory Model

Quarks → Stack (fast, automatic)
Atoms → Heap (managed, tracked)
Molecules → Container (holds both)
text


## Security Model

The Guardian uses LUT (Lookup Table) to track every pointer allocation.

- Every allocation is registered
- Every deallocation is validated
- No dangling pointers
- No use-after-free
- No double-free

---
*The Guardian — Safe by Design, Not by Choice.*
