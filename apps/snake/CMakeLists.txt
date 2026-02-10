# ================================================================================= #
# -------------------------------- Snake Showcase --------------------------------- #
# ================================================================================= #
#
# Project: Snake Game - Data Structures Performance Showcase
#
# Description:
#   A simple Snake game implementation that demonstrates the robustness and
#   performance of the custom data structure library (ads_lib). This application
#   serves as a practical stress test for the underlying containers and algorithms.
#
# Components:
#   - snake_engine: Core game logic library.
#   - app_Snake_TUI: Terminal-based interactive game.
#   - app_Snake_Stress: Performance and stress testing executable.
#   - runSnake: Custom target for quick launch.
#   - runSnakeTests: GoogleTest suite for engine validation.
#
# ================================================================================= #

# Build option to enable or disable the Snake showcase application.
option(ENABLE_SNAKE_APP "Build Snake showcase targets" ON)

# Early exit if Snake showcase is disabled.
if(NOT ENABLE_SNAKE_APP)
    message(STATUS "Snake showcase disabled (use -DENABLE_SNAKE_APP=ON)")
    return()
endif()

# Define common warning flags for all Snake targets.
set(SNAKE_COMMON_WARNINGS
    -Wall        # Enable all standard warnings.
    -Wextra      # Enable extra warnings.
    -Wpedantic   # Enforce ISO C++ compliance.
    -Wshadow     # Warn about variable shadowing.
)

# ============================= Snake Engine Library ============================== #

# Create static library for core Snake game logic.
add_library(snake_engine STATIC
    src/Snake_Engine.cpp
)

# Place compiled library in centralized lib directory.
set_target_properties(snake_engine PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# Export include directory for public API access.
target_include_directories(snake_engine
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Link against the main data structures library.
target_link_libraries(snake_engine
    PUBLIC
        ads_lib  # Provides core containers and algorithms.
)

# Configure compiler options with build-specific optimizations.
target_compile_options(snake_engine PRIVATE
    ${SNAKE_COMMON_WARNINGS}
    $<$<CONFIG:Debug>:-g -O0>
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
    $<$<CONFIG:Sanitize>:-g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer>
    $<$<CONFIG:ThreadSanitize>:-g -O1 -fsanitize=thread -fno-omit-frame-pointer>
)

# Enable sanitizers for link stage when in Sanitize build.
target_link_options(snake_engine PRIVATE
    $<$<CONFIG:Sanitize>:-fsanitize=address,undefined>
    $<$<CONFIG:ThreadSanitize>:-fsanitize=thread>
)

# Add system include paths if provided (for external dependencies).
if(COMPILER_SYSTEM_INCLUDE_PATHS)
    foreach(dir IN LISTS COMPILER_SYSTEM_INCLUDE_PATHS)
        target_include_directories(snake_engine SYSTEM PRIVATE ${dir})
    endforeach()
endif()

# ========================== Executable Helper Function =========================== #

# Helper function to create Snake executables with consistent configuration.
# Args: TARGET_NAME - Name of the executable target.
#       SOURCE_FILE - Source file path for the executable.
function(snake_add_executable TARGET_NAME SOURCE_FILE)
    add_executable(${TARGET_NAME} ${SOURCE_FILE})

    # Link against the engine library.
    target_link_libraries(${TARGET_NAME} PRIVATE snake_engine)

    # Apply same compiler options as engine.
    target_compile_options(${TARGET_NAME} PRIVATE
        ${SNAKE_COMMON_WARNINGS}
        $<$<CONFIG:Debug>:-g -O0>
        $<$<CONFIG:Release>:-O3 -DNDEBUG>
        $<$<CONFIG:Sanitize>:-g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer>
        $<$<CONFIG:ThreadSanitize>:-g -O1 -fsanitize=thread -fno-omit-frame-pointer>
    )

    # Enable sanitizers for link stage.
    target_link_options(${TARGET_NAME} PRIVATE
        $<$<CONFIG:Sanitize>:-fsanitize=address,undefined>
        $<$<CONFIG:ThreadSanitize>:-fsanitize=thread>
    )

    # Add system include paths if available.
    if(COMPILER_SYSTEM_INCLUDE_PATHS)
        foreach(dir IN LISTS COMPILER_SYSTEM_INCLUDE_PATHS)
            target_include_directories(${TARGET_NAME} SYSTEM PRIVATE ${dir})
        endforeach()
    endif()
endfunction()

# ============================== Snake Applications =============================== #

# Terminal User Interface application for interactive play.
snake_add_executable(app_Snake_TUI src/main_Snake_TUI.cc)

# Stress testing application for performance validation.
snake_add_executable(app_Snake_Stress src/main_Snake_Stress.cc)

# Custom target for quick game launch.
add_custom_target(runSnake
    COMMAND $<TARGET_FILE:app_Snake_TUI>
    DEPENDS app_Snake_TUI
    COMMENT "Running Snake TUI"
    USES_TERMINAL  # Allows interactive terminal input.
)

# ================================= Testing Setup ================================= #

if(ENABLE_TESTING)
    # Detect available GoogleTest target (handles different import methods).
    set(SNAKE_GTEST_TARGET "")

    if(TARGET GTest::gtest_main)
        set(SNAKE_GTEST_TARGET GTest::gtest_main)
    elseif(TARGET gtest_main)
        set(SNAKE_GTEST_TARGET gtest_main)
    endif()

    # Build tests only if GoogleTest is available.
    if(SNAKE_GTEST_TARGET)
        # Create test executable.
        add_executable(runSnakeTests tests/Test_Snake_Engine.cpp)

        # Link against engine and test framework.
        target_link_libraries(runSnakeTests PRIVATE snake_engine ${SNAKE_GTEST_TARGET})

        # Apply consistent compiler options.
        target_compile_options(runSnakeTests PRIVATE
            ${SNAKE_COMMON_WARNINGS}
            $<$<CONFIG:Debug>:-g -O0>
            $<$<CONFIG:Release>:-O3 -DNDEBUG>
            $<$<CONFIG:Sanitize>:-g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer>
            $<$<CONFIG:ThreadSanitize>:-g -O1 -fsanitize=thread -fno-omit-frame-pointer>
        )

        # Enable sanitizers for link stage.
        target_link_options(runSnakeTests PRIVATE
            $<$<CONFIG:Sanitize>:-fsanitize=address,undefined>
            $<$<CONFIG:ThreadSanitize>:-fsanitize=thread>
        )

        # Add system include paths if provided.
        if(COMPILER_SYSTEM_INCLUDE_PATHS)
            foreach(dir IN LISTS COMPILER_SYSTEM_INCLUDE_PATHS)
                target_include_directories(runSnakeTests SYSTEM PRIVATE ${dir})
            endforeach()
        endif()

        # Integrate with CTest for automatic test discovery.
        include(GoogleTest)
        gtest_discover_tests(runSnakeTests)

        message(STATUS "Added Snake GoogleTest target: runSnakeTests")
    else()
        message(STATUS "Snake tests requested but GoogleTest target not available")
    endif()
endif()

# ================================================================================= #
# End of CMakeLists.txt
