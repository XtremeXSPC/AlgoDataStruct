# Richiede la versione minima di CMake e definisce il nome del progetto
cmake_minimum_required(VERSION 3.23)
project(AlgoDataStructProject LANGUAGES CXX)

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
add_executable(Tester src/main.cc)
target_compile_options(Tester PRIVATE -Wall -Wextra -pedantic -g)

# Collega l'eseguibile alla nostra libreria
target_link_libraries(Tester PRIVATE ads_lib)

# Target 'run' per eseguire comodamente l'eseguibile
add_custom_target(run
  COMMAND $<TARGET_FILE:Tester>
  USES_TERMINAL
)