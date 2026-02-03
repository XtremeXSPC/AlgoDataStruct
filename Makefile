# ================================================================================= #
# -------------------------- Makefile for AlgoDataStruct -------------------------- #
# ================================================================================= #
#
# This Makefile provides convenient commands to configure and build the project.
# It uses toolchain files to ensure the correct compiler selection.
#
# Main commands:
#   make                  - Configure with Clang and build (default)
#   make build            - Build the project (requires configure first)
#   make run              - Build and run test_BST
#   make clean            - Remove the build directory
#   make reconfigure      - Clean and reconfigure
#   make test             - Configure with testing enabled and run tests
#
# Documentation commands:
#   make docs             - Generate Doxygen documentation
#   make docs-open        - Generate and open documentation in browser
#   make docs-clean       - Remove generated documentation
#
# Specific configuration commands:
#   make configure-clang  - Configure with Clang (recommended)
#   make configure-gcc    - Configure with GCC
#   make configure-debug  - Configure Debug build with Clang
#   make configure-release- Configure Release build with Clang
#
# ================================================================================= #

# ---------------------------- Configuration Variables ---------------------------- #

# Default toolchain: Clang is recommended for this project.
DEFAULT_TOOLCHAIN := clang-toolchain.cmake

# Default build type: Debug is more useful during development.
DEFAULT_BUILD_TYPE := Debug

# Build directory.
BUILD_DIR := build

# Main test executable.
TARGET := test_BST
TEST_EXECUTABLE := $(BUILD_DIR)/bin/$(TARGET)

# -------------------------------- Default Target --------------------------------- #
# The default target configures with Clang and builds.
.DEFAULT_GOAL := all

# Declare all targets as phony (not real files).
.PHONY: all build run clean configure configure-clang configure-gcc \
	configure-debug configure-release configure-sanitize reconfigure test help \
	symlink check docs docs-open docs-clean

# --------------------------------- Main Targets ---------------------------------- #
# 'all' target: full configuration and build.
all: configure-clang build symlink
	@echo ""
	@echo "✓ Project configured and built successfully!"
	@echo ""
	@echo "Run 'make run' to execute default test ($(TARGET))"
	@echo "Run 'make help' to see all available commands"
	@echo ""

# 'build' target: compiles the project.
# Assumes 'configure' has already been run.
build:
	@if [ ! -d "$(BUILD_DIR)" ]; then \
		echo "Error: Build directory not found."; \
		echo "Run 'make configure' or 'make configure-clang' first"; \
		exit 1; \
	fi
	@echo "Building project..."
	cmake --build $(BUILD_DIR)

# 'run' target: builds and runs the main test.
run: build
	@echo ""
	@echo "Running $(TARGET)..."
	@echo ""
	@if [ ! -f "$(TEST_EXECUTABLE)" ]; then \
		echo "Error: $(TEST_EXECUTABLE) not found."; \
		echo "Available executables: test_BST, test_Singly_Linked_List, test_Stacks_Queues"; \
		exit 1; \
	fi
	@$(TEST_EXECUTABLE)

# 'clean' target: removes all generated files.
clean:
	@echo "Removing build directory..."
	rm -rf $(BUILD_DIR)
	rm -f compile_commands.json
	@echo "✓ Build directory cleaned"

# ----------------------------- Configuration Targets ----------------------------- #
# Configure with Clang (recommended).
# Uses Clang toolchain to ensure correct compiler selection.
configure-clang:
	@echo "Configuring with Clang toolchain (recommended)..."
	cmake -DCMAKE_TOOLCHAIN_FILE=clang-toolchain.cmake \
				-DCMAKE_BUILD_TYPE=$(DEFAULT_BUILD_TYPE) \
				-S . -B $(BUILD_DIR)
	@echo ""
	@echo "✓ Configuration complete!"
	@echo "  Compiler: Clang"
	@echo "  Build Type: $(DEFAULT_BUILD_TYPE)"
	@echo ""

# Configure with GCC (fallback).
# Useful when Clang is not available or for compatibility testing.
configure-gcc:
	@echo "Configuring with GCC toolchain..."
	cmake -DCMAKE_TOOLCHAIN_FILE=gcc-toolchain.cmake \
				-DCMAKE_BUILD_TYPE=$(DEFAULT_BUILD_TYPE) \
				-S . -B $(BUILD_DIR)
	@echo ""
	@echo "✓ Configuration complete!"
	@echo "  Compiler: GCC"
	@echo "  Build Type: $(DEFAULT_BUILD_TYPE)"
	@echo ""

# Debug configuration (with Clang).
# Optimizations disabled, full debug symbols.
configure-debug:
	@echo "Configuring Debug build with Clang..."
	cmake -DCMAKE_TOOLCHAIN_FILE=clang-toolchain.cmake \
				-DCMAKE_BUILD_TYPE=Debug \
				-S . -B $(BUILD_DIR)
	@echo ""
	@echo "✓ Debug configuration complete!"
	@echo ""

# Release configuration (with Clang).
# Maximum optimizations, no debug symbols.
configure-release:
	@echo "Configuring Release build with Clang..."
	cmake -DCMAKE_TOOLCHAIN_FILE=clang-toolchain.cmake \
				-DCMAKE_BUILD_TYPE=Release \
				-S . -B $(BUILD_DIR)
	@echo ""
	@echo "✓ Release configuration complete!"
	@echo ""

# Sanitize configuration (with Clang).
configure-sanitize:
	@echo "Configuring Sanitize build with Clang..."
	cmake -DCMAKE_TOOLCHAIN_FILE=clang-toolchain.cmake \
	      -DCMAKE_BUILD_TYPE=Sanitize \
	      -S . -B $(BUILD_DIR)
	@echo ""
	@echo "✓ Sanitize configuration complete!"
	@echo "  Address Sanitizer: Enabled"
	@echo "  UB Sanitizer: Enabled"
	@echo ""

