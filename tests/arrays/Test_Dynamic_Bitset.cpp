//===---------------------------------------------------------------------------===//
/**
 * @file Test_Dynamic_Bitset.cpp
 * @brief Google Test unit tests for DynamicBitset.
 * @version 0.1
 * @date 2026-07-15
 *
 * @copyright MIT License 2026
 */
//===---------------------------------------------------------------------------===//

#include "ads/arrays/Dynamic_Bitset.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <bitset>
#include <cstddef>
#include <limits>
#include <new>
#include <random>
#include <string>
#include <utility>
#include <vector>

using namespace ads::arrays;

namespace {

//===----- ORACLE HELPERS ------------------------------------------------------===//

// Verifies that a bitset matches an oracle bit-for-bit and in its aggregate queries.
auto matches(const DynamicBitset& bs, const std::vector<bool>& oracle) -> ::testing::AssertionResult {
  if (bs.size() != oracle.size()) {
    return ::testing::AssertionFailure() << "size " << bs.size() << " != oracle " << oracle.size();
  }
  size_t set_count = 0;
  for (size_t i = 0; i < oracle.size(); ++i) {
    if (bs[i] != oracle[i]) {
      return ::testing::AssertionFailure() << "bit " << i << ": " << bs[i] << " != " << oracle[i];
    }
    set_count += oracle[i] ? 1 : 0;
  }
  if (bs.count() != set_count) {
    return ::testing::AssertionFailure() << "count " << bs.count() << " != " << set_count;
  }
  if (bs.any() != (set_count > 0)) {
    return ::testing::AssertionFailure() << "any() mismatch";
  }
  if (bs.none() != (set_count == 0)) {
    return ::testing::AssertionFailure() << "none() mismatch";
  }
  const bool all_set = (set_count == oracle.size()); // vacuously true when empty
  if (bs.all() != all_set) {
    return ::testing::AssertionFailure() << "all() mismatch";
  }
  return ::testing::AssertionSuccess();
}

// Oracle bitwise operators with zero-extension to the longer length.
auto oracle_binary(const std::vector<bool>& a, const std::vector<bool>& b, char op) -> std::vector<bool> {
  const size_t      n = std::max(a.size(), b.size());
  std::vector<bool> r(n, false);
  for (size_t i = 0; i < n; ++i) {
    const bool ai = (i < a.size()) ? a[i] : false;
    const bool bi = (i < b.size()) ? b[i] : false;
    r[i]          = (op == '&') ? (ai && bi) : (op == '|') ? (ai || bi) : (ai != bi);
  }
  return r;
}

auto oracle_shl(const std::vector<bool>& a, size_t n) -> std::vector<bool> {
  std::vector<bool> r(a.size(), false);
  for (size_t i = 0; i < a.size(); ++i) {
    if (i >= n) {
      r[i] = a[i - n];
    }
  }
  return r;
}

auto oracle_shr(const std::vector<bool>& a, size_t n) -> std::vector<bool> {
  std::vector<bool> r(a.size(), false);
  for (size_t i = 0; i < a.size(); ++i) {
    if (i + n < a.size()) {
      r[i] = a[i + n];
    }
  }
  return r;
}

// Builds a bitset and a matching oracle from an MSB-first pattern string.
auto from_pattern(const std::string& msb_first) -> std::pair<DynamicBitset, std::vector<bool>> {
  const size_t      n = msb_first.size();
  DynamicBitset     bs(n, false);
  std::vector<bool> oracle(n, false);
  for (size_t i = 0; i < n; ++i) {
    const bool bit = (msb_first[n - 1 - i] == '1');
    if (bit) {
      bs.set(i);
    }
    oracle[i] = bit;
  }
  return {std::move(bs), std::move(oracle)};
}

// Representative sizes, including empty, sub-word, and word-boundary values.
const std::vector<size_t> kSizes = {0, 1, 7, 63, 64, 65, 100, 127, 128, 129, 200};

} // namespace

//===----- CONSTRUCTION TESTS --------------------------------------------------===//

TEST(DynamicBitset, DefaultIsEmpty) {
  DynamicBitset bs;
  EXPECT_EQ(bs.size(), 0u);
  EXPECT_TRUE(bs.is_empty());
  EXPECT_EQ(bs.count(), 0u);
  EXPECT_FALSE(bs.any());
  EXPECT_TRUE(bs.none());
  EXPECT_TRUE(bs.all()); // vacuously true
  EXPECT_EQ(bs.find_first(), DynamicBitset::npos);
  EXPECT_EQ(bs.to_string(), "");
}

