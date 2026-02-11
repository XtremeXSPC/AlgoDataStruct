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
#   make clean            - Remove all build directories
#   make reconfigure      - Clean and reconfigure
#   make test             - Configure with testing enabled and run tests
#
# Cleanup commands:
#   make clean-clang      - Remove Clang build directories only
#   make clean-gcc        - Remove GCC build directories only
#   make clean-sanitize   - Remove sanitizer build directories only
#   make clean-thread     - Remove thread-sanitizer build directories only
#   make clean-tests      - Remove test-enabled build directories only
#
# Documentation commands:
#   make docs             - Generate Doxygen documentation
#   make docs-open        - Generate and open documentation in browser
#   make docs-clean       - Remove generated documentation
#
# Specific configuration commands:
#   make configure-clang    - Configure with Clang (recommended)
#   make configure-gcc      - Configure with GCC
#   make configure-debug    - Configure Debug build with Clang
#   make configure-release  - Configure Release build with Clang
#
# ================================================================================= #

# ---------------------------- Configuration Variables ---------------------------- #

# Default toolchain: Clang is recommended for this project.
DEFAULT_TOOLCHAIN := clang-toolchain.cmake

# Default build type: Debug is more useful during development.
DEFAULT_BUILD_TYPE := Debug

# Dedicated build directories per toolchain/profile to avoid cache contamination.
BUILD_ROOT := build
BUILD_DIR_CLANG_ROOT := $(BUILD_ROOT)/clang
BUILD_DIR_GCC_ROOT := $(BUILD_ROOT)/gcc
DEFAULT_BUILD_DIR := $(BUILD_DIR_CLANG_ROOT)/debug
LAST_BUILD_DIR_FILE := .last_build_dir

# Clang build directories for different configurations.
BUILD_DIR_CLANG_DEBUG := $(BUILD_DIR_CLANG_ROOT)/debug
BUILD_DIR_CLANG_RELEASE := $(BUILD_DIR_CLANG_ROOT)/release
BUILD_DIR_CLANG_TESTS := $(BUILD_DIR_CLANG_ROOT)/debug-tests
BUILD_DIR_CLANG_SANITIZE := $(BUILD_DIR_CLANG_ROOT)/sanitize
BUILD_DIR_CLANG_SANITIZE_TESTS := $(BUILD_DIR_CLANG_ROOT)/sanitize-tests
BUILD_DIR_CLANG_THREAD_SANITIZE := $(BUILD_DIR_CLANG_ROOT)/thread-sanitize
BUILD_DIR_CLANG_THREAD_SANITIZE_TESTS := $(BUILD_DIR_CLANG_ROOT)/thread-sanitize-tests

# GCC build directories for different configurations.
BUILD_DIR_GCC_DEBUG := $(BUILD_DIR_GCC_ROOT)/debug
BUILD_DIR_GCC_RELEASE := $(BUILD_DIR_GCC_ROOT)/release
BUILD_DIR_GCC_TESTS := $(BUILD_DIR_GCC_ROOT)/debug-tests
BUILD_DIR_GCC_SANITIZE := $(BUILD_DIR_GCC_ROOT)/sanitize
BUILD_DIR_GCC_SANITIZE_TESTS := $(BUILD_DIR_GCC_ROOT)/sanitize-tests
BUILD_DIR_GCC_THREAD_SANITIZE := $(BUILD_DIR_GCC_ROOT)/thread-sanitize
BUILD_DIR_GCC_THREAD_SANITIZE_TESTS := $(BUILD_DIR_GCC_ROOT)/thread-sanitize-tests

BUILD_DIRS_CLANG := $(BUILD_DIR_CLANG_DEBUG) $(BUILD_DIR_CLANG_RELEASE) \
	$(BUILD_DIR_CLANG_TESTS) $(BUILD_DIR_CLANG_SANITIZE) \
	$(BUILD_DIR_CLANG_SANITIZE_TESTS) $(BUILD_DIR_CLANG_THREAD_SANITIZE) \
	$(BUILD_DIR_CLANG_THREAD_SANITIZE_TESTS)

