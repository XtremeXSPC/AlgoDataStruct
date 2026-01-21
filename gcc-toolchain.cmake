# ================================================================================= #
# ----------------- CMake Toolchain File for GCC (AlgoDataStruct) ----------------- #
# ================================================================================= #
#
# Description:
#   This toolchain file configures CMake to use GCC as the compiler for the
#   AlgoDataStruct project. It serves as a robust fallback when Clang is not
#   available or when GCC-specific features are desired.
#
# Design Philosophy:
#   - Find the newest available GCC version for best C++20 support
#   - Handle GCC's unique characteristics on different platforms
#   - Detect and avoid Clang masquerading as "g++" (common on macOS)
#   - Provide clear guidance if GCC is not available
#
# Usage:
#   cmake -DCMAKE_TOOLCHAIN_FILE=gcc-toolchain.cmake -B build
#   cmake --build build
#
# ================================================================================= #

# Prevent duplicate execution of this toolchain file.
if(DEFINED _GCC_TOOLCHAIN_LOADED)
    return()
endif()
set(_GCC_TOOLCHAIN_LOADED TRUE)

message(STATUS "==> Using GCC Toolchain for AlgoDataStruct")

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
# Check for cached GCC path from previous configuration.
if(DEFINED CACHE{CACHED_GCC_EXECUTABLE} AND EXISTS "${CACHED_GCC_EXECUTABLE}")
    set(GCC_EXECUTABLE "${CACHED_GCC_EXECUTABLE}")
    message(STATUS "    Using cached GCC: ${GCC_EXECUTABLE}")
else()
    # Platform-specific search strategy.
    # Priority: Newer versions first for better C++20 support.
    if(APPLE)
        # macOS with Homebrew - GCC is not the system default.
        # Prefer newer versions and look in Homebrew paths.
        set(COMPILER_SEARCH_NAMES
            g++-15 g++-14 g++-13 g++-12 g++-11 g++-10 g++
        )
        set(COMPILER_SEARCH_PATHS
            /opt/homebrew/bin           # Apple Silicon Homebrew
            /usr/local/bin              # Intel Mac Homebrew
            /opt/local/bin              # MacPorts
            /sw/bin                     # Fink
        )
        # Note: Deliberately NOT including /usr/bin first on macOS because
        # "g++" there is often just an alias to Clang.
    elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
        # Linux - prefer newer versions, but check unversioned too.
        set(COMPILER_SEARCH_NAMES
            g++-15 g++-14 g++-13 g++-12 g++-11 g++-10 g++
        )

        # Detect Linux distribution for better diagnostics later.
        if(EXISTS "/etc/os-release")
            file(READ "/etc/os-release" OS_RELEASE)
            if(OS_RELEASE MATCHES "ID=ubuntu" OR OS_RELEASE MATCHES "ID=debian")
                set(LINUX_DISTRO "Debian-based")
            elseif(OS_RELEASE MATCHES "ID=fedora" OR OS_RELEASE MATCHES "ID=rhel")
                set(LINUX_DISTRO "RedHat-based")
            elseif(OS_RELEASE MATCHES "ID=arch")
                set(LINUX_DISTRO "Arch-based")
            else()
                set(LINUX_DISTRO "Generic Linux")
            endif()
        else()
            set(LINUX_DISTRO "Unknown Linux")
        endif()

        set(COMPILER_SEARCH_PATHS
            /usr/bin
            /usr/local/bin
            /opt/gcc/bin
            $ENV{HOME}/.local/bin
        )
    else()
        # BSD and other Unix systems.
        set(COMPILER_SEARCH_NAMES
            g++15 g++14 g++13 g++12 g++11 g++10 g++
        )
        set(COMPILER_SEARCH_PATHS
            /usr/local/bin
            /usr/bin
            /opt/local/bin
        )
    endif()

    # First pass: search in priority paths.
    find_program(GCC_EXECUTABLE
        NAMES ${COMPILER_SEARCH_NAMES}
        PATHS ${COMPILER_SEARCH_PATHS}
        DOC "Path to the g++ executable"
        NO_DEFAULT_PATH  # Check only our specified paths first
    )

    # Second pass: fallback to system PATH if not found.
    if(NOT GCC_EXECUTABLE)
        find_program(GCC_EXECUTABLE
            NAMES ${COMPILER_SEARCH_NAMES}
            DOC "Path to the g++ executable"
        )
    endif()

    # Cache the result for faster reconfiguration.
    if(GCC_EXECUTABLE)
        set(CACHED_GCC_EXECUTABLE "${GCC_EXECUTABLE}" CACHE INTERNAL "Cached GCC path")
        message(STATUS "    Found candidate: ${GCC_EXECUTABLE}")
    endif()
