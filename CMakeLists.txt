# ================================================================================= #
# ------------- AlgoDataStruct: Data Structures Library Configuration ------------- #
# ================================================================================= #
#
# This CMake configuration works in tandem with the toolchain files to provide
# a clean and maintainable build system. The toolchain files handle compiler
# selection and basic configuration, while this file focuses on project-specific
# settings and target definitions.
#
# Key Design Decisions:
#   - Toolchain files (clang-toolchain.cmake, gcc-toolchain.cmake) handle
#     compiler selection, RPATH, and standard library configuration.
#   - This file focuses on project structure, compiler flags, and target creation.
#   - Minimal duplication and clear separation of concerns.
#   - Modular CMake configuration using separate files in cmake/ directory:
#       * CompilerSetup.cmake       - Compiler flags and helper functions
#       * LibraryDefinition.cmake   - ads_lib header-only library definition
#       * MainExecutables.cmake     - Test executables for data structures
#       * GoogleTest.cmake          - GoogleTest integration (optional)
#
# Usage:
#   With Clang (recommended):
#     cmake -DCMAKE_TOOLCHAIN_FILE=clang-toolchain.cmake -B build
#
#   With GCC:
#     cmake -DCMAKE_TOOLCHAIN_FILE=gcc-toolchain.cmake -B build
#
#   Enable testing:
#     cmake -DCMAKE_TOOLCHAIN_FILE=clang-toolchain.cmake -DENABLE_TESTING=ON -B build
#
#   Then build:
#     cmake --build build
#
# ================================================================================= #

cmake_minimum_required(VERSION 3.20)
project(AlgoDataStructProject
    VERSION 1.0.0
    LANGUAGES CXX
    DESCRIPTION "Educational data structures library in C++"
)

# ------------------------------- ANSI Color Codes -------------------------------- #
# These color codes make the configuration output more readable in the terminal.
# They work on Unix-like systems and modern Windows terminals.
if(UNIX OR APPLE OR CMAKE_HOST_WIN32)
    string(ASCII 27 Esc)
    set(ANSI_RESET   "${Esc}[0m")
    set(ANSI_BOLD    "${Esc}[1m")
    set(ANSI_RED     "${Esc}[31m")
    set(ANSI_GREEN   "${Esc}[32m")
    set(ANSI_YELLOW  "${Esc}[33m")
    set(ANSI_BLUE    "${Esc}[34m")
    set(ANSI_CYAN    "${Esc}[36m")
endif()

# ------------------------- Toolchain Usage Verification -------------------------- #
# While it's possible to configure without a toolchain file, it's not recommended
# because CMake's automatic compiler detection may find wrappers (like ccache)
# instead of the actual compiler, causing issues with clangd and IDE support.
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE OR CMAKE_TOOLCHAIN_FILE STREQUAL "")
    message(WARNING
        "\n"
        "${ANSI_YELLOW}\n"
        "╔═══════════════════════════════════════════════════════════════╗\n"
        "║             WARNING: No Toolchain File Specified              ║\n"
        "╚═══════════════════════════════════════════════════════════════╝"
        "${ANSI_RESET}\n"
        "\n"
        "It's strongly recommended to use a toolchain file for consistent\n"
        "compiler selection and proper clangd support.\n"
        "\n"
        "Recommended configuration:\n"
        "  cmake -DCMAKE_TOOLCHAIN_FILE=clang-toolchain.cmake -B build\n"
        "\n"
        "Available toolchains:\n"
        "  - clang-toolchain.cmake (recommended, best IDE support)\n"
        "  - gcc-toolchain.cmake   (robust fallback)\n"
        "\n"
        "Continuing with automatic compiler detection...\n")
endif()

# ------------------------------- Project Settings -------------------------------- #
# These settings apply to all targets in the project.

# C++20 provides a good balance of modern features and compiler support.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Disable GNU extensions for portability.

# Generate compile_commands.json for clangd and other tools.
# This is essential for IDE support and code navigation.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Output directories for built executables and libraries.
# Putting them in the source tree makes them easier to find and run.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

