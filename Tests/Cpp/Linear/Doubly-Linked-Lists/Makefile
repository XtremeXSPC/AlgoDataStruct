# Makefile for DoublyLinkedList project - Cross-platform compatible

# Project structure based paths
PROJECT_ROOT := $(shell git rev-parse --show-toplevel)
HEADER_PATH := $(PROJECT_ROOT)/Cpp/Linear/Doubly-Linked-Lists

# Detect operating system
ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
    RM := del /Q
    RMDIR := rmdir /S /Q
    MKDIR := mkdir
    EXE_EXT := .exe
    PATH_SEP := \\
    RUN_PREFIX :=
else
    DETECTED_OS := $(shell uname -s)
    RM := rm -f
    RMDIR := rm -rf
    MKDIR := mkdir -p
    EXE_EXT :=
    PATH_SEP := /
    RUN_PREFIX := ./
endif

# Build directory
BUILD_DIR := build

# Compiler and flags
ifeq ($(DETECTED_OS),Windows)
    # Check if we're using MinGW/MSYS
    ifneq (,$(findstring MINGW,$(shell uname -s 2>/dev/null)))
        CXX := g++
    else
        CXX := cl
        CXXFLAGS := /nologo /EHsc /W4 /std:c++17
        INCLUDES := /I"$(HEADER_PATH)"
        OBJ_EXT := .obj
    endif
else
    # For Linux and macOS
    CXX := g++
    CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic
    INCLUDES := -I$(HEADER_PATH)
    OBJ_EXT := .o
endif

# Set default compiler to g++ if not set by OS detection
ifndef CXXFLAGS
    CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic
    INCLUDES := -I$(HEADER_PATH)
    OBJ_EXT := .o
endif

# Files
SRC := main.cc
OBJ := $(BUILD_DIR)/main$(OBJ_EXT)
EXECUTABLE := $(BUILD_DIR)/dll-test$(EXE_EXT)

# Default target
all: prepare $(EXECUTABLE)

# Prepare build directory
prepare:
	@$(MKDIR) $(BUILD_DIR) 2>/dev/null || true

# Compile the executable
ifeq ($(CXX),cl)
# MSVC compilation
$(EXECUTABLE): $(OBJ)
	$(CXX) $(CXXFLAGS) /Fe$@ $<

$(OBJ): $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) /c $< /Fo$@
else
# GCC/Clang compilation
$(EXECUTABLE): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ): $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@
endif

# Run the executable
run: $(EXECUTABLE)
	$(RUN_PREFIX)$(EXECUTABLE)

# Clean object files and executable
clean:
ifeq ($(DETECTED_OS),Windows)
	@if exist $(BUILD_DIR) $(RMDIR) $(BUILD_DIR)
else
	$(RMDIR) $(BUILD_DIR) 2>/dev/null || true
endif

# Print information about the build
info:
	@echo "Detected OS: $(DETECTED_OS)"
	@echo "Compiler: $(CXX)"
	@echo "Project root: $(PROJECT_ROOT)"
	@echo "Header path: $(HEADER_PATH)"
	@echo "Source file: $(SRC)"
	@echo "Executable: $(EXECUTABLE)"

# Check if header file exists
check:
	@echo "Checking for header file..."
ifeq ($(DETECTED_OS),Windows)
	@if not exist "$(HEADER_PATH)\DoublyLinkedList.hpp" echo Error: Header file not found at $(HEADER_PATH)\DoublyLinkedList.hpp
	@if exist "$(HEADER_PATH)\DoublyLinkedList.hpp" echo Header file found at $(HEADER_PATH)\DoublyLinkedList.hpp
else
	@test -f "$(HEADER_PATH)/DoublyLinkedList.hpp" && echo "Header file found at $(HEADER_PATH)/DoublyLinkedList.hpp" || echo "Error: Header file not found at $(HEADER_PATH)/DoublyLinkedList.hpp"
endif

# Set project root from command line
set-root:
	@echo "Current project root is set to: $(PROJECT_ROOT)"
	@echo "You can change it using: make PROJECT_ROOT=your/path all"

# Phony targets
.PHONY: all prepare run clean info check set-root