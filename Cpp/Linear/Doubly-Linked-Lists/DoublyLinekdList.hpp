#ifndef DOUBLY_LINKED_LIST_HPP
#define DOUBLY_LINKED_LIST_HPP

#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <stdexcept>

namespace ads {

/**
 * @brief Generic doubly linked list implementation.
 *
 * This class implements a doubly linked list using advanced C++ constructs
 * such as smart pointers, templates, iterators, and operator overloading.
 */
template <typename T>
class DoublyLinkedList {
private:
  /**
   * @brief Node structure for the list.
   */
  struct Node {
    T                     data;
    std::shared_ptr<Node> next;
    std::weak_ptr<Node>   prev;

    // Constructor by value
    explicit Node(const T& value) : data(value), next(nullptr) {}

    // Move constructor
    explicit Node(T&& value) : data(std::move(value)), next(nullptr) {}

    // Copy constructor deleted to prevent unwanted copies
    Node(const Node&) = delete;

    // Assignment operator deleted
    Node& operator=(const Node&) = delete;
  };

  std::shared_ptr<Node> head;
  std::shared_ptr<Node> tail;
  size_t                size_;

public:
  /**
   * @brief Iterator class to traverse the list.
   */
  class Iterator {
  private:
    std::shared_ptr<Node> current;

  public:
    // Types required for STL iterators
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = T*;
    using reference         = T&;

    // Constructor
    explicit Iterator(std::shared_ptr<Node> node) : current(node) {}

    // Dereferencing
    reference operator*() const {
      if (!current)
        throw std::runtime_error("Dereferencing a null iterator");
      return current->data;
    }

    // Member access
    pointer operator->() const {
      if (!current)
        throw std::runtime_error("Member access on a null iterator");
      return &(current->data);
    }

    // Pre-increment
    Iterator& operator++() {
      if (!current)
        throw std::runtime_error("Incrementing a null iterator");
      current = current->next;
      return *this;
    }

    // Post-increment
    Iterator operator++(int) {
      Iterator temp = *this;
      ++(*this);
      return temp;
    }

    // Pre-decrement
    Iterator& operator--() {
      if (!current) {
        throw std::runtime_error("Decrementing a null iterator");
      }

      // If current is nullptr, we can't go back
      if (current->prev.expired()) {
        throw std::runtime_error("Decrementing beyond the beginning of the list");
      }

      current = current->prev.lock();
      return *this;
    }

    // Post-decrement
    Iterator operator--(int) {
      Iterator temp = *this;
      --(*this);
      return temp;
    }

    // Equality comparison
    bool operator==(const Iterator& other) const { return current == other.current; }

    // Inequality comparison
    bool operator!=(const Iterator& other) const { return !(*this == other); }

    // Get current node (for internal use)
    std::shared_ptr<Node> getNode() const { return current; }
  };

  /**
   * @brief Constant iterator class to traverse the list in read-only mode.
   */
  class ConstIterator {
  private:
    std::shared_ptr<Node> current;

  public:
    // Types required for STL iterators
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = const T*;
    using reference         = const T&;

    // Constructor
    explicit ConstIterator(std::shared_ptr<Node> node) : current(node) {}

    // Conversion from Iterator to ConstIterator
    ConstIterator(const Iterator& iter) : current(iter.getNode()) {}

    // Dereferencing
    reference operator*() const {
      if (!current)
        throw std::runtime_error("Dereferencing a null iterator");
      return current->data;
    }

    // Member access
    pointer operator->() const {
      if (!current)
        throw std::runtime_error("Member access on a null iterator");
      return &(current->data);
    }

    // Pre-increment
    ConstIterator& operator++() {
      if (!current)
        throw std::runtime_error("Incrementing a null iterator");
      current = current->next;
      return *this;
    }

    // Post-increment
    ConstIterator operator++(int) {
      ConstIterator temp = *this;
      ++(*this);
      return temp;
    }

