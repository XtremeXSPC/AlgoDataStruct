# ============================================================================ #
# ---------- AlgoDataStruct: Data Structures Library Configuration ----------- #
# ============================================================================ #
#
# This CMake configuration works in tandem with the toolchain files to provide
# a clean and maintainable build system. The toolchain files handle compiler
# selection and basic configuration, while this file focuses on project-specific
# settings and target definitions.
#
# Key Design Decisions:
# - Toolchain files (clang-toolchain.cmake, gcc-toolchain.cmake) handle
#   compiler selection, RPATH, and standard library configuration
# - This file focuses on project structure, compiler flags, and target creation
# - Minimal duplication and clear separation of concerns
#
# Usage:
#   With Clang (recommended):
#     cmake -DCMAKE_TOOLCHAIN_FILE=clang-toolchain.cmake -B build
#   
#   With GCC:
#     cmake -DCMAKE_TOOLCHAIN_FILE=gcc-toolchain.cmake -B build
#   
#   Then build:
#     cmake --build build
#
# ============================================================================ #

cmake_minimum_required(VERSION 3.20)
project(AlgoDataStructProject 
    VERSION 1.0.0
    LANGUAGES CXX
    DESCRIPTION "Educational data structures library in C++"
)

# ----------------------------- ANSI Color Codes ----------------------------- #
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

# ----------------------- Toolchain Usage Verification ----------------------- #
# While it's possible to configure without a toolchain file, it's not recommended
# because CMake's automatic compiler detection may find wrappers (like ccache)
# instead of the actual compiler, causing issues with clangd and IDE support.
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE OR CMAKE_TOOLCHAIN_FILE STREQUAL "")
    message(WARNING
        "\n"
        "${ANSI_YELLOW}╔═══════════════════════════════════════════════════════════════╗\n"
        "║          WARNING: No Toolchain File Specified                 ║\n"
        "╚═══════════════════════════════════════════════════════════════╝${ANSI_RESET}\n"
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

# ----------------------------- Project Settings ----------------------------- #
# These settings apply to all targets in the project.

# C++20 provides a good balance of modern features and compiler support.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Disable GNU extensions for portability

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

# -------------------- Simplified Include Path Detection --------------------- #
# With toolchain files handling compiler selection, we only need basic
# include path detection for clangd support. This is much simpler than
# the original version because we know the compiler is correctly configured.
function(detect_compiler_system_includes OUTPUT_VARIABLE)
    # Check if we've already detected paths in this configuration.
    if(DEFINED CACHE{COMPILER_SYSTEM_INCLUDES_CACHED})
        set(${OUTPUT_VARIABLE} "${COMPILER_SYSTEM_INCLUDES_CACHED}" PARENT_SCOPE)
        message(STATUS "${ANSI_CYAN}Using cached compiler include paths${ANSI_RESET}")
        return()
    endif()

    set(DETECTED_PATHS "")
    
    # Query the compiler for its include search paths.
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -E -x c++ -v /dev/null
        OUTPUT_VARIABLE COMPILER_OUTPUT
        ERROR_VARIABLE COMPILER_OUTPUT  # Compilers often print to stderr
        RESULT_VARIABLE EXIT_CODE
        TIMEOUT 5
    )

    if(EXIT_CODE EQUAL 0)
        # Parse the compiler output to extract include paths.
        string(REPLACE "\n" ";" OUTPUT_LINES "${COMPILER_OUTPUT}")
        set(IS_PARSING_INCLUDES FALSE)
        
        foreach(line ${OUTPUT_LINES})
            # Start of include path section
            if(line MATCHES "^#include.*search starts here:")
                set(IS_PARSING_INCLUDES TRUE)
            # End of include path section
            elseif(line MATCHES "^End of search list.")
                break()
            # Parse actual include paths
            elseif(IS_PARSING_INCLUDES)
                string(STRIP "${line}" path)
                # Exclude framework directories on macOS (they're not regular headers)
                if(IS_DIRECTORY "${path}" AND NOT path MATCHES "\\(framework directory\\)")
                    list(APPEND DETECTED_PATHS "${path}")
                endif()
            endif()
        endforeach()
    endif()

    # Cache results for faster reconfiguration
    if(DETECTED_PATHS)
        list(REMOVE_DUPLICATES DETECTED_PATHS)
        set(COMPILER_SYSTEM_INCLUDES_CACHED "${DETECTED_PATHS}" CACHE INTERNAL 
            "Cached compiler include paths")
        set(${OUTPUT_VARIABLE} "${DETECTED_PATHS}" PARENT_SCOPE)
        
        list(LENGTH DETECTED_PATHS PATH_COUNT)
        message(STATUS "${ANSI_GREEN}Detected ${PATH_COUNT} compiler include paths${ANSI_RESET}")
    else()
        message(STATUS "${ANSI_YELLOW}Could not auto-detect include paths (may still work fine)${ANSI_RESET}")
        set(${OUTPUT_VARIABLE} "" PARENT_SCOPE)
    endif()
