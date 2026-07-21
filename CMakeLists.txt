cmake_minimum_required(VERSION 3.10)
project(TheGuardian VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================
# INCLUDE DIRECTORIES
# ============================================
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/vm
    ${CMAKE_SOURCE_DIR}/src/compiler
    ${CMAKE_SOURCE_DIR}/src/om
    ${CMAKE_SOURCE_DIR}/src/memory
    ${CMAKE_SOURCE_DIR}/src/atoms
    ${CMAKE_SOURCE_DIR}/src/atoms/primitive
    ${CMAKE_SOURCE_DIR}/src/atoms/text
    ${CMAKE_SOURCE_DIR}/src/atoms/containers
    ${CMAKE_SOURCE_DIR}/src/atoms/special
)

# ============================================
# CORE LIBRARY
# ============================================
add_library(guardian_core
    src/core/atom.cpp
    src/core/molecule.cpp
)

# ============================================
# MEMORY LIBRARY
# ============================================
add_library(guardian_memory
    src/memory/memory.cpp
)

# ============================================
# VM LIBRARY
# ============================================
add_library(guardian_vm
    src/vm/vm.cpp
    src/vm/formats/format_registry.cpp
    src/vm/formats/guardian_format.cpp
)

target_link_libraries(guardian_vm
    guardian_core
    guardian_memory
)

# ============================================
# COMPILER LIBRARY
# ============================================
add_library(guardian_compiler
    src/compiler/lexer.cpp
)

# ============================================
# OM LANGUAGE
# ============================================
add_library(om
    src/om/lexer.cpp
    src/om/parser.cpp
    src/om/codegen.cpp
    src/om/compiler.cpp
)

target_link_libraries(om
    guardian_core
    guardian_vm
    guardian_compiler
    guardian_memory
)

# Om Compiler Executable
add_executable(omc
    src/om/main.cpp
)

target_link_libraries(omc
    om
    guardian_core
    guardian_vm
    guardian_compiler
    guardian_memory
)

# ============================================
# GUARDIAN CLI
# ============================================
add_executable(guardianc
    src/main.cpp
)

target_link_libraries(guardianc
    guardian_core
    guardian_vm
    guardian_compiler
    guardian_memory
)

# ============================================
# TESTS
# ============================================
enable_testing()

# Main test runner (includes all tests)
add_executable(run_tests
    tests/test_runner.cpp
    tests/test_atoms.cpp
    tests/test_memory.cpp
)

target_link_libraries(run_tests
    guardian_core
    guardian_vm
    guardian_compiler
    guardian_memory
)

add_test(NAME GuardianTests COMMAND run_tests)

# ============================================
# INSTALLATION
# ============================================
install(DIRECTORY include/guardian/ DESTINATION include/guardian
        FILES_MATCHING PATTERN "*.hpp")

install(TARGETS guardian_core guardian_vm guardian_compiler guardian_memory om
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(TARGETS guardianc omc
        RUNTIME DESTINATION bin)

configure_file(${CMAKE_SOURCE_DIR}/guardian.pc.in ${CMAKE_BINARY_DIR}/guardian.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/guardian.pc DESTINATION lib/pkgconfig)

# Atom types
include_directories(
    ${CMAKE_SOURCE_DIR}/src/atoms/numeric
    ${CMAKE_SOURCE_DIR}/src/atoms/container
    ${CMAKE_SOURCE_DIR}/src/atoms/system
    ${CMAKE_SOURCE_DIR}/src/atoms/io
)

add_executable(test_new_atoms
    tests/test_new_atoms.cpp
)

target_link_libraries(test_new_atoms
    guardian_core
    guardian_memory
)
