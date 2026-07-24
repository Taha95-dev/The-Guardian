cmake_minimum_required(VERSION 3.10)
project(TheGuardian VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/vm
    ${CMAKE_SOURCE_DIR}/src/memory
    ${CMAKE_SOURCE_DIR}/src/parser
    ${CMAKE_SOURCE_DIR}/src/format
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
# PARSER FRAMEWORK
# ============================================
add_library(guardian_parser
    src/parser/parser.cpp
)

# ============================================
# FORMAT FRAMEWORK
# ============================================
add_library(guardian_format
    src/format/format.cpp
    src/format/gbin_format.cpp
)

# ============================================
# VM LIBRARY
# ============================================
add_library(guardian_vm
    src/vm/vm.cpp
)

target_link_libraries(guardian_vm
    guardian_core
    guardian_memory
    guardian_parser
    guardian_format
)

# ============================================
# GUARDIAN CLI
# ============================================
add_executable(guardianc
    src/main.cpp
)

target_link_libraries(guardianc
    guardian_core
    guardian_memory
    guardian_parser
    guardian_format
    guardian_vm
)

# ============================================
# INSTALLATION
# ============================================
install(TARGETS guardian_core guardian_memory guardian_parser guardian_format guardian_vm
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(TARGETS guardianc
        RUNTIME DESTINATION bin)

install(DIRECTORY src/core/ DESTINATION include/guardian/core
        FILES_MATCHING PATTERN "*.hpp")
install(DIRECTORY src/vm/ DESTINATION include/guardian/vm
        FILES_MATCHING PATTERN "*.hpp")
install(DIRECTORY src/memory/ DESTINATION include/guardian/memory
        FILES_MATCHING PATTERN "*.hpp")
install(DIRECTORY src/parser/ DESTINATION include/guardian/parser
        FILES_MATCHING PATTERN "*.hpp")
install(DIRECTORY src/format/ DESTINATION include/guardian/format
        FILES_MATCHING PATTERN "*.hpp")
