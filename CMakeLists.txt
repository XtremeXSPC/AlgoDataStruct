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
#       * MainExecutables.cmake     - Demo executables from examples/*/main_*.cc
#       * GoogleTest.cmake          - GoogleTest integration (optional)
#
# Usage:
#   With presets (recommended):
#     cmake --preset debug
#     cmake --build --preset build-debug
#
#   Or manually with Clang:
#     cmake -DCMAKE_TOOLCHAIN_FILE=clang-toolchain.cmake -S . -B build/clang/debug
#
#   Enable testing:
#     cmake -DCMAKE_TOOLCHAIN_FILE=clang-toolchain.cmake -DENABLE_TESTING=ON -S . -B build/clang/debug-tests
#
# ================================================================================= #

# 3.23 required by target_sources(FILE_SET HEADERS) in ads_lib (also covers presets v3).
cmake_minimum_required(VERSION 3.23)
project(AlgoDataStructProject
    VERSION 1.0.0
    LANGUAGES CXX
    DESCRIPTION "Educational data structures library in C++"
)

# ------------------------------- ANSI Color Codes -------------------------------- #
# ANSI colors for readable output; honor the NO_COLOR convention (no-color.org).
if(NOT DEFINED ENV{NO_COLOR})
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
        "  cmake --preset debug\n"
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

# Default build type for single-config generators (Ninja/Unix Makefiles).
# Multi-config generators (Xcode, Visual Studio) manage configurations differently.
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
    message(STATUS "${ANSI_YELLOW}No build type selected, defaulting to Debug${ANSI_RESET}")
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    Debug
    Release
    RelWithDebInfo
    MinSizeRel
    Sanitize
    ThreadSanitize
)

# Generate compile_commands.json for clangd and other tools.
# This is essential for IDE support and code navigation.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------ Project Options --------------------------------- #
# Keep option definitions close to the core project settings so later modules can
# branch on them consistently.

option(ENABLE_DEMOS "Build demo executables from examples/*/main_*.cc" ON)
option(ENABLE_APPS "Build showcase applications and games" ON)
option(ENABLE_TESTING "Enable unit testing with GoogleTest" OFF)
option(DISABLE_PCH "Disable precompiled headers (for debugging)" OFF)
option(DISABLE_UNITY_BUILD "Disable Unity Build for tests (for faster incremental builds)" OFF)

# Outputs under the build tree, keeping the source tree free of build artifacts.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

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

# Shared build options target; must precede anything that links it.
include(cmake/BuildOptions.cmake)

# Library definition: Header-only ads_lib target.
include(cmake/LibraryDefinition.cmake)

# Demo executables built from examples/*/main_*.cc.
if(ENABLE_DEMOS)
    include(cmake/MainExecutables.cmake)
else()
    unset(ADS_DEFAULT_RUN_TARGET)
endif()

# ---------------------------- GoogleTest Integration ----------------------------- #
# GoogleTest support is optional and disabled by default.
# Enable it with: cmake -DENABLE_TESTING=ON ...

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

# Convenience 'run' target that builds and runs the default demo.
if(DEFINED ADS_DEFAULT_RUN_TARGET AND TARGET ${ADS_DEFAULT_RUN_TARGET})
    add_custom_target(run
        COMMAND $<TARGET_FILE:${ADS_DEFAULT_RUN_TARGET}>
        DEPENDS ${ADS_DEFAULT_RUN_TARGET}
        COMMENT "${ANSI_CYAN}Running ${ADS_DEFAULT_RUN_TARGET}${ANSI_RESET}"
        USES_TERMINAL
    )
endif()

# ----------------------------- Configuration Summary ----------------------------- #
# Print a nicely formatted summary of the configuration.

message(STATUS "")
message(STATUS "${ANSI_BLUE}╔═══════════════════════════════════════════════════════════════╗${ANSI_RESET}")
message(STATUS "${ANSI_BLUE}╟${ANSI_RESET}─────────── AlgoDataStruct - Configuration Summary ────────────${ANSI_BLUE}╢${ANSI_RESET}")
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

if(ENABLE_DEMOS)
    if(DEFINED ADS_DEFAULT_RUN_TARGET)
        message(STATUS "  ${ANSI_CYAN}Demos${ANSI_RESET}          : ${ANSI_GREEN}Enabled${ANSI_RESET} (default: ${ADS_DEFAULT_RUN_TARGET})")
    else()
        message(STATUS "  ${ANSI_CYAN}Demos${ANSI_RESET}          : ${ANSI_YELLOW}Enabled${ANSI_RESET} (no demo sources found)")
    endif()
else()
    message(STATUS "  ${ANSI_CYAN}Demos${ANSI_RESET}          : Disabled (use -DENABLE_DEMOS=ON)")
endif()

if(ENABLE_APPS AND APPS_CONFIGURED)
    message(STATUS "  ${ANSI_CYAN}Apps${ANSI_RESET}           : ${ANSI_GREEN}Enabled${ANSI_RESET}")
elseif(ENABLE_APPS)
    message(STATUS "  ${ANSI_CYAN}Apps${ANSI_RESET}           : ${ANSI_YELLOW}Requested but not configured${ANSI_RESET}")
else()
    message(STATUS "  ${ANSI_CYAN}Apps${ANSI_RESET}           : Disabled (use -DENABLE_APPS=ON)")
endif()

# Report on build optimizations.
if(NOT DISABLE_PCH)
    message(STATUS "  ${ANSI_CYAN}PCH${ANSI_RESET}            : ${ANSI_GREEN}Enabled${ANSI_RESET} (STL + GoogleTest headers)")
else()
    message(STATUS "  ${ANSI_CYAN}PCH${ANSI_RESET}            : ${ANSI_YELLOW}Disabled${ANSI_RESET} (use -DDISABLE_PCH=OFF)")
endif()

if(ENABLE_TESTING AND NOT DISABLE_UNITY_BUILD)
    message(STATUS "  ${ANSI_CYAN}Unity Build${ANSI_RESET}    : ${ANSI_GREEN}Enabled${ANSI_RESET} (tests only, batch size: 8)")
elseif(ENABLE_TESTING AND DISABLE_UNITY_BUILD)
    message(STATUS "  ${ANSI_CYAN}Unity Build${ANSI_RESET}    : ${ANSI_YELLOW}Disabled${ANSI_RESET} (use -DDISABLE_UNITY_BUILD=OFF)")
else()
    message(STATUS "  ${ANSI_CYAN}Unity Build${ANSI_RESET}    : N/A (testing not enabled)")
endif()

message(STATUS "")
message(STATUS "${ANSI_BLUE}═════════════════════════════════════════════════════════════════${ANSI_RESET}")
message(STATUS "")

# ================================================================================= #
# End of CMakeLists.txt
