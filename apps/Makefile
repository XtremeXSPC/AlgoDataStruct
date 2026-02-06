# ================================================================================= #
# --------------------------- Makefile for Snake Apps ----------------------------- #
# ================================================================================= #
#
# This Makefile provides convenient commands to configure and run the applications
# under apps/, with a workflow consistent with the root Makefile.
#
# Main commands:
#   make                  - Configure with Clang and build Snake targets (default)
#   make build            - Build Snake targets (requires configure first)
#   make run              - Build and run app_Snake_TUI
#   make stress           - Build and run app_Snake_Stress
#   make test             - Configure with testing and run runSnakeTests
#   make clean            - Remove the shared build directory (../build)
#
# ================================================================================= #

# ---------------------------- Configuration Variables ---------------------------- #

# Root project directory (one level above apps/).
PROJECT_ROOT := ..

# Default toolchain/build settings.
DEFAULT_BUILD_TYPE := Debug
CLANG_TOOLCHAIN := $(PROJECT_ROOT)/clang-toolchain.cmake
GCC_TOOLCHAIN := $(PROJECT_ROOT)/gcc-toolchain.cmake

# Shared build directory used by the root project.
BUILD_DIR := $(PROJECT_ROOT)/build

# Snake targets and executables.
APP_TARGET := app_Snake_TUI
STRESS_TARGET := app_Snake_Stress
TEST_TARGET := runSnakeTests

APP_EXECUTABLE := $(BUILD_DIR)/bin/$(APP_TARGET)
STRESS_EXECUTABLE := $(BUILD_DIR)/bin/$(STRESS_TARGET)
TEST_EXECUTABLE := $(BUILD_DIR)/bin/$(TEST_TARGET)

# Runtime defaults (override from CLI, e.g. make stress STRESS_EPISODES=500).
RUN_SEED ?= 1337
RUN_MAX_TICKS ?= 500
STRESS_EPISODES ?= 250
STRESS_MAX_TICKS ?= 2000
STRESS_SEED ?= 1337

# -------------------------------- Default Target --------------------------------- #
.DEFAULT_GOAL := all

.PHONY: all build run stress clean configure configure-clang configure-gcc \
	configure-debug configure-release configure-sanitize reconfigure test sanitize \
	help check run-tui run-stress

# --------------------------------- Main Targets ---------------------------------- #
all: configure-clang build
	@echo ""
	@echo "✓ Snake apps configured and built successfully!"
	@echo ""
	@echo "Run 'make run' to start Snake TUI"
	@echo "Run 'make stress' to launch stress workload"
	@echo "Run 'make help' to see all available commands"
	@echo ""

build:
	@if [ ! -d "$(BUILD_DIR)" ]; then \
		echo "Error: Build directory not found."; \
		echo "Run 'make configure' or 'make configure-clang' first"; \
		exit 1; \
	fi
	@echo "Building Snake targets..."
	cmake --build $(BUILD_DIR) --target $(APP_TARGET) $(STRESS_TARGET)

run-tui: run

run: build
	@echo ""
	@echo "Running $(APP_TARGET) (seed=$(RUN_SEED), max_ticks=$(RUN_MAX_TICKS))..."
	@echo ""
	@if [ ! -f "$(APP_EXECUTABLE)" ]; then \
		echo "Error: $(APP_EXECUTABLE) not found."; \
		echo "Run 'make build' first"; \
		exit 1; \
	fi
	@$(APP_EXECUTABLE) $(RUN_SEED) $(RUN_MAX_TICKS)

run-stress: stress

stress: build
	@echo ""
	@echo "Running $(STRESS_TARGET) (episodes=$(STRESS_EPISODES), max_ticks=$(STRESS_MAX_TICKS), seed=$(STRESS_SEED))..."
	@echo ""
	@if [ ! -f "$(STRESS_EXECUTABLE)" ]; then \
		echo "Error: $(STRESS_EXECUTABLE) not found."; \
		echo "Run 'make build' first"; \
		exit 1; \
	fi
	@$(STRESS_EXECUTABLE) $(STRESS_EPISODES) $(STRESS_MAX_TICKS) $(STRESS_SEED)

