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

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include <cctype>
#include <charconv>
#include <cstdint>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

namespace {

using ads::apps::snake::CellDelta;
using ads::apps::snake::Direction;
using ads::apps::snake::Position;
using ads::apps::snake::SnakeEngine;

//===--------------------------- TERMINAL ROW LAYOUT ---------------------------===//

// Fixed terminal row indices for different sections of the TUI. These are 1-indexed
// for ANSI cursor positioning and include spacing for a title and controls legend.
constexpr int kTitleRow        = 1;
constexpr int kLegendRow       = 2;
constexpr int kTopBorderRow    = 4;
constexpr int kBoardStartRow   = kTopBorderRow + 1;
constexpr int kBoardStartCol   = 2; // Column offset inside border '|'.
constexpr int kBottomBorderRow = static_cast<int>(SnakeEngine::kRows) + kBoardStartRow;
constexpr int kStatusRow       = kBottomBorderRow + 2;
constexpr int kFoodRow         = kStatusRow + 1;
constexpr int kPromptRow       = kFoodRow + 1;
constexpr int kFinalOutputRow  = kPromptRow + 1;

// Minimum terminal dimensions for proper rendering.
constexpr int kMinTerminalRows = kFinalOutputRow + 3;                      // Final output needs ~3-4 lines.
constexpr int kMinTerminalCols = static_cast<int>(SnakeEngine::kCols) + 2; // Board + borders.

//===--------------------------------- STYLING ---------------------------------===//

constexpr const char* kStyleReset  = "\033[0m";
constexpr const char* kStyleBold   = "\033[1m";
constexpr const char* kStyleDim    = "\033[2m";
constexpr const char* kStyleTitle  = "\033[1;97m";
constexpr const char* kStyleFrame  = "\033[96m";
constexpr const char* kStyleHead   = "\033[1;92m";
constexpr const char* kStyleBody   = "\033[32m";
constexpr const char* kStyleFood   = "\033[1;91m";
constexpr const char* kStyleAccent = "\033[93m";
constexpr const char* kStyleMuted  = "\033[90m";
constexpr const char* kStyleAlive  = "\033[1;92m";
constexpr const char* kStyleDead   = "\033[1;91m";
constexpr const char* kStylePrompt = "\033[1;96m";
constexpr const char* kStyleError  = "\033[1;91m";

//===---------------------------- BOX-DRAWING GLYPHS ---------------------------===//

// Unicode box-drawing characters for cleaner borders.
constexpr const char* kBoxTopLeft     = "╔";
constexpr const char* kBoxTopRight    = "╗";
constexpr const char* kBoxBottomLeft  = "╚";
constexpr const char* kBoxBottomRight = "╝";
constexpr const char* kBoxHorizontal  = "═";
constexpr const char* kBoxVertical    = "║";

// Single-width Unicode glyphs for cute snake rendering.
constexpr const char* kSnakeHead = "◉"; // U+25C9: Fisheye (filled circle with outline)
constexpr const char* kSnakeBody = "○"; // U+25CB: White circle
constexpr const char* kSnakeFood = "●"; // U+25CF: Black circle

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

/**
 * @brief Retrieves current terminal dimensions using ioctl.
 * @return Terminal size when available, std::nullopt otherwise.
 */
struct TerminalSize {
  int rows = 0;
  int cols = 0;
};

/// @brief Gets the current terminal size in rows and columns.
[[nodiscard]] auto get_terminal_size() -> std::optional<TerminalSize> {
  struct winsize ws = {};

  if (::ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != 0) {
    return std::nullopt;
  }

  return TerminalSize{.rows = static_cast<int>(ws.ws_row), .cols = static_cast<int>(ws.ws_col)};
}

//===------------------------------ STYLE HELPERS ------------------------------===//

/// @brief Returns a string label for the given direction enum value.
[[nodiscard]] auto direction_label(Direction direction) -> const char* {
  using enum Direction;

  switch (direction) {
  case kUp:
    return "UP";
  case kDown:
    return "DOWN";
  case kLeft:
    return "LEFT";
  case kRight:
    return "RIGHT";
  default:
    return "UNKNOWN";
  }
}

/// @brief Returns a checkerboard glyph for empty cells based on their coordinates.
[[nodiscard]] auto checker_empty_glyph(const Position& cell) -> char {
  return ((cell.row + cell.col) % 2 == 0) ? '.' : ' ';
}

/// @brief Draws a single cell at the given board coordinates with styling based on its content.
auto draw_cell(const Position& cell, char glyph) -> void {
  switch (glyph) {
  case '@':
    std::cout << kStyleHead << kSnakeHead << kStyleReset;
    return;
  case 'o':
    std::cout << kStyleBody << kSnakeBody << kStyleReset;
    return;
  case '*':
    std::cout << kStyleFood << kSnakeFood << kStyleReset;
    return;
  case ' ': {
    const char empty_glyph = checker_empty_glyph(cell);
    std::cout << kStyleDim << kStyleMuted << empty_glyph << kStyleReset;
    return;
  }
  default:
    std::cout << kStyleAccent << glyph << kStyleReset;
    return;
  }
}

/// @brief Draws the horizontal border of the game board at the specified row.
/// @param row Terminal row position for the border.
/// @param is_top True for top border, false for bottom border.
auto draw_horizontal_border(int row, bool is_top) -> void {
  ansi_move_to(row, 1);
  std::cout << kStyleBold << kStyleFrame;
  std::cout << (is_top ? kBoxTopLeft : kBoxBottomLeft);
  for (std::size_t col = 0; col < SnakeEngine::kCols; ++col) {
    std::cout << kBoxHorizontal;
  }
  std::cout << (is_top ? kBoxTopRight : kBoxBottomRight);
  std::cout << kStyleReset;
}

/// @brief Draws the title and controls legend at the top of the screen.
auto draw_header() -> void {
  ansi_move_to(kTitleRow, 1);
  ansi_clear_line();
  std::cout << kStyleTitle << "ADS Snake TUI" << kStyleReset;

  ansi_move_to(kLegendRow, 1);
  ansi_clear_line();
  std::cout << kStyleMuted << "Controls: W/A/S/D move (no Enter) | Q quit" << kStyleReset;
}

/// @brief Draws the input prompt line at the bottom of the TUI.
auto draw_prompt() -> void {
  ansi_move_to(kPromptRow, 1);
  ansi_clear_line();
  std::cout << kStylePrompt << "Key [W/A/S/D, Q]> " << kStyleReset << std::flush;
}

//===------------------------------ INPUT HELPERS ------------------------------===//

/**
 * @brief RAII guard enabling non-canonical terminal input for single-key controls.
 * @details When stdin is a TTY, disables canonical mode and echo so movement keys are
 *          processed immediately without requiring Enter. Original terminal settings
 *          are restored on destruction.
 */
class TerminalInputModeGuard {
public:
  TerminalInputModeGuard() { enable_raw_mode(); }

