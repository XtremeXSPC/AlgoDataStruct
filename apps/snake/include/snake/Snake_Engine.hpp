//===---------------------------------------------------------------------------===//
/**
 * @file Snake_Engine.hpp
 * @brief Snake game engine used to validate ADS data structures in integration.
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

enum class Direction : std::uint8_t {
  kUp,
  kDown,
  kLeft,
  kRight,
};

struct Position {
  int row = 0;
  int col = 0;

  auto operator==(const Position& other) const noexcept -> bool = default;
};

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

struct ScoreEntry {
  int         score  = 0;
  std::size_t run_id = 0;

  [[nodiscard]] auto operator<(const ScoreEntry& other) const noexcept -> bool;
  [[nodiscard]] auto operator>(const ScoreEntry& other) const noexcept -> bool;
  [[nodiscard]] auto operator==(const ScoreEntry& other) const noexcept -> bool = default;
};

using Leaderboard = ads::trees::AVLTree<ScoreEntry>;

class SnakeEngine {
public:
  static constexpr std::size_t   kRows               = 20;
  static constexpr std::size_t   kCols               = 30;
  static constexpr std::size_t   kBoardCellCount     = kRows * kCols;
  static constexpr std::size_t   kInitialSnakeLength = 3;
  static constexpr std::uint32_t kDefaultSeed        = 1'337U;

  using BoardRow = ads::arrays::StaticArray<char, kCols>;
  using Board    = ads::arrays::StaticArray<BoardRow, kRows>;

  explicit SnakeEngine(std::uint32_t seed = kDefaultSeed);

  auto reset(std::uint32_t seed) -> void;
  auto reset() -> void;

  auto set_direction(Direction direction) -> void;

  [[nodiscard]] auto direction() const noexcept -> Direction;
  [[nodiscard]] auto is_alive() const noexcept -> bool;
  [[nodiscard]] auto score() const noexcept -> int;
  [[nodiscard]] auto tick() const noexcept -> std::size_t;
  [[nodiscard]] auto body_size() const noexcept -> std::size_t;
  [[nodiscard]] auto has_food() const noexcept -> bool;

  [[nodiscard]] auto head() const -> Position;
  [[nodiscard]] auto food() const -> Position;

  auto step() -> bool;

  [[nodiscard]] auto render_board() const -> Board;
  [[nodiscard]] auto is_consistent() const -> bool;
  [[nodiscard]] auto replay_log() const noexcept -> const ads::arrays::DynamicArray<TickSnapshot>&;

private:
  ads::arrays::CircularArray<Position>         snake_;
  ads::associative::HashMap<std::size_t, bool> occupied_;
  ads::arrays::DynamicArray<TickSnapshot>      replay_log_;

  std::mt19937  rng_;
  Direction     direction_ = Direction::kRight;
  Position      food_      = {};
  bool          has_food_  = false;
  bool          alive_     = true;
  int           score_     = 0;
  std::size_t   tick_      = 0;
  std::uint32_t seed_      = kDefaultSeed;

  [[nodiscard]] static auto is_opposite(Direction lhs, Direction rhs) noexcept -> bool;
  [[nodiscard]] static auto in_bounds(const Position& position) noexcept -> bool;
  [[nodiscard]] static auto to_key(const Position& position) noexcept -> std::size_t;

  [[nodiscard]] auto next_position(const Position& current) const -> Position;

  auto place_food() -> void;
  auto append_snapshot(bool grew) -> void;
};

[[nodiscard]] auto to_char(Direction direction) noexcept -> char;

} // namespace ads::apps::snake

#endif // SNAKE_ENGINE_HPP

//===---------------------------------------------------------------------------===//
