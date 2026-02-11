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

# List all the headers and implementation files that are part of this library.
# While not strictly necessary for a header-only library, this helps IDEs
# and build systems understand the project structure.
target_sources(ads_lib
    INTERFACE
        FILE_SET HEADERS
        BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
        FILES
            # Algorithms.
            include/ads/algorithms/Sorting.hpp

            # Array implementations.
            include/ads/arrays/Array_Exception.hpp
            include/ads/arrays/Dynamic_Array.hpp
            include/ads/arrays/Static_Array.hpp
            include/ads/arrays/Circular_Array.hpp

            # List interface.
            include/ads/lists/List.hpp
            include/ads/lists/List_Exception.hpp
            include/ads/lists/Doubly_Linked_List.hpp
            include/ads/lists/Singly_Linked_List.hpp
            include/ads/lists/Circular_Linked_List.hpp

            # Stack interface and implementations.
            include/ads/stacks/Stack.hpp
            include/ads/stacks/Stack_Exception.hpp
            include/ads/stacks/Array_Stack.hpp
            include/ads/stacks/Linked_Stack.hpp

            # Queue interface and implementations.
            include/ads/queues/Queue.hpp
            include/ads/queues/Queue_Exception.hpp
            include/ads/queues/Circular_Array_Deque.hpp
            include/ads/queues/Circular_Array_Queue.hpp
            include/ads/queues/Linked_Queue.hpp
            include/ads/queues/Priority_Queue.hpp

            # Binary Tree interface and implementations.
            include/ads/trees/Binary_Tree.hpp
            include/ads/trees/Binary_Tree_Exception.hpp
            include/ads/trees/Fenwick_Tree_Exception.hpp
            include/ads/trees/Segment_Tree_Exception.hpp
            include/ads/trees/Binary_Search_Tree.hpp
            include/ads/trees/AVL_Tree.hpp
            include/ads/trees/Complete_Binary_Tree.hpp
            include/ads/trees/Fenwick_Tree.hpp
            include/ads/trees/Fenwick_Tree_Range_Update.hpp
            include/ads/trees/Segment_Tree.hpp

            # Heap implementations.
            include/ads/heaps/Heap_Exception.hpp
            include/ads/heaps/Min_Heap.hpp
            include/ads/heaps/Max_Heap.hpp

            # Hash Table implementations.
            include/ads/hash/Hash_Table_Exception.hpp
            include/ads/hash/Hash_Table_Chaining.hpp
            include/ads/hash/Hash_Table_Open_Addressing.hpp

            # Graph implementations.
            include/ads/graphs/Graph_Adjacency_List.hpp
            include/ads/graphs/Graph_Adjacency_Matrix.hpp
            include/ads/graphs/Disjoint_Set_Union.hpp

            # Associative structures.
            include/ads/associative/Dictionary.hpp
            include/ads/associative/Hash_Map.hpp
            include/ads/associative/Tree_Map.hpp
            include/ads/associative/Tree_Map_Exception.hpp
            include/ads/associative/Hash_Set.hpp
            include/ads/associative/Tree_Set.hpp

    # Template implementations (.tpp files)
    # These are included by the headers, but we list them here so they appear
    # in IDEs and are tracked for changes.
    INTERFACE
        src/ads/algorithms/Sorting.tpp
        src/ads/arrays/Static_Array.tpp
        src/ads/arrays/Dynamic_Array.tpp
        src/ads/arrays/Circular_Array.tpp
        src/ads/lists/Doubly_Linked_List.tpp
        src/ads/lists/Singly_Linked_List.tpp
        src/ads/lists/Circular_Linked_List.tpp
        src/ads/stacks/Array_Stack.tpp
        src/ads/stacks/Linked_Stack.tpp
        src/ads/queues/Circular_Array_Queue.tpp
        src/ads/queues/Circular_Array_Deque.tpp
        src/ads/queues/Linked_Queue.tpp
        src/ads/queues/Priority_Queue.tpp
        src/ads/trees/Trie.tpp
        src/ads/trees/B_Tree.tpp
        src/ads/trees/AVL_Tree.tpp
        src/ads/trees/Red_Black_Tree.tpp
        src/ads/trees/Binary_Search_Tree.tpp
        src/ads/trees/Complete_Binary_Tree.tpp
        src/ads/trees/Fenwick_Tree.tpp
        src/ads/trees/Fenwick_Tree_Range_Update.tpp
        src/ads/trees/Segment_Tree.tpp
        src/ads/heaps/Min_Heap.tpp
        src/ads/heaps/Max_Heap.tpp
        src/ads/hash/Hash_Table_Chaining.tpp
        src/ads/hash/Hash_Table_Open_Addressing.tpp
        src/ads/graphs/Graph_Adjacency_List.tpp
        src/ads/graphs/Graph_Adjacency_Matrix.tpp
        src/ads/associative/Hash_Map.tpp
        src/ads/associative/Tree_Map.tpp
        src/ads/associative/Hash_Set.tpp
        src/ads/associative/Tree_Set.tpp
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
