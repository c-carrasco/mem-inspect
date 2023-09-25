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
      std::atomic_flag_clear_explicit (&_lock, std::memory_order_acquire);
    }

  private:
    std::atomic_flag _lock = ATOMIC_FLAG_INIT;
};

/// @brief This class implements a thread-safe single-linked sorted list.
template<typename K, typename V, typename Allocator>
class SortedList {
  public:
    /// @brief Represents a node in the sorted list.
    struct Node {
      K key;      ///< The key associated with the node.
      V value;    ///< The value associated with the node.
      Node *next; ///< A pointer to the next node in the list.
    };

    /// @brief Inserts an element.
    /// @param key The key to insert.
    /// @param val The value to insert.
    /// @return A pointer to the inserted node.
    // ~SortedList() {
    //   while (_head != nullptr)
    //     Allocator::free (std::exchange (_head, _head->next));
    // }

    /// @brief Searches for an element by key.
    /// @param key The key to search for.
    /// @return A pointer to the found node or nullptr if not found.
    Node * add (K key, V &&val) {
      const auto n { static_cast<Node *> (Allocator::malloc (sizeof (Node))) };
      n->key = key;
      n->value = std::forward<V> (val);
      n->next = nullptr;

      std::lock_guard<Mutex> guard { _mutex };

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

    /// @brief Deletes the specified element by key and returns its value.
    /// @param key The key to delete.
    /// @return An optional containing the value of the deleted element, or nullopt if not found.
    Node * find (K key) {
      std::lock_guard<Mutex> guard { _mutex };

      for (auto n = _head; n != nullptr; n = n->next) {
        if (n->key == key) return n;
      }

      return nullptr;
    }

    /// @brief Deletes the specified element and return the value of the deleted element
    std::optional<V> remove (K key) {
      std::lock_guard<Mutex> guard { _mutex };

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
    // std::mutex _mutex;
    Mutex _mutex;
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