endfunction()

# Run the detection function
detect_compiler_system_includes(COMPILER_SYSTEM_INCLUDE_PATHS)

# ----------------------- Library Definition: ads_lib ------------------------ #
#
# This is a header-only template library. We define it as an INTERFACE library
# because there are no compiled sources - everything is in headers and .tpp files.
# An INTERFACE library in CMake is used to specify properties that should be
# propagated to targets that link against it.
#
add_library(ads_lib INTERFACE)

# Specify where the public headers are located.
# Users of this library (our test executables) will automatically get this
# path added to their include directories.
target_include_directories(ads_lib INTERFACE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# List all the headers and implementation files that are part of this library.
# While not strictly necessary for a header-only library, this helps IDEs
# and build systems understand the project structure.
target_sources(ads_lib
    INTERFACE
        FILE_SET HEADERS
        BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
        FILES
            # List interface
            include/ads/lists/List.hpp
            include/ads/lists/Exception.hpp
            include/ads/lists/Doubly_Linked_List.hpp

            # Stack interface and implementations
            include/ads/stacks/Stack.hpp
            include/ads/stacks/Stack_Exception.hpp
            include/ads/stacks/Array_Stack.hpp
            include/ads/stacks/Linked_Stack.hpp

            # Queue interface and implementations
            include/ads/queues/Queue.hpp
            include/ads/queues/Queue_Exception.hpp
            include/ads/queues/Circular_Array_Queue.hpp
            include/ads/queues/Linked_Queue.hpp

            # Binary Tree interface and implementations
            include/ads/trees/Binary_Tree.hpp
            include/ads/trees/Binary_Tree_Exception.hpp
            include/ads/trees/Binary_Search_Tree.hpp

    # Template implementations (.tpp files)
    # These are included by the headers, but we list them here so they
    # appear in IDEs and are tracked for changes.
    INTERFACE
        src/ads/lists/Doubly_Linked_List.tpp
        src/ads/stacks/Array_Stack.tpp
        src/ads/stacks/Linked_Stack.tpp
        src/ads/queues/Circular_Array_Queue.tpp
        src/ads/queues/Linked_Queue.tpp
        src/ads/trees/Binary_Search_Tree.tpp
)

# --------------------- Helper Function: Add Executable ---------------------- #
#
# This function encapsulates all the logic for creating a test executable.
# It ensures consistent compiler flags, warning levels, and configuration
# across all test programs.
#
function(ads_add_executable TARGET_NAME SOURCE_FILE)
    # Create the executable target
    add_executable(${TARGET_NAME} ${SOURCE_FILE})
    
    # Link with our header-only library.
    # This automatically adds the include directory and all necessary properties.
    target_link_libraries(${TARGET_NAME} PRIVATE ads_lib)
    
    # ---------------------- Compiler Flags and Warnings --------------------- #
    # We use different flags based on build configuration (Debug, Release, etc.)
    # and compiler type (GCC vs Clang).
    
    # Common warning flags for all compilers.
    # These help catch potential bugs and encourage good coding practices.
    set(COMMON_WARNINGS
        -Wall          # Enable most common warnings
        -Wextra        # Enable extra warnings
        -Wpedantic     # Warn about non-standard C++
        -Wshadow       # Warn about variable shadowing
    )
    
    # Configuration-specific flags using generator expressions.
    # Generator expressions are evaluated at build time, not configure time.
    target_compile_options(${TARGET_NAME} PRIVATE
        ${COMMON_WARNINGS}
        
        # Debug configuration: No optimization, full debug info
        $<$<CONFIG:Debug>:-g -O0>
        
        # Release configuration: Maximum optimization, no debug info
        $<$<CONFIG:Release>:-O3 -DNDEBUG>
        
        # Sanitizer configuration (if using sanitizer build type)
        $<$<CONFIG:Sanitize>:-g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer>
    )
    
    # Compiler-specific definitions for enhanced debugging
    target_compile_definitions(${TARGET_NAME} PRIVATE
        # For GCC in Debug mode, enable STL debug mode
        # This adds runtime checks for iterators and containers
        $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GNU>>:_GLIBCXX_DEBUG>
        
        # For Clang/libc++, enable extensive hardening checks
        $<$<CXX_COMPILER_ID:Clang,AppleClang>:_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_EXTENSIVE>
    )
    
    # ----------------- Add System Include Paths for clangd ------------------ #
    # If we detected compiler include paths, add them as system includes.
    # This helps clangd find standard library headers.
    # System includes are treated specially: warnings in them are suppressed.
    if(COMPILER_SYSTEM_INCLUDE_PATHS)
        foreach(dir IN LISTS COMPILER_SYSTEM_INCLUDE_PATHS)
            target_include_directories(${TARGET_NAME} SYSTEM PRIVATE ${dir})
        endforeach()
    endif()
    
    # ------------------------- Linker Configuration ------------------------- #
    # Configuration-specific linker flags
    target_link_options(${TARGET_NAME} PRIVATE
        # Link sanitizer libraries for Sanitize builds
        $<$<CONFIG:Sanitize>:-fsanitize=address,undefined>
        
        # Strip symbols in Release builds to reduce executable size
        $<$<CONFIG:Release>:-s>
    )
    
    # Print a status message when the target is added
    message(STATUS "${ANSI_GREEN}Added test executable: ${TARGET_NAME}${ANSI_RESET}")
endfunction()

# -------------------------- Main Test Executables --------------------------- #
# These are the actual test programs that exercise the data structures library.

# Binary Search Tree test
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main_Binary_Search_Tree.cc")
    ads_add_executable(test_bst src/main_Binary_Search_Tree.cc)
endif()

# Stacks and Queues test
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main_Stacks_Queues.cc")
    ads_add_executable(test_stacks_queues src/main_Stacks_Queues.cc)
endif()

# Linked Lists test
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main_Linked_Lists.cc")
    ads_add_executable(test_lists src/main_Linked_Lists.cc)
endif()

# -------------------------- GoogleTest Integration -------------------------- #
# GoogleTest support is optional and disabled by default.
# Enable it with: cmake -DENABLE_TESTING=ON ...

option(ENABLE_TESTING "Enable unit testing with GoogleTest" OFF)

if(ENABLE_TESTING)
    enable_testing()
    
    # Download and configure GoogleTest using FetchContent
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    
    # Configure GoogleTest to work with MSVC's runtime library
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    
    FetchContent_MakeAvailable(googletest)
    
    # Add the test executable if test files exist
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test_DoublyLinkedList.cpp")
        add_executable(runTests tests/Test_DoublyLinkedList.cpp)
        target_compile_options(runTests PRIVATE -Wall -Wextra -Wpedantic -g)
        target_link_libraries(runTests PRIVATE ads_lib gtest gtest_main)
        
        # Register tests with CTest
        include(GoogleTest)
        gtest_discover_tests(runTests)
        
        message(STATUS "${ANSI_GREEN}GoogleTest enabled: runTests executable created${ANSI_RESET}")
    endif()
endif()

# ----------------------------- Utility Targets ------------------------------ #
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

# Convenience 'run' target that builds and runs the BST test
if(TARGET test_bst)
    add_custom_target(run
        COMMAND $<TARGET_FILE:test_bst>
        DEPENDS test_bst
        COMMENT "${ANSI_CYAN}Running Binary Search Tree tests${ANSI_RESET}"
        USES_TERMINAL
    )
endif()

# -------------------------- Configuration Summary --------------------------- #
# Print a nicely formatted summary of the configuration.

message(STATUS "")
message(STATUS "${ANSI_BLUE}╔═══════════════════════════════════════════════════════════════╗${ANSI_RESET}")
message(STATUS "${ANSI_BLUE}║${ANSI_RESET}       AlgoDataStruct - Configuration Summary                  ${ANSI_BLUE}║${ANSI_RESET}")
message(STATUS "${ANSI_BLUE}╚═══════════════════════════════════════════════════════════════╝${ANSI_RESET}")
message(STATUS "")

# Display toolchain information
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

# Report on clangd support
if(COMPILER_SYSTEM_INCLUDE_PATHS)
    list(LENGTH COMPILER_SYSTEM_INCLUDE_PATHS PATH_COUNT)
    message(STATUS "  ${ANSI_CYAN}Clangd Support${ANSI_RESET} : ${ANSI_GREEN}✓ (${PATH_COUNT} include paths)${ANSI_RESET}")
else()
    message(STATUS "  ${ANSI_CYAN}Clangd Support${ANSI_RESET} : ${ANSI_YELLOW}⚠ No include paths detected${ANSI_RESET}")
endif()

# Report on testing
if(ENABLE_TESTING)
    message(STATUS "  ${ANSI_CYAN}Testing${ANSI_RESET}        : ${ANSI_GREEN}Enabled (GoogleTest)${ANSI_RESET}")
else()
    message(STATUS "  ${ANSI_CYAN}Testing${ANSI_RESET}        : Disabled (use -DENABLE_TESTING=ON)")
endif()

message(STATUS "")
message(STATUS "${ANSI_BLUE}╚═══════════════════════════════════════════════════════════════╝${ANSI_RESET}")
message(STATUS "")

# ============================================================================ #
# End of CMakeLists.txt