endif()

# --------------------------- Compiler Not Found Error ---------------------------- #
if(NOT GCC_EXECUTABLE)
    message(FATAL_ERROR
        "\n"
        "╔═══════════════════════════════════════════════════════════════╗\n"
        "║                     GCC COMPILER NOT FOUND                    ║\n"
        "╚═══════════════════════════════════════════════════════════════╝\n"
        "\n"
        "This toolchain file requires GCC to be installed.\n"
        "\n"
        "Installation instructions for ${PLATFORM_NAME}:\n")

    if(APPLE)
        message(FATAL_ERROR
            "  macOS (Homebrew):\n"
            "    brew install gcc\n"
            "\n"
            "  macOS (MacPorts):\n"
            "    sudo port install gcc13\n"
            "\n"
            "  Note: macOS system 'g++' is usually Clang in disguise.\n"
            "        Use the clang-toolchain.cmake if you want Clang.\n"
            "\n"
            "After installation:\n"
            "  cmake -DCMAKE_TOOLCHAIN_FILE=gcc-toolchain.cmake -B build\n"
            "╚═══════════════════════════════════════════════════════════════╝\n")
    elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
        set(INSTALL_CMD "sudo apt install g++")
        if(LINUX_DISTRO MATCHES "RedHat")
            set(INSTALL_CMD "sudo dnf install gcc-c++")
        elseif(LINUX_DISTRO MATCHES "Arch")
            set(INSTALL_CMD "sudo pacman -S gcc")
        endif()

        message(FATAL_ERROR
            "  ${LINUX_DISTRO}:\n"
            "    ${INSTALL_CMD}\n"
            "\n"
            "After installation:\n"
            "  cmake -DCMAKE_TOOLCHAIN_FILE=gcc-toolchain.cmake -B build\n"
            "╚═══════════════════════════════════════════════════════════════╝\n")
    else()
        message(FATAL_ERROR
            "  Please install GCC using your system's package manager.\n"
            "\n"
            "After installation:\n"
            "  cmake -DCMAKE_TOOLCHAIN_FILE=gcc-toolchain.cmake -B build\n"
            "╚═══════════════════════════════════════════════════════════════╝\n")
    endif()
endif()

# ------------------------- Compiler Version Verification ------------------------- #
# Query the compiler to verify it's actually GCC and get version info.
execute_process(
    COMMAND ${GCC_EXECUTABLE} --version
    OUTPUT_VARIABLE GCC_VERSION_OUTPUT
    ERROR_VARIABLE GCC_VERSION_ERROR
    RESULT_VARIABLE GCC_VERSION_RESULT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    TIMEOUT 5
)

if(NOT GCC_VERSION_RESULT EQUAL 0)
    message(FATAL_ERROR
        "Failed to execute ${GCC_EXECUTABLE} --version\n"
        "Error: ${GCC_VERSION_ERROR}")
endif()

