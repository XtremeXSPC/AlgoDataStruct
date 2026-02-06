//===---------------------------------------------------------------------------===//
/**
 * @file Test_Snake_Engine.cpp
 * @brief Google Test unit tests for SnakeEngine.
 * @version 0.1
 * @date 2026-02-06
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "snake/Snake_Engine.hpp"

#include <gtest/gtest.h>

#include <random>

using ads::apps::snake::Direction;
using ads::apps::snake::SnakeEngine;

TEST(SnakeEngineTest, InitialStateIsConsistent) {
  SnakeEngine engine(42U);

  EXPECT_TRUE(engine.is_alive());
  EXPECT_EQ(engine.score(), 0);
  EXPECT_EQ(engine.tick(), 0U);
  EXPECT_EQ(engine.body_size(), SnakeEngine::kInitialSnakeLength);
  EXPECT_TRUE(engine.is_consistent());
  EXPECT_EQ(engine.replay_log().size(), 1U);
}

TEST(SnakeEngineTest, OppositeDirectionIsIgnored) {
  SnakeEngine engine(42U);

  const auto initial_head = engine.head();

  engine.set_direction(Direction::kLeft);
  engine.step();

  const auto moved_head = engine.head();

  EXPECT_EQ(moved_head.row, initial_head.row);
  EXPECT_EQ(moved_head.col, initial_head.col + 1);
  EXPECT_TRUE(engine.is_consistent());
}

TEST(SnakeEngineTest, RandomWalkMaintainsConsistency) {
  SnakeEngine  engine(1'234U);
  std::mt19937 rng(2'026U);

  std::uniform_int_distribution<int> direction_dist(0, 3);

  while (engine.is_alive() && engine.tick() < 500U) {
    switch (direction_dist(rng)) {
    case 0:
      engine.set_direction(Direction::kUp);
      break;
    case 1:
      engine.set_direction(Direction::kDown);
      break;
    case 2:
      engine.set_direction(Direction::kLeft);
      break;
    case 3:
      engine.set_direction(Direction::kRight);
      break;
    default:
      break;
    }

    engine.step();
    ASSERT_TRUE(engine.is_consistent()) << "Consistency failed at tick " << engine.tick();
  }
}

TEST(SnakeEngineTest, ReplayLogTracksTicks) {
  SnakeEngine engine(7U);

  std::size_t local_steps = 0;
  while (engine.is_alive() && local_steps < 25U) {
    engine.step();
    ++local_steps;
  }

  EXPECT_EQ(engine.tick(), local_steps);
  EXPECT_EQ(engine.replay_log().size(), local_steps + 1U);
}

//===---------------------------------------------------------------------------===//
