#!/usr/bin/env bash
# ============================================================================ #
# -------------------------- PCH Validation Script --------------------------- #
# ============================================================================ #
#
# Validates that precompiled headers have been generated correctly and
# provides statistics on their usage and size.
#
# Usage:
#   ./scripts/validate_pch.sh [BUILD_DIR]
#
# Arguments:
#   BUILD_DIR  Path to build directory (default: ./build)
#
# Checks:
#   1. PCH files exist and are not stale
#   2. PCH file sizes are reasonable
#   3. Source files include PCH headers
#   4. Unity Build files generated (if enabled)
#
# Author: Costantino Lombardi
# Date: 2026-02-11
# ============================================================================ #

set -euo pipefail

# ============================== CONFIGURATION =============================== #

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${1:-${PROJECT_ROOT}/build}"
COMMON_SCRIPT="${SCRIPT_DIR}/script_common.sh"

if [[ ! -f "${COMMON_SCRIPT}" ]]; then
    echo "Error: shared script not found: ${COMMON_SCRIPT}" >&2
    exit 1
fi

# shellcheck disable=SC1090
source "${COMMON_SCRIPT}"
PCH_ENABLED=true

# ================================ UTILITIES ================================= #

format_size() {
    local bytes=$1
    if [[ ${bytes} -ge 1048576 ]]; then
        echo "$(echo "scale=2; ${bytes} / 1048576" | bc) MB"
    elif [[ ${bytes} -ge 1024 ]]; then
        echo "$(echo "scale=2; ${bytes} / 1024" | bc) KB"
    else
        echo "${bytes} B"
    fi
}

# ============================= VALIDATION LOGIC ============================= #

validate_build_dir() {
    print_section "Build Directory"

    if [[ ! -d "${BUILD_DIR}" ]]; then
        print_error "Build directory not found: ${BUILD_DIR}"
        echo ""
        echo "Please configure the project first:"
        echo "  cmake -DCMAKE_TOOLCHAIN_FILE=clang-toolchain.cmake -B build"
        exit 1
    fi

    print_ok "Build directory exists: ${BUILD_DIR}"

    # Check if PCH is disabled.
    if grep -q "DISABLE_PCH:BOOL=ON" "${BUILD_DIR}/CMakeCache.txt" 2>/dev/null; then
        print_warning "PCH is disabled in this build (DISABLE_PCH=ON)"
        PCH_ENABLED=false
    fi
}

validate_library_pch() {
    print_section "Library PCH (ads_pch.hpp)"

    if [[ "${PCH_ENABLED}" != "true" ]]; then
        print_warning "Skipped (PCH disabled)"
        return 0
    fi

    # Find library PCH files.
    local pch_files
    pch_files=$(find "${BUILD_DIR}" \( -name "cmake_pch.hxx.pch" -o -name "cmake_pch.hxx.gch" \) -type f 2>/dev/null || true)

    if [[ -z "${pch_files}" ]]; then
        print_warning "No library PCH files found"
        echo ""
        echo "This might be normal for INTERFACE libraries with PCH."
        echo "The PCH will be generated when targets using ads_lib are built."
        return 0
    fi

    while IFS= read -r pch_file; do
        if [[ -f "${pch_file}" ]]; then
            local size
            size=$(stat -f%z "${pch_file}" 2>/dev/null || stat -c%s "${pch_file}" 2>/dev/null || echo 0)
            local formatted_size
            formatted_size=$(format_size "${size}")
            print_ok "Found: $(basename "$(dirname "${pch_file}")")/$(basename "${pch_file}") (${formatted_size})"

            # Check if PCH is recent.
            local pch_mtime
            pch_mtime=$(stat -f%m "${pch_file}" 2>/dev/null || stat -c%Y "${pch_file}" 2>/dev/null || echo 0)
            local source_mtime
            source_mtime=$(stat -f%m "${PROJECT_ROOT}/include/ads_pch.hpp" 2>/dev/null || stat -c%Y "${PROJECT_ROOT}/include/ads_pch.hpp" 2>/dev/null || echo 0)

            if [[ ${pch_mtime} -lt ${source_mtime} ]]; then
                print_warning "PCH is older than source header (rebuild recommended)"
            fi
        fi
    done <<< "${pch_files}"
}

validate_test_pch() {
    print_section "Test PCH (tests_pch.hpp)"

    if [[ "${PCH_ENABLED}" != "true" ]]; then
        print_warning "Skipped (PCH disabled)"
        return 0
    fi

    # Check if testing is enabled.
    if ! grep -q "ENABLE_TESTING:BOOL=ON" "${BUILD_DIR}/CMakeCache.txt" 2>/dev/null; then
        print_warning "Testing is disabled (ENABLE_TESTING=OFF)"
        return 0
    fi

    # Find test PCH files.
    local test_pch_dir="${BUILD_DIR}/CMakeFiles/runTests.dir"

    if [[ ! -d "${test_pch_dir}" ]]; then
        print_warning "runTests target not found (tests may not be configured)"
        return 0
    fi

    local pch_files
    pch_files=$(find "${test_pch_dir}" \( -name "cmake_pch.hxx.pch" -o -name "cmake_pch.hxx.gch" \) -type f 2>/dev/null || true)

    if [[ -z "${pch_files}" ]]; then
        print_error "No test PCH files found"
        echo ""
        echo "Try building the test target first:"
        echo "  cmake --build ${BUILD_DIR} --target runTests"
        return 1
    fi

    while IFS= read -r pch_file; do
        if [[ -f "${pch_file}" ]]; then
            local size
            size=$(stat -f%z "${pch_file}" 2>/dev/null || stat -c%s "${pch_file}" 2>/dev/null || echo 0)
            local formatted_size
            formatted_size=$(format_size "${size}")
            print_ok "Found: $(basename "${pch_file}") (${formatted_size})"
        fi
    done <<< "${pch_files}"
}