BUILD_DIRS_GCC := $(BUILD_DIR_GCC_DEBUG) $(BUILD_DIR_GCC_RELEASE) \
	$(BUILD_DIR_GCC_TESTS) $(BUILD_DIR_GCC_SANITIZE) \
	$(BUILD_DIR_GCC_SANITIZE_TESTS) $(BUILD_DIR_GCC_THREAD_SANITIZE) \
	$(BUILD_DIR_GCC_THREAD_SANITIZE_TESTS)

BUILD_DIRS_SANITIZE := $(BUILD_DIR_CLANG_SANITIZE) \
	$(BUILD_DIR_CLANG_SANITIZE_TESTS) $(BUILD_DIR_GCC_SANITIZE) \
	$(BUILD_DIR_GCC_SANITIZE_TESTS)

BUILD_DIRS_THREAD := $(BUILD_DIR_CLANG_THREAD_SANITIZE) \
	$(BUILD_DIR_CLANG_THREAD_SANITIZE_TESTS) $(BUILD_DIR_GCC_THREAD_SANITIZE) \
	$(BUILD_DIR_GCC_THREAD_SANITIZE_TESTS)

BUILD_DIRS_TESTS := $(BUILD_DIR_CLANG_TESTS) \
	$(BUILD_DIR_CLANG_SANITIZE_TESTS) \
	$(BUILD_DIR_CLANG_THREAD_SANITIZE_TESTS) $(BUILD_DIR_GCC_TESTS) \
	$(BUILD_DIR_GCC_SANITIZE_TESTS) $(BUILD_DIR_GCC_THREAD_SANITIZE_TESTS)

BUILD_DIRS_ALL := $(BUILD_DIRS_CLANG) $(BUILD_DIRS_GCC)

LEGACY_BUILD_DIRS := build-release build-tests build-sanitize build-sanitize-tests \
	build-thread-sanitize build-thread-sanitize-tests build-gcc build-release-gcc \
	build-tests-gcc build-sanitize-gcc build-sanitize-tests-gcc \
	build-thread-sanitize-gcc build-thread-sanitize-tests-gcc

# Main test executable.
TARGET := test_BST

# -------------------------------- Default Target --------------------------------- #
# The default target configures with Clang and builds.
.DEFAULT_GOAL := all

# Declare all targets as phony (not real files).
.PHONY: all build run clean clean-all clean-clang clean-gcc clean-sanitize clean-thread clean-tests \
	configure configure-clang configure-gcc configure-debug configure-release configure-sanitize \
	reconfigure test help symlink check docs docs-open docs-clean sanitize

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
	@BUILD_DIR_TO_USE=$$(if [ -f "$(LAST_BUILD_DIR_FILE)" ]; then cat "$(LAST_BUILD_DIR_FILE)"; else echo "$(DEFAULT_BUILD_DIR)"; fi); \
	if [ ! -d "$$BUILD_DIR_TO_USE" ]; then \
		echo "Error: Build directory '$$BUILD_DIR_TO_USE' not found."; \
		echo "Run one of: make configure-clang, make configure-gcc, make configure-release, make test"; \
		exit 1; \
	fi; \
	echo "Building project in $$BUILD_DIR_TO_USE..."; \
	cmake --build "$$BUILD_DIR_TO_USE"

# 'run' target: builds and runs the main test.
run: build
	@BUILD_DIR_TO_USE=$$(if [ -f "$(LAST_BUILD_DIR_FILE)" ]; then cat "$(LAST_BUILD_DIR_FILE)"; else echo "$(DEFAULT_BUILD_DIR)"; fi); \
	TEST_EXECUTABLE="$$BUILD_DIR_TO_USE/bin/$(TARGET)"; \
	echo ""; \
	echo "Running $(TARGET) from $$BUILD_DIR_TO_USE..."; \
	echo ""; \
	if [ ! -f "$$TEST_EXECUTABLE" ]; then \
		echo "Error: $$TEST_EXECUTABLE not found."; \
		echo "Available executables include: test_BST, test_Singly_Linked_List, test_Stacks_Queues"; \
		exit 1; \
	fi; \
	"$$TEST_EXECUTABLE"

# Internal helper macro for cleaning groups of build directories.
define remove_dirs
	@for dir in $(1); do \
		if [ -d "$$dir" ]; then \
			echo "  - removing $$dir"; \
			rm -rf "$$dir"; \
		fi; \
	done
endef

# 'clean' target: removes all generated build directories.
clean: clean-all