    // Pre-decrement
    ConstIterator& operator--() {
      if (!current) {
        throw std::runtime_error("Decrementing a null iterator");
      }

      if (current->prev.expired()) {
        throw std::runtime_error("Decrementing beyond the beginning of the list");
      }

      current = current->prev.lock();
      return *this;
    }

    // Post-decrement
    ConstIterator operator--(int) {
      ConstIterator temp = *this;
      --(*this);
      return temp;
    }

    // Equality comparison
    bool operator==(const ConstIterator& other) const { return current == other.current; }

    // Inequality comparison
    bool operator!=(const ConstIterator& other) const { return !(*this == other); }
  };

  // Begin and end iterators
  Iterator      begin() { return Iterator(head); }
  Iterator      end() { return Iterator(nullptr); }
  ConstIterator begin() const { return ConstIterator(head); }
  ConstIterator end() const { return ConstIterator(nullptr); }
  ConstIterator cbegin() const { return ConstIterator(head); }
  ConstIterator cend() const { return ConstIterator(nullptr); }

  /**
   * @brief Default constructor.
   */
  DoublyLinkedList() : head(nullptr), tail(nullptr), size_(0) {}

  /**
   * @brief Constructor with initializer_list.
   */
  DoublyLinkedList(std::initializer_list<T> list) : DoublyLinkedList() {
    for (const auto& item : list) {
      push_back(item);
    }
  }

  /**
   * @brief Copy constructor.
   */
  DoublyLinkedList(const DoublyLinkedList& other) : DoublyLinkedList() {
    for (const auto& item : other) {
      push_back(item);
    }
  }

  /**
   * @brief Move constructor.
   */
  DoublyLinkedList(DoublyLinkedList&& other) noexcept : head(std::move(other.head)), tail(std::move(other.tail)), size_(other.size_) {
    other.size_ = 0;
  }

  /**
   * @brief Copy assignment operator.
   */
  DoublyLinkedList& operator=(const DoublyLinkedList& other) {
    if (this != &other) {
      // Create a temporary copy using the copy constructor
      DoublyLinkedList temp(other);
      // Swap contents
      std::swap(head, temp.head);
      std::swap(tail, temp.tail);
      std::swap(size_, temp.size_);
    }
    return *this;
  }

  /**
   * @brief Move assignment operator.
   */
  DoublyLinkedList& operator=(DoublyLinkedList&& other) noexcept {
    if (this != &other) {
      // Release current resources
      clear();
      // Transfer resources
      head        = std::move(other.head);
      tail        = std::move(other.tail);
      size_       = other.size_;
      other.size_ = 0;
    }
    return *this;
  }

  /**
   * @brief Destructor.
   */
  ~DoublyLinkedList() { clear(); }

  /**
   * @brief Adds an element to the beginning of the list.
   *
   * @param value The value to add.
   */
  void push_front(const T& value) {
    auto newNode = std::make_shared<Node>(value);
    if (!head) {
      head = tail = newNode;
    } else {
      newNode->next = head;
      head->prev    = newNode;
      head          = newNode;
    }
    size_++;
  }

  /**
   * @brief Adds an element to the beginning of the list (move version).
   *
   * @param value The value to add.
   */
  void push_front(T&& value) {
    auto newNode = std::make_shared<Node>(std::move(value));
    if (!head) {
      head = tail = newNode;
    } else {
      newNode->next = head;
      head->prev    = newNode;
      head          = newNode;
    }
    size_++;
  }

  /**
   * @brief Adds an element to the end of the list.
   *
   * @param value The value to add.
   */
  void push_back(const T& value) {
    auto newNode = std::make_shared<Node>(value);
    if (!tail) {
      head = tail = newNode;
    } else {
      newNode->prev = tail;
      tail->next    = newNode;
      tail          = newNode;
    }
    size_++;
  }

