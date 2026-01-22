//===---------------------------------------------------------------------------===//
/**
 * @file Terminal_Colors.hpp
 * @brief ANSI terminal color codes for formatted console output
 * @version 0.1
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 *
 * This header provides macros for ANSI terminal colors that can be used
 * in demo programs to enhance readability of the output.
 */
//===---------------------------------------------------------------------------===//

#ifndef ADS_SUPPORT_TERMINAL_COLORS_HPP
#define ADS_SUPPORT_TERMINAL_COLORS_HPP

// Reset.
#define ANSI_RESET "\033[0m"

// Text attributes.
#define ANSI_BOLD "\033[1m"
#define ANSI_DIM "\033[2m"
#define ANSI_UNDERLINE "\033[4m"
#define ANSI_BLINK "\033[5m"
#define ANSI_INVERSE "\033[7m"

// Foreground colors.
#define ANSI_BLACK "\033[30m"
#define ANSI_RED "\033[31m"
#define ANSI_GREEN "\033[32m"
#define ANSI_YELLOW "\033[33m"
#define ANSI_BLUE "\033[34m"
#define ANSI_MAGENTA "\033[35m"
#define ANSI_CYAN "\033[36m"
#define ANSI_WHITE "\033[37m"

// Bright foreground colors.
#define ANSI_BRIGHT_BLACK "\033[90m"
#define ANSI_BRIGHT_RED "\033[91m"
#define ANSI_BRIGHT_GREEN "\033[92m"
#define ANSI_BRIGHT_YELLOW "\033[93m"
#define ANSI_BRIGHT_BLUE "\033[94m"
#define ANSI_BRIGHT_MAGENTA "\033[95m"
#define ANSI_BRIGHT_CYAN "\033[96m"
#define ANSI_BRIGHT_WHITE "\033[97m"

// Background colors.
#define ANSI_BG_BLACK "\033[40m"
#define ANSI_BG_RED "\033[41m"
#define ANSI_BG_GREEN "\033[42m"
#define ANSI_BG_YELLOW "\033[43m"
#define ANSI_BG_BLUE "\033[44m"
#define ANSI_BG_MAGENTA "\033[45m"
#define ANSI_BG_CYAN "\033[46m"
#define ANSI_BG_WHITE "\033[47m"

#endif // ADS_SUPPORT_TERMINAL_COLORS_HPP

//===---------------------------------------------------------------------------===//
