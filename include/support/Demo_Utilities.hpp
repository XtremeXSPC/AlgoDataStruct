//===---------------------------------------------------------------------------===//
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
//===---------------------------------------------------------------------------===//

#ifndef ADS_SUPPORT_DEMO_UTILITIES_HPP
#define ADS_SUPPORT_DEMO_UTILITIES_HPP

#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>

#include "Terminal_Colors.hpp"

//===---------------------------------------------------------------------------===//

namespace ads::demo {

namespace detail {

constexpr std::string_view kBoxTop    = "╔═══----------------------------------------------------═══╗";
constexpr std::string_view kBoxBottom = "╚═══----------------------------------------------------═══╝";
constexpr size_t           kBoxWidth  = 60;

inline auto center_text(std::string_view text, size_t width) -> std::string {
  if (text.size() >= width) {
    return std::string(text.substr(0, width));
  }

  const size_t padding = width - text.size();
  const size_t left    = padding / 2;
  const size_t right   = padding - left;

  return std::string(left, ' ') + std::string(text) + std::string(right, ' ');
}

inline void print_boxed_lines(std::initializer_list<std::string_view> lines, const char* color) {
  if (color != nullptr && color[0] != '\0') {
    std::cout << color;
  }

  std::cout << kBoxTop << "\n";
  for (const auto& line : lines) {
    std::cout << center_text(line, kBoxWidth) << "\n";
  }
  std::cout << kBoxBottom << "\n";

  if (color != nullptr && color[0] != '\0') {
    std::cout << ANSI_RESET;
  }
}

} // namespace detail

/**
 * @brief Print a formatted section separator with title.
 * @param title The title of the section.
 */
inline void print_section(const std::string& title) {
  std::cout << "\n";
  std::cout << ANSI_BOLD << ANSI_CYAN << "=====---------- " << title << " ----------=====";
  std::cout << ANSI_RESET << "\n";
}

/**
 * @brief Print a success message.
 * @param message The success message.
 */
inline void print_success(const std::string& message) {
  std::cout << ANSI_GREEN << "[OK] " << ANSI_RESET << message << "\n";
}

/**
 * @brief Print an error message.
 * @param message The error message.
 */
inline void print_error(const std::string& message) {
  std::cerr << ANSI_RED << "[ERROR] " << ANSI_RESET << message << "\n";
}

/**
 * @brief Print an info message.
 * @param message The info message.
 */
inline void print_info(const std::string& message) {
  std::cout << ANSI_BLUE << "[INFO] " << ANSI_RESET << message << "\n";
}

/**
 * @brief Print a warning message.
 * @param message The warning message.
 */
inline void print_warning(const std::string& message) {
  std::cout << ANSI_YELLOW << "[WARN] " << ANSI_RESET << message << "\n";
}

/**
 * @brief Print a program header.
 * @param program_name The name of the demo program.
 */
inline void print_header(std::string_view program_name) {
  detail::print_boxed_lines({program_name}, ANSI_BOLD ANSI_BLUE);
}

/**
 * @brief Print a multi-line program header.
 * @param lines Lines to print within the header box.
 */
inline void print_header(std::initializer_list<std::string_view> lines) {
  detail::print_boxed_lines(lines, ANSI_BOLD ANSI_BLUE);
}

/**
 * @brief Print a program footer.
 */
inline void print_footer() {
  std::cout << "\n";
  detail::print_boxed_lines({"ALL DEMOS COMPLETED SUCCESSFULLY!"}, ANSI_BOLD ANSI_GREEN);
}

/**
 * @brief Print a program footer with a custom message.
 * @param message The footer message.
 */
inline void print_footer(std::string_view message) {
  std::cout << "\n";
  detail::print_boxed_lines({message}, ANSI_BOLD ANSI_GREEN);
}

/**
 * @brief Print a container's contents.
 * @tparam Container Container type with begin/end iterators.
 * @param container The container to print.
 * @param name The name of the container.
 * @param separator The separator between elements (default: " ").
 */
template <typename Container>
void print_container(const Container& container, const std::string& name, const std::string& separator = " ") {
  if (!name.empty()) {
    std::cout << name << ": ";
  }
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

/**
 * @brief Print a container's contents with a prefix.
 * @tparam Container Container type with begin/end iterators.
 * @param container The container to print.
 * @param prefix The prefix to print before the elements.
 * @param separator The separator between elements (default: " ").
 */
template <typename Container>
void print_sequence(const Container& container, const std::string& prefix = "", const std::string& separator = " ") {
  std::cout << prefix;
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

//===---------------------------------------------------------------------------===//