# Create output directories if they don't exist.
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(MAKE_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

# ----------------------------- Modular Configuration ----------------------------- #
# Include modular CMake configuration files from the cmake/ directory.
# This keeps the main configuration file clean and maintainable.

# Compiler setup: Include path detection and helper functions.
include(cmake/CompilerSetup.cmake)

# Run the compiler include path detection.
detect_compiler_system_includes(COMPILER_SYSTEM_INCLUDE_PATHS)

# Library definition: Header-only ads_lib target.
include(cmake/LibraryDefinition.cmake)

# Main test executables for data structures.
include(cmake/MainExecutables.cmake)

# ---------------------------- GoogleTest Integration ----------------------------- #
# GoogleTest support is optional and disabled by default.
# Enable it with: cmake -DENABLE_TESTING=ON ...

option(ENABLE_APPS "Build showcase applications and games" ON)
option(ENABLE_TESTING "Enable unit testing with GoogleTest" OFF)

include(cmake/GoogleTest.cmake)

# ------------------------------- Apps Integration -------------------------------- #
# Showcase applications (for example games) live under apps/ and are
# built separately from the data structure demos in src/.
set(APPS_CONFIGURED OFF)
if(ENABLE_APPS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/apps/CMakeLists.txt")
    add_subdirectory(apps)
    set(APPS_CONFIGURED ON)
elseif(ENABLE_APPS)
    message(WARNING
        "${ANSI_YELLOW}ENABLE_APPS=ON but apps/CMakeLists.txt was not found. Skipping apps build.${ANSI_RESET}"
    )
endif()

# -------------------------------- Utility Targets -------------------------------- #
# These targets provide convenience commands for common development tasks.

# Create a symlink to compile_commands.json in the project root.
# This makes it easy for clangd and other tools to find the compilation database.
add_custom_target(symlink_compile_commands
    COMMAND ${CMAKE_COMMAND} -E create_symlink
            "${CMAKE_BINARY_DIR}/compile_commands.json"
            "${CMAKE_SOURCE_DIR}/compile_commands.json"
    COMMENT "${ANSI_CYAN}Creating symlink for compile_commands.json${ANSI_RESET}"
    VERBATIM
)

# Convenience 'run' target that builds and runs the BST test.
if(TARGET test_BST)
    add_custom_target(run
        COMMAND $<TARGET_FILE:test_BST>
        DEPENDS test_BST
        COMMENT "${ANSI_CYAN}Running Binary Search Tree tests${ANSI_RESET}"
        USES_TERMINAL
    )
endif()

# ----------------------------- Configuration Summary ----------------------------- #
# Print a nicely formatted summary of the configuration.

message(STATUS "")
message(STATUS "${ANSI_BLUE}╔═══════════════════════════════════════════════════════════════╗${ANSI_RESET}")
message(STATUS "${ANSI_BLUE}║${ANSI_RESET}            AlgoDataStruct - Configuration Summary             ${ANSI_BLUE}║${ANSI_RESET}")
message(STATUS "${ANSI_BLUE}╚═══════════════════════════════════════════════════════════════╝${ANSI_RESET}")
message(STATUS "")

# Display toolchain information.
if(CMAKE_TOOLCHAIN_FILE)
    get_filename_component(TOOLCHAIN_NAME ${CMAKE_TOOLCHAIN_FILE} NAME)
    message(STATUS "  ${ANSI_CYAN}Toolchain${ANSI_RESET}      : ${TOOLCHAIN_NAME}")
else()
    message(STATUS "  ${ANSI_CYAN}Toolchain${ANSI_RESET}      : ${ANSI_YELLOW}None (automatic detection)${ANSI_RESET}")
endif()

message(STATUS "  ${ANSI_CYAN}Compiler${ANSI_RESET}       : ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  ${ANSI_CYAN}Compiler Path${ANSI_RESET}  : ${CMAKE_CXX_COMPILER}")
message(STATUS "  ${ANSI_CYAN}Build Type${ANSI_RESET}     : ${CMAKE_BUILD_TYPE}")
message(STATUS "  ${ANSI_CYAN}C++ Standard${ANSI_RESET}   : C++${CMAKE_CXX_STANDARD}")

# Report on clangd support.
if(COMPILER_SYSTEM_INCLUDE_PATHS)
    list(LENGTH COMPILER_SYSTEM_INCLUDE_PATHS PATH_COUNT)
    message(STATUS "  ${ANSI_CYAN}Clangd Support${ANSI_RESET} : ${ANSI_GREEN}✓ (${PATH_COUNT} include paths)${ANSI_RESET}")
else()
    message(STATUS "  ${ANSI_CYAN}Clangd Support${ANSI_RESET} : ${ANSI_YELLOW}⚠ No include paths detected${ANSI_RESET}")
endif()

# Report on testing.
if(ENABLE_TESTING)
    message(STATUS "  ${ANSI_CYAN}Testing${ANSI_RESET}        : ${ANSI_GREEN}Enabled (GoogleTest)${ANSI_RESET}")
else()
    message(STATUS "  ${ANSI_CYAN}Testing${ANSI_RESET}        : Disabled (use -DENABLE_TESTING=ON)")
endif()

if(ENABLE_APPS AND APPS_CONFIGURED)
    message(STATUS "  ${ANSI_CYAN}Apps${ANSI_RESET}           : ${ANSI_GREEN}Enabled${ANSI_RESET}")
elseif(ENABLE_APPS)
    message(STATUS "  ${ANSI_CYAN}Apps${ANSI_RESET}           : ${ANSI_YELLOW}Requested but not configured${ANSI_RESET}")
else()
    message(STATUS "  ${ANSI_CYAN}Apps${ANSI_RESET}           : Disabled (use -DENABLE_APPS=ON)")
endif()

message(STATUS "")
message(STATUS "${ANSI_BLUE}╚═══════════════════════════════════════════════════════════════╝${ANSI_RESET}")
message(STATUS "")

# ================================================================================= #
# End of CMakeLists.txt
