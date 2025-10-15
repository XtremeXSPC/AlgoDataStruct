# ============================================================================ #
# ----------- AlgoDataStruct: Data Structures Library Configuration ---------- #
# ============================================================================ #
#
# This CMake configuration is designed to work seamlessly with clangd for
# excellent IDE support, while maintaining flexibility for different compilers.
#
# Key features:
# - Automatic compiler include path detection for clangd
# - Support for both GCC and Clang (including LLVM Clang on macOS)
# - Proper RPATH configuration for macOS
# - Clean separation between interface and implementation
# - Optional testing support with GoogleTest
#
# Usage:
#   Default (uses system compiler):
#     cmake -B build && cmake --build build
#
#   With specific compiler toolchain:
#     cmake -DCMAKE_TOOLCHAIN_FILE=clang-toolchain.cmake -B build
#     cmake -DCMAKE_TOOLCHAIN_FILE=gcc-toolchain.cmake -B build
#
# ============================================================================ #

cmake_minimum_required(VERSION 3.20)

# ---------------------------- Toolchain Handling ---------------------------- #
# If no toolchain is specified but a preference exists, suggest it
if(NOT CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{PREFERRED_COMPILER})
    message(STATUS "Hint: Set toolchain with -DCMAKE_TOOLCHAIN_FILE=$ENV{PREFERRED_COMPILER}-toolchain.cmake")
endif()

project(AlgoDataStructProject LANGUAGES CXX)

# -------------------- ANSI Color Codes for Pretty Output -------------------- #
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

