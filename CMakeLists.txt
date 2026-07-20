cmake_minimum_required(VERSION 3.10)
project(TheGuardian)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Core library (header-only atoms)
add_library(guardian_core
    src/core/atom.cpp
    src/core/molecule.cpp
)

# VM Library
add_library(guardian_vm
    src/vm/vm.cpp
    src/vm/formats/format_registry.cpp
    src/vm/formats/guardian_format.cpp
)

# Compiler Library
add_library(guardian_compiler
    src/compiler/lexer.cpp
)

# Main executable
add_executable(guardianc
    src/main.cpp
)

target_link_libraries(guardianc
    guardian_core
    guardian_vm
    guardian_compiler
)

# ============================================
# AXIOM LANGUAGE
# ============================================

# Axiom Library
add_library(axiom
    src/axiom/parser.cpp
    src/axiom/codegen.cpp
    src/axiom/compiler.cpp
)

# Axiom Compiler Executable
add_executable(axc
    src/axiom/main.cpp
)

target_link_libraries(axc
    axiom
    guardian_core
    guardian_vm
    guardian_compiler
)

# ============================================
# TESTS
# ============================================
enable_testing()

add_executable(run_tests
    tests/test_runner.cpp
    tests/test_atoms.cpp
)

target_link_libraries(run_tests
    guardian_core
    guardian_vm
    guardian_compiler
)

# Add test to CTest
add_test(NAME GuardianTests COMMAND run_tests)