clean:
	@echo "Removing shared build directory..."
	rm -rf $(BUILD_DIR)
	rm -f $(PROJECT_ROOT)/compile_commands.json
	@echo "✓ Build directory cleaned"

# ----------------------------- Configuration Targets ----------------------------- #
configure-clang:
	@echo "Configuring Snake apps with Clang toolchain (recommended)..."
	cmake -DCMAKE_TOOLCHAIN_FILE=$(CLANG_TOOLCHAIN) \
				-DENABLE_APPS=ON \
				-DENABLE_SNAKE_APP=ON \
				-DCMAKE_BUILD_TYPE=$(DEFAULT_BUILD_TYPE) \
				-S $(PROJECT_ROOT) -B $(BUILD_DIR)
	@echo ""
	@echo "✓ Configuration complete!"
	@echo "  Compiler: Clang"
	@echo "  Build Type: $(DEFAULT_BUILD_TYPE)"
	@echo ""

configure-gcc:
	@echo "Configuring Snake apps with GCC toolchain..."
	cmake -DCMAKE_TOOLCHAIN_FILE=$(GCC_TOOLCHAIN) \
				-DENABLE_APPS=ON \
				-DENABLE_SNAKE_APP=ON \
				-DCMAKE_BUILD_TYPE=$(DEFAULT_BUILD_TYPE) \
				-S $(PROJECT_ROOT) -B $(BUILD_DIR)
	@echo ""
	@echo "✓ Configuration complete!"
	@echo "  Compiler: GCC"
	@echo "  Build Type: $(DEFAULT_BUILD_TYPE)"
	@echo ""

configure-debug:
	@echo "Configuring Debug build with Clang..."
	cmake -DCMAKE_TOOLCHAIN_FILE=$(CLANG_TOOLCHAIN) \
				-DENABLE_APPS=ON \
				-DENABLE_SNAKE_APP=ON \
				-DCMAKE_BUILD_TYPE=Debug \
				-S $(PROJECT_ROOT) -B $(BUILD_DIR)
	@echo ""
	@echo "✓ Debug configuration complete!"
	@echo ""

configure-release:
	@echo "Configuring Release build with Clang..."
	cmake -DCMAKE_TOOLCHAIN_FILE=$(CLANG_TOOLCHAIN) \
				-DENABLE_APPS=ON \
				-DENABLE_SNAKE_APP=ON \
				-DCMAKE_BUILD_TYPE=Release \
				-S $(PROJECT_ROOT) -B $(BUILD_DIR)
	@echo ""
	@echo "✓ Release configuration complete!"
	@echo ""

configure-sanitize:
	@echo "Configuring Sanitize build with Clang..."
	cmake -DCMAKE_TOOLCHAIN_FILE=$(CLANG_TOOLCHAIN) \
	      -DENABLE_APPS=ON \
	      -DENABLE_SNAKE_APP=ON \
	      -DCMAKE_BUILD_TYPE=Sanitize \
	      -S $(PROJECT_ROOT) -B $(BUILD_DIR)
	@echo ""
	@echo "✓ Sanitize configuration complete!"
	@echo "  Address Sanitizer: Enabled"
	@echo "  UB Sanitizer: Enabled"
	@echo ""

configure: configure-clang

reconfigure: clean configure-clang
	@echo "✓ Reconfiguration complete!"

