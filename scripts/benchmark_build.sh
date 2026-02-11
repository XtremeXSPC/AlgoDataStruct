#!/usr/bin/env bash
# ============================================================================ #
# -------------------- Build Performance Benchmark Script -------------------- #
# ============================================================================ #
#
# Measures compilation time across different build optimization configurations:
#   1. Baseline (no optimizations)
#   2. PCH only
#   3. Unity Build only
#   4. PCH + Unity Build (full optimization)
#
# Usage:
#   ./scripts/benchmark_build.sh [OPTIONS]
#
# Options:
#   --clean        Force clean rebuild for all configurations
#   --tests        Benchmark test suite compilation only
#   --parallel N   Use N parallel jobs (default: CPU count)
#   --help         Display this help message
#
# Output:
#   Creates benchmark_results_<timestamp>.txt with timing data
#
# Requirements:
#   - CMake >= 3.20
#   - Time utility (GNU time preferred, BSD time supported)
#   - Toolchain file (clang-toolchain.cmake or gcc-toolchain.cmake)
#
# Author: Costantino Lombardi
# Date: 2026-02-11
# ============================================================================ #

set -euo pipefail

# ============================== CONFIGURATION =============================== #

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
TOOLCHAIN_FILE="${PROJECT_ROOT}/clang-toolchain.cmake"
COMMON_SCRIPT="${SCRIPT_DIR}/script_common.sh"

# Detect CPU count for parallel builds.
if command -v nproc &> /dev/null; then
    DEFAULT_JOBS=$(nproc)
elif command -v sysctl &> /dev/null; then
    DEFAULT_JOBS=$(sysctl -n hw.ncpu)
else
    DEFAULT_JOBS=4
fi

PARALLEL_JOBS=${DEFAULT_JOBS}
FORCE_CLEAN=false
TEST_ONLY=false
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
RESULTS_FILE="${PROJECT_ROOT}/benchmark_results_${TIMESTAMP}.txt"
DETECTED_COMPILER="Unknown"
DETECTED_COMPILER_PATH="Unknown"

if [[ ! -f "${COMMON_SCRIPT}" ]]; then
    echo "Error: shared script not found: ${COMMON_SCRIPT}" >&2
    exit 1
fi

# shellcheck disable=SC1090
source "${COMMON_SCRIPT}"

# ================================ UTILITIES ================================= #

print_result() {
    local config="$1"
    local time_sec="$2"
    echo -e "${GREEN}✓${RESET} ${BOLD}${config}${RESET}: ${time_sec}s"
}

# Portable high-resolution epoch seconds (macOS date lacks %N).
if date +%s.%N 2>/dev/null | grep -qv 'N'; then
    get_epoch_seconds() { date +%s.%N; }
else
    get_epoch_seconds() { perl -MTime::HiRes=time -e 'printf "%.3f\n", time'; }
fi

read_cache_value() {
    local cache_file="$1"
    local key="$2"
    local line
    line=$(grep -E "^${key}:[^=]*=" "${cache_file}" 2>/dev/null | head -n 1 || true)
    if [[ -z "${line}" ]]; then
        return 1
    fi
    echo "${line#*=}"
}

cache_matches_toolchain() {
    local cache_file="$1"
    local cached_toolchain
    cached_toolchain=$(read_cache_value "${cache_file}" "CMAKE_TOOLCHAIN_FILE" || true)
    if [[ -z "${cached_toolchain}" ]]; then
        return 1
    fi
    [[ "$(basename "${cached_toolchain}")" == "$(basename "${TOOLCHAIN_FILE}")" ]]
}

