# Richiede la versione minima di CMake e definisce il nome del progetto
cmake_minimum_required(VERSION 3.24)
project(AlgoDataStructProject LANGUAGES CXX)

# Imposta il nome del target eseguibile
set(PROJECT_EXECUTABLE_NAME "Tester")

# Imposta lo standard C++ a C++20 
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Abilita la generazione del database di compilazione per clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Libreria di Template 'ads' (AlgoDataStruct) ---
add_library(ads_lib INTERFACE)

# La directory 'include' è la directory pubblica
target_include_directories(ads_lib INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Specifica i file sorgente e header associati alla libreria
target_sources(ads_lib
  # Le intestazioni pubbliche (.hpp) fanno parte dell'interfaccia
  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
    FILES
      # Interfaccia della Lista
      include/ads/lists/List.hpp
      # Header specifici dell'implementazione
      include/ads/lists/Doubly_Linked_List.hpp
  # Le implementazioni dei template (.tpp) sono un dettaglio privato
  PRIVATE
    # Implementazioni specifiche
    src/ads/lists/Doubly_Linked_List.tpp
)

# --- Eseguibile per i Test ---
add_executable         (${PROJECT_EXECUTABLE_NAME} src/main.cc)
target_compile_options (${PROJECT_EXECUTABLE_NAME} PRIVATE -Wall -Wextra -pedantic -g -O1 -fsanitize=address)
target_link_options    (${PROJECT_EXECUTABLE_NAME} PRIVATE -fsanitize=address)
target_link_libraries  (${PROJECT_EXECUTABLE_NAME} PRIVATE ads_lib)

# Target 'run' per eseguire comodamente l'eseguibile
add_custom_target     (run COMMAND $<TARGET_FILE:${PROJECT_EXECUTABLE_NAME}> USES_TERMINAL)

# Collega l'eseguibile alla nostra libreria
target_link_libraries(${PROJECT_EXECUTABLE_NAME} PRIVATE ads_lib)

# --- INTEGRAZIONE GOOGLETEST E UNIT TESTING ---
# Abilita la directory dei test
enable_testing()

# Usa FetchContent per ottenere GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# Rende GoogleTest disponibile
FetchContent_MakeAvailable(googletest)

# Crea l'eseguibile per i test
add_executable(runTests tests/Test_DoublyLinkedList.cpp)
target_compile_options(runTests PRIVATE -Wall -Wextra -pedantic -g)

# Collega il nostro eseguibile dei test alla nostra libreria e a GoogleTest
target_link_libraries(runTests PRIVATE ads_lib gtest gtest_main)

# Aggiunge il nostro eseguibile al set di test gestito da CTest
include(GoogleTest)
gtest_add_tests(TARGET runTests)

