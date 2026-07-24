cmake_minimum_required(VERSION 3.10)
project(GuardianTest)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the Guardian library using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(GUARDIAN REQUIRED guardian)

# Create the executable
add_executable(test_guardian test_guardian.cpp)

# Manually add all Guardian libraries (pkg-config might miss some)
set(GUARDIAN_ALL_LIBS
    guardian_core
    guardian_format
    guardian_memory
    guardian_vm
    guardian_parser
    guardian_compiler
)

# Link with all Guardian libraries
target_link_libraries(test_guardian ${GUARDIAN_ALL_LIBS})
target_include_directories(test_guardian PRIVATE ${GUARDIAN_INCLUDE_DIRS})
target_compile_options(test_guardian PRIVATE ${GUARDIAN_CFLAGS_OTHER})

# Print configuration
message(STATUS "Guardian found: ${GUARDIAN_FOUND}")
message(STATUS "Guardian include dirs: ${GUARDIAN_INCLUDE_DIRS}")
message(STATUS "Guardian libraries: ${GUARDIAN_ALL_LIBS}")
