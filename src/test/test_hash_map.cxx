// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023 Carlos Carrasco
// ----------------------------------------------------------------------------
#include <stdlib.h>
#include <memory>
#include <string_view>

#include <gtest/gtest.h>

#include <meminspect/types.h>


namespace {

struct TestAllocator {
  static meminspect::malloc_t malloc;
  static meminspect::free_t free;
};
meminspect::malloc_t  TestAllocator::malloc { nullptr };
meminspect::free_t  TestAllocator::free { nullptr };

}

class HashMapPtrTest: public ::testing::Test {
  protected:
    void SetUp() override {
      TestAllocator::malloc = malloc;
      TestAllocator::free = free;
    }
};


// ----------------------------------------------------------------------------
// test_add
// ----------------------------------------------------------------------------
TEST_F (HashMapPtrTest, test_add) {
  meminspect::HashMapPtr<int, std::string_view, TestAllocator, 10> hashMap;

  const auto n0 { hashMap.add (reinterpret_cast<int *> (0), "zero") };
  ASSERT_EQ (n0->next, nullptr);
  const auto n2 { hashMap.add (reinterpret_cast<int *> (2), "two") };
  ASSERT_EQ (n2->next, nullptr);
  const auto n9 { hashMap.add (reinterpret_cast<int *> (9), "nine") };
  ASSERT_EQ (n9->next, nullptr);
  const auto n10 { hashMap.add (reinterpret_cast<int *> (10), "ten") };
  ASSERT_EQ (n0->next, n10);
}

// ----------------------------------------------------------------------------
// test_find
// ----------------------------------------------------------------------------
TEST_F (HashMapPtrTest, test_find) {
  meminspect::HashMapPtr<int, std::string_view, TestAllocator, 10> hashMap;

  ASSERT_EQ (hashMap.find (reinterpret_cast<int *> (1)), nullptr);

  const auto n0 { hashMap.add (reinterpret_cast<int *> (0), "zero") };
  const auto n2 { hashMap.add (reinterpret_cast<int *> (2), "two") };
  const auto n9 { hashMap.add (reinterpret_cast<int *> (9), "nine") };
  const auto n10 { hashMap.add (reinterpret_cast<int *> (10), "ten") };

  ASSERT_EQ (hashMap.find (reinterpret_cast<int *> (0)), n0);
  ASSERT_EQ (hashMap.find (reinterpret_cast<int *> (2)), n2);
  ASSERT_EQ (hashMap.find (reinterpret_cast<int *> (9)), n9);
  ASSERT_EQ (hashMap.find (reinterpret_cast<int *> (10)), n10);
}

// ----------------------------------------------------------------------------
// test_remove
// ----------------------------------------------------------------------------
TEST_F (HashMapPtrTest, test_remove) {
  meminspect::HashMapPtr<int, std::string_view, TestAllocator, 10> hashMap;

  const auto n0 { hashMap.add (reinterpret_cast<int *> (0), "zero") };
  ASSERT_NE (n0, nullptr);
  const auto n2 { hashMap.add (reinterpret_cast<int *> (2), "two") };
  ASSERT_NE (n2, nullptr);
  const auto n9 { hashMap.add (reinterpret_cast<int *> (9), "nine") };
  ASSERT_NE (n9, nullptr);
  const auto n10 { hashMap.add (reinterpret_cast<int *> (10), "ten") };
  ASSERT_NE (n10, nullptr);

  ASSERT_FALSE (hashMap.remove (reinterpret_cast<int *> (1)).has_value());

  const auto v0 { hashMap.remove (reinterpret_cast<int *> (0)) };
  ASSERT_TRUE (v0.has_value());
  ASSERT_EQ (*v0, "zero");

  const auto v2 { hashMap.remove (reinterpret_cast<int *> (2)) };
  ASSERT_TRUE (v2.has_value());
  ASSERT_EQ (*v2, "two");

  const auto v9 { hashMap.remove (reinterpret_cast<int *> (9)) };
  ASSERT_TRUE (v9.has_value());
  ASSERT_EQ (*v9, "nine");

  const auto v10 { hashMap.remove (reinterpret_cast<int *> (10)) };
  ASSERT_TRUE (v10.has_value());
  ASSERT_EQ (*v10, "ten");

  for (int i = 0; i < 20; ++i)
    ASSERT_FALSE (hashMap.remove (reinterpret_cast<int *> (i)).has_value());
}