# Alias for backward compatibility with old Makefile.
# 'configure' without specification uses Clang.
configure: configure-clang

# Full reconfiguration (clean + configure).
# Useful when changing toolchain or after CMakeLists.txt changes.
reconfigure: clean configure-clang
	@echo "✓ Reconfiguration complete!"

# ------------------------------------ Testing ------------------------------------ #
# Configure with GoogleTest enabled and run tests.
test:
	@echo "Configuring with testing enabled..."
	cmake -DCMAKE_TOOLCHAIN_FILE=clang-toolchain.cmake \
				-DCMAKE_BUILD_TYPE=Debug \
				-DENABLE_TESTING=ON \
				-S . -B $(BUILD_DIR)
	@echo "Building tests..."
	cmake --build $(BUILD_DIR)
	@echo ""
	@echo "Running tests..."
	@echo "================"
	cd $(BUILD_DIR) && ctest --output-on-failure

# Configure with sanitizers enabled and build.
sanitize: clean configure-sanitize build
	@echo ""
	@echo "Build with sanitizers complete!"
	@echo "Run 'make run' to execute the tests with sanitizers enabled"

# -------------------------------- Utility Targets -------------------------------- #
# Create symlink for compile_commands.json
# Needed for clangd and other LSPs.
symlink:
	@if [ -f "$(BUILD_DIR)/compile_commands.json" ]; then \
		ln -sf $(BUILD_DIR)/compile_commands.json compile_commands.json; \
		echo "✓ Symlink to compile_commands.json created"; \
	else \
		echo "⚠ Warning: compile_commands.json not found in build directory"; \
	fi

# Check that toolchain files exist.
check:
	@echo "Checking toolchain files..."
	@if [ ! -f "clang-toolchain.cmake" ]; then \
		echo "✗ clang-toolchain.cmake not found!"; \
		exit 1; \
	else \
		echo "✓ clang-toolchain.cmake found"; \
	fi
	@if [ ! -f "gcc-toolchain.cmake" ]; then \
		echo "✗ gcc-toolchain.cmake not found!"; \
		exit 1; \
	else \
		echo "✓ gcc-toolchain.cmake found"; \
	fi
	@echo "✓ All required files present"

# --------------------------------- Documentation --------------------------------- #
# Documentation directory.
DOCS_DIR := docs

# Generate documentation with Doxygen.
docs:
	@echo "Generating documentation with Doxygen..."
	@if ! command -v doxygen &> /dev/null; then \
		echo "Error: Doxygen is not installed."; \
		echo "Install with: sudo pacman -S doxygen graphviz"; \
		exit 1; \
	fi
	doxygen Doxyfile
	@echo ""
	@echo "✓ Documentation generated in $(DOCS_DIR)/html/"
	@echo "  Open $(DOCS_DIR)/html/index.html in your browser"
	@echo ""

# Generate and open documentation.
docs-open: docs
	@echo "Opening documentation..."
	@if command -v xdg-open &> /dev/null; then \
		xdg-open $(DOCS_DIR)/html/index.html; \
	elif command -v open &> /dev/null; then \
		open $(DOCS_DIR)/html/index.html; \
	else \
		echo "Could not detect browser opener."; \
		echo "Open $(DOCS_DIR)/html/index.html manually"; \
	fi

# Remove generated documentation.
docs-clean:
	@echo "Removing documentation directory..."
	rm -rf $(DOCS_DIR)
	@echo "✓ Documentation cleaned"

# ---------------------------- Help and Documentation ----------------------------- #
# Show all available commands with descriptions.
help:
	@echo ""
	@echo "╔═══════════════════════════════════════════════════════════════╗"
	@echo "║              AlgoDataStruct - Makefile Commands               ║"
	@echo "╚═══════════════════════════════════════════════════════════════╝"
	@echo ""
	@echo "Main Commands:"
	@echo "  make              - Configure with Clang and build (default)"
	@echo "  make build        - Build the project"
	@echo "  make run          - Build and run test_BST"
	@echo "  make clean        - Remove the build directory"
	@echo "  make reconfigure  - Clean and reconfigure"
	@echo "  make test         - Configure with testing and run tests"
	@echo ""
	@echo "Specific Configuration:"
	@echo "  make configure-clang   - Configure with Clang (recommended)"
	@echo "  make configure-gcc     - Configure with GCC"
	@echo "  make configure-debug   - Configure Debug build"
	@echo "  make configure-release - Configure Release build"
	@echo ""
	@echo "Documentation:"
	@echo "  make docs         - Generate Doxygen documentation"
	@echo "  make docs-open    - Generate and open documentation"
	@echo "  make docs-clean   - Remove generated documentation"
	@echo ""
	@echo "Utility:"
	@echo "  make symlink      - Create symlink for compile_commands.json"
	@echo "  make check        - Check for toolchain files"
	@echo "  make help         - Show this message"
	@echo ""
	@echo "Examples:"
	@echo "  make                            # Initial setup"
	@echo "  make build                      # Quick build"
	@echo "  make configure-release build    # Optimized build"
	@echo "  make reconfigure                # Full reset"
	@echo ""
	@echo "Notes:"
	@echo "  - Default toolchain is Clang"
	@echo "  - Default build type is Debug"
	@echo "  - 'make clean' also removes compile_commands.json"
	@echo ""
	@echo "═════════════════════════════════════════════════════════════════"
	@echo ""

# ================================================================================= #
# End of Makefile.