# ------------------------------------ Testing ------------------------------------ #
test:
	@echo "Configuring with Snake tests enabled..."
	cmake -DCMAKE_TOOLCHAIN_FILE=$(CLANG_TOOLCHAIN) \
				-DENABLE_APPS=ON \
				-DENABLE_SNAKE_APP=ON \
				-DENABLE_TESTING=ON \
				-DCMAKE_BUILD_TYPE=Debug \
				-S $(PROJECT_ROOT) -B $(BUILD_DIR)
	@echo "Building Snake test target..."
	cmake --build $(BUILD_DIR) --target $(TEST_TARGET)
	@echo ""
	@echo "Running $(TEST_TARGET)..."
	@echo "========================="
	@if [ ! -f "$(TEST_EXECUTABLE)" ]; then \
		echo "Error: $(TEST_EXECUTABLE) not found."; \
		echo "Ensure ENABLE_TESTING=ON and rebuild"; \
		exit 1; \
	fi
	@$(TEST_EXECUTABLE)

sanitize: clean configure-sanitize build
	@echo ""
	@echo "Build with sanitizers complete!"
	@echo "Run 'make run' or 'make stress' to execute with sanitizers enabled"

# -------------------------------- Utility Targets -------------------------------- #
check:
	@echo "Checking required files..."
	@if [ ! -f "$(CLANG_TOOLCHAIN)" ]; then \
		echo "✗ clang-toolchain.cmake not found at $(CLANG_TOOLCHAIN)!"; \
		exit 1; \
	else \
		echo "✓ clang-toolchain.cmake found"; \
	fi
	@if [ ! -f "$(GCC_TOOLCHAIN)" ]; then \
		echo "✗ gcc-toolchain.cmake not found at $(GCC_TOOLCHAIN)!"; \
		exit 1; \
	else \
		echo "✓ gcc-toolchain.cmake found"; \
	fi
	@if [ ! -f "$(PROJECT_ROOT)/CMakeLists.txt" ]; then \
		echo "✗ Root CMakeLists.txt not found at $(PROJECT_ROOT)/CMakeLists.txt!"; \
		exit 1; \
	else \
		echo "✓ Root CMakeLists.txt found"; \
	fi
	@echo "✓ All required files present"

# ---------------------------- Help and Documentation ----------------------------- #
help:
	@echo ""
	@echo "╔═══════════════════════════════════════════════════════════════╗"
	@echo "║                Snake Apps - Makefile Commands                 ║"
	@echo "╚═══════════════════════════════════════════════════════════════╝"
	@echo ""
	@echo "Main Commands:"
	@echo "  make              - Configure with Clang and build (default)"
	@echo "  make build        - Build app_Snake_TUI and app_Snake_Stress"
	@echo "  make run          - Build and run app_Snake_TUI"
	@echo "  make stress       - Build and run app_Snake_Stress"
	@echo "  make test         - Configure with testing and run runSnakeTests"
	@echo "  make clean        - Remove shared build directory (../build)"
	@echo "  make reconfigure  - Clean and reconfigure with Clang"
	@echo ""
	@echo "Specific Configuration:"
	@echo "  make configure-clang   - Configure with Clang (recommended)"
	@echo "  make configure-gcc     - Configure with GCC"
	@echo "  make configure-debug   - Configure Debug build"
	@echo "  make configure-release - Configure Release build"
	@echo "  make configure-sanitize - Configure Sanitize build"
	@echo ""
	@echo "Runtime Variables (override from CLI):"
	@echo "  RUN_SEED, RUN_MAX_TICKS"
	@echo "  STRESS_EPISODES, STRESS_MAX_TICKS, STRESS_SEED"
	@echo ""
	@echo "Examples:"
	@echo "  make                                # Initial setup"
	@echo "  make run RUN_SEED=42 RUN_MAX_TICKS=300"
	@echo "  make stress STRESS_EPISODES=500 STRESS_MAX_TICKS=1500"
	@echo "  make test"
	@echo ""
	@echo "Notes:"
	@echo "  - This Makefile configures the root project from apps/"
	@echo "  - 'make clean' removes ../build and ../compile_commands.json"
	@echo ""
	@echo "═════════════════════════════════════════════════════════════════"
	@echo ""

# ================================================================================= #
# End of Makefile.