  /**
   * @brief Adds an element to the end of the list (move version).
   *
   * @param value The value to add.
   */
  void push_back(T&& value) {
    auto newNode = std::make_shared<Node>(std::move(value));
    if (!tail) {
      head = tail = newNode;
    } else {
      newNode->prev = tail;
      tail->next    = newNode;
      tail          = newNode;
    }
    size_++;
  }

  /**
   * @brief Removes the element at the beginning of the list.
   *
   * @throw std::runtime_error if the list is empty.
   */
  void pop_front() {
    if (!head) {
      throw std::runtime_error("pop_front on empty list");
    }

    if (head == tail) {
      head.reset();
      tail.reset();
    } else {
      head = head->next;
      head->prev.reset(); // Cancel reference to old head
    }
    size_--;
  }

  /**
   * @brief Removes the element at the end of the list.
   *
   * @throw std::runtime_error if the list is empty.
   */
  void pop_back() {
    if (!tail) {
      throw std::runtime_error("pop_back on empty list");
    }

    if (head == tail) {
      head.reset();
      tail.reset();
    } else {
      auto newTail = tail->prev.lock();
      tail         = newTail;
      tail->next.reset(); // Cancel reference to old tail
    }
    size_--;
  }

  /**
   * @brief Inserts an element at the specified position.
   *
   * @param pos The position iterator.
   * @param value The value to insert.
   * @return Iterator to the position of the inserted element.
   */
  Iterator insert(Iterator pos, const T& value) {
    auto nodePos = pos.getNode();

    // If pos is end() or the list is empty, add to the end
    if (!nodePos) {
      push_back(value);
      return Iterator(tail);
    }

    // If pos is begin(), add to the beginning
    if (nodePos == head) {
      push_front(value);
      return Iterator(head);
    }

    // Otherwise, insert before pos
    auto newNode  = std::make_shared<Node>(value);
    auto prevNode = nodePos->prev.lock();

    newNode->next  = nodePos;
    newNode->prev  = prevNode;
    nodePos->prev  = newNode;
    prevNode->next = newNode;

    size_++;
    return Iterator(newNode);
  }

  /**
   * @brief Inserts an element at the specified position (move version).
   *
   * @param pos The position iterator.
   * @param value The value to insert.
   * @return Iterator to the position of the inserted element.
   */
  Iterator insert(Iterator pos, T&& value) {
    auto nodePos = pos.getNode();

    // If pos is end() or the list is empty, add to the end
    if (!nodePos) {
      push_back(std::move(value));
      return Iterator(tail);
    }

    // If pos is begin(), add to the beginning
    if (nodePos == head) {
      push_front(std::move(value));
      return Iterator(head);
    }

    // Otherwise, insert before pos
    auto newNode  = std::make_shared<Node>(std::move(value));
    auto prevNode = nodePos->prev.lock();

    newNode->next  = nodePos;
    newNode->prev  = prevNode;
    nodePos->prev  = newNode;
    prevNode->next = newNode;

    size_++;
    return Iterator(newNode);
  }

  /**
   * @brief Removes the element at the specified position.
   *
   * @param pos The position iterator.
   * @return Iterator to the next position.
   * @throw std::runtime_error if the position is invalid.
   */
  Iterator erase(Iterator pos) {
    auto nodePos = pos.getNode();
    if (!nodePos) {
      throw std::runtime_error("erase with invalid iterator");
    }

    // If it's the only element
    if (head == tail) {
      head.reset();
      tail.reset();
      size_ = 0;
      return end();
    }

    // If it's the first element
    if (nodePos == head) {
      head = head->next;
      head->prev.reset();
      size_--;
      return Iterator(head);
    }

    // If it's the last element
    if (nodePos == tail) {
      tail = tail->prev.lock();
      tail->next.reset();
      size_--;
      return end();
    }

    // Otherwise, it's an element in the middle
    auto prevNode = nodePos->prev.lock();
    auto nextNode = nodePos->next;

    prevNode->next = nextNode;
    nextNode->prev = prevNode;

    size_--;
    return Iterator(nextNode);
  }

