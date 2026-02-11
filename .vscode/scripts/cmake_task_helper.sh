#!/usr/bin/env sh
# ============================================================================ #
# ------------------------- CMake Task Helper Script ------------------------- #
# ============================================================================ #
#
# Helper script for VSCode tasks to manage CMake builds and configurations.
# It handles different build profiles, toolchains, and sanitizers.
#
# Usage:
#   ./.vscode/scripts/cmake_task_helper.sh [MODE] [ARGS...]
#
# Modes:
#   build        Configures (if needed) and builds the project.
#   reconfigure  Forces a reconfiguration of the project.
#
# Author: Costantino Lombardi
# Date: 2026-02-11
# ============================================================================ #

set -eu

# ================================ UTILITIES ================================= #

# Prints usage instructions to stdout.
usage() {
  cat <<'EOF'
Usage:
  cmake_task_helper.sh build <toolchain> <config> <enable_testing> <jobs> <link_current>
  cmake_task_helper.sh reconfigure <toolchain> <config> <enable_testing>
EOF
}

# Generates a build profile name based on config and testing flag.
# Returns: Profile name (e.g., "debug", "sanitize-tests").
profile_from_config() {
  config="$1"
  enable_testing="$2"

  case "${config}:${enable_testing}" in
    Debug:OFF) echo "debug" ;;
    Debug:ON) echo "debug-tests" ;;
    Release:OFF) echo "release" ;;
    Sanitize:OFF) echo "sanitize" ;;
    Sanitize:ON) echo "sanitize-tests" ;;
    ThreadSanitize:OFF) echo "thread-sanitize" ;;
    ThreadSanitize:ON) echo "thread-sanitize-tests" ;;
    *)
      echo "Unsupported config/testing combination: ${config}:${enable_testing}" >&2
      exit 2
      ;;
  esac
}

# Ensures the toolchain is supported (clang or gcc).
validate_toolchain() {
  toolchain="$1"
  case "$toolchain" in
    clang|gcc) ;;
    *)
      echo "Unsupported toolchain: ${toolchain}. Expected clang or gcc." >&2
      exit 2
      ;;
  esac
}

# Prevents using GCC sanitizers on macOS (not supported yet).
guard_gcc_sanitizers_on_macos() {
  toolchain="$1"
  config="$2"

  if [ "$toolchain" = "gcc" ] && [ "$(uname)" = "Darwin" ]; then
    case "$config" in
      Sanitize)
        echo "GCC sanitizer builds are not supported on macOS in this setup." >&2
        echo "Use toolchain=clang for sanitizer debug sessions." >&2
        exit 2
        ;;
      ThreadSanitize)
        echo "GCC thread sanitizer builds are not supported on macOS in this setup." >&2
        echo "Use toolchain=clang for TSan debug sessions." >&2
        exit 2
        ;;
    esac
  fi
}

# =============================== BUILD LOGIC ================================ #

# Runs CMake configuration if the build directory or CMakeCache.txt is missing.
configure_if_missing() {
  toolchain="$1"
  config="$2"
  enable_testing="$3"
  build_dir="$4"

  if [ ! -f "${build_dir}/CMakeCache.txt" ]; then
    cmake \
      -DCMAKE_TOOLCHAIN_FILE="${toolchain}-toolchain.cmake" \
      -DCMAKE_BUILD_TYPE="${config}" \
      -DENABLE_TESTING="${enable_testing}" \
      -B "${build_dir}"
  fi
}

# Cleans the build directory and re-runs CMake configuration.
# Used when toolchain or configuration options change significantly.
reconfigure_dir() {
  toolchain="$1"
  config="$2"
  enable_testing="$3"
  build_dir="$4"

  cmake -E remove_directory "${build_dir}"
  cmake \
    -DCMAKE_TOOLCHAIN_FILE="${toolchain}-toolchain.cmake" \
    -DCMAKE_BUILD_TYPE="${config}" \
    -DENABLE_TESTING="${enable_testing}" \
    -B "${build_dir}"
}

# Updates the 'build/current' symlink to point to the active toolchain build.
# This helps debuggers (like lldb) find the correct build artifacts.
update_current_symlink() {
  toolchain="$1"
  link_current="$2"

  if [ "${link_current}" = "ON" ]; then
    cmake -E make_directory build
    cmake -E rm -f build/current
    cmake -E create_symlink "${toolchain}" build/current
  fi
}

# =============================== MAIN SCRIPT ================================ #

if [ "$#" -lt 1 ]; then
  usage
  exit 2
fi

mode="$1"
shift

case "$mode" in
  build)
    if [ "$#" -ne 5 ]; then
      usage
      exit 2
    fi

    toolchain="$1"
    config="$2"
    enable_testing="$3"
    jobs="$4"
    link_current="$5"

    validate_toolchain "$toolchain"
    guard_gcc_sanitizers_on_macos "$toolchain" "$config"

    profile="$(profile_from_config "$config" "$enable_testing")"
    build_dir="build/${toolchain}/${profile}"

    configure_if_missing "$toolchain" "$config" "$enable_testing" "$build_dir"
    cmake --build "$build_dir" -j"$jobs"
    update_current_symlink "$toolchain" "$link_current"
    ;;
  reconfigure)
    if [ "$#" -ne 3 ]; then
      usage
      exit 2
    fi

    toolchain="$1"
    config="$2"
    enable_testing="$3"

    validate_toolchain "$toolchain"
    guard_gcc_sanitizers_on_macos "$toolchain" "$config"

    profile="$(profile_from_config "$config" "$enable_testing")"
    build_dir="build/${toolchain}/${profile}"
    reconfigure_dir "$toolchain" "$config" "$enable_testing" "$build_dir"
    ;;
  *)
    usage
    exit 2
    ;;
esac

# ============================================================================ #
# End of script.