detect_compiler_identity() {
    local cache_file=""
    local build_dirs=(
        "${PROJECT_ROOT}/build-baseline"
        "${PROJECT_ROOT}/build-pch"
        "${PROJECT_ROOT}/build-unity"
        "${PROJECT_ROOT}/build-optimized"
    )

    for build_dir in "${build_dirs[@]}"; do
        local candidate="${build_dir}/CMakeCache.txt"
        if [[ -f "${candidate}" ]] && cache_matches_toolchain "${candidate}"; then
            cache_file="${candidate}"
            break
        fi
    done

    local probe_dir=""
    if [[ -z "${cache_file}" ]]; then
        probe_dir="${PROJECT_ROOT}/build-benchmark-probe"
        cmake -DCMAKE_TOOLCHAIN_FILE="${TOOLCHAIN_FILE}" \
              -DENABLE_TESTING=ON \
              -B "${probe_dir}" \
              -S "${PROJECT_ROOT}" > /dev/null 2>&1 || return 1
        cache_file="${probe_dir}/CMakeCache.txt"
    fi

    local compiler_path
    compiler_path=$(read_cache_value "${cache_file}" "CMAKE_CXX_COMPILER" || true)

    # Clean up probe directory if we created one.
    if [[ -n "${probe_dir}" ]] && [[ -d "${probe_dir}" ]]; then
        rm -rf "${probe_dir}"
    fi

    if [[ -z "${compiler_path}" ]]; then
        return 1
    fi

    DETECTED_COMPILER_PATH="${compiler_path}"
    if [[ -x "${compiler_path}" ]]; then
        DETECTED_COMPILER="$("${compiler_path}" --version 2>/dev/null | head -n 1)"
    else
        DETECTED_COMPILER="${compiler_path}"
    fi
}

show_usage() {
    cat <<'USAGE'
Build Performance Benchmark Script

Measures compilation time across different build optimization configurations:
  1. Baseline (no optimizations)
  2. PCH only
  3. Unity Build only
  4. PCH + Unity Build (full optimization)

Usage:
  ./scripts/benchmark_build.sh [OPTIONS]

Options:
  --clean        Force clean rebuild for all configurations
  --tests        Benchmark test suite compilation only
  --parallel N   Use N parallel jobs (default: CPU count)
  --help         Display this help message
USAGE
}

# Parse command-line arguments.
parse_args() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            --clean)
                FORCE_CLEAN=true
                shift
                ;;
            --tests)
                TEST_ONLY=true
                shift
                ;;
            --parallel)
                if [[ $# -lt 2 ]] || [[ "$2" == --* ]]; then
                    print_error "--parallel requires a numeric argument"
                    exit 1
                fi
                if ! [[ "$2" =~ ^[0-9]+$ ]]; then
                    print_error "--parallel value must be a positive integer, got: $2"
                    exit 1
                fi
                PARALLEL_JOBS="$2"
                shift 2
                ;;
            --help)
                show_usage
                exit 0
                ;;
            *)
                print_error "Error: Unknown option: $1"
                echo "Use --help for usage information"
                exit 1
                ;;
        esac
    done
}

# ============================= BENCHMARK LOGIC ============================== #

# Measure build time for a given configuration.
# Usage: benchmark_config <name> <build_dir> <target> [cmake_flags...]
benchmark_config() {
    local config_name="$1"
    local build_dir="$2"
    local target="$3"
    shift 3
    local cmake_flags=("$@")

    print_section "Configuration: ${config_name}" "${CYAN}" true

    # Configure.
    if [[ ! -d "${build_dir}" ]] || [[ "${FORCE_CLEAN}" == "true" ]]; then
        echo "Configuring..."
        rm -rf "${build_dir}"
        cmake -DCMAKE_TOOLCHAIN_FILE="${TOOLCHAIN_FILE}" \
              -DENABLE_TESTING=ON \
              ${cmake_flags[@]+"${cmake_flags[@]}"} \
              -B "${build_dir}" \
              -S "${PROJECT_ROOT}" > /dev/null 2>&1
    else
        echo "Using existing configuration (use --clean to reconfigure)"
    fi

    # Build and measure time.
    echo "Building with -j${PARALLEL_JOBS}..."
    local start_time
    start_time=$(get_epoch_seconds)

    if [[ "${target}" == "all" ]]; then
        cmake --build "${build_dir}" --clean-first -j "${PARALLEL_JOBS}" > /dev/null 2>&1
    else
        cmake --build "${build_dir}" --clean-first --target "${target}" -j "${PARALLEL_JOBS}" > /dev/null 2>&1
    fi

    local end_time
    end_time=$(get_epoch_seconds)
    local elapsed
    elapsed=$(echo "${end_time} - ${start_time}" | bc)

    print_result "${config_name}" "${elapsed}"

    # Log to results file.
    echo "${config_name}: ${elapsed}s" >> "${RESULTS_FILE}"

    return 0
}