TEST(DynamicBitset, SizeFillFalse) {
  for (size_t n : kSizes) {
    DynamicBitset bs(n, false);
    EXPECT_EQ(bs.size(), n);
    EXPECT_EQ(bs.count(), 0u);
    EXPECT_TRUE(matches(bs, std::vector<bool>(n, false)));
  }
}

TEST(DynamicBitset, SizeFillTrue) {
  for (size_t n : kSizes) {
    DynamicBitset bs(n, true);
    EXPECT_EQ(bs.size(), n);
    EXPECT_EQ(bs.count(), n);
    EXPECT_EQ(bs.all(), true);
    EXPECT_TRUE(matches(bs, std::vector<bool>(n, true)));
  }
}

TEST(DynamicBitset, InitializerListOrder) {
  DynamicBitset bs{true, false, true, true}; // index 0 first
  EXPECT_EQ(bs.size(), 4u);
  EXPECT_TRUE(bs[0]);
  EXPECT_FALSE(bs[1]);
  EXPECT_TRUE(bs[2]);
  EXPECT_TRUE(bs[3]);
  EXPECT_EQ(bs.to_string(), "1101"); // MSB first
  EXPECT_EQ(bs.count(), 3u);
}

TEST(DynamicBitset, RangeConstructor) {
  std::vector<bool> v = {false, true, true, false, true, false, true};
  DynamicBitset     bs(v.begin(), v.end());
  EXPECT_TRUE(matches(bs, v));
}

TEST(DynamicBitset, RangeConstructorCrossesWordBoundary) {
  std::vector<bool> v;
  for (int i = 0; i < 200; ++i) {
    v.push_back((i % 3) == 0);
  }
  DynamicBitset bs(v.begin(), v.end());
  EXPECT_TRUE(matches(bs, v));
}

TEST(DynamicBitset, ExtremeSizeRequestsThrowWithoutCorruptingState) {
#if defined(__has_feature)
#if __has_feature(address_sanitizer)
  GTEST_SKIP() << "ASan aborts oversized allocations before operator new can throw std::bad_alloc";
#endif
#endif

  const size_t max_size = std::numeric_limits<size_t>::max();

  EXPECT_THROW((DynamicBitset(max_size, false)), std::bad_alloc);

  DynamicBitset       bs{true, false, true, true};
  const DynamicBitset original = bs;
  EXPECT_THROW(bs.resize(max_size, false), std::bad_alloc);
  EXPECT_EQ(bs, original);
}

//===----- SINGLE-BIT TESTS ----------------------------------------------------===//

TEST(DynamicBitset, SetResetFlipRoundtrip) {
  DynamicBitset             bs(130, false);
  std::vector<bool>         oracle(130, false);
  const std::vector<size_t> touch = {0, 1, 63, 64, 65, 100, 127, 128, 129};
  for (size_t p : touch) {
    bs.set(p);
    oracle[p] = true;
  }
  EXPECT_TRUE(matches(bs, oracle));

  bs.reset(64);
  oracle[64] = false;
  bs.flip(0);
  oracle[0] = !oracle[0];
  bs.flip(200 - 71); // 129
  oracle[129] = !oracle[129];
  EXPECT_TRUE(matches(bs, oracle));
}

TEST(DynamicBitset, TestBoundsCheck) {
  DynamicBitset bs(10, false);
  EXPECT_NO_THROW((void)bs.test(9));
  EXPECT_THROW((void)bs.test(10), ArrayOutOfRangeException);
  EXPECT_THROW(bs.set(10), ArrayOutOfRangeException);
  EXPECT_THROW(bs.reset(10), ArrayOutOfRangeException);
  EXPECT_THROW(bs.flip(10), ArrayOutOfRangeException);
}

TEST(DynamicBitset, ProxyReference) {
  DynamicBitset bs(8, false);
  bs[2] = true;
  EXPECT_TRUE(bs.test(2));
  bs[5] = bs[2]; // copy from another proxy
  EXPECT_TRUE(bs.test(5));
  bs[2].flip();
  EXPECT_FALSE(bs.test(2));
  EXPECT_TRUE(~bs[2]); // complement of a cleared bit
  const bool v = bs[5];
  EXPECT_TRUE(v);
  EXPECT_EQ(bs.count(), 1u);
}

TEST(DynamicBitset, SetWithValueArgument) {
  DynamicBitset bs(4, false);
  bs.set(1, true);
  bs.set(3, true);
  bs.set(1, false);
  EXPECT_FALSE(bs.test(1));
  EXPECT_TRUE(bs.test(3));
}