clean-all:
	@echo "Removing all build directories..."
	$(call remove_dirs,$(BUILD_DIRS_ALL))
	$(call remove_dirs,$(LEGACY_BUILD_DIRS))
	rm -f compile_commands.json $(LAST_BUILD_DIR_FILE)
	@echo "✓ All build directories cleaned"

clean-clang:
	@echo "Removing Clang build directories..."
	$(call remove_dirs,$(BUILD_DIRS_CLANG))
	$(call remove_dirs,build-release build-tests build-sanitize \
		build-sanitize-tests build-thread-sanitize \
		build-thread-sanitize-tests)
	rm -f compile_commands.json
	@echo "✓ Clang build directories cleaned"

clean-gcc:
	@echo "Removing GCC build directories..."
	$(call remove_dirs,$(BUILD_DIRS_GCC))
	$(call remove_dirs,build-gcc build-release-gcc build-tests-gcc \
		build-sanitize-gcc build-sanitize-tests-gcc \
		build-thread-sanitize-gcc build-thread-sanitize-tests-gcc)
	@echo "✓ GCC build directories cleaned"

clean-sanitize:
	@echo "Removing sanitizer build directories..."
	$(call remove_dirs,$(BUILD_DIRS_SANITIZE))
	$(call remove_dirs,build-sanitize build-sanitize-tests \
		build-sanitize-gcc build-sanitize-tests-gcc)
	@echo "✓ Sanitizer build directories cleaned"

clean-thread:
	@echo "Removing thread-sanitizer build directories..."
	$(call remove_dirs,$(BUILD_DIRS_THREAD))
	$(call remove_dirs,build-thread-sanitize build-thread-sanitize-tests \
		build-thread-sanitize-gcc build-thread-sanitize-tests-gcc)
	@echo "✓ Thread-sanitizer build directories cleaned"

clean-tests:
	@echo "Removing test-enabled build directories..."
	$(call remove_dirs,$(BUILD_DIRS_TESTS))
	$(call remove_dirs,build-tests build-sanitize-tests \
		build-thread-sanitize-tests build-tests-gcc \
		build-sanitize-tests-gcc build-thread-sanitize-tests-gcc)
	@echo "✓ Test-enabled build directories cleaned"

# ----------------------------- Configuration Targets ----------------------------- #
# Configure with Clang (recommended).
# Uses Clang toolchain to ensure correct compiler selection.
configure-clang:
	@echo "Configuring with Clang toolchain (recommended)..."
	cmake -DCMAKE_TOOLCHAIN_FILE=clang-toolchain.cmake \
					-DCMAKE_BUILD_TYPE=$(DEFAULT_BUILD_TYPE) \
					-S . -B $(BUILD_DIR_CLANG_DEBUG)
	@echo "$(BUILD_DIR_CLANG_DEBUG)" > $(LAST_BUILD_DIR_FILE)
	@echo ""
	@echo "✓ Configuration complete!"
	@echo "  Compiler: Clang"
	@echo "  Build Type: $(DEFAULT_BUILD_TYPE)"
	@echo "  Build Dir: $(BUILD_DIR_CLANG_DEBUG)"
	@echo ""

# Configure with GCC (fallback).
# Useful when Clang is not available or for compatibility testing.
configure-gcc:
	@echo "Configuring with GCC toolchain..."
	cmake -DCMAKE_TOOLCHAIN_FILE=gcc-toolchain.cmake \
					-DCMAKE_BUILD_TYPE=$(DEFAULT_BUILD_TYPE) \
					-S . -B $(BUILD_DIR_GCC_DEBUG)
	@echo "$(BUILD_DIR_GCC_DEBUG)" > $(LAST_BUILD_DIR_FILE)
	@echo ""
	@echo "✓ Configuration complete!"
	@echo "  Compiler: GCC"
	@echo "  Build Type: $(DEFAULT_BUILD_TYPE)"
	@echo "  Build Dir: $(BUILD_DIR_GCC_DEBUG)"
	@echo ""

# Debug configuration (with Clang).
# Optimizations disabled, full debug symbols.
configure-debug: configure-clang

