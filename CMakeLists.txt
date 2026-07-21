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
)

# ============================================
# CORE LIBRARY (Atom/Molecule + LUT)
# ============================================
add_library(guardian_core
    src/core/atom.cpp
    src/core/molecule.cpp
)

target_include_directories(guardian_core PUBLIC
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/include/guardian/core
)

# ============================================
# VM LIBRARY (Binary formats + Execution)
# ============================================
add_library(guardian_vm
    src/vm/vm.cpp
    src/vm/formats/format_registry.cpp
    src/vm/formats/guardian_format.cpp
)

target_include_directories(guardian_vm PUBLIC
    ${CMAKE_SOURCE_DIR}/src/vm
    ${CMAKE_SOURCE_DIR}/src/vm/formats
    ${CMAKE_SOURCE_DIR}/include/guardian/vm
)

target_link_libraries(guardian_vm
    guardian_core
)

# ============================================
# COMPILER LIBRARY (Lexer/Parser base)
# ============================================
add_library(guardian_compiler
    src/compiler/lexer.cpp
)

target_include_directories(guardian_compiler PUBLIC
    ${CMAKE_SOURCE_DIR}/src/compiler
    ${CMAKE_SOURCE_DIR}/include/guardian/compiler
)

# ============================================
# OM LANGUAGE
# ============================================

# Om Library
add_library(om
    src/om/lexer.cpp
    src/om/parser.cpp
    src/om/codegen.cpp
    src/om/compiler.cpp
)

target_include_directories(om PUBLIC
    ${CMAKE_SOURCE_DIR}/src/om
    ${CMAKE_SOURCE_DIR}/include/guardian/om
)

target_link_libraries(om
    guardian_core
    guardian_vm
    guardian_compiler
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
)

# ============================================
# GUARDIAN CLI (Original)
# ============================================
add_executable(guardianc
    src/main.cpp
)

target_link_libraries(guardianc
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

add_test(NAME GuardianTests COMMAND run_tests)

# ============================================
# INSTALLATION
# ============================================

# Install headers
install(DIRECTORY include/guardian/ DESTINATION include/guardian
        FILES_MATCHING PATTERN "*.hpp")

# Install libraries
install(TARGETS guardian_core guardian_vm guardian_compiler om
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

# Install executables
install(TARGETS guardianc omc
        RUNTIME DESTINATION bin)

# Install pkg-config file
configure_file(${CMAKE_SOURCE_DIR}/guardian.pc.in ${CMAKE_BINARY_DIR}/guardian.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/guardian.pc DESTINATION lib/pkgconfig)