# ----------------------------- Project Settings ----------------------------- #
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable compilation database for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# ------------------------ macOS RPATH Configuration ------------------------- #
if(APPLE)
    set(CMAKE_MACOSX_RPATH ON)
    set(CMAKE_SKIP_BUILD_RPATH FALSE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    
    # If using LLVM Clang, get its library path for RPATH
    if(CMAKE_CXX_COMPILER MATCHES "llvm" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        get_filename_component(COMPILER_BIN_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)
        get_filename_component(COMPILER_ROOT ${COMPILER_BIN_DIR} DIRECTORY)
        set(COMPILER_LIB_DIR "${COMPILER_ROOT}/lib/c++")
        
        if(EXISTS "${COMPILER_LIB_DIR}")
            set(CMAKE_INSTALL_RPATH "${COMPILER_LIB_DIR}")
            message(STATUS "${ANSI_CYAN}RPATH configured for: ${COMPILER_LIB_DIR}${ANSI_RESET}")
        endif()
    endif()
endif()

# ------------- Compiler Include Path Auto-Detection for clangd -------------- #
#
# This function detects the system include paths that the compiler uses,
# which is critical for clangd to understand the code correctly.
#
function(detect_compiler_system_includes OUTPUT_VARIABLE)
    # Check cache first for performance
    if(DEFINED CACHE{COMPILER_SYSTEM_INCLUDES_CACHED})
        set(${OUTPUT_VARIABLE} "${COMPILER_SYSTEM_INCLUDES_CACHED}" PARENT_SCOPE)
        message(STATUS "${ANSI_GREEN}Clangd: Using cached compiler include paths${ANSI_RESET}")
        return()
    endif()

    set(DETECTED_PATHS "")

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        # GCC-specific path detection
        message(STATUS "${ANSI_CYAN}Clangd: Detecting GCC system include paths...${ANSI_RESET}")
        
        if(APPLE)
            # macOS with Homebrew GCC needs special handling
            execute_process(
                COMMAND brew --prefix
                OUTPUT_VARIABLE BREW_PREFIX
                OUTPUT_STRIP_TRAILING_WHITESPACE
                ERROR_QUIET
                RESULT_VARIABLE BREW_RESULT
            )
            
            # Fallback to common locations if brew command fails
            if(NOT BREW_RESULT EQUAL 0 OR NOT BREW_PREFIX)
                foreach(prefix "/opt/homebrew" "/usr/local")
                    if(IS_DIRECTORY "${prefix}")
                        set(BREW_PREFIX "${prefix}")
                        break()
                    endif()
                endforeach()
            endif()

            if(BREW_PREFIX)
                # Get GCC version
                execute_process(
                    COMMAND ${CMAKE_CXX_COMPILER} -dumpfullversion
                    OUTPUT_VARIABLE GCC_FULL_VERSION
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_QUIET
                )
                
                string(REGEX MATCH "^[0-9]+" GCC_VERSION "${GCC_FULL_VERSION}")
                
                # Get machine triple (e.g., aarch64-apple-darwin23)
                execute_process(
                    COMMAND ${CMAKE_CXX_COMPILER} -dumpmachine
                    OUTPUT_VARIABLE GCC_MACHINE
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_QUIET
                )

                # Build list of candidate paths
                set(GCC_INCLUDE_PATHS
                    "${BREW_PREFIX}/include/c++/${GCC_VERSION}"
                    "${BREW_PREFIX}/include/c++/${GCC_VERSION}/${GCC_MACHINE}"
                    "${BREW_PREFIX}/include/c++/${GCC_VERSION}/backward"
                    "${BREW_PREFIX}/lib/gcc/${GCC_VERSION}/include"
                    "${BREW_PREFIX}/lib/gcc/${GCC_VERSION}/include-fixed"
                )

                foreach(path IN LISTS GCC_INCLUDE_PATHS)
                    if(IS_DIRECTORY "${path}")
                        list(APPEND DETECTED_PATHS "${path}")
                    endif()
                endforeach()
            endif()
        else()
            # Linux GCC - use -v to get search paths
            execute_process(
                COMMAND ${CMAKE_CXX_COMPILER} -E -x c++ -v /dev/null
                OUTPUT_VARIABLE GCC_VERBOSE_OUTPUT
                ERROR_VARIABLE GCC_VERBOSE_OUTPUT
                RESULT_VARIABLE EXIT_CODE
                TIMEOUT 5
            )

            if(EXIT_CODE EQUAL 0)
                string(REPLACE "\n" ";" OUTPUT_LINES "${GCC_VERBOSE_OUTPUT}")
                set(IS_PARSING_INCLUDES FALSE)
                
                foreach(line ${OUTPUT_LINES})
                    if(line MATCHES "^#include <...> search starts here:")
                        set(IS_PARSING_INCLUDES TRUE)
                    elseif(line MATCHES "^End of search list.")
                        break()
                    elseif(IS_PARSING_INCLUDES)
                        string(STRIP "${line}" path)
                        if(IS_DIRECTORY "${path}")
                            list(APPEND DETECTED_PATHS "${path}")
                        endif()
                    endif()
                endforeach()
            endif()
        endif()
        
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
        # Clang-specific path detection
        message(STATUS "${ANSI_CYAN}Clangd: Detecting Clang system include paths...${ANSI_RESET}")
        
        execute_process(
            COMMAND ${CMAKE_CXX_COMPILER} -E -x c++ -v /dev/null
            OUTPUT_VARIABLE CLANG_VERBOSE_OUTPUT
            ERROR_VARIABLE CLANG_VERBOSE_OUTPUT
            RESULT_VARIABLE EXIT_CODE
            TIMEOUT 5
        )
        
        if(EXIT_CODE EQUAL 0)
            string(REPLACE "\n" ";" OUTPUT_LINES "${CLANG_VERBOSE_OUTPUT}")
            set(IS_PARSING_INCLUDES FALSE)
            
            foreach(line ${OUTPUT_LINES})
                if(line MATCHES "^#include.*search starts here:")
                    set(IS_PARSING_INCLUDES TRUE)
                elseif(line MATCHES "^End of search list.")
                    break()
                elseif(IS_PARSING_INCLUDES)
                    string(STRIP "${line}" path)
                    # Exclude framework directories on macOS
                    if(IS_DIRECTORY "${path}" AND NOT path MATCHES "\\(framework directory\\)")
                        list(APPEND DETECTED_PATHS "${path}")
                    endif()
                endif()
            endforeach()
        endif()
    endif()

    # Cache and return results
    if(DETECTED_PATHS)
        list(REMOVE_DUPLICATES DETECTED_PATHS)
        set(COMPILER_SYSTEM_INCLUDES_CACHED "${DETECTED_PATHS}" CACHE INTERNAL "Cached compiler include paths")
        set(${OUTPUT_VARIABLE} "${DETECTED_PATHS}" PARENT_SCOPE)
        
        list(LENGTH DETECTED_PATHS PATH_COUNT)
        message(STATUS "${ANSI_GREEN}Clangd: Found ${PATH_COUNT} compiler include paths${ANSI_RESET}")
        
        # Show first few paths for verification (in debug mode)
        if(CMAKE_BUILD_TYPE MATCHES "Debug")
            list(LENGTH DETECTED_PATHS NUM_PATHS)
            math(EXPR SHOW_COUNT "min(3, ${NUM_PATHS})")
            list(SUBLIST DETECTED_PATHS 0 ${SHOW_COUNT} SAMPLE_PATHS)
            foreach(path IN LISTS SAMPLE_PATHS)
                message(STATUS "  ${path}")
            endforeach()
            if(NUM_PATHS GREATER 3)
                math(EXPR REMAINING "${NUM_PATHS} - 3")
                message(STATUS "  ... and ${REMAINING} more")
            endif()
        endif()
    else()
        message(WARNING "${ANSI_YELLOW}Clangd: Could not detect compiler include paths${ANSI_RESET}")
        set(${OUTPUT_VARIABLE} "" PARENT_SCOPE)
    endif()
endfunction()

# Run the detection
detect_compiler_system_includes(COMPILER_SYSTEM_INCLUDE_PATHS)

# ----------------------- Library Definition: ads_lib ------------------------ #
#
# This is a header-only template library, defined as an INTERFACE library.
# The public headers are in include/, and template implementations in src/.
#
add_library(ads_lib INTERFACE)

target_include_directories(ads_lib INTERFACE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Specify all headers and implementation files
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

    # Template implementations are listed separately
    # They are not strictly necessary here since they're included by the headers,
    # but listing them helps IDEs and build systems track dependencies
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
# This function creates an executable with all the correct compiler flags,
# include paths, and sanitizer settings for our project.
#
function(ads_add_executable TARGET_NAME SOURCE_FILE)
    add_executable(${TARGET_NAME} ${SOURCE_FILE})
    
    target_link_libraries(${TARGET_NAME} PRIVATE ads_lib)
    
    # Common compiler flags
    target_compile_options(${TARGET_NAME} PRIVATE
        -Wall -Wextra -Wpedantic
        $<$<CONFIG:Debug>:-g -O0>
        $<$<CONFIG:Release>:-O2 -DNDEBUG>
        $<$<CONFIG:Sanitize>:-g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer>
    )
    
    # Linker flags
    target_link_options(${TARGET_NAME} PRIVATE
        $<$<CONFIG:Sanitize>:-fsanitize=address,undefined>
    )
    
    # Add detected system include paths for better clangd support
    if(COMPILER_SYSTEM_INCLUDE_PATHS)
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" AND APPLE)
            # For GCC on macOS, use nostdinc++ and add paths manually
            # This ensures clangd sees exactly what the compiler sees
            target_compile_options(${TARGET_NAME} PRIVATE -nostdinc++)
            foreach(dir IN LISTS COMPILER_SYSTEM_INCLUDE_PATHS)
                target_compile_options(${TARGET_NAME} PRIVATE "-isystem${dir}")
            endforeach()
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
            # For Clang, add as system includes
            foreach(dir IN LISTS COMPILER_SYSTEM_INCLUDE_PATHS)
                target_include_directories(${TARGET_NAME} SYSTEM PRIVATE ${dir})
            endforeach()
        endif()
    endif()
    
    # Platform-specific configurations
    if(APPLE)
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
            target_compile_options(${TARGET_NAME} PRIVATE -stdlib=libstdc++)
            target_link_options(${TARGET_NAME} PRIVATE -stdlib=libstdc++)
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
            target_compile_options(${TARGET_NAME} PRIVATE -stdlib=libc++)
            target_link_options(${TARGET_NAME} PRIVATE -stdlib=libc++)
            
            # Set RPATH for LLVM Clang
            if(CMAKE_CXX_COMPILER MATCHES "llvm" AND COMPILER_LIB_DIR AND EXISTS "${COMPILER_LIB_DIR}")
                set_target_properties(${TARGET_NAME} PROPERTIES
                    INSTALL_RPATH "${COMPILER_LIB_DIR}"
                    BUILD_WITH_INSTALL_RPATH TRUE
                )
            endif()
        endif()
    endif()
    
    message(STATUS "${ANSI_GREEN}Added executable: ${TARGET_NAME}${ANSI_RESET}")
endfunction()

# -------------------------- Main Test Executables --------------------------- #

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

option(ENABLE_TESTING "Enable unit testing with GoogleTest" OFF)

if(ENABLE_TESTING)
    enable_testing()
    
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    
    FetchContent_MakeAvailable(googletest)
    
    # Add test executable if it exists
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test_DoublyLinkedList.cpp")
        add_executable(runTests tests/Test_DoublyLinkedList.cpp)
        target_compile_options(runTests PRIVATE -Wall -Wextra -Wpedantic -g)
        target_link_libraries(runTests PRIVATE ads_lib gtest gtest_main)
        
        include(GoogleTest)
        gtest_add_tests(TARGET runTests)
        
        message(STATUS "${ANSI_GREEN}GoogleTest enabled: runTests executable created${ANSI_RESET}")
    endif()
endif()

# ----------------------------- Utility Targets ------------------------------ #

# Create symlink for compile_commands.json (for clangd)
add_custom_target(symlink_compile_commands
    COMMAND ${CMAKE_COMMAND} -E create_symlink 
            "${CMAKE_BINARY_DIR}/compile_commands.json"
            "${CMAKE_SOURCE_DIR}/compile_commands.json"
    COMMENT "${ANSI_CYAN}Creating symlink for compile_commands.json${ANSI_RESET}"
    VERBATIM
)

# 'run' target for convenience (runs the BST test by default)
if(TARGET test_bst)
    add_custom_target(run
        COMMAND $<TARGET_FILE:test_bst>
        DEPENDS test_bst
        COMMENT "${ANSI_CYAN}Running Binary Search Tree tests${ANSI_RESET}"
        USES_TERMINAL
    )
endif()

# -------------------------- Configuration Summary --------------------------- #

message(STATUS "")
message(STATUS "${ANSI_BLUE}╔═══════════════════════════════════════════════════════╗${ANSI_RESET}")
message(STATUS "${ANSI_BLUE}║${ANSI_RESET}       AlgoDataStruct - Configuration Summary          ${ANSI_BLUE}║${ANSI_RESET}")
message(STATUS "${ANSI_BLUE}╚═══════════════════════════════════════════════════════╝${ANSI_RESET}")
message(STATUS "")
message(STATUS "  ${ANSI_CYAN}Compiler${ANSI_RESET}       : ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  ${ANSI_CYAN}Compiler Path${ANSI_RESET}  : ${CMAKE_CXX_COMPILER}")
message(STATUS "  ${ANSI_CYAN}Build Type${ANSI_RESET}     : ${CMAKE_BUILD_TYPE}")
message(STATUS "  ${ANSI_CYAN}C++ Standard${ANSI_RESET}   : C++${CMAKE_CXX_STANDARD}")

if(COMPILER_SYSTEM_INCLUDE_PATHS)
    list(LENGTH COMPILER_SYSTEM_INCLUDE_PATHS PATH_COUNT)
    message(STATUS "  ${ANSI_CYAN}Clangd Support${ANSI_RESET} : ${ANSI_GREEN}✓ (${PATH_COUNT} include paths)${ANSI_RESET}")
else()
    message(STATUS "  ${ANSI_CYAN}Clangd Support${ANSI_RESET} : ${ANSI_YELLOW}⚠ No include paths detected${ANSI_RESET}")
endif()

if(ENABLE_TESTING)
    message(STATUS "  ${ANSI_CYAN}Testing${ANSI_RESET}        : ${ANSI_GREEN}Enabled (GoogleTest)${ANSI_RESET}")
else()
    message(STATUS "  ${ANSI_CYAN}Testing${ANSI_RESET}        : Disabled (use -DENABLE_TESTING=ON)")
endif()

message(STATUS "")
message(STATUS "${ANSI_BLUE}╚═══════════════════════════════════════════════════════╝${ANSI_RESET}")
message(STATUS "")

# ============================================================================ #