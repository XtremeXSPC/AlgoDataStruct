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

using ads::apps::snake::Direction;
using ads::apps::snake::SnakeEngine;

/**
 * @brief Parses a character input into a direction, if valid.
 *
 * @param raw The character input representing a direction.
 * @return std::optional<Direction> The parsed direction, or std::nullopt if invalid.
 */
[[nodiscard]] auto parse_direction(char raw) -> std::optional<Direction> {
  // Convert to lowercase for case-insensitive parsing.
  const auto command = static_cast<char>(std::tolower(static_cast<unsigned char>(raw)));

  // Map input characters to directions (WASD).
  switch (command) {
  case 'w':
    return Direction::kUp;
  case 's':
    return Direction::kDown;
  case 'a':
    return Direction::kLeft;
  case 'd':
    return Direction::kRight;
  default:
    return std::nullopt;
  }
}

/**
 * @brief Prints the top scores from the leaderboard.
 *
 * @param leaderboard The leaderboard containing score entries.
 * @param count The number of top scores to print.
 */
[[nodiscard]] auto parse_u32_arg(const char* value, std::uint32_t fallback) -> std::uint32_t {
  if (value == nullptr) {
    return fallback;
  }

  char* end = nullptr;

  // Use strtoul for parsing and check for errors and overflow.
  const auto parsed  = std::strtoul(value, &end, 10);
  const auto max_u32 = static_cast<unsigned long>(std::numeric_limits<std::uint32_t>::max());

  // Check for parsing errors and overflow.
  if (end == value || *end != '\0' || parsed > max_u32) {
    return fallback;
  }

  return static_cast<std::uint32_t>(parsed);
}

/**
 * @brief Parses a string argument into a size_t, with error handling and fallback.
 *
 * @param value The C-string to parse.
 * @param fallback The value to return if parsing fails.
 * @return std::size_t The parsed value, or the fallback if parsing fails.
 */
[[nodiscard]] auto parse_usize_arg(const char* value, std::size_t fallback) -> std::size_t {
  if (value == nullptr) {
    return fallback;
  }

  char* end = nullptr;

  // Use strtoul for parsing and check for errors and overflow.
  const auto parsed = std::strtoull(value, &end, 10);

  // Check for parsing errors.
  if (end == value || *end != '\0') {
    return fallback;
  }

  return static_cast<std::size_t>(parsed);
}

/**
 * @brief Renders the current game board to the console.
 *
 * @param engine The SnakeEngine instance containing the game state to render.
 */
auto print_board(const SnakeEngine& engine) -> void {
  const SnakeEngine::Board board = engine.render_board();

  std::cout << '\n';
  std::cout << '+';
  for (std::size_t col = 0; col < SnakeEngine::kCols; ++col) {
    std::cout << '-';
  }
  std::cout << "+\n";

  for (std::size_t row = 0; row < SnakeEngine::kRows; ++row) {
    std::cout << '|';
    for (std::size_t col = 0; col < SnakeEngine::kCols; ++col) {
      std::cout << board[row][col];
    }
    std::cout << "|\n";
  }

  std::cout << '+';
  for (std::size_t col = 0; col < SnakeEngine::kCols; ++col) {
    std::cout << '-';
  }
  std::cout << "+\n";

  std::cout << "Score: " << engine.score() << " | Tick: " << engine.tick() << " | Length: " << engine.body_size()
            << "\n";

  if (engine.has_food()) {
    const auto food = engine.food();
    std::cout << "Food: (" << food.row << ", " << food.col << ")\n";
  } else {
    std::cout << "Food: none (board fully occupied)\n";
  }
}

} // namespace

//===------------------------------ MAIN FUNCTION ------------------------------===//

auto main(int argc, char** argv) -> int {
  // Parse command-line arguments for TUI configuration.
  const std::uint32_t seed = (argc > 1) ? parse_u32_arg(argv[1], SnakeEngine::kDefaultSeed) : SnakeEngine::kDefaultSeed;
  const std::size_t   max_ticks = (argc > 2) ? parse_usize_arg(argv[2], 500U) : 500U;

  SnakeEngine engine(seed);

  std::cout << "Snake TUI started with seed=" << seed << " and max_ticks=" << max_ticks << "\n";
  std::cout << "Commands: w/a/s/d + ENTER to move, ENTER alone to continue, q to quit.\n";

  // Main game loop: render board, read user input, update game state, and check consistency.
  while (engine.is_alive() && engine.tick() < max_ticks) {
    print_board(engine);

    std::cout << "Move> ";

    std::string line;
    if (!std::getline(std::cin, line)) {
      break;
    }

    // Process user input for direction changes or quit command.
    if (!line.empty()) {
      const auto command = static_cast<char>(std::tolower(static_cast<unsigned char>(line.front())));
      if (command == 'q') {
        std::cout << "Quit requested.\n";
        break;
      }

      const auto next_direction = parse_direction(command);
      if (next_direction.has_value()) {
        engine.set_direction(*next_direction);
      }
    }

    engine.step();

    // Check for consistency after each step.
    if (!engine.is_consistent()) {
      std::cerr << "Internal consistency check failed at tick " << engine.tick() << "\n";
      return 2;
    }
  }

  print_board(engine);

  const auto& replay = engine.replay_log();
  std::cout << "\nFinal state: " << (engine.is_alive() ? "alive" : "dead") << "\n";
  std::cout << "Final score: " << engine.score() << "\n";
  std::cout << "Ticks executed: " << engine.tick() << "\n";
  std::cout << "Replay snapshots: " << replay.size() << "\n";

  // Print recent moves from the replay log for debugging purposes.
  if (replay.size() > 0U) {
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
