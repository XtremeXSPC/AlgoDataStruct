//===---------------------------------------------------------------------------===//
/**
 * @file Snake_Engine.hpp
 * @brief Core Snake engine and value types for demos, stress workloads, and tests.
 * @details This header exposes a deterministic game engine that is intentionally
 *          built on top of ADS containers to validate integration behavior under
 *          interactive and stress scenarios.
 * @version 0.1
 * @date 2026-02-06
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#pragma once

#ifndef SNAKE_ENGINE_HPP
#define SNAKE_ENGINE_HPP

#include "ads/arrays/Circular_Array.hpp"
#include "ads/arrays/Dynamic_Array.hpp"
#include "ads/arrays/Static_Array.hpp"
#include "ads/associative/Hash_Map.hpp"
#include "ads/trees/AVL_Tree.hpp"

#include <cstddef>
#include <cstdint>
#include <random>

namespace ads::apps::snake {

/**
 * @brief Cardinal directions accepted by the Snake engine.
 */
enum class Direction : std::uint8_t {
  kUp,
  kDown,
  kLeft,
  kRight,
};

/**
 * @brief Grid position expressed as row/column coordinates.
 */
struct Position {
  int row = 0;
  int col = 0;

  /**
   * @brief Equality comparison for coordinates.
   */
  [[nodiscard]] auto operator==(const Position& other) const noexcept -> bool = default;
};

/**
 * @brief Single cell change produced by a tick, used for differential rendering.
 */
struct CellDelta {
  int  row   = 0;
  int  col   = 0;
  char glyph = ' ';
};

/**
 * @brief Immutable snapshot of the engine state for replay and diagnostics.
 */
struct TickSnapshot {
  std::size_t tick      = 0;
  Position    head      = {};
  Direction   direction = Direction::kRight;
  bool        grew      = false;
  bool        alive     = true;
  int         score     = 0;
  std::size_t body_size = 0;
  bool        has_food  = false;
  Position    food      = {};
};

/**
 * @brief Score record used in AVL-based leaderboard ranking.
 * @details Ordering is by ascending `score`, then ascending `run_id`, providing
 *          a strict total order suitable for `AVLTree` keys and allowing duplicate
 *          scores from different runs.
 */
struct ScoreEntry {
  int         score  = 0;
  std::size_t run_id = 0;

  /**
   * @brief Three-way comparison used to synthesize relational operators.
   */
  [[nodiscard]] auto operator<=>(const ScoreEntry& other) const noexcept = default;

  /**
   * @brief Equality comparison for score entries.
   */
  [[nodiscard]] auto operator==(const ScoreEntry& other) const noexcept -> bool = default;
};

/**
 * @brief Ordered leaderboard containing results from stress/game runs.
 */
using Leaderboard = ads::trees::AVLTree<ScoreEntry>;

/**
 * @brief Deterministic Snake engine built on ADS containers.
 * @details Internal storage intentionally uses:
 *            - `CircularArray` for body updates at both ends.
 *            - `HashMap` for O(1)-average occupancy checks.
 *            - `DynamicArray` for replay history.
 *            - `StaticArray` for fixed-size board rendering.
 */
class SnakeEngine {
public:
  /// @brief Number of board rows.
  static constexpr std::size_t kRows = 20;

  /// @brief Number of board columns.
  static constexpr std::size_t kCols = 30;

  /// @brief Total number of cells on the board.
  static constexpr std::size_t kBoardCellCount = kRows * kCols;

  /// @brief Initial snake length after reset.
  static constexpr std::size_t kInitialSnakeLength = 3;

  /// @brief Default deterministic seed used by demos/tests.
  static constexpr std::uint32_t kDefaultSeed = 1'337U;

  /**
   * @brief One fixed-size row used in board rendering.
   */
  using BoardRow = ads::arrays::StaticArray<char, kCols>;

  /**
   * @brief Fixed-size board snapshot returned by `render_board()`.
   */
  using Board = ads::arrays::StaticArray<BoardRow, kRows>;

  /**
   * @brief Constructs a new engine and immediately initializes game state.
   * @param seed Seed used by deterministic random food placement.
   */
  explicit SnakeEngine(std::uint32_t seed = kDefaultSeed);

  /**
   * @brief Resets engine state using the provided seed.
   * @param seed New deterministic seed.
   */
  auto reset(std::uint32_t seed) -> void;

  /**
   * @brief Resets engine state reusing the last configured seed.
   */
  auto reset() -> void;

