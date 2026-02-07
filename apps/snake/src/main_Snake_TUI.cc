//===---------------------------------------------------------------------------===//
/**
 * @file main_Snake_TUI.cc
 * @brief Interactive Snake TUI demo built on top of ADS structures.
 * @version 0.1
 * @date 2026-02-06
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "snake/Snake_Engine.hpp"

#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <optional>
#include <string>

namespace {

using ads::apps::snake::CellDelta;
using ads::apps::snake::Direction;
using ads::apps::snake::SnakeEngine;

//===--------------------------- TERMINAL ROW LAYOUT ---------------------------===//

// Fixed terminal row indices for different sections of the TUI. These are 1-indexed
// for ANSI cursor positioning and are calculated based on the board size and layout.
constexpr int kTopBorderRow    = 1;
constexpr int kBoardStartRow   = 2;
constexpr int kBoardStartCol   = 2; // column offset inside border '|'
constexpr int kBottomBorderRow = static_cast<int>(SnakeEngine::kRows) + kBoardStartRow;
constexpr int kStatusRow       = kBottomBorderRow + 1;
constexpr int kFoodRow         = kStatusRow + 1;
constexpr int kPromptRow       = kFoodRow + 1;

//===------------------------------ ANSI HELPERS -------------------------------===//

/// @brief Clears the entire terminal screen and moves cursor to the home position.
inline auto ansi_clear_screen() -> void {
  std::cout << "\033[2J\033[H";
}

/// @brief Moves the terminal cursor to the given 1-indexed row and column.
inline auto ansi_move_to(int row, int col) -> void {
  std::cout << "\033[" << row << ';' << col << 'H';
}

/// @brief Hides the terminal cursor to prevent flickering during rendering.
inline auto ansi_hide_cursor() -> void {
  std::cout << "\033[?25l";
}

/// @brief Restores the terminal cursor visibility.
inline auto ansi_show_cursor() -> void {
  std::cout << "\033[?25h";
}

/// @brief Erases the entire current line without moving the cursor.
inline auto ansi_clear_line() -> void {
  std::cout << "\033[2K";
}

//===------------------------------ INPUT PARSING ------------------------------===//

/**
 * @brief Parses a character input into a direction using WASD mapping.
 * @param raw Single character from user input.
 * @return The corresponding direction, or std::nullopt if the character is not a valid command.
 */
[[nodiscard]] auto parse_direction(char raw) -> std::optional<Direction> {
  const auto command = static_cast<char>(std::tolower(static_cast<unsigned char>(raw)));

  using enum Direction;

  switch (command) {
  case 'w':
    return kUp;
  case 's':
    return kDown;
  case 'a':
    return kLeft;
  case 'd':
    return kRight;
  default:
    return std::nullopt;
  }
}

/**
 * @brief Parses a C-string into a uint32_t with overflow protection.
 * @param value Null-terminated string to parse (may be nullptr).
 * @param fallback Value returned when parsing fails or input is null.
 * @return The parsed integer, or @p fallback on error.
 */
[[nodiscard]] auto parse_u32_arg(const char* value, std::uint32_t fallback) -> std::uint32_t {
  if (value == nullptr) {
    return fallback;
  }

  char* end = nullptr;

  const auto parsed  = std::strtoul(value, &end, 10);
  const auto max_u32 = static_cast<unsigned long>(std::numeric_limits<std::uint32_t>::max());

  if (end == value || *end != '\0' || parsed > max_u32) {
    return fallback;
  }

  return static_cast<std::uint32_t>(parsed);
}

/**
 * @brief Parses a C-string into a size_t with error handling.
 * @param value Null-terminated string to parse (may be nullptr).
 * @param fallback Value returned when parsing fails or input is null.
 * @return The parsed value, or @p fallback on error.
 */
[[nodiscard]] auto parse_usize_arg(const char* value, std::size_t fallback) -> std::size_t {
  if (value == nullptr) {
    return fallback;
  }

  char* end = nullptr;

  const auto parsed = std::strtoull(value, &end, 10);

  if (end == value || *end != '\0') {
    return fallback;
  }

  return static_cast<std::size_t>(parsed);
}

//===---------------------------- RENDERING HELPERS ----------------------------===//

/**
 * @brief Refreshes the status and food information lines at their fixed terminal rows.
 * @param engine Engine instance to read score, tick, length, and food data from.
 */
auto draw_status_line(const SnakeEngine& engine) -> void {
  ansi_move_to(kStatusRow, 1);
  ansi_clear_line();
  std::cout << "Score: " << engine.score() << " | Tick: " << engine.tick() << " | Length: " << engine.body_size();

  ansi_move_to(kFoodRow, 1);
  ansi_clear_line();
  if (engine.has_food()) {
    const auto food = engine.food();
    std::cout << "Food: (" << food.row << ", " << food.col << ")";
  } else {
    std::cout << "Food: none (board fully occupied)";
  }
}

/**
 * @brief Performs a complete screen redraw of the board, borders, status, and prompt.
 * @details Used for the initial frame and when the snake dies (final state display).
 *          Clears the screen, renders all cells via render_board(), and positions the
 *          cursor at the input prompt row.
 *
 * @param engine Engine instance providing the board snapshot and game metadata.
 */
