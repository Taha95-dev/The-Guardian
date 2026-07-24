cmake_minimum_required(VERSION 3.10)
project(The-Guardian VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Options
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" OFF)

# ============================================
# CORE LIBRARY
# ============================================
set(CORE_SOURCES
    src/core/atom.cpp
    src/core/molecule.cpp
)

add_library(guardian_core STATIC ${CORE_SOURCES})

target_include_directories(guardian_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(guardian_core PUBLIC cxx_std_17)

# ============================================
# MEMORY LIBRARY
# ============================================
set(MEMORY_SOURCES
    src/memory/memory.cpp
)

add_library(guardian_memory STATIC ${MEMORY_SOURCES})

target_include_directories(guardian_memory PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(guardian_memory PUBLIC cxx_std_17)

# ============================================
# FORMAT LIBRARY
# ============================================
set(FORMAT_SOURCES
    src/format/format.cpp
    src/format/gbin_format.cpp
)

add_library(guardian_format STATIC ${FORMAT_SOURCES})

target_include_directories(guardian_format PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(guardian_format
    guardian_core
)

target_compile_features(guardian_format PUBLIC cxx_std_17)

# ============================================
# PARSER LIBRARY
# ============================================
set(PARSER_SOURCES
    src/parser/parser.cpp
)

add_library(guardian_parser STATIC ${PARSER_SOURCES})

target_include_directories(guardian_parser PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(guardian_parser
    guardian_core
)

target_compile_features(guardian_parser PUBLIC cxx_std_17)

# ============================================
# VM LIBRARY
# ============================================
set(VM_SOURCES
    src/vm/vm.cpp
)

add_library(guardian_vm STATIC ${VM_SOURCES})

target_include_directories(guardian_vm PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(guardian_vm
    guardian_core
)

target_compile_features(guardian_vm PUBLIC cxx_std_17)

# ============================================
# MAIN EXECUTABLE
# ============================================
set(MAIN_SOURCES
    src/main.cpp
)

add_executable(guardianc ${MAIN_SOURCES})

target_link_libraries(guardianc
    guardian_core
    guardian_memory
    guardian_format
    guardian_parser
    guardian_vm
)

target_compile_features(guardianc PUBLIC cxx_std_17)

# ============================================
# TESTS
# ============================================
if(BUILD_TESTS)
    enable_testing()
    
    # Test runner
    add_executable(test_runner
        tests/test_runner.cpp
        tests/test_atoms.cpp
        tests/test_quarks.cpp
        tests/test_format.cpp
        tests/test_memory.cpp
    )
    
    target_link_libraries(test_runner
        guardian_core
        guardian_memory
        guardian_format
        guardian_vm
    )
    
    target_compile_features(test_runner PUBLIC cxx_std_17)
    
    # Add tests
    add_test(NAME test_runner COMMAND test_runner)
endif()

# ============================================
# INSTALL
# ============================================
install(TARGETS guardian_core guardian_memory guardian_format guardian_vm
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(TARGETS guardianc
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/guardian
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

# ============================================
# PKG-CONFIG
# ============================================
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/guardian.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/guardian.pc
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/guardian.pc
    DESTINATION lib/pkgconfig
)

# ============================================
# PRINT BUILD INFO
# ============================================
message(STATUS "The-Guardian Build Configuration:")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Tests: ${BUILD_TESTS}")
message(STATUS "  Build Examples: ${BUILD_EXAMPLES}")
