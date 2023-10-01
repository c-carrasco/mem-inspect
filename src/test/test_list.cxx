// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023 Carlos Carrasco
// ----------------------------------------------------------------------------
#include <stdlib.h>
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

class ListPtrTest: public ::testing::Test {
 protected:
  void SetUp() override {
    TestAllocator::malloc = malloc;
    TestAllocator::free = free;
  }
};


// ----------------------------------------------------------------------------
// test_add
// ----------------------------------------------------------------------------
TEST_F (ListPtrTest, test_add) {
  meminspect::ListPtr<int32_t, TestAllocator> list;

  ASSERT_EQ (list.head(), nullptr);
  ASSERT_EQ (list.tail(), nullptr);

  int32_t i0 { 100 };
  const auto n0 { list.add (&i0) };
  ASSERT_EQ (n0->value, &i0);
  ASSERT_EQ (n0->next, nullptr);

  ASSERT_EQ (list.head(), n0);
  ASSERT_EQ (list.tail(), n0);

  int32_t i1 { 101 };
  const auto n1 { list.add (&i1) };
  ASSERT_EQ (n1->value, &i1);
  ASSERT_EQ (n1->next, nullptr);

  ASSERT_EQ (n0->next, n1);
  ASSERT_EQ (list.head(), n0);
  ASSERT_EQ (list.tail(), n1);

  int32_t i2 { 102 };
  const auto n2 { list.add (&i2) };
  ASSERT_EQ (n2->value, &i2);
  ASSERT_EQ (n2->next, nullptr);

  ASSERT_EQ (n1->next, n2);
  ASSERT_EQ (list.head(), n0);
  ASSERT_EQ (list.tail(), n2);
}

// ----------------------------------------------------------------------------
// test_find
// ----------------------------------------------------------------------------
TEST_F (ListPtrTest, test_find) {
  meminspect::ListPtr<int32_t, TestAllocator> list;

  int32_t i0 { 100 };
  int32_t i1 { 101 };
  int32_t i2 { 102 };
  int32_t i3 { 103 };

  ASSERT_EQ (list.find (&i0), nullptr);

  const auto n0 { list.add (&i0) };
  ASSERT_EQ (list.find (&i0), n0);
  ASSERT_EQ (list.find (&i1), nullptr);

  const auto n1 { list.add (&i1) };
  ASSERT_EQ (list.find (&i0), n0);
  ASSERT_EQ (list.find (&i1), n1);
  ASSERT_EQ (list.find (&i2), nullptr);

  const auto n2 { list.add (&i2) };
  ASSERT_EQ (list.find (&i0), n0);
  ASSERT_EQ (list.find (&i1), n1);
  ASSERT_EQ (list.find (&i2), n2);
  ASSERT_EQ (list.find (&i3), nullptr);
}

// ----------------------------------------------------------------------------
// test_remove
// ----------------------------------------------------------------------------
TEST_F (ListPtrTest, test_remove) {
  meminspect::ListPtr<int32_t, TestAllocator> list;

  int32_t i0 { 100 };
  int32_t i1 { 101 };
  int32_t i2 { 102 };

  ASSERT_EQ (list.find (&i0), nullptr);
  list.remove (&i0);
  list.add (&i0);
  ASSERT_NE (list.find (&i0), nullptr);
  list.remove (&i0);
  ASSERT_EQ (list.find (&i0), nullptr);
  const auto n0 { list.add (&i0) };

  ASSERT_EQ (list.find (&i1), nullptr);
  list.remove (&i1);
  list.add (&i1);
  ASSERT_NE (list.find (&i1), nullptr);
  list.remove (&i1);
  ASSERT_EQ (list.find (&i1), nullptr);
  const auto n1 { list.add (&i1) };

  ASSERT_EQ (list.find (&i2), nullptr);
  list.remove (&i2);
  list.add (&i2);
  ASSERT_NE (list.find (&i2), nullptr);
  list.remove (&i2);
  ASSERT_EQ (list.find (&i2), nullptr);
  const auto n2 { list.add (&i2) };

  ASSERT_EQ (list.head(), n0);
  ASSERT_EQ (list.tail(), n2);

  ASSERT_EQ (list.find (&i0), n0);
  ASSERT_EQ (list.find (&i1), n1);
  ASSERT_EQ (list.find (&i2), n2);

  list.remove (&i1);
  ASSERT_EQ (list.find (&i1), nullptr);
  ASSERT_EQ (list.head(), n0);
  ASSERT_EQ (list.tail(), n2);
  ASSERT_EQ (n0->next, n2);

  list.remove (&i0);
  ASSERT_EQ (list.find (&i0), nullptr);
  ASSERT_EQ (list.head(), n2);
  ASSERT_EQ (list.tail(), n2);
}

// ----------------------------------------------------------------------------
// test_operator
// ----------------------------------------------------------------------------
TEST_F (ListPtrTest, test_operator) {
  meminspect::ListPtr<int32_t, TestAllocator> list;

  list += 1000;

  int32_t i0 { 100 };
  int32_t i1 { 101 };
  int32_t i2 { 102 };

  list.add (&i0);
  list.add (&i1);
  list.add (&i2);

  list += 100;

  ASSERT_EQ (*list.find (&i0)->value, 200);
  ASSERT_EQ (*list.find (&i1)->value, 201);
  ASSERT_EQ (*list.find (&i2)->value, 202);

  list -= 200;

  ASSERT_EQ (*list.find (&i0)->value, 0);
  ASSERT_EQ (*list.find (&i1)->value, 1);
  ASSERT_EQ (*list.find (&i2)->value, 2);
}