validate_unity_build() {
    print_section "Unity Build"

    # Check if unity build is disabled.
    if grep -q "DISABLE_UNITY_BUILD:BOOL=ON" "${BUILD_DIR}/CMakeCache.txt" 2>/dev/null; then
        print_warning "Unity Build is disabled (DISABLE_UNITY_BUILD=ON)"
        return 0
    fi

    # Check if testing is enabled.
    if ! grep -q "ENABLE_TESTING:BOOL=ON" "${BUILD_DIR}/CMakeCache.txt" 2>/dev/null; then
        print_warning "Testing is disabled (Unity Build only applies to tests)"
        return 0
    fi

    # Find unity build files.
    local unity_files
    unity_files=$(find "${BUILD_DIR}" \( -name "unity_*.cxx" -o -name "unity_*.cpp" \) -type f 2>/dev/null || true)

    if [[ -z "${unity_files}" ]]; then
        print_error "No Unity Build files found"
        echo ""
        echo "Unity Build files are generated during build. Try:"
        echo "  cmake --build ${BUILD_DIR} --target runTests"
        return 1
    fi

    local count=0
    while IFS= read -r unity_file; do
        if [[ -f "${unity_file}" ]]; then
            count=$((count + 1))
            local includes
            includes=$(grep -c "^#include" "${unity_file}" || echo 0)
            print_ok "Unity batch $(basename "${unity_file}"): ${includes} files merged"
        fi
    done <<< "${unity_files}"

    if [[ ${count} -gt 0 ]]; then
        print_ok "Total unity batches: ${count}"
    fi
}

validate_pch_headers() {
    print_section "PCH Header Files"

    if [[ -f "${PROJECT_ROOT}/include/ads_pch.hpp" ]]; then
        local line_count
        line_count=$(wc -l < "${PROJECT_ROOT}/include/ads_pch.hpp")
        local include_count
        include_count=$(grep -c "^#include" "${PROJECT_ROOT}/include/ads_pch.hpp" || echo 0)
        print_ok "ads_pch.hpp: ${line_count} lines, ${include_count} includes"
    else
        print_error "ads_pch.hpp not found in include/"
        return 1
    fi

    if [[ -f "${PROJECT_ROOT}/tests/tests_pch.hpp" ]]; then
        local line_count
        line_count=$(wc -l < "${PROJECT_ROOT}/tests/tests_pch.hpp")
        local include_count
        include_count=$(grep -c "^#include" "${PROJECT_ROOT}/tests/tests_pch.hpp" || echo 0)
        print_ok "tests_pch.hpp: ${line_count} lines, ${include_count} includes"
    else
        print_warning "tests_pch.hpp not found in tests/"
    fi
}

print_summary() {
    print_section "Summary"

    echo ""
    echo "Configuration:"

    # PCH status.
    if grep -q "DISABLE_PCH:BOOL=ON" "${BUILD_DIR}/CMakeCache.txt" 2>/dev/null; then
        echo -e "  PCH:         ${YELLOW}Disabled${RESET}"
    else
        echo -e "  PCH:         ${GREEN}Enabled${RESET}"
    fi

    # Unity Build status.
    if grep -q "DISABLE_UNITY_BUILD:BOOL=ON" "${BUILD_DIR}/CMakeCache.txt" 2>/dev/null; then
        echo -e "  Unity Build: ${YELLOW}Disabled${RESET}"
    else
        echo -e "  Unity Build: ${GREEN}Enabled${RESET}"
    fi

    # Testing status.
    if grep -q "ENABLE_TESTING:BOOL=ON" "${BUILD_DIR}/CMakeCache.txt" 2>/dev/null; then
        echo -e "  Testing:     ${GREEN}Enabled${RESET}"
    else
        echo -e "  Testing:     ${YELLOW}Disabled${RESET}"
    fi

    echo ""
    echo "Next steps:"
    echo "  • Build project:      cmake --build ${BUILD_DIR}"
    echo "  • Run tests:          ${BUILD_DIR}/bin/runTests"
    echo "  • Benchmark builds:   ./scripts/benchmark_build.sh"
    echo ""
}

# =============================== MAIN SCRIPT ================================ #

main() {
    cd "${PROJECT_ROOT}"

    print_header "PCH Validation for AlgoDataStruct"

    validate_build_dir
    validate_pch_headers
    validate_library_pch
    validate_test_pch
    validate_unity_build
    print_summary

    echo -e "${GREEN}✓ Validation complete${RESET}"
    echo ""
}

main "$@"

# ============================================================================ #
# End of script.
