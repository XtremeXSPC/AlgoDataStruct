# ================================================================================= #
# ----------------------------- Apps Subproject Entry ----------------------------- #
# ================================================================================= #

# Check that the required library target from the root CMakeLists.txt is available.
if(NOT TARGET ads_lib)
    message(FATAL_ERROR "apps/ requires the ads_lib target from the root CMakeLists.txt")
endif()

add_subdirectory(snake)
