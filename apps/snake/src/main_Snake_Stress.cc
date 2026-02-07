//===---------------------------------------------------------------------------===//
/**
 * @file main_Snake_Stress.cc
 * @brief Automatic Snake stress workload for structural consistency checks.
 * @version 0.1
 * @date 2026-02-06
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "ads/arrays/Dynamic_Array.hpp"
#include "ads/associative/Hash_Map.hpp"
#include "snake/Snake_Engine.hpp"

#include <cstdint>
#include <cstdlib>
#include <format>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>

namespace {

using ads::apps::snake::Direction;
using ads::apps::snake::Leaderboard;
using ads::apps::snake::ScoreEntry;
using ads::apps::snake::SnakeEngine;

/**
 * @brief Parses a string argument into a uint32_t, with error handling and fallback.
 *
 * @param value The C-string to parse.
 * @param fallback The value to return if parsing fails.
 * @return std::uint32_t The parsed value, or the fallback if parsing fails.
 */
[[nodiscard]] auto parse_u32_arg(const char* value, std::uint32_t fallback) -> std::uint32_t {
  if (value == nullptr) {
    return fallback;
  }

  char* end = nullptr;

  // Use strtoul for parsing and check for errors and overflow.
  const auto parsed = std::strtoul(value, &end, 10);

  // Check for parsing errors and overflow.
  if (end == value || *end != '\0' || parsed > static_cast<unsigned long>(std::numeric_limits<std::uint32_t>::max())) {
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
 * @brief Generates a random direction using the provided random number generator.
 *
 * @param rng The random number generator to use for direction selection.
 * @return Direction A randomly selected direction (up, down, left, right).
 */
[[nodiscard]] auto random_direction(std::mt19937& rng) -> Direction {
  std::uniform_int_distribution<int> direction_dist(0, 3);

  // Map the random integer to a direction enum value.
  switch (direction_dist(rng)) {
  case 0:
    return Direction::kUp;
  case 1:
    return Direction::kDown;
  case 2:
    return Direction::kLeft;
  case 3:
    return Direction::kRight;
  default:
    return Direction::kRight;
  }
}

/**
 * @brief Prints the top scores from the leaderboard.
 *
 * @param leaderboard The leaderboard containing score entries.
 * @param count The number of top scores to print.
 */
auto print_top_scores(const Leaderboard& leaderboard, std::size_t count) -> void {
  ads::arrays::DynamicArray<ScoreEntry> ordered_entries;

  // Collect entries from the leaderboard into a dynamic array for sorting.
  for (const auto& entry : leaderboard) {
    ordered_entries.push_back(entry);
  }

  if (ordered_entries.is_empty()) {
    std::cout << "Top scores: none\n";
    return;
  }

  // Sort entries in ascending order (best scores last).
  const std::size_t actual_count = (count < ordered_entries.size()) ? count : ordered_entries.size();

  // Print top scores from best to worst.
  std::cout << "Top scores (best to worst):\n";
  for (std::size_t rank = 0; rank < actual_count; ++rank) {
    const std::size_t index = ordered_entries.size() - 1U - rank;
    const auto&       entry = ordered_entries[index];

    std::cout << "  #" << (rank + 1U) << " score=" << entry.score << " run_id=" << entry.run_id << "\n";
  }
}

} // namespace

//===------------------------------ MAIN FUNCTION ------------------------------===//

/**
 * @brief Main function for the Snake stress test.
 * @details Runs multiple episodes of the Snake game with random direction changes,
 *          checking for consistency after each step and summarizing results at the end.
 *          Command-line arguments allow configuration of episodes, max ticks, and seed.
 *
 * @param argc Argument count.
 * @param argv Argument vector (expects optional [episodes] [max_ticks] [seed]).
 * @return int Exit code (0 on success, 2 on consistency failure).
 */
auto main(int argc, char** argv) -> int {
  // Parse command-line arguments for stress test configuration.
  const std::size_t   episodes  = (argc > 1) ? parse_usize_arg(argv[1], 250U) : 250U;
  const std::size_t   max_ticks = (argc > 2) ? parse_usize_arg(argv[2], 2'000U) : 2'000U;
  const std::uint32_t seed = (argc > 3) ? parse_u32_arg(argv[3], SnakeEngine::kDefaultSeed) : SnakeEngine::kDefaultSeed;

  // Initialize random number generator for the stress test suite.
  std::mt19937                                suite_rng(seed);
  std::bernoulli_distribution                 change_dir_dist(0.35);
  Leaderboard                                 leaderboard;
  ads::associative::HashMap<int, std::size_t> score_frequency;

  // Statistics tracking variables.
  std::size_t total_ticks          = 0;
  std::size_t total_score          = 0;
  std::size_t consistency_failures = 0;
  std::size_t run_id_counter       = 0;
  std::size_t completed_runs       = 0;

  // Print initial configuration.
  std::cout << "Snake stress started with episodes=" << episodes << ", max_ticks=" << max_ticks << ", seed=" << seed
            << "\n";

  // Main stress test loop: run multiple episodes of the game with random direction changes.
  for (std::size_t run = 0; run < episodes; ++run) {
    const std::uint32_t episode_seed = suite_rng();
    SnakeEngine         engine(episode_seed);
    bool                run_failed = false;

    while (engine.is_alive() && engine.tick() < max_ticks) {
      if (change_dir_dist(suite_rng)) {
        engine.set_direction(random_direction(suite_rng));
      }

      engine.step();

      // Check for consistency after each step.
      if (!engine.is_consistent()) {
        ++consistency_failures;
        std::cerr << "Consistency failure detected at run=" << run << " tick=" << engine.tick()
                  << " seed=" << episode_seed << "\n";
        run_failed = true;
        break;
      }
    }

    // Final consistency check after episode completion.
    if (run_failed || !engine.is_consistent()) {
      if (!run_failed) {
        ++consistency_failures;
        std::cerr << "Final consistency failure at run=" << run << " seed=" << episode_seed << "\n";
      }
      break;
    }

    const int score = engine.score();

    total_ticks += engine.tick();
    total_score += static_cast<std::size_t>(score);

    score_frequency[score] += 1U;
    leaderboard.insert(ScoreEntry{.score = score, .run_id = run_id_counter});

    ++run_id_counter;
    ++completed_runs;
  }

  // Print summary statistics after stress test completion.
  if (consistency_failures > 0U) {
    std::cerr << "Stress failed: consistency_failures=" << consistency_failures << "\n";
    return 2;
  }

  const double avg_ticks =
      (completed_runs > 0U) ? static_cast<double>(total_ticks) / static_cast<double>(completed_runs) : 0.0;
  const double avg_score =
      (completed_runs > 0U) ? static_cast<double>(total_score) / static_cast<double>(completed_runs) : 0.0;

  // Print detailed statistics about the stress test results.
  std::cout << "\nStress summary\n";
  std::cout << "  Runs completed: " << completed_runs << "\n";
  std::cout << "  Total ticks: " << total_ticks << "\n";
  std::cout << std::format("  Avg ticks/run: {:.2f}\n", avg_ticks);
  std::cout << std::format("  Avg score/run: {:.2f}\n", avg_score);
  std::cout << "  Distinct scores: " << score_frequency.size() << "\n";

  // Print score distribution.
  if (!leaderboard.is_empty()) {
    const auto& best = leaderboard.find_max();
    std::cout << "  Best score: " << best.score << " (run_id=" << best.run_id << ")\n";
  }

  print_top_scores(leaderboard, 5U);

  return 0;
}

//===---------------------------------------------------------------------------===//
