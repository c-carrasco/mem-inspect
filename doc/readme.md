MemInspect Library
==================

**Welcome to the MemInspect Library**, a versatile C++, header-only library designed for memory tracking and leak detection.
This library provides a hassle-free experience and is designed to be a simple and valuable complement to more powerful and complex solution like Valgrind, especially in scenarios where such tools are unavailable or impractical, such as embedded platforms.

# Quick Start Guide

This guide will help you get started with using the MemoryTracker class to monitor and analyze memory allocation in your programs.

### 1. Including the Necessary Headers

In your C++ source code, include the necessary header file to access the MemoryTracker class:


```CPP
#include <meminspect/memory_tracker.h>
```

### 2. Configuring global settings (optional).

You can increase the size of the internal structures to try to improve the performance:

- Define MEMISPECT_HASHMAP_SIZE with your desired amount of bytes (1024 by default)

```CPP
#define MEMISPECT_HASHMAP_SIZE 20480
#include <meminspect/memory_tracker.h>
```

### 3. Creating a MemoryTracker Object

To start tracking memory usage, create an instance of the MemoryTracker class. This will automatically register memory usage with the MemoryInspector:

```CPP
  meminspect::MemoryTracker memoryTracker;
```

### 4. Monitoring Memory Allocation

Now that you have a MemoryTracker object, any memory allocated using the default allocator (e.g., new, malloc, etc.) will be automatically tracked.

### 5. Retrieving Allocated Memory Size

To retrieve the total number of allocated bytes, you can use the getAllocatedBytes method:

```CPP
  const size_t allocatedBytes { memoryTracker.getAllocatedBytes() };
```

This will provide you with the total number of bytes currently allocated on the heap since the `memoryTracker` object was created.

### 6. Cleaning Up

When you're done with memory tracking, the MemoryTracker destructor will automatically unregister memory usage, so there's no need for explicit cleanup.

# Example

Here's a simple example of how to use the MemoryTracker class:

```CPP
#include <iostream>

#include <meminspect/memory_tracker.h>

int main() {
  meminspect::MemoryTracker memoryTracker;

  // Allocate some memory
  auto buffer { std::make_unique<int []> (1000) };

  // Retrieve allocated bytes
  size_t allocatedBytes { memoryTracker.getAllocatedBytes() };

  // Print the allocated bytes
  std::cout << "Allocated Bytes: " << allocatedBytes << " bytes" << std::endl;

  return 0;
}
```

# Installation

MemInspect is a header-only C++ library. Just copy the `src/include/meminspect` folder to system or project's include path.