auto draw_full_board(const SnakeEngine& engine) -> void {
  const SnakeEngine::Board board = engine.render_board();

  ansi_clear_screen();
  ansi_hide_cursor();

  // Top border.
  ansi_move_to(kTopBorderRow, 1);
  std::cout << '+';
  for (std::size_t col = 0; col < SnakeEngine::kCols; ++col) {
    std::cout << '-';
  }
  std::cout << '+';

  // Board rows.
  for (std::size_t row = 0; row < SnakeEngine::kRows; ++row) {
    ansi_move_to(kBoardStartRow + static_cast<int>(row), 1);
    std::cout << '|';
    for (std::size_t col = 0; col < SnakeEngine::kCols; ++col) {
      std::cout << board[row][col];
    }
    std::cout << '|';
  }

  // Bottom border.
  ansi_move_to(kBottomBorderRow, 1);
  std::cout << '+';
  for (std::size_t col = 0; col < SnakeEngine::kCols; ++col) {
    std::cout << '-';
  }
  std::cout << '+';

  draw_status_line(engine);

  // Position cursor at prompt row.
  ansi_move_to(kPromptRow, 1);
  ansi_clear_line();
  std::cout << "Move> " << std::flush;
}

/**
 * @brief Applies differential rendering by updating only the cells that changed.
 * @details Iterates over the CellDelta list from the engine, moves the cursor to each
 *          affected terminal position, and writes the new glyph. Refreshes the status
 *          line and repositions the cursor at the input prompt afterwards.
 * @param engine Engine instance providing the delta list and game metadata.
 */
auto apply_deltas(const SnakeEngine& engine) -> void {
  const auto& deltas = engine.deltas();

  for (const auto& delta : deltas) {
    ansi_move_to(delta.row + kBoardStartRow, delta.col + kBoardStartCol);
    std::cout << delta.glyph;
  }

  draw_status_line(engine);

  ansi_move_to(kPromptRow, 1);
  ansi_clear_line();
  std::cout << "Move> " << std::flush;
}

} // namespace

//===------------------------------ MAIN FUNCTION ------------------------------===//

/**
 * @brief Entry point for the interactive Snake TUI.
 * @details Accepts optional CLI arguments: seed (argv[1]) and max_ticks (argv[2]).
 *          Draws the initial board, then enters the game loop where each iteration
 *          reads a WASD/quit command, advances the engine, and applies differential
 *          rendering. On exit, prints a final summary with score and replay data.
 *
 * @param argc Argument count.
 * @param argv Argument vector (expects optional [seed] [max_ticks]).
 * @return 0 on success, 2 on consistency failure during play, 3 on final consistency
 *         failure.
 */
auto main(int argc, char** argv) -> int {
  const std::uint32_t seed = (argc > 1) ? parse_u32_arg(argv[1], SnakeEngine::kDefaultSeed) : SnakeEngine::kDefaultSeed;
  const std::size_t   max_ticks = (argc > 2) ? parse_usize_arg(argv[2], 500U) : 500U;

  SnakeEngine engine(seed);

  // Initial full render.
  draw_full_board(engine);

  // Main game loop with differential rendering.
  while (engine.is_alive() && engine.tick() < max_ticks) {
    ansi_show_cursor();

    std::string line;
    if (!std::getline(std::cin, line)) {
      break;
    }

    ansi_hide_cursor();

    if (!line.empty()) {
      const auto command = static_cast<char>(std::tolower(static_cast<unsigned char>(line.front())));
      if (command == 'q') {
        break;
      }

      const auto next_direction = parse_direction(command);
      if (next_direction.has_value()) {
        engine.set_direction(*next_direction);
      }
    }

    engine.step();

    if (!engine.is_consistent()) {
      ansi_show_cursor();
      ansi_move_to(kPromptRow + 1, 1);
      std::cerr << "Internal consistency check failed at tick " << engine.tick() << "\n";
      return 2;
    }

    if (engine.is_alive()) {
      apply_deltas(engine);
    } else {
      draw_full_board(engine);
    }
  }

  // Final output below the TUI frame.
  ansi_show_cursor();
  ansi_move_to(kPromptRow + 1, 1);

  const auto& replay = engine.replay_log();
  std::cout << "\nFinal state: " << (engine.is_alive() ? "alive" : "dead") << "\n";
  std::cout << "Final score: " << engine.score() << "\n";
  std::cout << "Ticks executed: " << engine.tick() << "\n";
  std::cout << "Replay snapshots: " << replay.size() << "\n";

  if (!replay.is_empty()) {
    const std::size_t history_to_print = (replay.size() < 5U) ? replay.size() : 5U;
    std::cout << "Recent moves: ";
    for (std::size_t i = replay.size() - history_to_print; i < replay.size(); ++i) {
      std::cout << ads::apps::snake::to_char(replay[i].direction);
      if (i + 1U < replay.size()) {
        std::cout << ' ';
      }
    }
    std::cout << '\n';
  }

  return engine.is_consistent() ? 0 : 3;
}

//===---------------------------------------------------------------------------===//
