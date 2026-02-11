# ================================================================================= #
# ---------------- CMake Toolchain File for Clang (AlgoDataStruct) ---------------- #
# ================================================================================= #
#
# Description:
#   This toolchain file configures CMake to use LLVM Clang as the compiler
#   for the AlgoDataStruct project. It ensures that clangd receives the correct
#   compiler information and that the project builds with modern Clang features.
#
# Design Philosophy:
#   - Prefer LLVM Clang over AppleClang for better standards support
#   - Handle libc++ linking and RPATH correctly on macOS
#   - Provide clear error messages if Clang is not available
#   - Cache the compiler path for faster reconfiguration
#
# Usage:
#   cmake -DCMAKE_TOOLCHAIN_FILE=clang-toolchain.cmake -B build
#   cmake --build build
#
# ================================================================================= #

# Prevent duplicate execution of this toolchain file.
if(DEFINED _CLANG_TOOLCHAIN_LOADED)
    return()
endif()
set(_CLANG_TOOLCHAIN_LOADED TRUE)

message(STATUS "==> Using Clang Toolchain for AlgoDataStruct")

# ------------------------------ Platform Detection ------------------------------- #
if(APPLE)
    set(PLATFORM_NAME "macOS")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(PLATFORM_NAME "Linux")
elseif(CMAKE_SYSTEM_NAME MATCHES "BSD")
    set(PLATFORM_NAME "BSD")
else()
    set(PLATFORM_NAME "Unix")
endif()

message(STATUS "    Platform: ${PLATFORM_NAME}")

# ------------------------ Compiler Search and Validation ------------------------- #
# Check for cached Clang path from previous configuration.
if(DEFINED CACHE{CACHED_CLANG_EXECUTABLE} AND EXISTS "${CACHED_CLANG_EXECUTABLE}")
    set(CLANG_EXECUTABLE "${CACHED_CLANG_EXECUTABLE}")
    message(STATUS "    Using cached Clang: ${CLANG_EXECUTABLE}")
else()
    # Platform-specific search strategy.
    # Priority: LLVM Clang > AppleClang (on macOS only as fallback)
    if(APPLE)
        # On macOS, strongly prefer Homebrew LLVM Clang for best C++20 support.
        # AppleClang is acceptable but LLVM Clang provides more features.
        set(COMPILER_SEARCH_NAMES
            clang++                     # Generic name (usually LLVM on Homebrew)
            clang++-21 clang++-20 clang++-19 clang++-18 clang++-17 clang++-16 clang++
        )
        set(COMPILER_SEARCH_PATHS
            /opt/homebrew/opt/llvm/bin  # Homebrew LLVM (Apple Silicon) - PRIORITY
            /usr/local/opt/llvm/bin     # Homebrew LLVM (Intel Mac)
            /opt/local/bin              # MacPorts
            /usr/bin                    # System AppleClang (fallback)
        )
    elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
        # On Linux, prefer newer LLVM versions.
        set(COMPILER_SEARCH_NAMES
            clang++-21 clang++-20 clang++-19 clang++-18 clang++-17 clang++-16 clang++
        )
        set(COMPILER_SEARCH_PATHS
            /usr/bin
            /usr/local/bin
            /opt/llvm/bin
            $ENV{HOME}/.local/bin
        )
    else()
        # BSD and other Unix systems.
        set(COMPILER_SEARCH_NAMES
            clang++-21 clang++-20 clang++-19 clang++-18 clang++-17 clang++-16 clang++
        )
        set(COMPILER_SEARCH_PATHS
            /usr/local/bin
            /usr/bin
            /opt/local/bin
        )
    endif()

    # First pass: search in priority paths.
    find_program(CLANG_EXECUTABLE
        NAMES ${COMPILER_SEARCH_NAMES}
        PATHS ${COMPILER_SEARCH_PATHS}
        DOC "Path to the clang++ executable"
        NO_DEFAULT_PATH  # Only check our specified paths first
    )

    # Second pass: fallback to system PATH if not found.
    if(NOT CLANG_EXECUTABLE)
        find_program(CLANG_EXECUTABLE
            NAMES ${COMPILER_SEARCH_NAMES}
            DOC "Path to the clang++ executable"
        )
    endif()

    # Cache the result for faster reconfiguration.
    if(CLANG_EXECUTABLE)
        set(CACHED_CLANG_EXECUTABLE "${CLANG_EXECUTABLE}" CACHE INTERNAL "Cached Clang path")
        message(STATUS "    Found Clang: ${CLANG_EXECUTABLE}")
    endif()
endif()

