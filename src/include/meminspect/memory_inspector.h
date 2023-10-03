// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023 Carlos Carrasco
// ----------------------------------------------------------------------------
#ifndef __MEM_INSPECT_MEMORY_INSPECTOR_H__
#define __MEM_INSPECT_MEMORY_INSPECTOR_H__
#include <cinttypes>
#include <meminspect/types.h>


namespace meminspect {

/// @brief A class for tracking memory allocations and deallocations.
/// This class provides static methods for tracking memory allocations, reallocations, and deallocations.
/// @tparam Allocator The custom allocator type to use for memory management.
template<typename Allocator>
class MemoryInspector {
  public:
    /// @brief Allocates memory of a specified size and tracks the allocation.
    /// @param size The size of memory to allocate.
    /// @return A pointer to the allocated memory.
    static inline void * alloc (size_t size) {
      std::lock_guard<Mutex> guard { _mutex };

      const auto addr { Allocator::malloc (size) };
      _allocatedBytes += size;

      return _mem.add (addr, std::move (size))->key;
    }

    /// @brief Reallocates memory to a new size and tracks the reallocation.
    /// @param ptr A pointer to the previously allocated memory.
    /// @param size The new size of memory to allocate.
    /// @return A pointer to the reallocated memory.
    static inline void * realloc (void *ptr, size_t size) {
      std::lock_guard<Mutex> guard { _mutex };

      const auto addr { Allocator::realloc (ptr, size) };

      _allocatedBytes += size;

      const auto oldSize { _mem.remove (ptr) };
      if (oldSize)
        _allocatedBytes -= *oldSize;

      return _mem.add (addr, std::move (size))->key;
    }

    /// @brief Allocates memory for an array of num objects of size size.
    /// @param num The number of objects.
    /// @param size The size of each object.
    /// @return A pointer to the allocated memory.
    static inline void * calloc (size_t num, size_t size) {
      const auto addr { Allocator::calloc (num, size) };
      _allocatedBytes += size * num;

      return _mem.add (addr, std::move (size * num))->key;
    }

    /// @brief Allocate size bytes of uninitialized storage whose alignment is specified by alignment.
    /// @param alignment Specifies the alignment..
    /// @param size The number of bytes to allocate.
    /// @return A pointer to the allocated memory.
    static inline void * aligned_alloc (size_t alignment, size_t size) {
      std::lock_guard<Mutex> guard { _mutex };

      const auto addr { Allocator::aligned_alloc (alignment, size) };
      _allocatedBytes += size;

      return _mem.add (addr, std::move (size))->key;
    }

    /// @brief Deallocates memory and tracks the deallocation.
    /// @param ptr A pointer to the memory to deallocate.
    static inline void dealloc (void *ptr) {
      std::lock_guard<Mutex> guard { _mutex };

      const auto size { _mem.remove (ptr) };
      if (size)
        _allocatedBytes -= *size;

      Allocator::free (ptr);
    }

    /// @brief Tracks the addition of memory size.
    /// @param size A pointer to the size of memory to add.
    static inline void add (size_t *size) {
      std::lock_guard<Mutex> guard { _mutex };

      _allocatedBytes.add (size);
    }

    /// @brief Tracks the removal of memory size.
    /// @param size A pointer to the size of memory to remove.
    static inline void remove (size_t *size) {
      std::lock_guard<Mutex> guard { _mutex };

      _allocatedBytes.remove (size);
    }

  private:
    static HashMapPtr<void, size_t, Allocator> _mem;   ///< A HashMap for tracking allocated memory.
    static ListPtr<size_t, Allocator> _allocatedBytes; ///< A List for counting allocated bytes.
    static Mutex _mutex;                               ///< A mutex to make code thread-safe.
};

template<typename Allocator>
HashMapPtr<void, size_t, Allocator> MemoryInspector<Allocator>::_mem {};

template<typename Allocator>
ListPtr<size_t, Allocator> MemoryInspector<Allocator>::_allocatedBytes {};

template<typename Allocator>
Mutex MemoryInspector<Allocator>::_mutex {};

}

#endif
