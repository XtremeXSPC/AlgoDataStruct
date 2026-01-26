//===---------------------------------------------------------------------------===//
/**
 * @file main Stacks_Queues.cc
 * @author Costantino Lombardi
 * @brief Comprehensive demo program for Stack and Queue implementations.
 * @version 0.1
 * @date 2025-01-15
 *
 * @copyright MIT License 2025
 *
 * This program demonstrates the usage of Stack and Queue data structures, showcasing
 * their operations and performance comparison.
 */
//===---------------------------------------------------------------------------===//

#include <chrono>
#include <iostream>
#include <string>

#include "../include/ads/queues/Circular_Array_Queue.hpp"
#include "../include/ads/queues/Linked_Queue.hpp"
#include "../include/ads/stacks/Array_Stack.hpp"
#include "../include/ads/stacks/Linked_Stack.hpp"
#include "support/Demo_Utilities.hpp"

using std::cerr;
using std::cout;
using std::exception;
using std::string;
using std::to_string;
using std::vector;

using namespace ads::queues;
using namespace ads::stacks;

//===---------------------------- HELPER FUNCTIONS -----------------------------===//

// Helper function to demonstrate polymorphic usage.
template <typename T>
void demo_stack_interface(Stack<T>& stack, const string& stack_type) {
  ads::demo::print_section("Testing " + stack_type);

  // Test push operations.
  cout << "Pushing elements: 10, 20, 30\n";
  stack.push(10);
  stack.push(20);
  stack.push(30);

  cout << "Stack size: " << stack.size() << '\n';
  cout << "Top element: " << stack.top() << '\n';

  // Test pop operations.
  cout << "\nPopping elements:\n";
  while (!stack.is_empty()) {
    cout << "  Popped: " << stack.top() << " (size: " << stack.size() << ")\n";
    stack.pop();
  }

  // Test exception handling.
  cout << "\nTesting exception on empty stack...\n";
  try {
    stack.pop();
  } catch (const ads::stacks::StackUnderflowException& e) {
    cout << "  Exception caught correctly: " << e.what() << '\n';
  }
}

//===-------------------------- BASIC OPERATIONS DEMO --------------------------===//

// Demonstrates basic insertion and extraction.
template <typename T>
void demo_queue_interface(Queue<T>& queue, const string& queue_type) {
  ads::demo::print_section("Testing " + queue_type);

  // Test enqueue operations.
  cout << "Enqueuing elements: 100, 200, 300\n";
  queue.enqueue(100);
  queue.enqueue(200);
  queue.enqueue(300);

  cout << "Queue size: " << queue.size() << '\n';
  cout << "Front element: " << queue.front() << '\n';
  cout << "Rear element: " << queue.rear() << '\n';

  // Test dequeue operations.
  cout << "\nDequeuing elements:\n";
  while (!queue.is_empty()) {
    cout << "  Dequeued: " << queue.front() << " (size: " << queue.size() << ")\n";
    queue.dequeue();
  }

  // Test exception handling.
  cout << "\nTesting exception on empty queue...\n";
  try {
    queue.dequeue();
  } catch (const ads::queues::QueueUnderflowException& e) {
    cout << "  Exception caught correctly: " << e.what() << '\n';
  }
}

//===------------------------- PERFORMANCE COMPARISON --------------------------===//

// Performance comparison function.
void performance_comparison() {
  ads::demo::print_section("Performance Comparison");
  const int iterations = 100000;

  // Stack performance test.
  {
    ArrayStack<int>  array_stack;
    LinkedStack<int> linked_stack;

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
      array_stack.push(i);
    }
    for (int i = 0; i < iterations; ++i) {
      array_stack.pop();
    }
    auto end            = std::chrono::high_resolution_clock::now();
    auto array_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
      linked_stack.push(i);
    }
    for (int i = 0; i < iterations; ++i) {
      linked_stack.pop();
    }
    end                  = std::chrono::high_resolution_clock::now();
    auto linked_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    cout << "Stack Performance (" << iterations << " push/pop operations):\n";
    cout << "  ArrayStack:  " << array_duration.count() << " ms\n";
    cout << "  LinkedStack: " << linked_duration.count() << " ms\n";
  }

  // Queue performance test.
  {
    CircularArrayQueue<int> array_queue;
    LinkedQueue<int>        linked_queue;

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
      array_queue.enqueue(i);
    }
    for (int i = 0; i < iterations; ++i) {
      array_queue.dequeue();
    }
    auto end            = std::chrono::high_resolution_clock::now();
    auto array_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
      linked_queue.enqueue(i);
    }
    for (int i = 0; i < iterations; ++i) {
      linked_queue.dequeue();
    }
    end                  = std::chrono::high_resolution_clock::now();
    auto linked_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    cout << "\nQueue Performance (" << iterations << " enqueue/dequeue operations):\n";
    cout << "  CircularArrayQueue: " << array_duration.count() << " ms\n";
    cout << "  LinkedQueue:        " << linked_duration.count() << " ms\n";
  }
}

//===----------------------- EMPLACE FUNCTIONALITY DEMO ------------------------===//

// Test emplace functionality with complex types.
void demo_emplace_functionality() {
  ads::demo::print_section("Testing Emplace with Complex Types");

  struct Person {
    string name;
    int    age;

    Person(string n, int a) : name(std::move(n)), age(a) { cout << "  Constructed: " << name << " (age " << age << ")\n"; }
  };

  ArrayStack<Person>         stack;
  CircularArrayQueue<Person> queue;

  cout << "\nEmplacing into stack:\n";
  stack.emplace("Alice", 25);
  stack.emplace("Bob", 30);

  cout << "\nEmplacing into queue:\n";
  queue.emplace("Charlie", 35);
  queue.emplace("Diana", 28);
}

//===------------------------------ MAIN FUNCTION ------------------------------===//

auto main() -> int {
  try {
    ads::demo::print_header("STACKS AND QUEUES - COMPREHENSIVE DEMO");

    // Test Stack implementations.
    ArrayStack<int>  array_stack;
    LinkedStack<int> linked_stack;

    demo_stack_interface(array_stack, "ArrayStack");
    demo_stack_interface(linked_stack, "LinkedStack");

    // Test Queue implementations.
    CircularArrayQueue<int> circular_queue;
    LinkedQueue<int>        linked_queue;

    demo_queue_interface(circular_queue, "CircularArrayQueue");
    demo_queue_interface(linked_queue, "LinkedQueue");

    // Test move semantics.
    ads::demo::print_section("Testing Move Semantics");
    ArrayStack<string> stack1;
    stack1.push("Hello");
    stack1.push("World");

    ArrayStack<string> stack2 = std::move(stack1);
    cout << "After move, stack2 size: " << stack2.size() << '\n';
    cout << "After move, stack1 size: " << stack1.size() << " (should be 0)\n";

    // Test emplace functionality.
    demo_emplace_functionality();

    // Performance comparison.
    performance_comparison();

  } catch (const exception& e) {
    cerr << "Unexpected error: " << e.what() << '\n';
    return 1;
  }

  ads::demo::print_footer();
  return 0;
}

//===---------------------------------------------------------------------------===//