# =============================== MAIN SCRIPT ================================ #

main() {
    parse_args "$@"

    cd "${PROJECT_ROOT}"

    print_header "AlgoDataStruct Build Performance Benchmark" "${BLUE}"
    detect_compiler_identity || true

    echo ""
    echo "Configuration:"
    echo "  Project Root : ${PROJECT_ROOT}"
    echo "  Toolchain    : ${TOOLCHAIN_FILE}"
    echo "  Compiler     : ${DETECTED_COMPILER}"
    echo "  Compiler Path: ${DETECTED_COMPILER_PATH}"
    echo "  Parallel Jobs: ${PARALLEL_JOBS}"
    echo "  Force Clean  : ${FORCE_CLEAN}"
    echo "  Test Only    : ${TEST_ONLY}"
    echo "  Results File : ${RESULTS_FILE}"
    echo ""

    # Initialize results file.
    {
        echo "AlgoDataStruct Build Performance Benchmark"
        echo "=========================================="
        echo "Date: $(date)"
        echo "CPU Cores: ${PARALLEL_JOBS}"
        echo "Compiler: ${DETECTED_COMPILER}"
        echo "Compiler Path: ${DETECTED_COMPILER_PATH}"
        echo ""
        echo "Results:"
        echo "--------"
    } > "${RESULTS_FILE}"

    # Run benchmarks.
    if [[ "${TEST_ONLY}" == "true" ]]; then
        TARGET="runTests"
    else
        TARGET="all"
    fi

    benchmark_config \
        "1. Baseline (No Optimizations)" \
        "${PROJECT_ROOT}/build-baseline" \
        "${TARGET}" \
        -DDISABLE_PCH=ON -DDISABLE_UNITY_BUILD=ON

    benchmark_config \
        "2. PCH Only" \
        "${PROJECT_ROOT}/build-pch" \
        "${TARGET}" \
        -DDISABLE_UNITY_BUILD=ON

    benchmark_config \
        "3. Unity Build Only" \
        "${PROJECT_ROOT}/build-unity" \
        "${TARGET}" \
        -DDISABLE_PCH=ON

    benchmark_config \
        "4. Full Optimization (PCH + Unity Build)" \
        "${PROJECT_ROOT}/build-optimized" \
        "${TARGET}"

    # Summary.
    print_section "Benchmark Complete" "${CYAN}" true
    echo ""
    echo "Results saved to: ${RESULTS_FILE}"
    echo ""
    echo "Summary:"
    tail -n +10 "${RESULTS_FILE}"

    # Calculate speedup.
    echo "" | tee -a "${RESULTS_FILE}"
    echo "Analysis:" | tee -a "${RESULTS_FILE}"
    echo "---------" | tee -a "${RESULTS_FILE}"

    baseline=$(grep "^1\." "${RESULTS_FILE}" | awk '{print $NF}' | sed 's/s//')
    optimized=$(grep "^4\." "${RESULTS_FILE}" | awk '{print $NF}' | sed 's/s//')

    if [[ -n "${baseline}" ]] && [[ -n "${optimized}" ]]; then
        speedup=$(echo "scale=2; (${baseline} - ${optimized}) / ${baseline} * 100" | bc)
        echo "Baseline:   ${baseline}s" | tee -a "${RESULTS_FILE}"
        echo "Optimized:  ${optimized}s" | tee -a "${RESULTS_FILE}"
        echo -e "${BOLD}${GREEN}Speedup:    ${speedup}%${RESET}"
        echo "Speedup:    ${speedup}%" >> "${RESULTS_FILE}"
    fi

    echo ""
    echo -e "${GREEN}✓ Benchmark completed successfully${RESET}"
}

main "$@"

# ============================================================================ #
# End of Script.
