//===---------------------------------------------------------------------------===//
/**
 * @file Snake_Engine.cpp
 * @brief Implementation of the Snake integration/stress engine.
 * @version 0.1
 * @date 2026-02-06
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "snake/Snake_Engine.hpp"

#include <algorithm>
#include <random>

namespace ads::apps::snake {

//===------------------------- SNAKE ENGINE LIFECYCLE --------------------------===//

// Constructor initializes the engine with a given seed for reproducibility.
SnakeEngine::SnakeEngine(std::uint32_t seed) :
    snake_(kBoardCellCount),
    occupied_(kBoardCellCount * 2U),
    rng_(seed),
    seed_(seed) {
  reset(seed);
}

// Resets the game state to the initial configuration using the provided seed.
auto SnakeEngine::reset(std::uint32_t seed) -> void {
  seed_ = seed;
  rng_.seed(seed_);

  snake_.clear();
  occupied_.clear();
  replay_log_.clear();
  deltas_.clear();

  direction_ = Direction::kRight;
  alive_     = true;
  score_     = 0;
  tick_      = 0;
  has_food_  = false;

  const auto start_row = static_cast<int>(kRows / 2U);
  const auto start_col = static_cast<int>(kCols / 2U);

  for (std::size_t offset = 0; offset < kInitialSnakeLength; ++offset) {
    Position segment{};
    segment.row = start_row;
    segment.col = start_col - static_cast<int>(offset);

    snake_.push_back(segment);
    occupied_.put(to_key(segment), true);
  }

  place_food();
  append_snapshot(false);
}

auto SnakeEngine::reset() -> void {
  reset(seed_);
}

//===------------------------------ GAME CONTROL -------------------------------===//

auto SnakeEngine::set_direction(Direction direction) -> void {
  if (!is_opposite(direction_, direction)) {
    direction_ = direction;
  }
}

auto SnakeEngine::step() -> bool {
  deltas_.clear();

  if (!alive_) {
    return false;
  }

  const Position current_head = snake_.front();
  const Position next_head    = next_position(current_head);

  if (!in_bounds(next_head)) {
    alive_ = false;
    ++tick_;
    append_snapshot(false);
    return false;
  }

  const bool     grew_this_tick = has_food_ && (next_head == food_);
  const Position old_tail       = snake_.back();

  // Moving into the previous tail cell is valid when not growing,
  // because the tail is removed in the same tick.
  const bool moving_into_tail = (!grew_this_tick) && (next_head == old_tail);

  if (occupied_.contains(to_key(next_head)) && !moving_into_tail) {
    alive_ = false;
    ++tick_;
    append_snapshot(false);
    return false;
  }

  snake_.push_front(next_head);
  occupied_.put(to_key(next_head), true);

  if (grew_this_tick) {
    ++score_;
    place_food();
  } else {
    occupied_.erase(to_key(old_tail));
    snake_.pop_back();
  }

  // Record cell deltas for differential rendering.
  deltas_.push_back(CellDelta{next_head.row, next_head.col, '@'});
  deltas_.push_back(CellDelta{current_head.row, current_head.col, 'o'});

  if (!grew_this_tick) {
    deltas_.push_back(CellDelta{old_tail.row, old_tail.col, ' '});
  }

  if (grew_this_tick && has_food_) {
    deltas_.push_back(CellDelta{food_.row, food_.col, '*'});
  }

  ++tick_;
  append_snapshot(grew_this_tick);

  return alive_;
}

//===-------------------------------- ACCESSORS --------------------------------===//

auto SnakeEngine::direction() const noexcept -> Direction {
  return direction_;
}

auto SnakeEngine::is_alive() const noexcept -> bool {
  return alive_;
}

auto SnakeEngine::score() const noexcept -> int {
  return score_;
}

auto SnakeEngine::tick() const noexcept -> std::size_t {
  return tick_;
}

auto SnakeEngine::body_size() const noexcept -> std::size_t {
  return snake_.size();
}

auto SnakeEngine::has_food() const noexcept -> bool {
  return has_food_;
}

auto SnakeEngine::head() const -> Position {
  return snake_.front();
}

auto SnakeEngine::food() const -> Position {
  return food_;
}

auto SnakeEngine::replay_log() const noexcept -> const ads::arrays::DynamicArray<TickSnapshot>& {
  return replay_log_;
}

auto SnakeEngine::deltas() const noexcept -> const ads::arrays::DynamicArray<CellDelta>& {
  return deltas_;
}

//===-------------------------------- RENDERING --------------------------------===//

auto SnakeEngine::render_board() const -> Board {
  Board board;

  for (std::size_t row = 0; row < kRows; ++row) {
    board[row].fill(' ');
  }

  for (std::size_t i = 0; i < snake_.size(); ++i) {
    const Position segment                                                              = snake_[i];
    const char     glyph                                                                = (i == 0U) ? '@' : 'o';
    board[static_cast<std::size_t>(segment.row)][static_cast<std::size_t>(segment.col)] = glyph;
  }

  if (has_food_) {
    board[static_cast<std::size_t>(food_.row)][static_cast<std::size_t>(food_.col)] = '*';
  }

  return board;
}

auto SnakeEngine::is_consistent() const -> bool {
  if (snake_.is_empty()) {
    return false;
  }

  if (snake_.size() != occupied_.size()) {
    return false;
  }

  ads::associative::HashMap<std::size_t, bool> visited(snake_.size() * 2U + 1U);

  for (std::size_t i = 0; i < snake_.size(); ++i) {
    const Position segment = snake_[i];

    if (!in_bounds(segment)) {
      return false;
    }

    const std::size_t key = to_key(segment);
    if (visited.contains(key)) {
      return false;
    }

    if (!occupied_.contains(key)) {
      return false;
    }

    visited.put(key, true);
  }

  for (const auto& [key, _] : occupied_) {
    if (!visited.contains(key)) {
      return false;
    }
  }

  if (has_food_) {
    if (!in_bounds(food_)) {
      return false;
    }

    if (occupied_.contains(to_key(food_))) {
      return false;
    }
  }

  return true;
}

//===--------------------------- INTERNAL UTILITIES ----------------------------===//

auto SnakeEngine::is_opposite(Direction lhs, Direction rhs) noexcept -> bool {
  return (lhs == Direction::kUp && rhs == Direction::kDown) || (lhs == Direction::kDown && rhs == Direction::kUp)
         || (lhs == Direction::kLeft && rhs == Direction::kRight)
         || (lhs == Direction::kRight && rhs == Direction::kLeft);
}

auto SnakeEngine::in_bounds(const Position& position) noexcept -> bool {
  return position.row >= 0 && position.row < static_cast<int>(kRows) && position.col >= 0
         && position.col < static_cast<int>(kCols);
}

auto SnakeEngine::to_key(const Position& position) noexcept -> std::size_t {
  return static_cast<std::size_t>(position.row) * kCols + static_cast<std::size_t>(position.col);
}

auto SnakeEngine::next_position(const Position& current) const -> Position {
  Position next = current;

  switch (direction_) {
  case Direction::kUp:
    --next.row;
    break;
  case Direction::kDown:
    ++next.row;
    break;
  case Direction::kLeft:
    --next.col;
    break;
  case Direction::kRight:
    ++next.col;
    break;
  }

  return next;
}

auto SnakeEngine::place_food() -> void {
  if (snake_.size() >= kBoardCellCount) {
    has_food_ = false;
    return;
  }

  std::uniform_int_distribution<int> row_dist(0, static_cast<int>(kRows - 1U));
  std::uniform_int_distribution<int> col_dist(0, static_cast<int>(kCols - 1U));

  const std::size_t max_attempts = std::min<std::size_t>(kBoardCellCount * 2U, static_cast<std::size_t>(4'096U));

  for (std::size_t attempt = 0; attempt < max_attempts; ++attempt) {
    Position candidate{};
    candidate.row = row_dist(rng_);
    candidate.col = col_dist(rng_);

    if (!occupied_.contains(to_key(candidate))) {
      food_     = candidate;
      has_food_ = true;
      return;
    }
  }

  for (std::size_t row = 0; row < kRows; ++row) {
    for (std::size_t col = 0; col < kCols; ++col) {
      Position candidate{};
      candidate.row = static_cast<int>(row);
      candidate.col = static_cast<int>(col);

      if (!occupied_.contains(to_key(candidate))) {
        food_     = candidate;
        has_food_ = true;
        return;
      }
    }
  }

  has_food_ = false;
}

auto SnakeEngine::append_snapshot(bool grew) -> void {
  TickSnapshot snapshot{};

  snapshot.tick      = tick_;
  snapshot.direction = direction_;
  snapshot.grew      = grew;
  snapshot.alive     = alive_;
  snapshot.score     = score_;
  snapshot.body_size = snake_.size();
  snapshot.has_food  = has_food_;
  snapshot.food      = food_;
  snapshot.head      = snake_.front();

  replay_log_.push_back(snapshot);
}

auto to_char(Direction direction) noexcept -> char {
  switch (direction) {
  case Direction::kUp:
    return 'U';
  case Direction::kDown:
    return 'D';
  case Direction::kLeft:
    return 'L';
  case Direction::kRight:
    return 'R';
  }

  return '?';
}

} // namespace ads::apps::snake

//===---------------------------------------------------------------------------===//