# Release configuration (with Clang).
# Maximum optimizations, no debug symbols.
configure-release:
	@echo "Configuring Release build with Clang..."
	cmake -DCMAKE_TOOLCHAIN_FILE=clang-toolchain.cmake \
					-DCMAKE_BUILD_TYPE=Release \
					-S . -B $(BUILD_DIR_CLANG_RELEASE)
	@echo "$(BUILD_DIR_CLANG_RELEASE)" > $(LAST_BUILD_DIR_FILE)
	@echo ""
	@echo "✓ Release configuration complete!"
	@echo "  Build Dir: $(BUILD_DIR_CLANG_RELEASE)"
	@echo ""

# Sanitize configuration (with Clang).
configure-sanitize:
	@echo "Configuring Sanitize build with Clang..."
	cmake -DCMAKE_TOOLCHAIN_FILE=clang-toolchain.cmake \
	      -DCMAKE_BUILD_TYPE=Sanitize \
	      -S . -B $(BUILD_DIR_CLANG_SANITIZE)
	@echo "$(BUILD_DIR_CLANG_SANITIZE)" > $(LAST_BUILD_DIR_FILE)
	@echo ""
	@echo "✓ Sanitize configuration complete!"
	@echo "  Address Sanitizer: Enabled"
	@echo "  UB Sanitizer: Enabled"
	@echo "  Build Dir: $(BUILD_DIR_CLANG_SANITIZE)"
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
					-S . -B $(BUILD_DIR_CLANG_TESTS)
	@echo "$(BUILD_DIR_CLANG_TESTS)" > $(LAST_BUILD_DIR_FILE)
	@echo "Building tests..."
	cmake --build $(BUILD_DIR_CLANG_TESTS)
	@echo ""
	@echo "Running tests..."
	@echo "================"
	cd $(BUILD_DIR_CLANG_TESTS) && ctest --output-on-failure

# Configure with sanitizers enabled and build.
sanitize: clean configure-sanitize build
	@echo ""
	@echo "Build with sanitizers complete!"
	@echo "Run 'make run' to execute the tests with sanitizers enabled"

# -------------------------------- Utility Targets -------------------------------- #
# Create symlink for compile_commands.json
# Needed for clangd and other LSPs.
symlink:
	@BUILD_DIR_TO_USE=$$(if [ -f "$(LAST_BUILD_DIR_FILE)" ]; then cat "$(LAST_BUILD_DIR_FILE)"; else echo "$(DEFAULT_BUILD_DIR)"; fi); \
	if [ -f "$$BUILD_DIR_TO_USE/compile_commands.json" ]; then \
		ln -sf "$$BUILD_DIR_TO_USE/compile_commands.json" compile_commands.json; \
		echo "✓ Symlink to compile_commands.json created"; \
	else \
		echo "⚠ Warning: compile_commands.json not found in $$BUILD_DIR_TO_USE"; \
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
	@echo "═════════════════ AlgoDataStruct - Makefile Commands ═════════════════"
	@echo ""
	@echo "Main Commands:"
	@echo "  make                   - Configure with Clang and build (default)"
	@echo "  make build             - Build using the last configured build directory"
	@echo "  make run               - Build and run test_BST from last configured dir"
	@echo "  make clean             - Remove all build directories"
	@echo "  make reconfigure       - Clean and reconfigure"
	@echo "  make test              - Configure with testing and run tests"
	@echo ""
	@echo "Cleanup:"
	@echo "  make clean-clang       - Remove Clang build directories only"
	@echo "  make clean-gcc         - Remove GCC build directories only"
	@echo "  make clean-sanitize    - Remove sanitizer build directories only"
	@echo "  make clean-thread      - Remove thread-sanitizer build directories only"
	@echo "  make clean-tests       - Remove test-enabled build directories only"
	@echo ""
	@echo "Specific Configuration:"
	@echo "  make configure-clang   - Configure Debug build with Clang in 'build/clang/debug'"
	@echo "  make configure-gcc     - Configure Debug build with GCC in 'build/gcc/debug'"
	@echo "  make configure-debug   - Alias of configure-clang"
	@echo "  make configure-release - Configure Release build in 'build/clang/release'"
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
	@echo "Notes:"
	@echo "  - Build directories are dedicated per toolchain/profile"
	@echo "  - 'make build' and 'make run' use the last configure target executed"
	@echo "  - 'make clean' also removes compile_commands.json symlink"
	@echo ""
	@echo "══════════════════════════════════════════════════════════════════════"
	@echo ""

# ================================================================================= #
# End of Makefile.