//===----- WHOLE-BITSET MUTATOR TESTS ------------------------------------------===//

TEST(DynamicBitset, GlobalSetResetFlipPreserveTailMask) {
  for (size_t n : kSizes) {
    DynamicBitset bs(n, false);
    bs.set();
    EXPECT_EQ(bs.count(), n) << "set() at size " << n;
    EXPECT_TRUE(bs.all());
    bs.flip();
    EXPECT_EQ(bs.count(), 0u) << "flip() at size " << n;
    EXPECT_TRUE(bs.none());
    bs.set();
    bs.reset();
    EXPECT_EQ(bs.count(), 0u);
  }
}

TEST(DynamicBitset, FlipTwiceIsIdentity) {
  auto [bs, oracle]      = from_pattern("110100101110001");
  DynamicBitset original = bs;
  bs.flip();
  bs.flip();
  EXPECT_EQ(bs, original);
}

//===----- SCANNING TESTS ------------------------------------------------------===//

TEST(DynamicBitset, FindWalksAllSetBits) {
  DynamicBitset             bs(300, false);
  const std::vector<size_t> positions = {0, 5, 63, 64, 65, 130, 191, 192, 255, 299};
  for (size_t p : positions) {
    bs.set(p);
  }
  std::vector<size_t> walked;
  for (size_t p = bs.find_first(); p != DynamicBitset::npos; p = bs.find_next(p)) {
    walked.push_back(p);
  }
  EXPECT_EQ(walked, positions);
  EXPECT_EQ(walked.size(), bs.count());
}

TEST(DynamicBitset, FindOnEmptyAndAllZero) {
  DynamicBitset empty;
  EXPECT_EQ(empty.find_first(), DynamicBitset::npos);
  EXPECT_EQ(empty.find_next(0), DynamicBitset::npos);

  DynamicBitset zeros(100, false);
  EXPECT_EQ(zeros.find_first(), DynamicBitset::npos);
  EXPECT_EQ(zeros.find_next(50), DynamicBitset::npos);
  EXPECT_EQ(zeros.find_next(99), DynamicBitset::npos);
}

TEST(DynamicBitset, FindOnAllOnes) {
  DynamicBitset ones(70, true);
  EXPECT_EQ(ones.find_first(), 0u);
  EXPECT_EQ(ones.find_next(0), 1u);
  EXPECT_EQ(ones.find_next(68), 69u);
  EXPECT_EQ(ones.find_next(69), DynamicBitset::npos);
}

//===----- BITWISE TESTS -------------------------------------------------------===//

TEST(DynamicBitset, BitwiseSameSize) {
  auto [a, ao] = from_pattern("1100101011110000");
  auto [b, bo] = from_pattern("1010101010101010");

  EXPECT_TRUE(matches(a & b, oracle_binary(ao, bo, '&')));
  EXPECT_TRUE(matches(a | b, oracle_binary(ao, bo, '|')));
  EXPECT_TRUE(matches(a ^ b, oracle_binary(ao, bo, '^')));

  DynamicBitset     not_a = ~a;
  std::vector<bool> not_oracle(ao.size());
  for (size_t i = 0; i < ao.size(); ++i) {
    not_oracle[i] = !ao[i];
  }
  EXPECT_TRUE(matches(not_a, not_oracle));
}

