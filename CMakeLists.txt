cmake_minimum_required(VERSION 3.10)
project(TheGuardian)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Only ONE library — the core
add_library(guardian_core
    src/core/atom.cpp
    src/core/molecule.cpp
)

# The Guardian executable
add_executable(guardianc
    src/main.cpp
)

target_link_libraries(guardianc
    guardian_core
)

# VM Library
add_library(guardian_vm
    src/vm/vm.cpp
)

# Compiler Library
add_library(guardian_compiler
    src/compiler/lexer.cpp
)

# Link them to guardianc
target_link_libraries(guardianc
    guardian_core
    guardian_vm
    guardian_compiler
)