  /**
   * @brief Sets movement direction for the next tick.
   * @details Opposite direction changes are ignored to avoid immediate reversal.
   * @param direction Requested movement direction.
   */
  auto set_direction(Direction direction) -> void;

  /** @brief Returns current movement direction. */
  [[nodiscard]] auto direction() const noexcept -> Direction;

  /** @brief Returns whether the snake is still alive. */
  [[nodiscard]] auto is_alive() const noexcept -> bool;

  /** @brief Returns current score (foods eaten). */
  [[nodiscard]] auto score() const noexcept -> int;

  /** @brief Returns number of completed ticks. */
  [[nodiscard]] auto tick() const noexcept -> std::size_t;

  /** @brief Returns current snake length. */
  [[nodiscard]] auto body_size() const noexcept -> std::size_t;

  /** @brief Returns whether a food item is currently active on board. */
  [[nodiscard]] auto has_food() const noexcept -> bool;

  /**
   * @brief Returns current head position.
   * @return Head coordinates.
   */
  [[nodiscard]] auto head() const -> Position;

  /**
   * @brief Returns current food position.
   * @return Food coordinates.
   * @note Value is meaningful only when `has_food()` is true.
   */
  [[nodiscard]] auto food() const -> Position;

  /**
   * @brief Advances the simulation by one tick.
   * @return True if the snake remains alive after the tick, false otherwise.
   */
  auto step() -> bool;

  /**
   * @brief Produces a renderable board snapshot.
   * @return Board with snake body/head and food glyphs.
   */
  [[nodiscard]] auto render_board() const -> Board;

  /**
   * @brief Validates internal engine invariants.
   * @return True if body, occupancy map, and food state are consistent.
   */
  [[nodiscard]] auto is_consistent() const -> bool;

  /**
   * @brief Returns immutable replay history collected at each tick.
   * @return Reference to snapshots including initial state.
   */
  [[nodiscard]] auto replay_log() const noexcept -> const ads::arrays::DynamicArray<TickSnapshot>&;

  /**
   * @brief Returns cell deltas produced by the most recent tick.
   * @return Reference to the delta list (empty on death or before first step).
   */
  [[nodiscard]] auto deltas() const noexcept -> const ads::arrays::DynamicArray<CellDelta>&;

private:
  //===----------------------------- DATA MEMBERS ------------------------------===//
  ads::arrays::CircularArray<Position>         snake_;      ///< Snake body segments from head to tail.
  ads::arrays::DynamicArray<TickSnapshot>      replay_log_; ///< Replay history of game ticks.
  ads::arrays::DynamicArray<CellDelta>         deltas_;     ///< Cell changes from the most recent tick.
  ads::associative::HashMap<std::size_t, bool> occupied_;   ///< Occupancy map for O(1) checks.

  /// Game state variables.
  std::mt19937  rng_;
  Direction     direction_ = Direction::kRight;
  Position      food_      = {};
  bool          has_food_  = false;
  bool          alive_     = true;
  int           score_     = 0;
  std::size_t   tick_      = 0;
  std::uint32_t seed_      = kDefaultSeed;

  //===------------------------ PRIVATE HELPER METHODS -------------------------===//

  /** @brief Returns whether two directions are opposite. */
  [[nodiscard]] static auto is_opposite(Direction lhs, Direction rhs) noexcept -> bool;

  /** @brief Returns whether a position is within the board bounds. */
  [[nodiscard]] static auto in_bounds(const Position& position) noexcept -> bool;

  /** @brief Converts a position to a unique key for occupancy tracking. */
  [[nodiscard]] static auto to_key(const Position& position) noexcept -> std::size_t;

  /** @brief Computes the next head position based on the current direction. */
  [[nodiscard]] auto next_position(const Position& current) const -> Position;

  /**
   * @brief Places food at a random unoccupied position on the board.
   * @details If the board is full, sets `has_food_` to false.
   */
  auto place_food() -> void;

  /**
   * @brief Appends a snapshot of the current state to the replay log.
   * @param grew Whether the snake grew during this tick (used for snapshot data).
   */
  auto append_snapshot(bool grew) -> void;
};

/**
 * @brief Converts a direction to compact character representation.
 * @param direction Direction enum value.
 * @return One of `U`, `D`, `L`, `R`.
 */
[[nodiscard]] auto to_char(Direction direction) noexcept -> char;

} // namespace ads::apps::snake

#endif // SNAKE_ENGINE_HPP

//===---------------------------------------------------------------------------===//