  /**
   * @brief Removes all elements from the list.
   */
  void clear() {
    // Explicitly release nodes to avoid circular references
    while (head) {
      auto next = head->next;
      head->next.reset();
      head = next;
    }
    tail.reset();
    size_ = 0;
  }

  /**
   * @brief Checks if the list is empty.
   *
   * @return true if the list is empty, false otherwise.
   */
  [[nodiscard]] bool empty() const { return size_ == 0; }

  /**
   * @brief Returns the size of the list.
   *
   * @return The number of elements in the list.
   */
  [[nodiscard]] size_t size() const { return size_; }

  /**
   * @brief Access to the first element.
   *
   * @return Reference to the first element.
   * @throw std::runtime_error if the list is empty.
   */
  T& front() {
    if (!head) {
      throw std::runtime_error("front on empty list");
    }
    return head->data;
  }

  /**
   * @brief Access to the first element (const version).
   *
   * @return Constant reference to the first element.
   * @throw std::runtime_error if the list is empty.
   */
  const T& front() const {
    if (!head) {
      throw std::runtime_error("front on empty list");
    }
    return head->data;
  }

  /**
   * @brief Access to the last element.
   *
   * @return Reference to the last element.
   * @throw std::runtime_error if the list is empty.
   */
  T& back() {
    if (!tail) {
      throw std::runtime_error("back on empty list");
    }
    return tail->data;
  }

  /**
   * @brief Access to the last element (const version).
   *
   * @return Constant reference to the last element.
   * @throw std::runtime_error if the list is empty.
   */
  const T& back() const {
    if (!tail) {
      throw std::runtime_error("back on empty list");
    }
    return tail->data;
  }

  /**
   * @brief Searches for an element in the list.
   *
   * @param value The value to search for.
   * @return Iterator to the position of the element, or end() if not found.
   */
  Iterator find(const T& value) {
    for (auto it = begin(); it != end(); ++it) {
      if (*it == value) {
        return it;
      }
    }
    return end();
  }

  /**
   * @brief Searches for an element in the list (const version).
   *
   * @param value The value to search for.
   * @return ConstIterator to the position of the element, or end() if not found.
   */
  ConstIterator find(const T& value) const {
    for (auto it = cbegin(); it != cend(); ++it) {
      if (*it == value) {
        return it;
      }
    }
    return cend();
  }

  /**
   * @brief Applies a function to each element of the list.
   *
   * @param func The function to apply.
   */
  void for_each(const std::function<void(T&)>& func) {
    for (auto& item : *this) {
      func(item);
    }
  }

  /**
   * @brief Applies a function to each element of the list (const version).
   *
   * @param func The function to apply.
   */
  void for_each(const std::function<void(const T&)>& func) const {
    for (const auto& item : *this) {
      func(item);
    }
  }

  /**
   * @brief Equality operator.
   *
   * @param other The list to compare with.
   * @return true if the lists are equal, false otherwise.
   */
  bool operator==(const DoublyLinkedList& other) const {
    if (size_ != other.size_) {
      return false;
    }

    auto it1 = cbegin();
    auto it2 = other.cbegin();

    while (it1 != cend()) {
      if (*it1 != *it2) {
        return false;
      }
      ++it1;
      ++it2;
    }

    return true;
  }

  /**
   * @brief Inequality operator.
   *
   * @param other The list to compare with.
   * @return true if the lists are different, false otherwise.
   */
  bool operator!=(const DoublyLinkedList& other) const { return !(*this == other); }

  /**
   * @brief Output operator.
   *
   * @param os The output stream.
   * @param list The list to print.
   * @return The output stream.
   */
  friend std::ostream& operator<<(std::ostream& os, const DoublyLinkedList& list) {
    os << "[";
    bool first = true;
    for (const auto& item : list) {
      if (!first) {
        os << ", ";
      }
      os << item;
      first = false;
    }
    os << "]";
    return os;
  }
};

} // namespace ads

#endif // DOUBLY_LINKED_LIST_HPP