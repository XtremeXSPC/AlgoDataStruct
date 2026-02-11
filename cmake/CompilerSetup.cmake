# ================================================================================= #
# ---------------------- Compiler Setup and Helper Functions ---------------------- #
# ================================================================================= #
#
# This module provides:
#   - Automatic detection of compiler system include paths for clangd support
#   - ads_add_executable(): Helper function for creating test executables with
#     consistent compiler flags, warnings, and configuration
#
# Usage:
#   include(cmake/CompilerSetup.cmake)
#   ads_add_executable(my_test_target src/my_test.cc)
#
# ================================================================================= #

# ----------------------- Simplified Include Path Detection ----------------------- #
# With toolchain files handling compiler selection, we only need basic include path
# detection for clangd support.
function(detect_compiler_system_includes OUTPUT_VARIABLE)
    # Include-path cache must be tied to the active compiler to avoid stale paths
    # when reconfiguring the same build directory with a different toolchain.
    set(CURRENT_COMPILER_CACHE_KEY
        "${CMAKE_CXX_COMPILER}|${CMAKE_CXX_COMPILER_ID}|${CMAKE_CXX_COMPILER_VERSION}"
    )

    # Check if we've already detected paths in this configuration.
    if(DEFINED CACHE{COMPILER_SYSTEM_INCLUDES_CACHED}
       AND DEFINED CACHE{COMPILER_SYSTEM_INCLUDES_CACHE_KEY}
       AND COMPILER_SYSTEM_INCLUDES_CACHE_KEY STREQUAL CURRENT_COMPILER_CACHE_KEY)
        set(${OUTPUT_VARIABLE} "${COMPILER_SYSTEM_INCLUDES_CACHED}" PARENT_SCOPE)
        message(STATUS "${ANSI_CYAN}Using cached compiler include paths${ANSI_RESET}")
        return()
    endif()

    set(DETECTED_PATHS "")

    # Query the compiler for its include search paths.
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -E -x c++ -v /dev/null
        OUTPUT_VARIABLE COMPILER_OUTPUT
        ERROR_VARIABLE COMPILER_OUTPUT  # Compilers often print to stderr.
        RESULT_VARIABLE EXIT_CODE
        TIMEOUT 5
    )

    if(EXIT_CODE EQUAL 0)
        # Parse the compiler output to extract include paths.
        string(REPLACE "\n" ";" OUTPUT_LINES "${COMPILER_OUTPUT}")
        set(IS_PARSING_INCLUDES FALSE)

        foreach(line ${OUTPUT_LINES})
            # Start of include path section.
            if(line MATCHES "^#include.*search starts here:")
                set(IS_PARSING_INCLUDES TRUE)
            # End of include path section.
            elseif(line MATCHES "^End of search list.")
                break()
            # Parse actual include paths.
            elseif(IS_PARSING_INCLUDES)
                string(STRIP "${line}" path)
                # Exclude framework directories on macOS (they're not regular headers).
                if(IS_DIRECTORY "${path}" AND NOT path MATCHES "\\(framework directory\\)")
                    list(APPEND DETECTED_PATHS "${path}")
                endif()
            endif()
        endforeach()
    endif()

    # Cache results for faster reconfiguration.
    if(DETECTED_PATHS)
        list(REMOVE_DUPLICATES DETECTED_PATHS)
        set(COMPILER_SYSTEM_INCLUDES_CACHED "${DETECTED_PATHS}" CACHE INTERNAL
            "Cached compiler include paths")
        set(COMPILER_SYSTEM_INCLUDES_CACHE_KEY "${CURRENT_COMPILER_CACHE_KEY}" CACHE INTERNAL
            "Compiler identity for cached compiler include paths")
        set(${OUTPUT_VARIABLE} "${DETECTED_PATHS}" PARENT_SCOPE)

        list(LENGTH DETECTED_PATHS PATH_COUNT)
        message(STATUS "${ANSI_GREEN}Detected ${PATH_COUNT} compiler include paths${ANSI_RESET}")
    else()
        message(STATUS "${ANSI_YELLOW}Could not auto-detect include paths (may still work fine)${ANSI_RESET}")
        set(${OUTPUT_VARIABLE} "" PARENT_SCOPE)
    endif()
endfunction()

# ------------------------ Helper Function: Add Executable ------------------------ #
#
# This function encapsulates all the logic for creating a test executable.
# It ensures consistent compiler flags, warning levels, and configuration
# across all test programs.
#
function(ads_add_executable TARGET_NAME SOURCE_FILE)
    # Create the executable target.
    add_executable(${TARGET_NAME} ${SOURCE_FILE})

    # Link with our header-only library.
    # This automatically adds the include directory and all necessary properties.
    target_link_libraries(${TARGET_NAME} PRIVATE ads_lib)

    # --------------------- Compiler Flags and Warnings ---------------------- #
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

        # Debug configuration: No optimization, full debug info.
        $<$<CONFIG:Debug>:-g -O0>

        # Release configuration: Maximum optimization, no debug info.
        $<$<CONFIG:Release>:-O3 -DNDEBUG>

        # Address/UB sanitizers configuration.
        $<$<CONFIG:Sanitize>:-g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer>

        # Thread sanitizer configuration.
        $<$<CONFIG:ThreadSanitize>:-g -O1 -fsanitize=thread -fno-omit-frame-pointer>
    )

    # Compiler-specific definitions for enhanced debugging.
    target_compile_definitions(${TARGET_NAME} PRIVATE
        # For GCC in Debug mode, enable STL debug mode.
        # This adds runtime checks for iterators and containers.
        $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GNU>>:_GLIBCXX_DEBUG>

        # For Clang/libc++, enable extensive hardening checks.
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
    # Configuration-specific linker flags.
    target_link_options(${TARGET_NAME} PRIVATE
        # Link Address/UB sanitizer runtime libraries.
        $<$<CONFIG:Sanitize>:-fsanitize=address,undefined>

        # Link thread sanitizer runtime library.
        $<$<CONFIG:ThreadSanitize>:-fsanitize=thread>

        # Strip symbols in Release builds to reduce executable size.
        $<$<CONFIG:Release>:-s>
    )

    # Print a status message when the target is added.
    message(STATUS "${ANSI_GREEN}Added test executable: ${TARGET_NAME}${ANSI_RESET}")
endfunction()
