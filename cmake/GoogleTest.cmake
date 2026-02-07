# ================================================================================= #
# ---------------------------- GoogleTest Integration ----------------------------- #
# ================================================================================= #
#
# GoogleTest support is optional and disabled by default.
# Enable it with: cmake -DENABLE_TESTING=ON ...
#
# This module handles:
#   - GoogleTest discovery (system-installed or FetchContent)
#   - Test file collection from tests/ directory
#   - Creation of the runTests executable
#   - Integration with CTest
#
# Prerequisites:
#   - ads_lib target must be defined (from LibraryDefinition.cmake)
#   - ENABLE_TESTING option must be set to ON
#
# Usage:
#   option(ENABLE_TESTING "Enable unit testing with GoogleTest" OFF)
#   if(ENABLE_TESTING)
#       include(cmake/GoogleTest.cmake)
#   endif()
#
# ================================================================================= #

if(NOT ENABLE_TESTING)
    return()
endif()

enable_testing()

# Prefer a system-installed GoogleTest if available to avoid header/library
# mismatches when system include paths are configured (e.g. via CPATH).
find_package(GTest QUIET)

if(NOT GTest_FOUND)
    # Download and configure GoogleTest using FetchContent.
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    # Configure GoogleTest to work with MSVC's runtime library.
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(googletest)
endif()

# Collect all test source files.
set(TEST_SOURCES "")

# Add test files if they exist.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test_Static_Array.cpp")
    list(APPEND TEST_SOURCES tests/Test_Static_Array.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test_Dynamic_Array.cpp")
    list(APPEND TEST_SOURCES tests/Test_Dynamic_Array.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test_Circular_Array.cpp")
    list(APPEND TEST_SOURCES tests/Test_Circular_Array.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test_Array_Concepts.cpp")
    list(APPEND TEST_SOURCES tests/Test_Array_Concepts.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test_Sorting.cpp")
    list(APPEND TEST_SOURCES tests/Test_Sorting.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test_SinglyLinkedList.cpp")
    list(APPEND TEST_SOURCES tests/Test_Singly_Linked_List.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test_DoublyLinkedList.cpp")
    list(APPEND TEST_SOURCES tests/Test_DoublyLinkedList.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test_Circular_Linked_List.cpp")
    list(APPEND TEST_SOURCES tests/Test_Circular_Linked_List.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test_Stacks.cpp")
    list(APPEND TEST_SOURCES tests/Test_Stacks.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test_Queues.cpp")
    list(APPEND TEST_SOURCES tests/Test_Queues.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test_Deque.cpp")
    list(APPEND TEST_SOURCES tests/Test_Deque.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test_Complete_Binary_Tree.cpp")
   list(APPEND TEST_SOURCES tests/Test_Complete_Binary_Tree.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test_Binary_Search_Tree.cpp")
    list(APPEND TEST_SOURCES tests/Test_Binary_Search_Tree.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test_B_Tree.cpp")
   list(APPEND TEST_SOURCES tests/Test_B_Tree.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test_AVL_Tree.cpp")
    list(APPEND TEST_SOURCES tests/Test_AVL_Tree.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test_Red_Black_Tree.cpp")
    list(APPEND TEST_SOURCES tests/Test_Red_Black_Tree.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test_Trie.cpp")
    list(APPEND TEST_SOURCES tests/Test_Trie.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test_Tree_Concepts.cpp")
    list(APPEND TEST_SOURCES tests/Test_Tree_Concepts.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test_Fenwick_Tree.cpp")
    list(APPEND TEST_SOURCES tests/Test_Fenwick_Tree.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test_Fenwick_Tree_Range_Update.cpp")
    list(APPEND TEST_SOURCES tests/Test_Fenwick_Tree_Range_Update.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test_Segment_Tree.cpp")
    list(APPEND TEST_SOURCES tests/Test_Segment_Tree.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test_Disjoint_Set_Union.cpp")
   list(APPEND TEST_SOURCES tests/Test_Disjoint_Set_Union.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test_Heaps.cpp")
    list(APPEND TEST_SOURCES tests/Test_Heaps.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test_HashTables.cpp")
    list(APPEND TEST_SOURCES tests/Test_HashTables.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test_Tree_Map.cpp")
    list(APPEND TEST_SOURCES tests/Test_Tree_Map.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test_Tree_Set.cpp")
    list(APPEND TEST_SOURCES tests/Test_Tree_Set.cpp)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test_Hash_Set.cpp")
    list(APPEND TEST_SOURCES tests/Test_Hash_Set.cpp)
endif()

# Create test executable if we have test files.
if(TEST_SOURCES)
    add_executable(runTests ${TEST_SOURCES})
    target_compile_options(runTests PRIVATE -Wall -Wextra -Wpedantic -g)
    if(GTest_FOUND)
        target_link_libraries(runTests PRIVATE ads_lib GTest::gtest_main)
    else()
        target_link_libraries(runTests PRIVATE ads_lib gtest_main)
    endif()

    # Register tests with CTest.
    include(GoogleTest)
    gtest_discover_tests(runTests)

    list(LENGTH TEST_SOURCES TEST_COUNT)
    message(STATUS "${ANSI_GREEN}GoogleTest enabled: runTests executable created with ${TEST_COUNT} test files${ANSI_RESET}")
endif()
