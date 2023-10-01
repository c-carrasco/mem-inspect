// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023 Carlos Carrasco
// ----------------------------------------------------------------------------
#ifndef __MEM_INSPECT_TYPES_H__
#define __MEM_INSPECT_TYPES_H__
#include <array>
#include <atomic>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>


namespace meminspect {

/// @brief Type alias for the `malloc` function pointer.
using malloc_t = std::add_pointer<void * (size_t)>::type;
/// @brief Type alias for the `realloc` function pointer.
using realloc_t = std::add_pointer<void * (void *, size_t)>::type;
/// @brief Type alias for the `calloc` function pointer.
using calloc_t = std::add_pointer<void * (size_t, size_t)>::type;
/// @brief Type alias for the `realloc` function pointer.
using aligned_alloc_t = std::add_pointer<void * (size_t, size_t)>::type;
/// @brief Type alias for the `free` function pointer.
using free_t = std::add_pointer<void (void *)>::type;

/// @brief This class is a synchronization primitive that can be used to protect shared data from being simultaneously accessed by multiple threads.
class Mutex {
  public:
    /// @brief Locks the mutex.
    inline void lock() {
      while (std::atomic_flag_test_and_set_explicit (&_lock, std::memory_order_acquire)) {
        // empty
      }
    }

    /// @brief Unlocks the mutex.
    inline void unlock() {
      std::atomic_flag_clear_explicit (&_lock, std::memory_order_release);
    }

  private:
    std::atomic_flag _lock = ATOMIC_FLAG_INIT;
};

/// @brief A templated linked list implementation with custom memory allocation.
///
/// @tparam T The type of the values stored in the list.
/// @tparam Allocator The allocator class responsible for memory management.
template<typename T, typename Allocator>
class ListPtr {
  public:
    /// @brief Represents a node in the linked list.
    struct Node {
      T *value;   ///< Pointer to the stored value
      Node *next; ///< Pointer to the next node in the list.
    };

    /// @brief Constructor
    inline ListPtr () {
      // empty
    }

    /// @brief Constructor
    inline ListPtr (T *value) {
      add (value);
    }

    /// @brief Destructor to free all allocated memory.
    ~ListPtr() {
      while (_head != nullptr)
        Allocator::free (std::exchange (_head, _head->next));
    }

    /// @brief Adds a new node with the given value to the end of the list.
    /// @param value Pointer to the value to be added.
    /// @return Pointer to the newly added node.
    Node * add (T *value) {
      const auto n { static_cast<Node *> (Allocator::malloc (sizeof (Node))) };
      n->value = value;
      n->next = nullptr;

      if (_head == nullptr) {
        assert (_tail == nullptr);

        _head = n;
        _tail = n;
        return n;
      }


      _tail->next = n;
      _tail = n;

      return n;
    }

    /// @brief Finds a node with the given value in the list.
    /// @param value Pointer to the value to search for.
    /// @return Pointer to the node containing the value, or nullptr if not found.
    Node * find (const T *value) const {
      for (auto *it = _head; it != nullptr; it = it->next) {
        if (it->value == value)
          return it;
      }

      return nullptr;
    }

    /// @brief Removes a node with the given value from the list.
    /// @param value Pointer to the value to be removed.
    void remove (const T *value) {
      Node *it0 { nullptr };
      Node *it1 { _head };

      while (it1 != nullptr) {
        if (it1->value == value) {
          if (it0 != nullptr)
            it0->next = it1->next;
          else
            _head = it1->next;

          if (it1 == _tail)
            _tail = it0;

          Allocator::free (it1);

          return;
        }

        it0 = it1;
        it1 = it1->next;
      }
    }

    /// @brief Gets a pointer to the head of the list.
    /// @return Pointer to the head node.
    inline Node * head() const { return _head; }

    /// @brief Gets a pointer to the tail of the list.
    /// @return Pointer to the tail node.
    inline Node * tail() const { return _tail; }

    /// @brief Overloaded addition assignment operator.
    /// @param v Value to add to all elements in the list.
    /// @return Reference to the modified list.
    inline ListPtr<T, Allocator> & operator+= (T v) {
      for (auto *it = _head; it != nullptr; it = it->next)
        *(it->value) += v;

      return *this;
    }

