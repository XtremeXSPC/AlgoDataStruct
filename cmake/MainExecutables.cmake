# ================================================================================= #
# ----------------------------- Main Test Executables ----------------------------- #
# ================================================================================= #
#
# These are the actual test programs that exercise the data structures library.
# Each executable tests a specific data structure or component.
#
# Prerequisites:
#   - ads_lib target must be defined (from LibraryDefinition.cmake)
#   - ads_add_executable() function must be available (from CompilerSetup.cmake)
#
# Usage:
#   include(cmake/MainExecutables.cmake)
#
# ================================================================================= #

# Static Array test.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main_Static_Array.cc")
    ads_add_executable(test_Static_Array src/main_Static_Array.cc)
endif()

# Dynamic Array test.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main_Dynamic_Array.cc")
    ads_add_executable(test_Dynamic_Array src/main_Dynamic_Array.cc)
endif()

# Circular Array test.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main_Circular_Array.cc")
    ads_add_executable(test_Circular_Array src/main_Circular_Array.cc)
endif()

# Singly Linked List test.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main_Singly_Linked_List.cc")
    ads_add_executable(test_Singly_Linked_List src/main_Singly_Linked_List.cc)
endif()

# Doubly Linked Lists test.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main_Doubly_Linked_List.cc")
    ads_add_executable(test_Doubly_Linked_List src/main_Doubly_Linked_List.cc)
endif()

# Circular Linked List test.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main_Circular_Linked_List.cc")
    ads_add_executable(test_Circular_Linked_List src/main_Circular_Linked_List.cc)
endif()

# Stacks and Queues test.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main_Stacks_Queues.cc")
    ads_add_executable(test_Stacks_Queues src/main_Stacks_Queues.cc)
endif()

# Deque test.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main_Deque.cc")
    ads_add_executable(test_Deque src/main_Deque.cc)
endif()

# Priority Queue test.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main_Priority_Queue.cc")
    ads_add_executable(test_Priority_Queue src/main_Priority_Queue.cc)
endif()

# Complete Binary Tree test.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main_Complete_Binary_Tree.cc")
    ads_add_executable(test_Complete_Binary_Tree src/main_Complete_Binary_Tree.cc)
endif()

# Binary Search Tree test.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main_Binary_Search_Tree.cc")
    ads_add_executable(test_BST src/main_Binary_Search_Tree.cc)
endif()

# Binary B-Tree test.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main_B_Tree.cc")
    ads_add_executable(test_BBT src/main_B_Tree.cc)
endif()

# AVL Tree test.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main_AVL_Tree.cc")
    ads_add_executable(test_AVL src/main_AVL_Tree.cc)
endif()

# Red Black Tree test.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main_Red_Black_Tree.cc")
    ads_add_executable(test_RBT src/main_Red_Black_Tree.cc)
endif()

# Trie test.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main_Trie.cc")
    ads_add_executable(test_Trie src/main_Trie.cc)
endif()

# Fenwick Tree test.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main_Fenwick_Tree.cc")
    ads_add_executable(test_Fenwick_Tree src/main_Fenwick_Tree.cc)
endif()

# Segment Tree test.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main_Segment_Tree.cc")
    ads_add_executable(test_Segment_Tree src/main_Segment_Tree.cc)
endif()

# Dijkstra Integration test (Graph + PriorityQueue).
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main_Dijkstra_Integration.cc")
    ads_add_executable(test_Dijkstra src/main_Dijkstra_Integration.cc)
endif()

# Graph Adjacency List test.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main_Graph_Adjacency_List.cc")
    ads_add_executable(test_Graph_ADJ_List src/main_Graph_Adjacency_List.cc)
endif()

# Graph Adjacency Matrix test.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main_Graph_Adjacency_Matrix.cc")
    ads_add_executable(test_Graph_ADJ_Matrix src/main_Graph_Adjacency_Matrix.cc)
endif()

# Disjoint Set Union test.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main_Disjoint_Set_Union.cc")
    ads_add_executable(test_Disjoint_Set_Union src/main_Disjoint_Set_Union.cc)
endif()

# TreeMap test.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main_Tree_Map.cc")
    ads_add_executable(test_Tree_Map src/main_Tree_Map.cc)
endif()

# HashMap test.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main_Hash_Map.cc")
    ads_add_executable(test_HashMap src/main_Hash_Map.cc)
endif()

# Hash Table Chaining test.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main_Hash_Table_Chaining.cc")
    ads_add_executable(test_Hash_Chaining src/main_Hash_Table_Chaining.cc)
endif()

# Hash Table Open Addressing test.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main_Hash_Table_Open_Addressing.cc")
    ads_add_executable(test_Hash_Open_Addr src/main_Hash_Table_Open_Addressing.cc)
endif()

# Heaps test.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main_Heaps.cc")
    ads_add_executable(test_Heaps src/main_Heaps.cc)
endif()
