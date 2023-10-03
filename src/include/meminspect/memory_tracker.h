// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023 Carlos Carrasco
// ----------------------------------------------------------------------------
#ifndef __MEM_INSPECT_MEMORY_TRACKER_H__
#define __MEM_INSPECT_MEMORY_TRACKER_H__
#include <meminspect/memory_hook.h>
#include <meminspect/memory_inspector.h>


namespace meminspect {

/// @brief This class is a part of the meminspect C++ library, designed for measuring memory usage in your C++ applications.
class MemoryTracker {
  public:
    /// @brief Constructor for MemoryTracker.
    /// Initializes the memory tracker and registers the memory usage with the MemoryInspector.
    inline MemoryTracker() noexcept {
      MemoryInspector<DefaultAllocator>::add (&_bytes);
    }

    /// @brief Destructor for MemoryTracker.
    /// Unregisters the memory usage from the MemoryInspector.
    inline ~MemoryTracker() noexcept {
      MemoryInspector<DefaultAllocator>::remove (&_bytes);
    }

    /// @brief Get the number of allocated bytes (heap).
    /// @return The total number of allocated bytes.
    inline size_t getAllocatedBytes() { return _bytes; }

  private:
    size_t _bytes { 0 }; //< The number of allocated bytes.
};

}

#endif
