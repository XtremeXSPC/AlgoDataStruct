# ================================================================================= #
# ----------------------------- Shared Build Options ------------------------------ #
# ================================================================================= #
#
# INTERFACE target with the compiler/linker flags shared by every first-party
# target: warnings, per-config code generation and sanitizer wiring in one place.
#   Usage: target_link_libraries(my_target PRIVATE ads_build_options)
#
# Hardening macros (_GLIBCXX_DEBUG, _LIBCPP_HARDENING_MODE) are NOT here: they
# change the libstdc++/libc++ ABI, so they go only on targets that don't link a
# separately-compiled library like GoogleTest (see ads_add_executable).
#
# ================================================================================= #

add_library(ads_build_options INTERFACE)

target_compile_options(ads_build_options INTERFACE
    # Warnings (all compilers).
    -Wall -Wextra -Wpedantic -Wshadow

    # Per-config code generation. Sanitize/ThreadSanitize are custom build types,
    # so their flags are explicit; use ';' so each flag is a separate argument.
    $<$<CONFIG:Debug>:-g;-O0>
    $<$<CONFIG:Release>:-O3;-DNDEBUG>
    $<$<CONFIG:Sanitize>:-g;-O1;-fsanitize=address,undefined;-fno-omit-frame-pointer>
    $<$<CONFIG:ThreadSanitize>:-g;-O1;-fsanitize=thread;-fno-omit-frame-pointer>
)

target_link_options(ads_build_options INTERFACE
    $<$<CONFIG:Sanitize>:-fsanitize=address,undefined>
    $<$<CONFIG:ThreadSanitize>:-fsanitize=thread>
    $<$<CONFIG:Release>:-s>  # Strip optimized Release binaries.
)

message(STATUS "${ANSI_GREEN}Shared build options target 'ads_build_options' defined${ANSI_RESET}")
