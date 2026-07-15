//===---------------------------------------------------------------------------===//
/**
 * @file Test_KD_Tree.cpp
 * @brief Google Test unit tests for KDTree.
 * @version 0.1
 * @date 2026-07-13
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "ads/trees/search/KD_Tree.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <random>
#include <vector>

using namespace ads::trees;

namespace {

//===----- TEST UTILITIES ------------------------------------------------------===//

using KdP2 = std::array<int, 2>;
using KdP3 = std::array<int, 3>;

static_assert(KDCoordinate<int>);
static_assert(KDCoordinate<double>);
static_assert(!KDCoordinate<bool>);
static_assert(!KDCoordinate<const int>);

template <typename Point>
auto kd_dist2(const Point& a, const Point& b) -> double {
  double sum = 0.0;
  for (std::size_t i = 0; i < a.size(); ++i) {
    const double d = static_cast<double>(a[i]) - static_cast<double>(b[i]);
    sum += d * d;
  }
  return sum;
}

template <typename Point>
auto kd_brute_nearest_dist(const std::vector<Point>& points, const Point& query) -> double {
  double best = std::numeric_limits<double>::infinity();
  for (const Point& p : points) {
    best = std::min(best, kd_dist2(p, query));
  }
  return best;
}

template <typename Tree, typename Point>
auto kd_to_vector(const Tree& tree, const Point& lower, const Point& upper) -> std::vector<Point> {
  auto               found = tree.range_search(lower, upper);
  std::vector<Point> out;
  out.reserve(found.size());
  for (std::size_t i = 0; i < found.size(); ++i) {
    out.push_back(found[i]);
  }
  return out;
}

template <typename Neighbors, typename Point>
auto kd_neighbors_to_dists(const Neighbors& found, const Point& query) -> std::vector<double> {
  std::vector<double> dists;
  dists.reserve(found.size());
  for (std::size_t i = 0; i < found.size(); ++i) {
    dists.push_back(kd_dist2(found[i], query));
  }
  return dists;
}

} // namespace

//===----- BASIC PROPERTIES ----------------------------------------------------===//

TEST(KDTreeBasic, EmptyTree) {
  KDTree<int, 2> tree;
  EXPECT_TRUE(tree.is_empty());
  EXPECT_EQ(tree.size(), 0U);
  EXPECT_EQ(tree.height(), -1);
  EXPECT_FALSE(tree.contains(KdP2{0, 0}));
  EXPECT_THROW((void)tree.nearest(KdP2{0, 0}), EmptyTreeException);
  EXPECT_TRUE(tree.k_nearest(KdP2{0, 0}, 3).is_empty());
  EXPECT_TRUE(tree.range_search(KdP2{-9, -9}, KdP2{9, 9}).is_empty());
}

TEST(KDTreeBasic, SinglePoint) {
  KDTree<int, 2> tree;
  tree.insert(KdP2{3, 4});
  EXPECT_EQ(tree.size(), 1U);
  EXPECT_EQ(tree.height(), 0);
  EXPECT_TRUE(tree.contains(KdP2{3, 4}));
  EXPECT_FALSE(tree.contains(KdP2{3, 5}));
  EXPECT_EQ(tree.nearest(KdP2{0, 0}), (KdP2{3, 4}));

  auto knn = tree.k_nearest(KdP2{0, 0}, 5);
  ASSERT_EQ(knn.size(), 1U);
  EXPECT_EQ(knn[0], (KdP2{3, 4}));
}

TEST(KDTreeBasic, InsertAndContains) {
  KDTree<int, 2>          tree;
  const std::vector<KdP2> points = {{5, 4}, {2, 6}, {13, 3}, {8, 7}, {3, 1}, {10, 2}};
  for (const KdP2& p : points) {
    tree.insert(p);
  }
  EXPECT_EQ(tree.size(), points.size());
  for (const KdP2& p : points) {
    EXPECT_TRUE(tree.contains(p)) << "missing (" << p[0] << ", " << p[1] << ")";
  }
  EXPECT_FALSE(tree.contains(KdP2{5, 5}));
  EXPECT_FALSE(tree.contains(KdP2{0, 0}));

  std::size_t visited = 0;
  tree.for_each([&](const KdP2&) -> void { ++visited; });
  EXPECT_EQ(visited, points.size());
}

//===----- BALANCED BUILD ------------------------------------------------------===//

TEST(KDTreeBuild, BalancedHeightIsLogarithmic) {
  std::mt19937                       rng(20'260'713U);
  std::uniform_int_distribution<int> dist(-1'000, 1'000);
  std::vector<KdP2>                  points(1'000);
  for (KdP2& p : points) {
    p = {dist(rng), dist(rng)};
  }

  KDTree<int, 2> tree(points.begin(), points.end());
  EXPECT_EQ(tree.size(), points.size());
  const int bound = 2 * static_cast<int>(std::ceil(std::log2(static_cast<double>(points.size()))));
  EXPECT_LE(tree.height(), bound);
  for (const KdP2& p : points) {
    EXPECT_TRUE(tree.contains(p));
  }
}

TEST(KDTreeBuild, DynamicArrayConstructor) {
  ads::arrays::DynamicArray<KdP2> points;
  for (const KdP2& p : {KdP2{1, 2}, {3, 4}, {5, 6}, {7, 8}}) {
    points.push_back(p);
  }
  KDTree<int, 2> tree(points);
  EXPECT_EQ(tree.size(), 4U);
  EXPECT_TRUE(tree.contains(KdP2{5, 6}));
}

TEST(KDTreeBuild, ExactDuplicatesRemainBalanced) {
  const std::vector<KdP2> points(1'024, KdP2{7, 7});
  KDTree<int, 2>          tree(points.begin(), points.end());

  EXPECT_EQ(tree.size(), points.size());
  EXPECT_LE(tree.height(), 10);
  EXPECT_TRUE(tree.contains(KdP2{7, 7}));
  EXPECT_EQ(tree.range_search(KdP2{7, 7}, KdP2{7, 7}).size(), points.size());
}

TEST(KDTreeBuild, EqualSplitCoordinatesRemainSearchable) {
  std::vector<KdP2> points;
  for (int value = -128; value <= 128; ++value) {
    points.push_back(KdP2{5, value});
  }

  KDTree<int, 2> tree(points.begin(), points.end());
  for (const KdP2& point : points) {
    EXPECT_TRUE(tree.contains(point));
  }
}

//===----- NEAREST-NEIGHBOUR ORACLE --------------------------------------------===//

TEST(KDTreeNearest, MatchesBruteForce2D) {
  std::mt19937                       rng(11U);
  std::uniform_int_distribution<int> dist(-100, 100);

  for (int trial = 0; trial < 30; ++trial) {
    const std::size_t n = 1U + static_cast<std::size_t>(trial) * 5U;
    std::vector<KdP2> points(n);
    for (KdP2& p : points) {
      p = {dist(rng), dist(rng)};
    }
    KDTree<int, 2> tree(points.begin(), points.end());

    for (int q = 0; q < 40; ++q) {
      const KdP2 query = {dist(rng), dist(rng)};
      EXPECT_EQ(kd_dist2(tree.nearest(query), query), kd_brute_nearest_dist(points, query));
    }
  }
}

TEST(KDTreeNearest, MatchesBruteForce3D) {
  std::mt19937                       rng(22U);
  std::uniform_int_distribution<int> dist(-40, 40);

  std::vector<KdP3> points(300);
  for (KdP3& p : points) {
    p = {dist(rng), dist(rng), dist(rng)};
  }
  KDTree<int, 3> tree(points.begin(), points.end());

  for (int q = 0; q < 200; ++q) {
    const KdP3 query = {dist(rng), dist(rng), dist(rng)};
    EXPECT_EQ(kd_dist2(tree.nearest(query), query), kd_brute_nearest_dist(points, query));
  }
}

TEST(KDTreeNearest, DynamicInsertsStayCorrect) {
  std::mt19937                       rng(33U);
  std::uniform_int_distribution<int> dist(-50, 50);

  std::vector<KdP2> points(60);
  for (KdP2& p : points) {
    p = {dist(rng), dist(rng)};
  }
  KDTree<int, 2> tree(points.begin(), points.begin() + 30);
  for (std::size_t i = 30; i < points.size(); ++i) {
    tree.insert(points[i]);
  }
  EXPECT_EQ(tree.size(), points.size());

  for (int q = 0; q < 100; ++q) {
    const KdP2 query = {dist(rng), dist(rng)};
    EXPECT_EQ(kd_dist2(tree.nearest(query), query), kd_brute_nearest_dist(points, query));
  }
}

//===----- K-NEAREST ORACLE ----------------------------------------------------===//

TEST(KDTreeKNearest, MatchesBruteForce) {
  std::mt19937                       rng(44U);
  std::uniform_int_distribution<int> dist(-60, 60);

  for (int trial = 0; trial < 15; ++trial) {
    const std::size_t n = 5U + static_cast<std::size_t>(trial) * 6U;
    std::vector<KdP2> points(n);
    for (KdP2& p : points) {
      p = {dist(rng), dist(rng)};
    }
    KDTree<int, 2> tree(points.begin(), points.end());

    for (std::size_t k : {std::size_t{1}, std::size_t{3}, std::size_t{7}, n + 2}) {
      const KdP2 query = {dist(rng), dist(rng)};
      auto       knn   = tree.k_nearest(query, k);
      EXPECT_EQ(knn.size(), std::min(k, n));

      std::vector<double> got = kd_neighbors_to_dists(knn, query);
      EXPECT_TRUE(std::is_sorted(got.begin(), got.end())); // ascending by distance

      std::vector<double> all;
      all.reserve(n);
      for (const KdP2& p : points) {
        all.push_back(kd_dist2(p, query));
      }
      std::sort(all.begin(), all.end());
      all.resize(std::min(k, n));
      EXPECT_EQ(got, all) << "k = " << k;
    }
  }
}

//===----- RANGE SEARCH ORACLE -------------------------------------------------===//

TEST(KDTreeRange, MatchesBruteForce) {
  std::mt19937                       rng(55U);
  std::uniform_int_distribution<int> dist(-80, 80);

  for (int trial = 0; trial < 20; ++trial) {
    const std::size_t n = 1U + static_cast<std::size_t>(trial) * 7U;
    std::vector<KdP2> points(n);
    for (KdP2& p : points) {
      p = {dist(rng), dist(rng)};
    }
    KDTree<int, 2> tree(points.begin(), points.end());

    for (int q = 0; q < 20; ++q) {
      int        a     = dist(rng);
      int        b     = dist(rng);
      int        c     = dist(rng);
      int        d     = dist(rng);
      const KdP2 lower = {std::min(a, b), std::min(c, d)};
      const KdP2 upper = {std::max(a, b), std::max(c, d)};

      std::vector<KdP2> got = kd_to_vector(tree, lower, upper);
      std::vector<KdP2> expected;
      for (const KdP2& p : points) {
        if (p[0] >= lower[0] && p[0] <= upper[0] && p[1] >= lower[1] && p[1] <= upper[1]) {
          expected.push_back(p);
        }
      }
      std::sort(got.begin(), got.end());
      std::sort(expected.begin(), expected.end());
      EXPECT_EQ(got, expected);
    }
  }
}

TEST(KDTreeRange, InvalidBoxThrows) {
  KDTree<int, 2> tree;
  tree.insert(KdP2{1, 1});
  EXPECT_THROW((void)tree.range_search(KdP2{5, 0}, KdP2{0, 9}), BinaryTreeException);
}

//===----- DUPLICATES ----------------------------------------------------------===//

TEST(KDTreeDuplicates, StoresAndCountsDuplicates) {
  KDTree<int, 2> tree;
  tree.insert(KdP2{2, 2});
  tree.insert(KdP2{2, 2});
  tree.insert(KdP2{2, 2});
  tree.insert(KdP2{9, 9});

  EXPECT_EQ(tree.size(), 4U);
  EXPECT_TRUE(tree.contains(KdP2{2, 2}));

  auto knn = tree.k_nearest(KdP2{2, 2}, 3);
  ASSERT_EQ(knn.size(), 3U);
  for (std::size_t i = 0; i < knn.size(); ++i) {
    EXPECT_EQ(knn[i], (KdP2{2, 2}));
  }

  auto in_box = tree.range_search(KdP2{0, 0}, KdP2{3, 3});
  EXPECT_EQ(in_box.size(), 3U);
}

//===----- DEGENERATE (UNBALANCED) SHAPES --------------------------------------===//

TEST(KDTreeDegenerate, DeepChainDoesNotOverflow) {
  const int      n = 20'000;
  KDTree<int, 2> tree;
  // Points increasing on both axes force a long descent under the cycling rule.
  for (int i = 0; i < n; ++i) {
    tree.insert(KdP2{i, i});
  }
  EXPECT_EQ(tree.size(), static_cast<std::size_t>(n));
  EXPECT_GT(tree.height(), 100); // clearly unbalanced, yet every op below is iterative

  EXPECT_TRUE(tree.contains(KdP2{15'000, 15'000}));
  // (12345,12345) and (12344,12344) are both at squared distance 1: assert the
  // distance, not a specific tied point.
  EXPECT_EQ(kd_dist2(tree.nearest(KdP2{12'345, 12'344}), KdP2{12'345, 12'344}), 1.0);

  auto in_box = tree.range_search(KdP2{10, 10}, KdP2{20, 20});
  EXPECT_EQ(in_box.size(), 11U); // (10,10) .. (20,20)

  std::size_t visited = 0;
  tree.for_each([&](const KdP2&) -> void { ++visited; });
  EXPECT_EQ(visited, static_cast<std::size_t>(n));
  // Destructor (iterative teardown) runs here without overflowing.
}

//===----- COORDINATE TYPES ----------------------------------------------------===//

TEST(KDTreeTypes, UnsignedCoordinatesNoWraparound) {
  using PU = std::array<unsigned, 2>;
  KDTree<unsigned, 2> tree;
  tree.insert(PU{10U, 10U});
  tree.insert(PU{100U, 100U});
  tree.insert(PU{50U, 40U});

  // Query below all points: the distance helper computes the unsigned magnitude
  // before converting it to the accumulation type, so subtraction cannot wrap.
  EXPECT_EQ(tree.nearest(PU{0U, 0U}), (PU{10U, 10U}));
  EXPECT_EQ(tree.nearest(PU{60U, 45U}), (PU{50U, 40U}));
}

TEST(KDTreeTypes, UInt64DistancesPreserveLowBits) {
  using Point                  = std::array<std::uint64_t, 1>;
  constexpr std::uint64_t base = std::uint64_t{1} << 63U;

  KDTree<std::uint64_t, 1> tree;
  tree.insert(Point{base});
  tree.insert(Point{base + 2U});

  EXPECT_EQ(tree.nearest(Point{base}), (Point{base}));
  auto nearest = tree.k_nearest(Point{base}, 2);
  ASSERT_EQ(nearest.size(), 2U);
  EXPECT_EQ(nearest[0], (Point{base}));
  EXPECT_EQ(nearest[1], (Point{base + 2U}));
}

TEST(KDTreeTypes, DoubleCoordinates) {
  using PD                 = std::array<double, 2>;
  std::vector<PD>   points = {{0.0, 0.0}, {1.5, 2.5}, {-3.0, 4.0}, {2.0, -1.0}};
  KDTree<double, 2> tree(points.begin(), points.end());

  EXPECT_EQ(tree.nearest(PD{1.4, 2.4}), (PD{1.5, 2.5}));
  auto knn = tree.k_nearest(PD{0.0, 0.0}, 2);
  ASSERT_EQ(knn.size(), 2U);
  EXPECT_EQ(knn[0], (PD{0.0, 0.0}));
}

TEST(KDTreeTypes, SingleDimension) {
  KDTree<int, 1> tree;
  for (int v : {5, 1, 9, 3, 7}) {
    tree.insert(std::array<int, 1>{v});
  }
  EXPECT_EQ(tree.nearest(std::array<int, 1>{10}), (std::array<int, 1>{9}));
  auto in_box = tree.range_search(std::array<int, 1>{2}, std::array<int, 1>{7});
  EXPECT_EQ(in_box.size(), 3U); // 3, 5, 7
}

TEST(KDTreeTypes, RejectsNonFiniteCoordinatesWithoutMutation) {
  using Point           = std::array<double, 2>;
  const double nan      = std::numeric_limits<double>::quiet_NaN();
  const double infinity = std::numeric_limits<double>::infinity();

  KDTree<double, 2> tree;
  EXPECT_THROW(tree.insert(Point{nan, 0.0}), BinaryTreeException);
  EXPECT_TRUE(tree.is_empty());

  tree.insert(Point{1.0, 2.0});
  EXPECT_THROW((void)tree.contains(Point{infinity, 2.0}), BinaryTreeException);
  EXPECT_THROW((void)tree.nearest(Point{nan, 0.0}), BinaryTreeException);
  EXPECT_THROW((void)tree.k_nearest(Point{0.0, infinity}, 1), BinaryTreeException);
  EXPECT_THROW((void)tree.range_search(Point{0.0, 0.0}, Point{infinity, 3.0}), BinaryTreeException);
  EXPECT_EQ(tree.size(), 1U);

  const std::vector<Point> invalid_points = {{0.0, 0.0}, {nan, 1.0}};
  EXPECT_THROW((KDTree<double, 2>(invalid_points.begin(), invalid_points.end())), BinaryTreeException);
}

TEST(KDTreeTypes, RejectsUnrepresentableSquaredDistance) {
  using Point               = std::array<long double, 1>;
  const long double maximum = std::numeric_limits<long double>::max();

  KDTree<long double, 1> tree;
  tree.insert(Point{maximum});
  EXPECT_THROW((void)tree.nearest(Point{-maximum}), BinaryTreeException);
  EXPECT_THROW((void)tree.k_nearest(Point{-maximum}, 1), BinaryTreeException);
}

//===----- MOVE SEMANTICS ------------------------------------------------------===//

TEST(KDTreeMove, MoveConstructorTransfersOwnership) {
  std::vector<KdP2> points = {{1, 1}, {2, 2}, {3, 3}, {4, 4}};
  KDTree<int, 2>    source(points.begin(), points.end());
  KDTree<int, 2>    moved(std::move(source));

  EXPECT_EQ(moved.size(), 4U);
  EXPECT_TRUE(moved.contains(KdP2{3, 3}));
  EXPECT_TRUE(source.is_empty()); // NOLINT(bugprone-use-after-move): checking moved-from state
  EXPECT_EQ(source.size(), 0U);
}

TEST(KDTreeMove, MoveAssignmentReplacesTarget) {
  std::vector<KdP2> a = {{1, 1}, {2, 2}};
  std::vector<KdP2> b = {{7, 7}, {8, 8}, {9, 9}};
  KDTree<int, 2>    target(a.begin(), a.end());
  KDTree<int, 2>    source(b.begin(), b.end());

  target = std::move(source);
  EXPECT_EQ(target.size(), 3U);
  EXPECT_TRUE(target.contains(KdP2{8, 8}));
  EXPECT_FALSE(target.contains(KdP2{1, 1}));
  EXPECT_TRUE(source.is_empty()); // NOLINT(bugprone-use-after-move): checking moved-from state
}