    /// @brief Overloaded subtraction assignment operator.
    /// @param v Value to subtract from all elements in the list.
    /// @return Reference to the modified list.
    inline ListPtr<T, Allocator> & operator-= (T v) {
      for (auto *it = _head; it != nullptr; it = it->next)
        *(it->value) -= v;

      return *this;
    }

  private:
    Node *_head { nullptr }; ///< Pointer to the head of the list.
    Node *_tail { nullptr }; ///< Pointer to the tail of the list.
};

/// @brief This class implements a single-linked sorted list.
template<typename K, typename V, typename Allocator>
class SortedList {
  public:
    /// @brief Represents a node in the sorted list.
    struct Node {
      K key;      ///< The key associated with the node.
      V value;    ///< The value associated with the node.
      Node *next; ///< A pointer to the next node in the list.
    };

    /// @brief Destructor to free all allocated memory.
    ~SortedList() {
      while (_head != nullptr)
        Allocator::free (std::exchange (_head, _head->next));
    }

    /// @brief Inserts an element.
    /// @param key The key to insert.
    /// @param val The value to insert.
    /// @return A pointer to the inserted node.
    Node * add (K key, V &&val) {
      const auto n { static_cast<Node *> (Allocator::malloc (sizeof (Node))) };
      n->key = key;
      n->value = std::forward<V> (val);
      n->next = nullptr;

      // if head is nullptr, the list doesn’t yet exist, so we create one.
      if (_head == nullptr) {
        _head = n;
        return n;
      }

      if (key < _head->key) {
        n->next = _head;
        _head = n;
        return n;
      }

      for (auto *it = _head; it != nullptr; it = it->next) {
        if ((key > it->key) && ((it->next == nullptr) || (key < it->next->key))) {
          n->next = it->next;
          it->next = n;

          return n;
        }
      }

      throw std::runtime_error { "repeated value" };
    }

    /// @brief Searches for an element by key.
    /// @param key The key to search for.
    /// @return A pointer to the found node or nullptr if not found.
    Node * find (K key) {
      for (auto n = _head; n != nullptr; n = n->next) {
        if (n->key == key) return n;
      }

      return nullptr;
    }

    /// @brief Deletes the specified element and return the value of the deleted element.
    /// @param key The key to delete.
    /// @return An optional containing the value of the deleted element, or nullopt if not found.
    std::optional<V> remove (K key) {
      if (!_head)
        return std::nullopt;

      if (_head->key == key) {
        const auto v { _head->value };
        Allocator::free (std::exchange (_head, _head->next));
        return { v };
      }

      for (auto n = _head->next, p = _head; n != nullptr; p = n, n = n->next) {
        if (n->key == key) {
          const auto v { n->value };
          std::swap (p->next, n->next);
          Allocator::free (n);

          return { v };
        }
      }

      return std::nullopt;
    }

  private:
    Node *_head { nullptr };
};

/// @brief Basic HashMap class.
template<typename K, typename V, typename Allocator, size_t S=1024>
class HashMapPtr {
  public:
    /// @brief Inserts an element.
    /// @param p The key pointer to insert.
    /// @param v The value to insert.
    /// @return A pointer to the inserted node in the sorted list.
    inline typename SortedList<K *, V, Allocator>::Node * add (K *p, V &&v) {
      const auto k { reinterpret_cast<uintptr_t> (p) % S };
      return _map[k].add (p, std::forward<V> (v));
    }

    /// @brief Deletes an element.
    /// @param p The key pointer to delete.
    /// @return An optional containing the value of the deleted element, or nullopt if not found.
    inline std::optional<V> remove (K *p) {
      const auto k { reinterpret_cast<uintptr_t> (p) % S };
      return _map[k].remove (p);
    }

    /// @brief Finds an element.
    /// @param p The key pointer to search for.
    /// @return A pointer to the found node in the sorted list, or nullptr if not found.
    inline typename SortedList<K *, V, Allocator>::Node * find (K *p) {
      const auto k { reinterpret_cast<uintptr_t> (p) % S };
      return _map[k].find (p);
    }

  private:
    std::array<SortedList<K *, V, Allocator>, S> _map;
};

}

#endif