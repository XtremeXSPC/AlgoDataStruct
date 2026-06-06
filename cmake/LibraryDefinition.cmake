# ================================================================================= #
# -------------------------- Library Definition: ads_lib -------------------------- #
# ================================================================================= #
#
# This is a header-only template library. We define it as an INTERFACE library
# because there are no compiled sources - everything is in headers and .tpp files.
# An INTERFACE library in CMake is used to specify properties that should be
# propagated to targets that link against it.
#
# Usage:
#   include(cmake/LibraryDefinition.cmake)
#   target_link_libraries(my_target PRIVATE ads_lib)
#
# ================================================================================= #

add_library(ads_lib INTERFACE)

# Specify where the public headers are located.
# Users of this library (our test executables) will automatically get this path
# added to their include directories.
target_include_directories(ads_lib INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Discover headers and template implementation files automatically so the build
# metadata stays aligned with the repository as new structures are added.
file(GLOB_RECURSE ADS_PUBLIC_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/ads/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/support/*.hpp"
)
file(GLOB_RECURSE ADS_TEMPLATE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ads/*.tpp"
)

list(SORT ADS_PUBLIC_HEADERS)
list(SORT ADS_TEMPLATE_SOURCES)

# Headers and .tpp bodies in one header file set: members are treated as headers
# (never compiled), so the .tpp files appear in IDEs without becoming TUs.
target_sources(ads_lib
    INTERFACE
        FILE_SET HEADERS
        BASE_DIRS
            "${CMAKE_CURRENT_SOURCE_DIR}/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/src"
        FILES
            ${ADS_PUBLIC_HEADERS}
            ${ADS_TEMPLATE_SOURCES}
)

# ------------------------------ Precompiled Headers ------------------------------ #
# Apply precompiled headers to reduce compilation times for STL includes.
# The PCH includes the most frequently used standard library headers across
# the project. This provides significant speedup for incremental builds.
#
# To disable PCH: cmake -DDISABLE_PCH=ON ...

if(NOT DISABLE_PCH)
    # Use BUILD_INTERFACE generator expression to ensure the PCH path is only
    # used during build, not in exported configurations.
    target_precompile_headers(ads_lib INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/ads_pch.hpp>"
    )

    message(STATUS "${ANSI_GREEN}Precompiled headers enabled for ads_lib${ANSI_RESET}")
else()
    message(STATUS "${ANSI_YELLOW}Precompiled headers disabled (DISABLE_PCH=ON)${ANSI_RESET}")
endif()
