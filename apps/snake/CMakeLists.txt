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

# ============================= Snake Engine Library ============================== #

# Create static library for core Snake game logic.
add_library(snake_engine STATIC
    src/Snake_Engine.cpp
)

# Public API include dir + ads_lib (PUBLIC: part of the API) and ads_build_options
# (PRIVATE: flags that must not propagate to consumers).
target_include_directories(snake_engine
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(snake_engine
    PUBLIC
        ads_lib            # Provides core containers and algorithms.
    PRIVATE
        ads_build_options  # Shared warnings / optimization / sanitizer flags.
)
ads_apply_clangd_includes(snake_engine)

# ========================== Executable Helper Function =========================== #

# Helper function to create Snake executables with consistent configuration.
# Args: TARGET_NAME - Name of the executable target.
#       SOURCE_FILE - Source file path for the executable.
function(snake_add_executable TARGET_NAME SOURCE_FILE)
    add_executable(${TARGET_NAME} ${SOURCE_FILE})
    target_link_libraries(${TARGET_NAME} PRIVATE snake_engine ads_build_options)
    ads_apply_clangd_includes(${TARGET_NAME})
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
        # Test executable: engine + shared build options + test framework.
        add_executable(runSnakeTests tests/Test_Snake_Engine.cpp)
        target_link_libraries(runSnakeTests PRIVATE
            snake_engine
            ads_build_options
            ${SNAKE_GTEST_TARGET}
        )

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
