Memory Inspector
================

[![ci](https://github.com/c-carrasco/mem-inspect/actions/workflows/main.yml/badge.svg)](https://github.com/c-carrasco/mem-inspect/actions/workflows/main.yml)

C++ library for memory tracking and leak detection.

# Overview

MemInspect is a versatile C++ library designed for memory tracking and leak detection. While it does not aim to replace tools like Valgrind, it serves as a valuable complement, especially in scenarios where such tools are unavailable or impractical, such as embedded platforms.

# Features

- Cross-platform compatible.
- Easy to use.
- Good granularity: can track memory of functions, classes, ... No need to measure the entire program.

# Prerequisites to build this project

Before diving into `meminspect`, make sure you have the following tools and dependencies set up:

- Conan 1.x
- CMake 3.20 or higer
- GCC, Clang or AppleClang
- GNU Make or ninja
- Docker (optional)
- Doxygen (for generating documentation)

# Build the project

## Achieve Reproducible Builds with Docker

For a streamlined development environment, Docker is your friend. It's highly recommended to utilize Docker for building your code.

## The Building Script

Located in the root folder of the project, the build.sh file acts as the catalyst for your building endeavors.

**Usage Guide**
---------------
* **_release_**
Construct the code in release mode.
* **_debug_**
Construct the code in debug mode (selected if no mode is specified).
* **_clean_**
Wipe out the _build_ directory along with its contents.
* **_verbose_**
Force GNU make to show the commands.
* **_ninja_**
Opt for `ninja` over GNU Make for code compilation.
* **_tests_**
Execute tests post compilation.
* **_asan=on_**
Enable the [Address Sanitizer]
* **_ubsan=on_**
Enable the [Undefined Behavior Sanitizer]
* **_docker[=compiler]_**
Use docker for local development.
  Available options:
    - `gcc13`: Use GCC 13 as default compiler (selected if no compiler is specified).
    - `clang16`: Use Clang 16 as default compiler.

Examples:

```
# Compile code in release mode
./build.sh clean release

# Build code in debug mode using Docker image with Clang 16, Address Sanitizer enabled, and run unit tests
./build.sh docker=clang16 debug test asan=on

# Start docker dev environment with gcc13
./build.sh docker=gcc13
```

# How to use it

See [documentation](https://github.com/c-carrasco/mem-inspect/settings/pages) for details.

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

# Contributing
We welcome contributions from the community.

# License
This project is licensed under the MIT License - see the [LICENSE](./LICENSE) file for details.