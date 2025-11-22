//===--------------------------------------------------------------------------===//
/**
 * @file Demo_Utilities.hpp
 * @brief Common utility functions for demo programs
 * @version 0.1
 * @date 2025-11-21
 *
 * @copyright MIT License 2025
 *
 * This header provides common utility functions used across demo programs
 * to avoid code duplication and ensure consistent formatting.
 */
//===--------------------------------------------------------------------------===//

#ifndef ADS_SUPPORT_DEMO_UTILITIES_HPP
#define ADS_SUPPORT_DEMO_UTILITIES_HPP

#include <iostream>
#include <string>

#include "Terminal_Colors.hpp"

namespace ads::demo {

/**
 * @brief Print a formatted section separator with title
 * @param title The title of the section
 */
inline void print_section(const std::string& title) {
  std::cout << "\n" << ANSI_BOLD << ANSI_CYAN << "========== " << title << " ==========" << ANSI_RESET << "\n";
}

/**
 * @brief Print a success message
 * @param message The success message
 */
inline void print_success(const std::string& message) {
  std::cout << ANSI_GREEN << "[OK] " << ANSI_RESET << message << "\n";
}

/**
 * @brief Print an error message
 * @param message The error message
 */
inline void print_error(const std::string& message) {
  std::cerr << ANSI_RED << "[ERROR] " << ANSI_RESET << message << "\n";
}

/**
 * @brief Print an info message
 * @param message The info message
 */
inline void print_info(const std::string& message) {
  std::cout << ANSI_BLUE << "[INFO] " << ANSI_RESET << message << "\n";
}

/**
 * @brief Print a warning message
 * @param message The warning message
 */
inline void print_warning(const std::string& message) {
  std::cout << ANSI_YELLOW << "[WARN] " << ANSI_RESET << message << "\n";
}

/**
 * @brief Print a program header
 * @param program_name The name of the demo program
 */
inline void print_header(const std::string& program_name) {
  std::cout << ANSI_BOLD << ANSI_BLUE;
  std::cout << "========================================\n";
  std::cout << "  " << program_name << "\n";
  std::cout << "========================================\n";
  std::cout << ANSI_RESET;
}

/**
 * @brief Print a program footer
 */
inline void print_footer() {
  std::cout << ANSI_BOLD << ANSI_GREEN;
  std::cout << "\n";
  std::cout << "========================================\n";
  std::cout << "  All Demos Completed Successfully!\n";
  std::cout << "========================================\n";
  std::cout << ANSI_RESET;
}

/**
 * @brief Print a container's contents
 * @tparam Container Container type with begin/end iterators
 * @param container The container to print
 * @param name The name of the container
 * @param separator The separator between elements (default: " ")
 */
template <typename Container>
void print_container(const Container& container, const std::string& name, const std::string& separator = " ") {
  std::cout << name << ": ";
  bool first = true;
  for (const auto& elem : container) {
    if (!first) {
      std::cout << separator;
    }
    std::cout << elem;
    first = false;
  }
  std::cout << "\n";
}

} // namespace ads::demo

#endif // ADS_SUPPORT_DEMO_UTILITIES_HPP
