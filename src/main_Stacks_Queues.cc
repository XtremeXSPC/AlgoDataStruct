//===--------------------------------------------------------------------------===//
/**
 * @file main Stacks_Queues.cc
 * @author Costantino Lombardi
 * @brief Comprehensive test program for Stack and Queue implementations
 * @version 0.1
 * @date 2025-01-15
 *
 * @copyright MIT License 2025
 *
 */
//===--------------------------------------------------------------------------===//

#include <chrono>
#include <iostream>
#include <string>

#include "../include/ads/queues/Circular_Array_Queue.hpp"
#include "../include/ads/queues/Linked_Queue.hpp"
#include "../include/ads/stacks/Array_Stack.hpp"
#include "../include/ads/stacks/Linked_Stack.hpp"

using namespace std;

// Helper function to demonstrate polymorphic usage
template <typename T>
void test_stack_interface(ads::stack::Stack<T>& stack, const string& stack_type) {
  cout << "\n========== Testing " << stack_type << " ==========\n";

  // Test push operations
  cout << "Pushing elements: 10, 20, 30\n";
  stack.push(10);
  stack.push(20);
  stack.push(30);

  cout << "Stack size: " << stack.size() << '\n';
  cout << "Top element: " << stack.top() << '\n';

  // Test pop operations
  cout << "\nPopping elements:\n";
  while (!stack.is_empty()) {
    cout << "  Popped: " << stack.top() << " (size: " << stack.size() << ")\n";
    stack.pop();
  }

  // Test exception handling
  cout << "\nTesting exception on empty stack...\n";
  try {
    stack.pop();
  } catch (const ads::stack::StackUnderflowException& e) {
    cout << "  Exception caught correctly: " << e.what() << '\n';
  }
}

// Helper function for queue testing
template <typename T>
void test_queue_interface(ads::queue::Queue<T>& queue, const string& queue_type) {
  cout << "\n========== Testing " << queue_type << " ==========\n";

  // Test enqueue operations
  cout << "Enqueuing elements: 100, 200, 300\n";
  queue.enqueue(100);
  queue.enqueue(200);
  queue.enqueue(300);

  cout << "Queue size: " << queue.size() << '\n';
  cout << "Front element: " << queue.front() << '\n';
  cout << "Rear element: " << queue.rear() << '\n';

  // Test dequeue operations
  cout << "\nDequeuing elements:\n";
  while (!queue.is_empty()) {
    cout << "  Dequeued: " << queue.front() << " (size: " << queue.size() << ")\n";
    queue.dequeue();
  }

  // Test exception handling
  cout << "\nTesting exception on empty queue...\n";
  try {
    queue.dequeue();
  } catch (const ads::queue::QueueUnderflowException& e) {
    cout << "  Exception caught correctly: " << e.what() << '\n';
  }
}

// Performance comparison function
void performance_comparison() {
  cout << "\n========== Performance Comparison ==========\n";
  const int iterations = 100000;

  // Stack performance test
  {
    ads::stack::ArrayStack<int>  array_stack;
    ads::stack::LinkedStack<int> linked_stack;

    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
      array_stack.push(i);
    }
    for (int i = 0; i < iterations; ++i) {
      array_stack.pop();
    }
    auto end            = chrono::high_resolution_clock::now();
    auto array_duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
      linked_stack.push(i);
    }
    for (int i = 0; i < iterations; ++i) {
      linked_stack.pop();
    }
    end                  = chrono::high_resolution_clock::now();
    auto linked_duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << "Stack Performance (" << iterations << " push/pop operations):\n";
    cout << "  ArrayStack:  " << array_duration.count() << " ms\n";
    cout << "  LinkedStack: " << linked_duration.count() << " ms\n";
  }

  // Queue performance test
  {
    ads::queue::CircularArrayQueue<int> array_queue;
    ads::queue::LinkedQueue<int>        linked_queue;

    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
      array_queue.enqueue(i);
    }
    for (int i = 0; i < iterations; ++i) {
      array_queue.dequeue();
    }
    auto end            = chrono::high_resolution_clock::now();
    auto array_duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    start = chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
      linked_queue.enqueue(i);
    }
    for (int i = 0; i < iterations; ++i) {
      linked_queue.dequeue();
    }
    end                  = chrono::high_resolution_clock::now();
    auto linked_duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << "\nQueue Performance (" << iterations << " enqueue/dequeue operations):\n";
    cout << "  CircularArrayQueue: " << array_duration.count() << " ms\n";
    cout << "  LinkedQueue:        " << linked_duration.count() << " ms\n";
  }
}

// Test emplace functionality with complex types
void test_emplace_functionality() {
  cout << "\n========== Testing Emplace with Complex Types ==========\n";

  struct Person {
    string name;
    int    age;

    Person(string n, int a) : name(std::move(n)), age(a) { cout << "  Constructed: " << name << " (age " << age << ")\n"; }
  };

  ads::stack::ArrayStack<Person>         stack;
  ads::queue::CircularArrayQueue<Person> queue;

  cout << "\nEmplacing into stack:\n";
  stack.emplace("Alice", 25);
  stack.emplace("Bob", 30);

  cout << "\nEmplacing into queue:\n";
  queue.emplace("Charlie", 35);
  queue.emplace("Diana", 28);
}

auto main() -> int {
  try {
    // Test Stack implementations
    ads::stack::ArrayStack<int>  array_stack;
    ads::stack::LinkedStack<int> linked_stack;

    test_stack_interface(array_stack, "ArrayStack");
    test_stack_interface(linked_stack, "LinkedStack");

    // Test Queue implementations
    ads::queue::CircularArrayQueue<int> circular_queue;
    ads::queue::LinkedQueue<int>        linked_queue;

    test_queue_interface(circular_queue, "CircularArrayQueue");
    test_queue_interface(linked_queue, "LinkedQueue");

    // Test move semantics
    cout << "\n========== Testing Move Semantics ==========\n";
    ads::stack::ArrayStack<string> stack1;
    stack1.push("Hello");
    stack1.push("World");

    ads::stack::ArrayStack<string> stack2 = std::move(stack1);
    cout << "After move, stack2 size: " << stack2.size() << '\n';
    cout << "After move, stack1 size: " << stack1.size() << " (should be 0)\n";

    // Test emplace functionality
    test_emplace_functionality();

    // Performance comparison
    performance_comparison();

  } catch (const exception& e) {
    cerr << "Unexpected error: " << e.what() << '\n';
    return 1;
  }

  cout << "\n========== All Tests Completed Successfully ==========\n";
  return 0;
}