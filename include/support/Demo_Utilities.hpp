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

#include "Terminal_Colors.hpp"

#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>

//===---------------------------------------------------------------------------===//

namespace ads::demo {

namespace detail {

constexpr std::string_view kBoxTop    = "╔═══--------------------------------------------------------------═══╗";
constexpr std::string_view kBoxBottom = "╚═══--------------------------------------------------------------═══╝";
constexpr size_t           kBoxWidth  = 70;

/**
 * @brief Centers a text string within a given width.
 * @param text The text to center.
 * @param width The width of the container.
 * @return The centered string.
 */
inline auto center_text(std::string_view text, size_t width) -> std::string {
  if (text.size() >= width) {
    return std::string(text.substr(0, width));
  }

  const size_t padding = width - text.size();
  const size_t left    = padding / 2;
  const size_t right   = padding - left;

  return std::string(left, ' ') + std::string(text) + std::string(right, ' ');
}

/**
 * @brief Print a series of lines within a boxed container.
 * @param lines The lines to print.
 * @param color The color code to use for the box.
 */
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

constexpr std::string_view kSectionLeftCap   = "=====----- "; ///< Fixed left cap (trailing space included).
constexpr std::string_view kSectionRightCap  = "=====";       ///< Fixed right cap.
constexpr size_t           kSectionWidth     = 70;            ///< Target banner width in characters.
constexpr size_t           kSectionMinDashes = 5;             ///< Minimum filler dashes before the right cap.

/**
 * @brief Build a fixed-width, left-aligned section banner.
 * @details Layout: "=====----- " + title + ' ' + filler dashes + "=====".
 *          The filler is sized so the banner is exactly kSectionWidth
 *          characters. When the title is too long to fit, the filler clamps to
 *          kSectionMinDashes and the banner overflows the target width instead
 *          of truncating the title.
 * @param title The section title (measured by byte length; assumes ASCII).
 * @return The assembled banner string without color codes or newlines.
 */
inline auto make_section_banner(std::string_view title) -> std::string {
  // Fixed overhead: left cap (with trailing space) + separator space + right cap.
  const size_t fixed  = kSectionLeftCap.size() + 1 + kSectionRightCap.size();
  size_t       dashes = kSectionMinDashes;
  if (kSectionWidth >= fixed + title.size() + kSectionMinDashes) {
    dashes = kSectionWidth - fixed - title.size();
  }

  std::string banner;
  banner.reserve(fixed + title.size() + dashes);
  banner += kSectionLeftCap;
  banner += title;
  banner += ' ';
  banner.append(dashes, '-');
  banner += kSectionRightCap;
  return banner;
}

} // namespace detail

/**
 * @brief Print a formatted section separator with title.
 * @param title The title of the section.
 */
inline void print_section(std::string_view title) {
  std::cout << "\n";
  std::cout << ANSI_BOLD << ANSI_CYAN << detail::make_section_banner(title);
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