  ~TerminalInputModeGuard() { restore(); }

  TerminalInputModeGuard(const TerminalInputModeGuard&)                    = delete;
  auto operator=(const TerminalInputModeGuard&) -> TerminalInputModeGuard& = delete;

  [[nodiscard]] auto is_raw_mode() const noexcept -> bool { return raw_mode_enabled_; }

private:
  auto enable_raw_mode() -> void {
    if (::isatty(STDIN_FILENO) == 0) {
      return;
    }

    if (::tcgetattr(STDIN_FILENO, &original_mode_) != 0) {
      return;
    }

    struct termios raw_mode = original_mode_;
    raw_mode.c_lflag &= static_cast<tcflag_t>(~(ICANON | ECHO));
    raw_mode.c_cc[VMIN]  = 1;
    raw_mode.c_cc[VTIME] = 0;

    if (::tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw_mode) == 0) {
      raw_mode_enabled_ = true;
    }
  }

  auto restore() -> void {
    if (!raw_mode_enabled_) {
      return;
    }

    (void)::tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_mode_);
  }

  struct termios original_mode_    = {};
  bool           raw_mode_enabled_ = false;
};

/**
 * @brief Reads one input command in raw mode (TTY) or line mode (non-TTY).
 * @param raw_mode_enabled Whether terminal raw mode is active.
 * @return A single character command, '\0' for ignorable input, or std::nullopt on EOF.
 */
