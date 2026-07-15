//===---------------------------------------------------------------------------===//
/**
 * @file main_Interval_Tree.cc
 * @author Costantino Lombardi
 * @brief Demo for the IntervalTree structure.
 * @version 0.1
 * @date 2026-07-14
 *
 * @copyright MIT License 2026
 *
 * This program demonstrates the interval tree: a balanced, augmented search tree
 * that answers stabbing and overlap queries over a set of closed intervals, here
 * modelled as named calendar reservations.
 */
//===---------------------------------------------------------------------------===//

#include "ads/arrays/Dynamic_Array.hpp"
#include "ads/trees/search/Interval_Tree.hpp"
#include "support/Demo_Utilities.hpp"

#include <iostream>
#include <string>

using std::cerr;
using std::cout;
using std::exception;
using std::string;

using ads::arrays::DynamicArray;
using ads::trees::IntervalTree;

namespace {

using Schedule = IntervalTree<int, string>;
using Interval = Schedule::Interval;

auto print_interval(const Interval& interval) -> void {
  cout << '[' << interval.low << ", " << interval.high << ']';
}

auto sample_schedule() -> Schedule {
  Schedule schedule;
  // Reservations as [start, end] slots on a shared resource.
  const DynamicArray<Schedule::Entry> bookings = []() -> DynamicArray<Schedule::Entry> {
    DynamicArray<Schedule::Entry> data;
    data.push_back({.interval = {.low = 9, .high = 11}, .value = "standup"});
    data.push_back({.interval = {.low = 10, .high = 12}, .value = "design review"});
    data.push_back({.interval = {.low = 13, .high = 15}, .value = "interview"});
    data.push_back({.interval = {.low = 14, .high = 16}, .value = "planning"});
    data.push_back({.interval = {.low = 16, .high = 18}, .value = "retro"});
    return data;
  }();

  for (const Schedule::Entry& booking : bookings) {
    schedule.insert(booking.interval, booking.value);
  }
  return schedule;
}

} // namespace

//===----- OVERLAP QUERY -------------------------------------------------------===//

auto demo_overlap() -> void {
  ads::demo::print_section("Interval Tree - Overlap Query");

  Schedule       schedule = sample_schedule();
  const Interval slot     = {.low = 10, .high = 13};

  cout << "bookings = " << schedule.size() << ", height = " << schedule.height() << "\n";
  cout << "meetings overlapping ";
  print_interval(slot);
  cout << ":\n";
  schedule.find_overlapping(slot, [](const Interval& interval, const string& name) -> void {
    cout << "  - " << name << ' ';
    print_interval(interval);
    cout << "\n";
  });
  ads::demo::print_success("Overlap search prunes subtrees whose max endpoint precedes the query.");
}

//===----- POINT STABBING ------------------------------------------------------===//

auto demo_stabbing() -> void {
  ads::demo::print_section("Interval Tree - Point Stabbing");

  Schedule  schedule = sample_schedule();
  const int hour     = 14;

  cout << "meetings in progress at hour " << hour << ":\n";
  schedule.find_containing(hour, [](const Interval& interval, const string& name) -> void {
    cout << "  - " << name << ' ';
    print_interval(interval);
    cout << "\n";
  });
  ads::demo::print_success("Point stabbing is an overlap query against the degenerate interval [p, p].");
}

//===----- ANY-OVERLAP CHECK ---------------------------------------------------===//

auto demo_any_overlap() -> void {
  ads::demo::print_section("Interval Tree - Availability Check");

  Schedule       schedule = sample_schedule();
  const Interval request  = {.low = 12, .high = 13};

  cout << "requested slot ";
  print_interval(request);
  cout << " is ";
  if (const auto* clash = schedule.find_any_overlap(request)) {
    cout << "busy (conflicts with \"" << clash->value << "\" ";
    print_interval(clash->interval);
    cout << ")\n";
  } else {
    cout << "free\n";
  }
  ads::demo::print_success("A single O(log n) descent decides availability.");
}

//===----- ENTRY POINT ---------------------------------------------------------===//

auto main() -> int {
  try {
    ads::demo::print_header("INTERVAL TREE - COMPREHENSIVE DEMO");
    demo_overlap();
    demo_stabbing();
    demo_any_overlap();

    ads::demo::print_footer();
    return 0;

  } catch (const exception& e) {
    cerr << "\nUnexpected error: " << e.what() << '\n';
    return 1;
  }
}

//===---------------------------------------------------------------------------===//
