# Requires the minimum version of CMake and defines the project name
cmake_minimum_required(VERSION 3.24)
project(AlgoDataStructProject LANGUAGES CXX)

# Sets the executable target name
set(PROJECT_EXECUTABLE_NAME "Tester")

# Sets the C++ standard to C++20 
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enables generation of the compilation database for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ----- Template Library 'ads' (AlgoDataStruct) ----- #
add_library(ads_lib INTERFACE)

# The 'include' directory is the public directory
target_include_directories(ads_lib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Specifies the source and header files associated with the library
target_sources(ads_lib
  # Public headers (.hpp) are part of the interface
  PUBLIC
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

  # Template implementations (.tpp) are a private detail
  PRIVATE
    # Specific implementations
    src/ads/lists/Doubly_Linked_List.tpp

    # Stack implementations
    src/ads/stacks/Array_Stack.tpp
    src/ads/stacks/Linked_Stack.tpp
    
    # Queue implementations
    src/ads/queues/Circular_Array_Queue.tpp
    src/ads/queues/Linked_Queue.tpp
)

# ----- Executable for Tests ----- #
add_executable         (${PROJECT_EXECUTABLE_NAME} src/main.cc src/test_stack_queue.cc)
target_compile_options (${PROJECT_EXECUTABLE_NAME} PRIVATE -Wall -Wextra -pedantic -g -O1 -fsanitize=address)
target_link_options    (${PROJECT_EXECUTABLE_NAME} PRIVATE -fsanitize=address)
target_link_libraries  (${PROJECT_EXECUTABLE_NAME} PRIVATE ads_lib)

# 'run' target to conveniently execute the executable
add_custom_target     (run COMMAND $<TARGET_FILE:${PROJECT_EXECUTABLE_NAME}> USES_TERMINAL)

# Links the executable to our library
target_link_libraries(${PROJECT_EXECUTABLE_NAME} PRIVATE ads_lib)

# ----- GOOGLE TEST INTEGRATION AND UNIT TESTING ----- #
# Enables the test directory
enable_testing()

# Uses FetchContent to obtain GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# Makes GoogleTest available
FetchContent_MakeAvailable(googletest)

# Creates the test executable
add_executable(runTests tests/Test_DoublyLinkedList.cpp)
target_compile_options(runTests PRIVATE -Wall -Wextra -pedantic -g)

# Links our test executable to our library and GoogleTest
target_link_libraries(runTests PRIVATE ads_lib gtest gtest_main)

# Adds our executable to the test set managed by CTest
include(GoogleTest)
gtest_add_tests(TARGET runTests)
