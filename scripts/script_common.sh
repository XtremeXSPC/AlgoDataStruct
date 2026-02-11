#!/usr/bin/env bash
# ============================================================================ #
# ------------------------- Shared Script Utilities -------------------------- #
# ============================================================================ #
#
# Common ANSI colors and printing helpers shared by scripts in this repository.
#
# Usage:
#   source "${SCRIPT_DIR}/script_common.sh"
#   print_header "My Title" "${BLUE}"
#   print_section "Step Name" "${CYAN}" true
#   print_ok "Everything passed"
#
# ============================================================================ #

if [[ -n "${ADS_SCRIPT_COMMON_SOURCED:-}" ]]; then
    return 0
fi
ADS_SCRIPT_COMMON_SOURCED=1

# Verify required external tools.
if ! command -v bc &>/dev/null; then
    echo "Error: required command 'bc' not found. Please install it." >&2
    return 1
fi

# ANSI color codes and border lines (used by sourcing scripts).
# shellcheck disable=SC2034
{
RESET='\033[0m'
BOLD='\033[1m'
RED='\033[31m'
GREEN='\033[32m'
YELLOW='\033[33m'
BLUE='\033[34m'
CYAN='\033[36m'

HEADER_LINE='═════════════════════════════════════════════════════════════════'
SECTION_DIVIDER='━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━'
}

print_header() {
    local title="$1"
    local color="${2:-${CYAN}}"
    echo -e "${BOLD}${color}${HEADER_LINE}${RESET}"
    echo -e "  ${BOLD}${color}${title}${RESET}"
    echo -e "${BOLD}${color}${HEADER_LINE}${RESET}"
}

print_section() {
    local title="$1"
    local color="${2:-${CYAN}}"
    local with_divider="${3:-false}"
    echo ""
    echo -e "${BOLD}${color}▶ ${title}${RESET}"
    if [[ "${with_divider}" == "true" ]]; then
        echo "${SECTION_DIVIDER}"
    fi
}

print_ok() {
    echo -e "  ${GREEN}✓${RESET} $1"
}

print_warning() {
    echo -e "  ${YELLOW}⚠${RESET} $1"
}

print_error() {
    echo -e "  ${RED}✗${RESET} $1" >&2
}
