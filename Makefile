# ================================================================================= #
# -------------------------- Makefile for AlgoDataStruct -------------------------- #
# ================================================================================= #
#
# Thin convenience layer over CMake presets (see CMakePresets.json), which are the
# single source of truth for toolchains, build types and binary directories. Every
# target here delegates to `cmake --preset`, `cmake --build --preset` or
# `ctest --preset` instead of duplicating the configuration matrix.
#
# Quick start:
#   make                  - Configure + build the default preset (debug, Clang)
#   make run              - Build and run a demo (TARGET=test_Binary_Search_Tree)
#   make test             - Configure, build and run the GoogleTest suite
#   make help             - List every command
#
# Selecting a configuration:
#   PRESET chooses the configure preset (default: debug). The build/test preset
#   names and the binary directory are derived from it automatically. Examples:
#     make build PRESET=release
#     make run   PRESET=sanitize TARGET=test_AVL_Tree
#   Convenience aliases (debug/release/sanitize/thread-sanitize/gcc) set PRESET.
#
# ================================================================================= #

# ---------------------------- Configuration Variables ---------------------------- #

# Configure preset to operate on; build/test presets are derived from it.
PRESET ?= debug

# Demo executable launched by `make run`.
TARGET ?= test_Binary_Search_Tree

# Derive the build preset and binary directory from PRESET. Binary directories
# follow the build/<toolchain>/<config> convention declared in CMakePresets.json,
# so they are computed here rather than duplicated.
BUILD_PRESET  := build-$(PRESET)
TOOLCHAIN_DIR := $(if $(filter %-gcc,$(PRESET)),gcc,clang)
CONFIG_DIR    := $(patsubst %-gcc,%,$(PRESET))
BUILD_DIR     := build/$(TOOLCHAIN_DIR)/$(CONFIG_DIR)
BIN_DIR       := $(BUILD_DIR)/bin

# Documentation output directory.
DOCS_DIR := docs

# Build directories from older project layouts, removed by `make clean`.
LEGACY_BUILD_DIRS := build-release build-tests build-sanitize build-sanitize-tests \
	build-thread-sanitize build-thread-sanitize-tests build-gcc build-release-gcc \
	build-tests-gcc build-sanitize-gcc build-sanitize-tests-gcc \
	build-thread-sanitize-gcc build-thread-sanitize-tests-gcc .last_build_dir

# -------------------------------- Default Target --------------------------------- #

.DEFAULT_GOAL := all

.PHONY: all configure build run test compile-commands symlink \
	debug release sanitize thread-sanitize gcc \
	configure-clang configure-gcc configure-debug configure-release configure-sanitize \
	reconfigure clean clean-clang clean-gcc clean-sanitize clean-thread clean-tests \
	check docs docs-open docs-clean help

# --------------------------------- Main Targets ---------------------------------- #

all: build compile-commands
	@echo ""
	@echo "✓ Built preset '$(PRESET)' in $(BUILD_DIR)."
	@echo "  Run 'make run' to execute $(TARGET), or 'make help' for all commands."
	@echo ""

# Configure the selected preset.
configure:
	cmake --preset $(PRESET)

# Build the selected preset. Configuring first is a no-op when already up to date.
build: configure
	cmake --build --preset $(BUILD_PRESET)

# Build and run a demo executable from the selected preset's bin directory.
run: build
	@if [ ! -x "$(BIN_DIR)/$(TARGET)" ]; then \
		echo "Error: $(BIN_DIR)/$(TARGET) not found."; \
		echo "Choose another demo with TARGET=<name>, e.g. make run TARGET=test_AVL_Tree"; \
		exit 1; \
	fi
	@echo ""
	@echo "Running $(TARGET) ($(PRESET))..."
	@echo ""
	@"$(BIN_DIR)/$(TARGET)"

# Configure, build and run the GoogleTest suite via CTest.
test:
	cmake --preset debug-tests
	cmake --build --preset build-debug-tests
	ctest --preset test-debug

# ------------------------------ Configuration Aliases ---------------------------- #

# Each alias pins PRESET and reuses the generic targets above.
debug:           ; @$(MAKE) --no-print-directory build PRESET=debug
release:         ; @$(MAKE) --no-print-directory build PRESET=release
sanitize:        ; @$(MAKE) --no-print-directory build PRESET=sanitize
thread-sanitize: ; @$(MAKE) --no-print-directory build PRESET=thread-sanitize
gcc:             ; @$(MAKE) --no-print-directory build PRESET=debug-gcc

# Backwards-compatible configure-only targets.
configure-clang:    ; @$(MAKE) --no-print-directory configure PRESET=debug
configure-gcc:      ; @$(MAKE) --no-print-directory configure PRESET=debug-gcc
configure-debug:    ; @$(MAKE) --no-print-directory configure PRESET=debug
configure-release:  ; @$(MAKE) --no-print-directory configure PRESET=release
configure-sanitize: ; @$(MAKE) --no-print-directory configure PRESET=sanitize

