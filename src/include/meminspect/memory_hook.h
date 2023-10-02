// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023 Carlos Carrasco
// ----------------------------------------------------------------------------
#ifndef __MEM_INSPECT_MEMORY_HOOK_H__
#define __MEM_INSPECT_MEMORY_HOOK_H__
#include <dlfcn.h>
#include <cstdlib>
#include <stdexcept>
#include <new>

#include <meminspect/memory_inspector.h>
#include <meminspect/types.h>


namespace meminspect {

struct DefaultAllocator {
  static meminspect::malloc_t malloc;
  static meminspect::realloc_t realloc;
  static meminspect::calloc_t calloc;
  static meminspect::aligned_alloc_t aligned_alloc;
  static meminspect::free_t free;
};

malloc_t DefaultAllocator::malloc { nullptr };
realloc_t DefaultAllocator::realloc { nullptr };
calloc_t DefaultAllocator::calloc { nullptr };
calloc_t DefaultAllocator::aligned_alloc { nullptr };
free_t DefaultAllocator::free { nullptr };

}

// ----------------------------------------------------------------------------
// malloc
// ----------------------------------------------------------------------------
extern void * malloc (size_t size) {
  if (!meminspect::DefaultAllocator::malloc) {
    meminspect::DefaultAllocator::malloc = reinterpret_cast<meminspect::malloc_t> (dlsym (RTLD_NEXT, "malloc"));
    if (dlerror() != nullptr)
      std::abort();

  }

  return meminspect::MemoryInspector<meminspect::DefaultAllocator>::alloc (size);
}

// ----------------------------------------------------------------------------
// realloc
// ----------------------------------------------------------------------------
extern void * realloc (void *ptr, size_t size) {
  if (!meminspect::DefaultAllocator::realloc) {
     meminspect::DefaultAllocator::realloc = reinterpret_cast<meminspect::realloc_t> (dlsym (RTLD_NEXT, "realloc"));
    if (dlerror() != nullptr)
      std::abort();
  }

  if (!meminspect::DefaultAllocator::free) {
     meminspect::DefaultAllocator::free = reinterpret_cast<meminspect::free_t> (dlsym (RTLD_NEXT, "free"));
    if (dlerror() != nullptr)
      std::abort();
  }

  return meminspect::MemoryInspector<meminspect::DefaultAllocator>::realloc (ptr, size);
}

// ----------------------------------------------------------------------------
// calloc
// ----------------------------------------------------------------------------
extern void * calloc (size_t num, size_t size) {
  if (!meminspect::DefaultAllocator::calloc) {
    meminspect::DefaultAllocator::calloc = reinterpret_cast<meminspect::calloc_t> (dlsym (RTLD_NEXT, "calloc"));
    if (dlerror() != nullptr)
      std::abort();
  }

  return meminspect::MemoryInspector<meminspect::DefaultAllocator>::calloc (num, size);
}

// ----------------------------------------------------------------------------
// aligned_alloc
// ----------------------------------------------------------------------------
extern void * aligned_alloc (size_t alignment, size_t size) {
  if (!meminspect::DefaultAllocator::aligned_alloc) {
    meminspect::DefaultAllocator::aligned_alloc = reinterpret_cast<meminspect::aligned_alloc_t> (dlsym (RTLD_NEXT, "aligned_alloc"));
    if (dlerror() != nullptr)
      std::abort();
  }

  return meminspect::MemoryInspector<meminspect::DefaultAllocator>::aligned_alloc (alignment, size);
}

// ----------------------------------------------------------------------------
// free
// ----------------------------------------------------------------------------
extern void free (void *ptr) {
  if (!meminspect::DefaultAllocator::free) {
     meminspect::DefaultAllocator::free = reinterpret_cast<meminspect::free_t> (dlsym (RTLD_NEXT, "free"));
    if (dlerror() != nullptr)
      std::abort();
  }

  meminspect::MemoryInspector<meminspect::DefaultAllocator>::dealloc (ptr);
}

// ----------------------------------------------------------------------------
// new
// ----------------------------------------------------------------------------
void * operator new (std::size_t sz) {
  return std::malloc (sz);
}

// ----------------------------------------------------------------------------
// new[]
// ----------------------------------------------------------------------------
void * operator new[] (std::size_t sz) {
  return std::malloc (sz);
}

// ----------------------------------------------------------------------------
// delete
// ----------------------------------------------------------------------------
void operator delete (void *ptr) noexcept {
  std::free (ptr);
}

// ----------------------------------------------------------------------------
// delete
// ----------------------------------------------------------------------------
void operator delete (void *ptr, std::size_t) noexcept {
  std::free (ptr);
}

// ----------------------------------------------------------------------------
// delete[]
// ----------------------------------------------------------------------------
void operator delete[] (void *ptr) noexcept {
  std::free (ptr);
}

// ----------------------------------------------------------------------------
// delete[]
// ----------------------------------------------------------------------------
void operator delete[] (void *ptr, std::size_t) noexcept {
  std::free (ptr);
}

#endif
