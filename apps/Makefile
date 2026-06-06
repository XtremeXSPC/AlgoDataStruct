# ================================================================================= #
# ---------------------------- Makefile for Snake Apps ---------------------------- #
# ================================================================================= #
#
# Convenience wrapper for the Snake showcase, consistent with the root Makefile.
# It delegates configuration/build to the project's CMake presets (run from the
# project root) and only builds/runs the Snake targets.
#
# Main commands:
#   make            - Configure with the 'debug' preset and build Snake targets
#   make run        - Build and run app_Snake_TUI
#   make stress     - Build and run app_Snake_Stress
#   make test       - Build and run runSnakeTests via the 'debug-tests' preset
#   make clean      - Remove the shared build tree (../build)
#   make help       - List every command
#
# Use PRESET=<name> to target another preset (e.g. make run PRESET=sanitize).
#
# ================================================================================= #

# ---------------------------- Configuration Variables ---------------------------- #

PROJECT_ROOT := $(abspath ..)

# Configure preset (presets live in the project root). Build preset and binary
# directory are derived from it, mirroring the root Makefile.
PRESET ?= debug
BUILD_PRESET  := build-$(PRESET)
TOOLCHAIN_DIR := $(if $(filter %-gcc,$(PRESET)),gcc,clang)
CONFIG_DIR    := $(patsubst %-gcc,%,$(PRESET))
BIN_DIR       := $(PROJECT_ROOT)/build/$(TOOLCHAIN_DIR)/$(CONFIG_DIR)/bin

# Snake targets.
APP_TARGET    := app_Snake_TUI
STRESS_TARGET := app_Snake_Stress
TEST_TARGET   := runSnakeTests

# Runtime defaults (override from CLI, e.g. make stress STRESS_EPISODES=500).
RUN_SEED         ?= 1337
RUN_MAX_TICKS    ?= 500
STRESS_EPISODES  ?= 250
STRESS_MAX_TICKS ?= 2000
STRESS_SEED      ?= 1337

# -------------------------------- Default Target --------------------------------- #
.DEFAULT_GOAL := all

.PHONY: all configure build run run-tui stress run-stress test clean reconfigure \
	check help

# --------------------------------- Main Targets ---------------------------------- #
all: build
	@echo ""
	@echo "✓ Snake apps built (preset '$(PRESET)')."
	@echo "  Run 'make run' for the TUI, 'make stress' for the workload, 'make help' for more."
	@echo ""

# CMake locates presets relative to the working directory, so configure and build
# are run from the project root (which owns CMakePresets.json).
configure:
	cd $(PROJECT_ROOT) && cmake --preset $(PRESET)

build: configure
	cd $(PROJECT_ROOT) && cmake --build --preset $(BUILD_PRESET) --target $(APP_TARGET) $(STRESS_TARGET)

run-tui: run
run: build
	@if [ ! -x "$(BIN_DIR)/$(APP_TARGET)" ]; then \
		echo "Error: $(BIN_DIR)/$(APP_TARGET) not found."; exit 1; \
	fi
	@echo ""
	@echo "Running $(APP_TARGET) (seed=$(RUN_SEED), max_ticks=$(RUN_MAX_TICKS))..."
	@echo ""
	@"$(BIN_DIR)/$(APP_TARGET)" $(RUN_SEED) $(RUN_MAX_TICKS)

run-stress: stress
stress: build
	@if [ ! -x "$(BIN_DIR)/$(STRESS_TARGET)" ]; then \
		echo "Error: $(BIN_DIR)/$(STRESS_TARGET) not found."; exit 1; \
	fi
	@echo ""
	@echo "Running $(STRESS_TARGET) (episodes=$(STRESS_EPISODES), max_ticks=$(STRESS_MAX_TICKS), seed=$(STRESS_SEED))..."
	@echo ""
	@"$(BIN_DIR)/$(STRESS_TARGET)" $(STRESS_EPISODES) $(STRESS_MAX_TICKS) $(STRESS_SEED)

test:
	cd $(PROJECT_ROOT) && cmake --preset debug-tests
	cd $(PROJECT_ROOT) && cmake --build --preset build-debug-tests --target $(TEST_TARGET)
	@TEST_EXE="$(PROJECT_ROOT)/build/clang/debug-tests/bin/$(TEST_TARGET)"; \
	if [ ! -x "$$TEST_EXE" ]; then echo "Error: $$TEST_EXE not found."; exit 1; fi; \
	echo ""; echo "Running $(TEST_TARGET)..."; echo "========================="; \
	"$$TEST_EXE"

reconfigure: clean configure
	@echo "✓ Reconfiguration complete!"

clean:
	@echo "Removing shared build tree..."
	@rm -rf $(PROJECT_ROOT)/build $(PROJECT_ROOT)/compile_commands.json
	@echo "✓ Build directories cleaned"

# -------------------------------- Utility Targets -------------------------------- #
check:
	@for f in "$(PROJECT_ROOT)/clang-toolchain.cmake" "$(PROJECT_ROOT)/gcc-toolchain.cmake" \
		"$(PROJECT_ROOT)/CMakePresets.json" "$(PROJECT_ROOT)/CMakeLists.txt"; do \
		if [ -f "$$f" ]; then echo "✓ $$f found"; else echo "✗ $$f missing"; exit 1; fi; \
	done
	@echo "✓ All required files present"

# ---------------------------- Help and Documentation ----------------------------- #
help:
	@echo ""
	@echo "═══════════════════ Snake Apps - Makefile Commands ═══════════════════"
	@echo ""
	@echo "Main:"
	@echo "  make                   - Configure + build Snake targets (preset debug)"
	@echo "  make build [PRESET=p]  - Build Snake targets with preset p"
	@echo "  make run               - Build and run app_Snake_TUI"
	@echo "  make stress            - Build and run app_Snake_Stress"
	@echo "  make test              - Build and run runSnakeTests (preset debug-tests)"
	@echo "  make clean             - Remove the shared build tree (../build)"
	@echo "  make reconfigure       - Clean and reconfigure"
	@echo ""
	@echo "Runtime variables (override from CLI):"
	@echo "  RUN_SEED, RUN_MAX_TICKS, STRESS_EPISODES, STRESS_MAX_TICKS, STRESS_SEED"
	@echo ""
	@echo "Examples:"
	@echo "  make run RUN_SEED=42 RUN_MAX_TICKS=300"
	@echo "  make stress STRESS_EPISODES=500 STRESS_MAX_TICKS=1500"
	@echo "  make run PRESET=sanitize"
	@echo ""
	@echo "══════════════════════════════════════════════════════════════════════"
	@echo ""

# ================================================================================= #
# End of Makefile.