reconfigure: clean configure
	@echo "✓ Reconfiguration complete!"

# -------------------------------- Utility Targets -------------------------------- #

# Symlink the active preset's compile_commands.json into the project root for clangd.
symlink: compile-commands
compile-commands:
	@if [ -f "$(BUILD_DIR)/compile_commands.json" ]; then \
		ln -sf "$(BUILD_DIR)/compile_commands.json" compile_commands.json; \
		echo "✓ Linked compile_commands.json from $(BUILD_DIR)"; \
	else \
		echo "⚠ $(BUILD_DIR)/compile_commands.json not found (configure first)"; \
	fi

# Verify the required toolchain and preset files are present.
check:
	@for f in clang-toolchain.cmake gcc-toolchain.cmake CMakePresets.json; do \
		if [ -f "$$f" ]; then echo "✓ $$f found"; else echo "✗ $$f missing"; exit 1; fi; \
	done
	@echo "✓ All required files present"

# -------------------------------- Cleanup Targets -------------------------------- #

clean:
	@echo "Removing all build directories..."
	@rm -rf build $(LEGACY_BUILD_DIRS) compile_commands.json
	@echo "✓ All build directories cleaned"

clean-clang:
	@rm -rf build/clang
	@echo "✓ Clang build directories cleaned"

clean-gcc:
	@rm -rf build/gcc
	@echo "✓ GCC build directories cleaned"

clean-sanitize:
	@rm -rf build/*/sanitize build/*/sanitize-tests
	@echo "✓ Sanitizer build directories cleaned"

clean-thread:
	@rm -rf build/*/thread-sanitize build/*/thread-sanitize-tests
	@echo "✓ Thread-sanitizer build directories cleaned"

clean-tests:
	@rm -rf build/*/*-tests
	@echo "✓ Test-enabled build directories cleaned"

# --------------------------------- Documentation --------------------------------- #

docs:
	@if ! command -v doxygen >/dev/null 2>&1; then \
		echo "Error: Doxygen is not installed (e.g. brew install doxygen graphviz)."; \
		exit 1; \
	fi
	doxygen Doxyfile
	@echo "✓ Documentation generated in $(DOCS_DIR)/html/ (open index.html)"

docs-open: docs
	@if command -v open >/dev/null 2>&1; then open $(DOCS_DIR)/html/index.html; \
	elif command -v xdg-open >/dev/null 2>&1; then xdg-open $(DOCS_DIR)/html/index.html; \
	else echo "Open $(DOCS_DIR)/html/index.html manually"; fi

docs-clean:
	@rm -rf $(DOCS_DIR)
	@echo "✓ Documentation cleaned"

# ---------------------------- Help and Documentation ----------------------------- #

help:
	@echo ""
	@echo "═════════════════ AlgoDataStruct - Makefile Commands ═════════════════"
	@echo ""
	@echo "Main:"
	@echo "  make                   - Configure + build the default preset (debug)"
	@echo "  make build  [PRESET=p] - Configure + build preset p (default: debug)"
	@echo "  make run    [TARGET=t] - Build and run demo t (default: $(TARGET))"
	@echo "  make test              - Configure, build and run the GoogleTest suite"
	@echo "  make reconfigure       - Clean and reconfigure"
	@echo ""
	@echo "Configuration aliases (set PRESET for you):"
	@echo "  make debug             - Build preset 'debug'   (Clang, Debug)"
	@echo "  make release           - Build preset 'release' (Clang, Release)"
	@echo "  make sanitize          - Build preset 'sanitize' (ASan + UBSan)"
	@echo "  make thread-sanitize   - Build preset 'thread-sanitize' (TSan)"
	@echo "  make gcc               - Build preset 'debug-gcc' (GCC, Debug)"
	@echo ""
	@echo "Cleanup:"
	@echo "  make clean             - Remove all build directories"
	@echo "  make clean-clang|clean-gcc|clean-sanitize|clean-thread|clean-tests"
	@echo ""
	@echo "Utility:"
	@echo "  make compile-commands  - Symlink compile_commands.json into the root"
	@echo "  make check             - Verify toolchain and preset files"
	@echo ""
	@echo "Documentation:"
	@echo "  make docs|docs-open|docs-clean"
	@echo ""
	@echo "Any preset from CMakePresets.json works via PRESET=<name>, e.g."
	@echo "  make build PRESET=debug-tests   make run PRESET=sanitize TARGET=test_AVL_Tree"
	@echo ""
	@echo "══════════════════════════════════════════════════════════════════════"
	@echo ""

# ================================================================================= #
# End of Makefile.