# --------------------------- Compiler Not Found Error ---------------------------- #
if(NOT CLANG_EXECUTABLE)
    message(FATAL_ERROR
        "\n"
        "═══════════════════ CLANG COMPILER NOT FOUND ════════════════════\n"
        "\n"
        "This toolchain file requires Clang to be installed.\n"
        "\n"
        "Installation instructions for ${PLATFORM_NAME}:\n")

    if(APPLE)
        message(FATAL_ERROR
            "  macOS:\n"
            "    Homebrew (recommended for LLVM Clang):\n"
            "      brew install llvm\n"
            "\n"
            "    Xcode Command Line Tools (AppleClang):\n"
            "      xcode-select --install\n"
            "\n"
            "After installation:\n"
            "  cmake -DCMAKE_TOOLCHAIN_FILE=clang-toolchain.cmake -B build\n"
            "═════════════════════════════════════════════════════════════════\n")
    elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
        message(FATAL_ERROR
            "  Linux:\n"
            "    Debian/Ubuntu:\n"
            "      sudo apt update && sudo apt install clang\n"
            "\n"
            "    Fedora/RHEL:\n"
            "      sudo dnf install clang\n"
            "\n"
            "    Arch Linux:\n"
            "      sudo pacman -S clang\n"
            "\n"
            "After installation:\n"
            "  cmake -DCMAKE_TOOLCHAIN_FILE=clang-toolchain.cmake -B build\n"
            "═════════════════════════════════════════════════════════════════\n")
    else()
        message(FATAL_ERROR
            "  Please install Clang using your system's package manager.\n"
            "\n"
            "After installation:\n"
            "  cmake -DCMAKE_TOOLCHAIN_FILE=clang-toolchain.cmake -B build\n"
            "═════════════════════════════════════════════════════════════════\n")
    endif()
endif()

# ------------------------- Compiler Version Verification ------------------------- #
# Query the compiler to verify it's actually Clang and get version info.
execute_process(
    COMMAND ${CLANG_EXECUTABLE} --version
    OUTPUT_VARIABLE CLANG_VERSION_OUTPUT
    ERROR_VARIABLE CLANG_VERSION_ERROR
    RESULT_VARIABLE CLANG_VERSION_RESULT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    TIMEOUT 5
)

if(NOT CLANG_VERSION_RESULT EQUAL 0)
    message(FATAL_ERROR
        "Failed to execute ${CLANG_EXECUTABLE} --version\n"
        "Error: ${CLANG_VERSION_ERROR}")
endif()

# Extract version and determine if it's LLVM Clang or AppleClang.
set(IS_APPLE_CLANG FALSE)
set(IS_LLVM_CLANG FALSE)

if(CLANG_VERSION_OUTPUT MATCHES "Apple clang version ([0-9]+\\.[0-9]+)")
    set(CLANG_VERSION "${CMAKE_MATCH_1}")
    set(IS_APPLE_CLANG TRUE)
    message(STATUS "    Compiler: Apple Clang ${CLANG_VERSION}")
    message(STATUS "    Note: For best C++20 support, consider using LLVM Clang (brew install llvm)")
elseif(CLANG_VERSION_OUTPUT MATCHES "clang version ([0-9]+\\.[0-9]+)")
    set(CLANG_VERSION "${CMAKE_MATCH_1}")
    set(IS_LLVM_CLANG TRUE)
    message(STATUS "    Compiler: LLVM Clang ${CLANG_VERSION}")
else()
    message(WARNING "Could not determine Clang version from output")
    set(CLANG_VERSION "unknown")
    # Assume LLVM Clang if we can't determine
    set(IS_LLVM_CLANG TRUE)
endif()

# --------------------------- Find Matching C Compiler ---------------------------- #
# Find the corresponding clang (C compiler) to match clang++ (C++).
get_filename_component(CLANG_DIR ${CLANG_EXECUTABLE} DIRECTORY)
get_filename_component(CLANG_NAME ${CLANG_EXECUTABLE} NAME)

# Derive C compiler name from C++ compiler name.
string(REPLACE "clang++" "clang" C_COMPILER_NAME ${CLANG_NAME})
string(REPLACE "++" "" C_COMPILER_NAME_ALT ${CLANG_NAME})

# Try to find clang in the same directory as clang++.
find_program(C_COMPILER_PATH
    NAMES ${C_COMPILER_NAME} ${C_COMPILER_NAME_ALT}
    HINTS ${CLANG_DIR}
    NO_DEFAULT_PATH
)

# If not found in same directory, try broader search.
if(NOT C_COMPILER_PATH)
    set(C_SEARCH_NAMES clang-19 clang-18 clang-17 clang-16 clang-15 clang)
    find_program(C_COMPILER_PATH
        NAMES ${C_SEARCH_NAMES}
        PATHS ${COMPILER_SEARCH_PATHS}
    )
endif()

# Fallback: use clang++ for both if no matching C compiler found.
if(NOT C_COMPILER_PATH)
    message(WARNING
        "Could not find matching Clang C compiler.\n"
        "    Using ${CLANG_EXECUTABLE} for both C and C++.")
    set(C_COMPILER_PATH ${CLANG_EXECUTABLE})
else()
    message(STATUS "    C Compiler: ${C_COMPILER_PATH}")
endif()