TEST(DynamicBitset, BitwiseDifferentSizesZeroExtend) {
  // Deliberately cross a word boundary: 70 bits vs 30 bits.
  DynamicBitset     a(70, false);
  DynamicBitset     b(30, false);
  std::vector<bool> ao(70, false);
  std::vector<bool> bo(30, false);
  std::mt19937      rng(12'345);
  for (size_t i = 0; i < 70; ++i) {
    const bool v = (rng() & 1u) != 0;
    if (v) {
      a.set(i);
    }
    ao[i] = v;
  }
  for (size_t i = 0; i < 30; ++i) {
    const bool v = (rng() & 1u) != 0;
    if (v) {
      b.set(i);
    }
    bo[i] = v;
  }

  for (char op : {'&', '|', '^'}) {
    const std::vector<bool> expected = oracle_binary(ao, bo, op);
    DynamicBitset           r1       = (op == '&') ? (a & b) : (op == '|') ? (a | b) : (a ^ b);
    DynamicBitset           r2       = (op == '&') ? (b & a) : (op == '|') ? (b | a) : (b ^ a); // commutative
    EXPECT_EQ(r1.size(), 70u);
    EXPECT_EQ(r2.size(), 70u);
    EXPECT_TRUE(matches(r1, expected)) << "op " << op << " (a,b)";
    EXPECT_TRUE(matches(r2, expected)) << "op " << op << " (b,a)";
  }
}

TEST(DynamicBitset, CompoundAssignGrowsLhs) {
  DynamicBitset a(10, true);
  DynamicBitset b(100, true);
  a |= b;
  EXPECT_EQ(a.size(), 100u);
  EXPECT_EQ(a.count(), 100u);
}

TEST(DynamicBitset, CompoundAssignHandlesSelfAliasing) {
  auto [original, oracle] = from_pattern("101101001011");

  DynamicBitset and_self = original;
  and_self &= and_self;
  EXPECT_EQ(and_self, original);

  DynamicBitset or_self = original;
  or_self |= or_self;
  EXPECT_EQ(or_self, original);

  DynamicBitset xor_self = original;
  xor_self ^= xor_self;
  EXPECT_TRUE(matches(xor_self, std::vector<bool>(oracle.size(), false)));
}

//===----- SHIFT TESTS ---------------------------------------------------------===//

TEST(DynamicBitset, Shifts) {
  auto [bs, oracle] = from_pattern("110100101110001010110010"); // 24 bits
  // Use a larger cross-word bitset too.
  DynamicBitset     big(200, false);
  std::vector<bool> big_oracle(200, false);
  std::mt19937      rng(999);
  for (size_t i = 0; i < 200; ++i) {
    const bool v = (rng() & 1u) != 0;
    if (v) {
      big.set(i);
    }
    big_oracle[i] = v;
  }

  const std::vector<size_t> amounts = {0, 1, 5, 24, 63, 64, 65, 100, 199, 200, 250};
  for (size_t s : amounts) {
    if (s <= oracle.size()) {
      EXPECT_TRUE(matches(bs << s, oracle_shl(oracle, s))) << "small << " << s;
      EXPECT_TRUE(matches(bs >> s, oracle_shr(oracle, s))) << "small >> " << s;
    }
    EXPECT_TRUE(matches(big << s, oracle_shl(big_oracle, s))) << "big << " << s;
    EXPECT_TRUE(matches(big >> s, oracle_shr(big_oracle, s))) << "big >> " << s;
  }
}

TEST(DynamicBitset, ShiftPreservesSize) {
  DynamicBitset bs(50, true);
  EXPECT_EQ((bs << 10).size(), 50u);
  EXPECT_EQ((bs >> 10).size(), 50u);
  EXPECT_EQ((bs << 100).size(), 50u);
  EXPECT_TRUE((bs << 100).none());
  EXPECT_TRUE((bs >> 100).none());
}

//===----- SIZING TESTS --------------------------------------------------------===//

TEST(DynamicBitset, ResizeGrowAndShrink) {
  auto [bs, oracle] = from_pattern("1011"); // bits: idx0=1,1=1,2=0,3=1

  bs.resize(10, true);
  oracle.resize(10, true);
  EXPECT_TRUE(matches(bs, oracle));

  bs.resize(70, false);
  oracle.resize(70, false);
  EXPECT_TRUE(matches(bs, oracle));

  bs.resize(66, false); // shrink across boundary
  oracle.resize(66);
  EXPECT_TRUE(matches(bs, oracle));

  bs.resize(2, false); // shrink low, drop high bits
  oracle.resize(2);
  EXPECT_TRUE(matches(bs, oracle));
}

TEST(DynamicBitset, ResizeGrowWithOnesWithinSameWord) {
  DynamicBitset bs(5, true); // 11111
  bs.resize(12, true);
  EXPECT_EQ(bs.count(), 12u);
  EXPECT_TRUE(bs.all());
}

TEST(DynamicBitset, PushBackBuildsPattern) {
  DynamicBitset     bs;
  std::vector<bool> oracle;
  std::mt19937      rng(7);
  for (int i = 0; i < 260; ++i) {
    const bool v = (rng() & 1u) != 0;
    bs.push_back(v);
    oracle.push_back(v);
  }
  EXPECT_TRUE(matches(bs, oracle));
}

TEST(DynamicBitset, Clear) {
  DynamicBitset bs(100, true);
  bs.clear();
  EXPECT_EQ(bs.size(), 0u);
  EXPECT_TRUE(bs.is_empty());
}

//===----- VALUE SEMANTICS TESTS -----------------------------------------------===//

TEST(DynamicBitset, CopyIsIndependent) {
  DynamicBitset a(20, false);
  a.set(3);
  a.set(15);
  DynamicBitset b = a; // copy ctor
  EXPECT_EQ(a, b);
  b.set(0);
  EXPECT_TRUE(b.test(0));
  EXPECT_FALSE(a.test(0));
  EXPECT_NE(a, b);
}

TEST(DynamicBitset, CopyAssignIsIndependent) {
  DynamicBitset a(20, true);
  DynamicBitset b(5, false);
  b = a; // copy assign (copy-and-swap)
  EXPECT_EQ(a, b);
  b.reset(0);
  EXPECT_TRUE(a.test(0));
}

TEST(DynamicBitset, MoveLeavesSourceEmpty) {
  DynamicBitset a(30, true);
  DynamicBitset copy = a;
  DynamicBitset b    = std::move(a); // move ctor
  EXPECT_EQ(b, copy);
  EXPECT_EQ(a.size(), 0u);
  EXPECT_TRUE(a.is_empty());

  DynamicBitset c(3, false);
  c = std::move(b); // move assign
  EXPECT_EQ(c, copy);
  EXPECT_EQ(b.size(), 0u);
}

TEST(DynamicBitset, Swap) {
  DynamicBitset a(10, true);
  DynamicBitset b(20, false);
  swap(a, b);
  EXPECT_EQ(a.size(), 20u);
  EXPECT_EQ(a.count(), 0u);
  EXPECT_EQ(b.size(), 10u);
  EXPECT_EQ(b.count(), 10u);
}

TEST(DynamicBitset, EqualityConsidersSize) {
  DynamicBitset a(10, false);
  DynamicBitset b(11, false);
  EXPECT_NE(a, b);
  DynamicBitset c(10, false);
  EXPECT_EQ(a, c);
  c.set(4);
  EXPECT_NE(a, c);
}

//===----- CONVERSION TESTS ----------------------------------------------------===//

TEST(DynamicBitset, ToStringMatchesStdBitset) {
  std::mt19937 rng(2'024);
  for (int trial = 0; trial < 20; ++trial) {
    std::bitset<80> ref;
    DynamicBitset   bs(80, false);
    for (size_t i = 0; i < 80; ++i) {
      if ((rng() & 1u) != 0) {
        ref.set(i);
        bs.set(i);
      }
    }
    EXPECT_EQ(bs.to_string(), ref.to_string());
  }
}

TEST(DynamicBitset, ToStringCustomChars) {
  DynamicBitset bs{true, false, true};
  EXPECT_EQ(bs.to_string('.', '#'), "#.#");
}

//===----- RANDOMIZED ORACLE CHURN ---------------------------------------------===//

TEST(DynamicBitset, RandomizedChurnAgainstOracle) {
  std::mt19937      rng(1'234'567);
  DynamicBitset     bs;
  std::vector<bool> oracle;

  for (int step = 0; step < 20'000; ++step) {
    const int op = static_cast<int>(rng() % 6);
    if (bs.is_empty() && op < 3) {
      bs.push_back(false);
      oracle.push_back(false);
      continue;
    }
    switch (op) {
    case 0: { // set
      const size_t p = rng() % bs.size();
      bs.set(p);
      oracle[p] = true;
      break;
    }
    case 1: { // reset
      const size_t p = rng() % bs.size();
      bs.reset(p);
      oracle[p] = false;
      break;
    }
    case 2: { // flip
      const size_t p = rng() % bs.size();
      bs.flip(p);
      oracle[p] = !oracle[p];
      break;
    }
    case 3: { // push_back
      const bool v = (rng() & 1u) != 0;
      bs.push_back(v);
      oracle.push_back(v);
      break;
    }
    case 4: { // resize
      const size_t new_size = rng() % 400;
      const bool   fill     = (rng() & 1u) != 0;
      bs.resize(new_size, fill);
      oracle.resize(new_size, fill);
      break;
    }
    case 5: { // shift
      if (!bs.is_empty()) {
        const size_t amount = rng() % (bs.size() + 5);
        if ((rng() & 1u) != 0) {
          bs <<= amount;
          oracle = oracle_shl(oracle, amount);
        } else {
          bs >>= amount;
          oracle = oracle_shr(oracle, amount);
        }
      }
      break;
    }
    default:
      break;
    }
    ASSERT_TRUE(matches(bs, oracle)) << "diverged at step " << step << " op " << op;
  }
}