[[nodiscard]] auto read_input_command(bool raw_mode_enabled) -> std::optional<char> {
  if (raw_mode_enabled) {
    char key = '\0';
    if (::read(STDIN_FILENO, &key, 1) <= 0) {
      return std::nullopt;
    }

    return key;
  }

  std::string line;
  if (!std::getline(std::cin, line)) {
    return std::nullopt;
  }

  if (line.empty()) {
    return '\0';
  }

  return line.front();
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

/// @brief Determines if a raw input character should be ignored as part of an escape sequence.
[[nodiscard]] auto is_escape_prefix_or_parameter(char raw_command) -> bool {
  return raw_command == '[' || raw_command == 'O' || (raw_command >= '0' && raw_command <= '?');
}

/// @brief Determines if a raw input character is the final byte of an escape sequence.
[[nodiscard]] auto is_escape_final_byte(char raw_command) -> bool {
  return raw_command >= '@' && raw_command <= '~';
}

/// @brief Checks if a raw input character should be ignored due to being part of an escape sequence.
[[nodiscard]] auto should_ignore_raw_key(bool raw_mode_enabled, char raw_command, bool& skipping_escape_sequence)
    -> bool {
  if (!raw_mode_enabled) {
    return false;
  }

  if (raw_command == '\033') {
    skipping_escape_sequence = true;
    return true;
  }

  if (!skipping_escape_sequence) {
    return false;
  }

  if (is_escape_prefix_or_parameter(raw_command)) {
    return true;
  }

  if (is_escape_final_byte(raw_command)) {
    skipping_escape_sequence = false;
    return true;
  }

  // Not a valid escape sequence, reset and process this character normally.
  skipping_escape_sequence = false;
  return false;
}

/// @brief Generic helper to parse an unsigned integer argument with error handling.
template <typename UInt>
[[nodiscard]] auto parse_unsigned_arg(const char* value, UInt fallback) -> UInt {
  static_assert(std::is_unsigned_v<UInt>);

  if (value == nullptr) {
    return fallback;
  }

  const std::string_view input(value);
  if (input.empty()) {
    return fallback;
  }

  UInt parsed          = 0;
  const auto [ptr, ec] = std::from_chars(input.data(), input.data() + input.size(), parsed, 10);

  if (ec != std::errc{} || ptr != input.data() + input.size()) {
    return fallback;
  }

  return parsed;
}

/**
 * @brief Parses a C-string into a uint32_t with overflow protection.
 * @param value Null-terminated string to parse (may be nullptr).
 * @param fallback Value returned when parsing fails or input is null.
 * @return The parsed integer, or @p fallback on error.
 */
[[nodiscard]] auto parse_u32_arg(const char* value, std::uint32_t fallback) -> std::uint32_t {
  return parse_unsigned_arg(value, fallback);
}

/**
 * @brief Parses a C-string into a size_t with error handling.
 * @param value Null-terminated string to parse (may be nullptr).
 * @param fallback Value returned when parsing fails or input is null.
 * @return The parsed value, or @p fallback on error.
 */
[[nodiscard]] auto parse_usize_arg(const char* value, std::size_t fallback) -> std::size_t {
  return parse_unsigned_arg(value, fallback);
}

//===---------------------------- RENDERING HELPERS ----------------------------===//

constexpr std::size_t kRecentMovesDisplayCount = 5;

/**
 * @brief Refreshes the status and food information lines at their fixed terminal rows.
 * @param engine Engine instance to read score, tick, length, and food data from.
 */
auto draw_status_line(const SnakeEngine& engine) -> void {
  const bool  alive       = engine.is_alive();
  const char* state_style = alive ? kStyleAlive : kStyleDead;

  ansi_move_to(kStatusRow, 1);
  ansi_clear_line();
  std::cout << kStyleAccent << "Score: " << engine.score() << kStyleReset << " | " << kStyleAccent
            << "Tick: " << engine.tick() << kStyleReset << " | " << kStyleAccent << "Length: " << engine.body_size()
            << kStyleReset << " | " << kStyleAccent << "Dir: " << direction_label(engine.direction()) << kStyleReset
            << " | " << kStyleAccent << "State: " << state_style << (alive ? "ALIVE" : "DEAD") << kStyleReset;

  ansi_move_to(kFoodRow, 1);
  ansi_clear_line();
  if (engine.has_food()) {
    const auto food = engine.food();
    std::cout << kStyleFood << "Food: (" << food.row << ", " << food.col << ")" << kStyleReset;
  } else {
    std::cout << kStyleMuted << "Food: none (board fully occupied)" << kStyleReset;
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
  draw_header();

  // Top border.
  draw_horizontal_border(kTopBorderRow, true);

  // Board rows.
  for (std::size_t row = 0; row < SnakeEngine::kRows; ++row) {
    ansi_move_to(kBoardStartRow + static_cast<int>(row), 1);
    std::cout << kStyleBold << kStyleFrame << kBoxVertical << kStyleReset;
    for (std::size_t col = 0; col < SnakeEngine::kCols; ++col) {
      const Position cell{.row = static_cast<int>(row), .col = static_cast<int>(col)};
      draw_cell(cell, board[row][col]);
    }
    std::cout << kStyleBold << kStyleFrame << kBoxVertical << kStyleReset;
  }

  // Bottom border.
  draw_horizontal_border(kBottomBorderRow, false);

  draw_status_line(engine);

  // Position cursor at prompt row.
  draw_prompt();
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
    const Position cell{.row = delta.row, .col = delta.col};
    draw_cell(cell, delta.glyph);
  }

  draw_status_line(engine);
  draw_prompt();
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
 *             Default seed: SnakeEngine::kDefaultSeed
 *             Default max_ticks: 500
 * @return 0 on success and consistent final state
 *         1 on terminal size validation failure
 *         2 on internal consistency failure during gameplay
 *         3 on final state consistency failure
 */
auto main(int argc, char** argv) -> int {
  const std::uint32_t seed = (argc > 1) ? parse_u32_arg(argv[1], SnakeEngine::kDefaultSeed) : SnakeEngine::kDefaultSeed;
  const std::size_t   max_ticks = (argc > 2) ? parse_usize_arg(argv[2], 500U) : 500U;

  // Validate terminal dimensions before starting.
  if (const auto terminal_size = get_terminal_size();
      terminal_size && terminal_size->rows > 0 && terminal_size->cols > 0
      && (terminal_size->rows < kMinTerminalRows || terminal_size->cols < kMinTerminalCols)) {
    std::cerr << kStyleError << "Error: Terminal too small!" << kStyleReset << "\n";
    std::cerr << "Required: " << kMinTerminalRows << " rows x " << kMinTerminalCols << " cols minimum\n";
    std::cerr << "Current:  " << terminal_size->rows << " rows x " << terminal_size->cols << " cols\n";
    std::cerr << "Please resize your terminal and try again.\n";
    return 1;
  }

  SnakeEngine            engine(seed);
  TerminalInputModeGuard input_mode_guard;

  // Initial full render.
  draw_full_board(engine);

  // Main game loop with differential rendering.
  bool skipping_escape_sequence = false;
  bool stop_requested           = false;
  while (!stop_requested && engine.is_alive() && engine.tick() < max_ticks) {
    const auto command_input = read_input_command(input_mode_guard.is_raw_mode());
    if (!command_input.has_value()) {
      stop_requested = true;
      continue;
    }

    const char raw_command = *command_input;
    if (should_ignore_raw_key(input_mode_guard.is_raw_mode(), raw_command, skipping_escape_sequence)) {
      continue;
    }

    if (raw_command == '\0' || raw_command == '\n' || raw_command == '\r') {
      continue;
    }

    const auto command = static_cast<char>(std::tolower(static_cast<unsigned char>(raw_command)));
    if (command == 'q') {
      stop_requested = true;
      continue;
    }

    const auto next_direction = parse_direction(command);
    if (!next_direction.has_value()) {
      draw_prompt();
      continue;
    }

    engine.set_direction(*next_direction);
    engine.step();

    if (!engine.is_consistent()) {
      ansi_show_cursor();
      ansi_move_to(kFinalOutputRow, 1);
      std::cerr << kStyleError << "Internal consistency check failed at tick " << engine.tick() << kStyleReset << "\n";
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
  ansi_move_to(kFinalOutputRow, 1);

  const auto& replay = engine.replay_log();
  std::cout << "\nFinal state: " << (engine.is_alive() ? "alive" : "dead") << "\n";
  std::cout << "Final score: " << engine.score() << "\n";
  std::cout << "Ticks executed: " << engine.tick() << "\n";
  std::cout << "Replay snapshots: " << replay.size() << "\n";

  if (!replay.is_empty()) {
    const std::size_t history_to_print =
        (replay.size() < kRecentMovesDisplayCount) ? replay.size() : kRecentMovesDisplayCount;
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