# ------------------------- Set CMake Compiler Variables -------------------------- #
# Force CMake to use the found compilers.
set(CMAKE_C_COMPILER   ${C_COMPILER_PATH}  CACHE PATH "C compiler"   FORCE)
set(CMAKE_CXX_COMPILER ${CLANG_EXECUTABLE} CACHE PATH "C++ compiler" FORCE)

# ---------------------- macOS-Specific libc++ Configuration ---------------------- #
# On macOS with LLVM Clang, we need to explicitly configure libc++ paths
# and RPATH to avoid runtime linking issues.
if(APPLE AND IS_LLVM_CLANG)
    # Get LLVM installation root directory.
    get_filename_component(LLVM_BIN_DIR ${CLANG_EXECUTABLE} DIRECTORY)
    get_filename_component(LLVM_ROOT ${LLVM_BIN_DIR} DIRECTORY)

    # Set paths for LLVM's libc++.
    set(LLVM_LIBCXX_INCLUDE "${LLVM_ROOT}/include/c++/v1")
    set(LLVM_LIBCXX_LIB "${LLVM_ROOT}/lib/c++")

    # Verify the paths exist.
    if(EXISTS "${LLVM_LIBCXX_INCLUDE}" AND EXISTS "${LLVM_LIBCXX_LIB}")
        # Configure compiler to use LLVM's libc++ instead of system's.
        # Use -nostdinc++ to prevent finding system headers.
        set(CMAKE_CXX_FLAGS "-nostdinc++ -I${LLVM_LIBCXX_INCLUDE}" CACHE STRING "" FORCE)

        # Configure linker to use LLVM's libc++ library.
        set(CMAKE_EXE_LINKER_FLAGS
            "-L${LLVM_LIBCXX_LIB}"
            CACHE STRING "" FORCE)

        # Tell CMake about the RPATH for installation.
        set(CMAKE_INSTALL_RPATH "${LLVM_LIBCXX_LIB}" CACHE STRING "" FORCE)
        set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE CACHE BOOL "" FORCE)
        set(CMAKE_SKIP_BUILD_RPATH FALSE CACHE BOOL "" FORCE)

        # Use libc++ explicitly (Clang default on macOS, but be explicit).
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++" CACHE STRING "" FORCE)

        message(STATUS "    libc++ configured:")
        message(STATUS "      Include: ${LLVM_LIBCXX_INCLUDE}")
        message(STATUS "      Library: ${LLVM_LIBCXX_LIB}")
        message(STATUS "      RPATH: ${LLVM_LIBCXX_LIB}")
    else()
        message(WARNING
            "LLVM Clang found but libc++ paths don't exist:\n"
            "    Expected include: ${LLVM_LIBCXX_INCLUDE}\n"
            "    Expected library: ${LLVM_LIBCXX_LIB}\n"
            "    Falling back to system libc++ (may cause issues)")
    endif()
elseif(APPLE AND IS_APPLE_CLANG)
    # AppleClang uses system libc++, which is fine.
    # Just ensure we're explicit about using libc++.
    set(CMAKE_CXX_FLAGS "-stdlib=libc++" CACHE STRING "" FORCE)
    message(STATUS "    Using system libc++ (AppleClang default)")
endif()

# -------------------------- C++ Standard Configuration --------------------------- #
# AlgoDataStruct uses C++20 for good balance of features and portability.
set(CMAKE_CXX_STANDARD 20 CACHE STRING "C++ standard" FORCE)
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE BOOL "Require C++ standard" FORCE)
set(CMAKE_CXX_EXTENSIONS OFF CACHE BOOL "Disable compiler extensions" FORCE)

# Add -std=c++20 explicitly to ensure it's in compile_commands.json for clangd.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20" CACHE STRING "" FORCE)

message(STATUS "    C++ Standard: C++20")

# ---------------------------- Success Summary Message ---------------------------- #
message(STATUS "")
message(STATUS "╔═══════════════════════════════════════════════════════════════╗")
message(STATUS "║ ---------- Clang Toolchain Successfully Configured ---------- ║")
message(STATUS "╚═══════════════════════════════════════════════════════════════╝")
message(STATUS "  Platform       : ${PLATFORM_NAME}")
if(IS_LLVM_CLANG)
    message(STATUS "  Compiler Type  : LLVM Clang")
else()
    message(STATUS "  Compiler Type  : Apple Clang")
endif()
message(STATUS "  Clang Version  : ${CLANG_VERSION}")
message(STATUS "  C++ Compiler   : ${CMAKE_CXX_COMPILER}")
message(STATUS "  C Compiler     : ${CMAKE_C_COMPILER}")
message(STATUS "  C++ Standard   : C++20")
if(APPLE AND IS_LLVM_CLANG AND EXISTS "${LLVM_LIBCXX_LIB}")
    message(STATUS "  libc++ RPATH   : ${LLVM_LIBCXX_LIB}")
endif()
message(STATUS "═════════════════════════════════════════════════════════════════")
message(STATUS "")

# ================================================================================= #
# End of Clang Toolchain File.