# ----------------------- Detect Clang Masquerading as GCC ------------------------ #
# On macOS, /usr/bin/g++ is often just Clang pretending to be GCC.
# We need to detect this and find the real GCC instead.
if(GCC_VERSION_OUTPUT MATCHES "clang" OR GCC_VERSION_OUTPUT MATCHES "Apple")
    message(WARNING
        "\n"
        "╔═══════════════════════════════════════════════════════════════╗\n"
        "║              WARNING: CLANG DETECTED AS 'g++'                 ║\n"
        "╚═══════════════════════════════════════════════════════════════╝\n"
        "\n"
        "The executable at ${GCC_EXECUTABLE} is actually Clang,\n"
        "not GCC (common on macOS where 'g++' is aliased to Clang).\n"
        "\n"
        "Searching for real GCC installation...")

    # Remove the path where we found fake GCC and search again.
    get_filename_component(FAKE_GCC_DIR ${GCC_EXECUTABLE} DIRECTORY)
    list(REMOVE_ITEM COMPILER_SEARCH_PATHS "${FAKE_GCC_DIR}")

    # Try to find real GCC, excluding the fake one's location.
    find_program(REAL_GCC_EXECUTABLE
        NAMES ${COMPILER_SEARCH_NAMES}
        PATHS ${COMPILER_SEARCH_PATHS}
        DOC "Path to the real g++ executable"
        NO_DEFAULT_PATH
    )

    if(REAL_GCC_EXECUTABLE)
        set(GCC_EXECUTABLE ${REAL_GCC_EXECUTABLE})
        set(CACHED_GCC_EXECUTABLE "${GCC_EXECUTABLE}" CACHE INTERNAL "Cached GCC path" FORCE)
        message(STATUS "    Found real GCC: ${GCC_EXECUTABLE}")

        # Re-verify version with the real GCC.
        execute_process(
            COMMAND ${GCC_EXECUTABLE} --version
            OUTPUT_VARIABLE GCC_VERSION_OUTPUT
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
    else()
        message(FATAL_ERROR
            "\n"
            "Could not find real GCC installation.\n"
            "\n"
            "Please install GCC using the instructions above, or use\n"
            "the clang-toolchain.cmake file if you want to use Clang.\n"
            "╚═══════════════════════════════════════════════════════════════╝\n")
    endif()
endif()

# ------------------------- Extract and Validate Version -------------------------- #
# Parse GCC version from output.
# Try multiple patterns for robustness across different GCC installations.
if(GCC_VERSION_OUTPUT MATCHES "gcc.*([0-9]+\\.[0-9]+\\.[0-9]+)")
    string(REGEX MATCH "([0-9]+\\.[0-9]+\\.[0-9]+)" GCC_VERSION "${GCC_VERSION_OUTPUT}")
    string(REGEX MATCH "^([0-9]+)" GCC_MAJOR_VERSION "${GCC_VERSION}")
elseif(GCC_VERSION_OUTPUT MATCHES "\\(GCC\\) ([0-9]+\\.[0-9]+)")
    string(REGEX MATCH "([0-9]+\\.[0-9]+)" GCC_VERSION "${GCC_VERSION_OUTPUT}")
    string(REGEX MATCH "^([0-9]+)" GCC_MAJOR_VERSION "${GCC_VERSION}")
else()
    # Fallback: use -dumpversion.
    execute_process(
        COMMAND ${GCC_EXECUTABLE} -dumpversion
        OUTPUT_VARIABLE GCC_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    string(REGEX MATCH "^([0-9]+)" GCC_MAJOR_VERSION "${GCC_VERSION}")
endif()

# Check if we have sufficient GCC version for C++20.
if(GCC_MAJOR_VERSION)
    if(GCC_MAJOR_VERSION LESS 10)
        message(WARNING
            "GCC version ${GCC_VERSION} has limited C++20 support.\n"
            "    GCC 10+ is recommended for full C++20 features.\n"
            "    Consider upgrading or using clang-toolchain.cmake.")
    elseif(GCC_MAJOR_VERSION LESS 11)
        message(STATUS "    GCC ${GCC_VERSION}: Good C++20 support")
    else()
        message(STATUS "    GCC ${GCC_VERSION}: Excellent C++20 support")
    endif()
else()
    message(WARNING "Could not determine GCC version")
    set(GCC_VERSION "unknown")
endif()

# --------------------------- Find Matching C Compiler ---------------------------- #
# Find the corresponding gcc (C compiler) to match g++ (C++).
get_filename_component(GCC_DIR ${GCC_EXECUTABLE} DIRECTORY)
get_filename_component(GCC_NAME ${GCC_EXECUTABLE} NAME)

# Derive C compiler name from C++ compiler name.
string(REPLACE "g++" "gcc" C_COMPILER_NAME ${GCC_NAME})
string(REPLACE "c++" "cc" C_COMPILER_NAME_ALT ${GCC_NAME})

# Try to find gcc in the same directory as g++.
find_program(C_COMPILER_PATH
    NAMES ${C_COMPILER_NAME} ${C_COMPILER_NAME_ALT}
    HINTS ${GCC_DIR}
    NO_DEFAULT_PATH
)

# If not found in same directory, try broader search.
if(NOT C_COMPILER_PATH)
    # Create list of C compiler candidates based on C++ names.
    set(C_SEARCH_NAMES)
    foreach(cpp_name IN LISTS COMPILER_SEARCH_NAMES)
        string(REPLACE "g++" "gcc" c_name ${cpp_name})
        string(REPLACE "++" "" c_name2 ${cpp_name})
        list(APPEND C_SEARCH_NAMES ${c_name} ${c_name2})
    endforeach()
    list(REMOVE_DUPLICATES C_SEARCH_NAMES)

    find_program(C_COMPILER_PATH
        NAMES ${C_SEARCH_NAMES}
        PATHS ${COMPILER_SEARCH_PATHS}
    )
endif()

# Fallback: use g++ for both if no matching C compiler found.
if(NOT C_COMPILER_PATH)
    message(WARNING
        "Could not find matching GCC C compiler.\n"
        "    Using ${GCC_EXECUTABLE} for both C and C++.")
    set(C_COMPILER_PATH ${GCC_EXECUTABLE})
else()
    # Verify version compatibility between C and C++ compilers.
    execute_process(
        COMMAND ${C_COMPILER_PATH} -dumpversion
        OUTPUT_VARIABLE C_COMPILER_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

    string(REGEX MATCH "^([0-9]+)" C_MAJOR_VERSION "${C_COMPILER_VERSION}")

    if(C_MAJOR_VERSION AND GCC_MAJOR_VERSION AND
       NOT C_MAJOR_VERSION STREQUAL GCC_MAJOR_VERSION)
        message(WARNING
            "C compiler version (${C_COMPILER_VERSION}) differs from\n"
            "    C++ compiler version (${GCC_VERSION}).\n"
            "    This may cause linking issues.")
    else()
        message(STATUS "    C Compiler: ${C_COMPILER_PATH}")
    endif()
endif()

# ------------------------- Set CMake Compiler Variables -------------------------- #
# Force CMake to use the found compilers.
set(CMAKE_C_COMPILER   ${C_COMPILER_PATH} CACHE PATH "C compiler"   FORCE)
set(CMAKE_CXX_COMPILER ${GCC_EXECUTABLE}  CACHE PATH "C++ compiler" FORCE)

# Set compiler IDs explicitly (helps CMake understand what we're using).
set(CMAKE_C_COMPILER_ID "GNU" CACHE STRING "C compiler ID" FORCE)
set(CMAKE_CXX_COMPILER_ID "GNU" CACHE STRING "C++ compiler ID" FORCE)

# -------------------- macOS-Specific libstdc++ Configuration --------------------- #
# On macOS, GCC uses libstdc++ instead of libc++.
# We need to be explicit about this.
if(APPLE)
    set(CMAKE_CXX_FLAGS "-stdlib=libstdc++" CACHE STRING "" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS "-stdlib=libstdc++" CACHE STRING "" FORCE)
    message(STATUS "    Using libstdc++ (GCC's standard library)")
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
message(STATUS "║             GCC Toolchain Successfully Configured             ║")
message(STATUS "╚═══════════════════════════════════════════════════════════════╝")
message(STATUS "  Platform       : ${PLATFORM_NAME}")
message(STATUS "  Compiler Type  : GNU GCC")
if(GCC_VERSION AND NOT GCC_VERSION STREQUAL "unknown")
    message(STATUS "  GCC Version    : ${GCC_VERSION}")
endif()
message(STATUS "  C++ Compiler   : ${CMAKE_CXX_COMPILER}")
message(STATUS "  C Compiler     : ${CMAKE_C_COMPILER}")
message(STATUS "  C++ Standard   : C++20")
if(APPLE)
    message(STATUS "  Std Library    : libstdc++")
endif()
message(STATUS "╚═══════════════════════════════════════════════════════════════╝")
message(STATUS "")

# ================================================================================= #
# End of GCC Toolchain File.
