// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023 Carlos Carrasco
// ----------------------------------------------------------------------------
#ifndef __MEM_INSPECT_TYPES_H__
#define __MEM_INSPECT_TYPES_H__
#include <mutex>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>


namespace meminspect {

using malloc_t = std::add_pointer<void * (size_t)>::type;
using realloc_t = std::add_pointer<void * (void *, size_t)>::type;
using free_t = std::add_pointer<void (void *)>::type;


/// @brief This class implement a single linked sorted list
template<typename K, typename V, typename Allocator>
class SortedList {
  public:
    struct Node {
      K key;
      V value;
      Node *next;
    };

    /// @brief Destructor
    ~SortedList() {
      while (_head != nullptr)
        Allocator::free (std::exchange (_head, _head->next));
    }

    /// @brief Inserts an element
    Node * add (K key, V &&val) {
      auto n { static_cast<Node *> (Allocator::malloc (sizeof (Node))) };
      n->key = key;
      n->value = std::forward<V> (val);
      n->next = nullptr;

      std::lock_guard<std::mutex> guard { _mutex };

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

    /// @brief Searches an element
    Node * find (K key) {
      std::lock_guard<std::mutex> guard { _mutex };

      for (auto n = _head; n != nullptr; n = n->next) {
        if (n->key == key) return n;
      }

      return nullptr;
    }

    /// @brief Deletes the specified element and return the value of the deleted element
    std::optional<V> remove (K key) {
      std::lock_guard<std::mutex> guard { _mutex };

      if (!_head)
        return std::nullopt;

      if (_head->key == key) {
        auto v { _head->value };
        Allocator::free (std::exchange (_head, _head->next));
        return { v };
      }

      for (auto n = _head->next, p = _head; n != nullptr; p = n, n = n->next) {
        if (n->key == key) {
          auto v { n->value };
          std::swap (p->next, n->next);
          Allocator::free (n);

          return { v };
        }
      }

      return std::nullopt;
    }

  private:
    Node *_head { nullptr };
    std::mutex _mutex;
};

/// @brief Basic HashMap class
template<typename K, typename V, typename Allocator, size_t S=100>
class HashMapPtr {
  public:
    /// @brief Inserts an element
    inline typename SortedList<K *, V, Allocator>::Node * add (K *p, V &&v) {
      const auto k { reinterpret_cast<uintptr_t> (p) % S };
      return _map[k].add (p, std::forward<V> (v));
    }

    /// @brief Deletes an element
    inline std::optional<V> remove (K *p) {
      const auto k { reinterpret_cast<uintptr_t> (p) % S };
      return _map[k].remove (p);
    }

    /// @brief Finds element
    inline typename SortedList<K *, V, Allocator>::Node * find (K *p) {
      const auto k { reinterpret_cast<uintptr_t> (p) % S };
      return _map[k].find (p);
    }

  private:
    std::array<SortedList<K *, V, Allocator>, S> _map;
};

}

#endif