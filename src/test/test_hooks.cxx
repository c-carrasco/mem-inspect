// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023 Carlos Carrasco
// ----------------------------------------------------------------------------
#include <memory>
#include <cstdlib>

#include <gtest/gtest.h>

#include <meminspect/memory_tracker.h>


// ----------------------------------------------------------------------------
// test_stl_container
// ----------------------------------------------------------------------------
TEST (MemoryTacker, test_stl_container) {
  meminspect::MemoryTracker mt;

  ASSERT_EQ (mt.getAllocatedBytes(), 0);

  std::vector<int32_t> buffer0 (10);
  std::generate(buffer0.begin(), buffer0.end(), [ i = 0 ] () mutable { return i++; });

  ASSERT_EQ (mt.getAllocatedBytes(), buffer0.size() * sizeof (decltype(buffer0)::value_type));
}

// ----------------------------------------------------------------------------
// test_smart_pointer
// ----------------------------------------------------------------------------
TEST (MemoryTacker, test_smart_pointer) {
  meminspect::MemoryTracker mt;

  ASSERT_EQ (mt.getAllocatedBytes(), 0);

  auto buffer = std::make_unique<uint8_t []> (20);
  std::fill_n (buffer.get(), 20, 99);

  ASSERT_EQ (mt.getAllocatedBytes(), 20);

  buffer.reset();

  ASSERT_EQ (mt.getAllocatedBytes(), 0);
}

// ----------------------------------------------------------------------------
// test_malloc
// ----------------------------------------------------------------------------
TEST (MemoryTacker, test_malloc) {
  meminspect::MemoryTracker mt;

  ASSERT_EQ (mt.getAllocatedBytes(), 0);

  void *mem { std::malloc (123) };

  ASSERT_EQ (mt.getAllocatedBytes(), 123);

  std::free (mem);

  ASSERT_EQ (mt.getAllocatedBytes(), 0);
}

// ----------------------------------------------------------------------------
// test_realloc
// ----------------------------------------------------------------------------
TEST (MemoryTacker, test_realloc) {
  meminspect::MemoryTracker mt;

  ASSERT_EQ (mt.getAllocatedBytes(), 0);

  void *mem0 { std::malloc (123) };

  ASSERT_EQ (mt.getAllocatedBytes(), 123);

  void *mem1 { std::realloc (mem0, 1024) };

  ASSERT_EQ (mt.getAllocatedBytes(), 1024);

  std::free (mem1);

  ASSERT_EQ (mt.getAllocatedBytes(), 0);
}

// ----------------------------------------------------------------------------
// test_calloc
// ----------------------------------------------------------------------------
TEST (MemoryTacker, test_calloc) {
  meminspect::MemoryTracker mt;

  ASSERT_EQ (mt.getAllocatedBytes(), 0);

  void *mem { std::calloc (4, 64) };

  ASSERT_EQ (mt.getAllocatedBytes(), 4 * 64);

  std::free (mem);

  ASSERT_EQ (mt.getAllocatedBytes(), 0);
}

// ----------------------------------------------------------------------------
// test_aligned_alloc
// ----------------------------------------------------------------------------
TEST (MemoryTacker, test_aligned_alloc) {
  meminspect::MemoryTracker mt;

  ASSERT_EQ (mt.getAllocatedBytes(), 0);

  void *mem { std::aligned_alloc (1024, 1024) };

  ASSERT_EQ (mt.getAllocatedBytes(), 1024);

  std::free (mem);

  ASSERT_EQ (mt.getAllocatedBytes(), 0);
}