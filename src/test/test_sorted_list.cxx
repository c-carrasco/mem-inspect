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

class SortedListTest: public ::testing::Test {
 protected:
  void SetUp() override {
    TestAllocator::malloc = malloc;
    TestAllocator::free = free;
  }
};


// ----------------------------------------------------------------------------
// test_add
// ----------------------------------------------------------------------------
TEST_F (SortedListTest, test_add) {
  meminspect::SortedList<int, std::string_view, TestAllocator> list;

  const auto n6 { list.add (6, "six") };
  ASSERT_EQ (n6->key, 6);
  ASSERT_EQ (n6->value, "six");
  ASSERT_EQ (n6->next, nullptr);

  const auto n3 { list.add (3, "three") };
  ASSERT_EQ (n3->key, 3);
  ASSERT_EQ (n3->value, "three");
  ASSERT_EQ (n3->next, n6);

  const auto n9 { list.add (9, "nine") };
  ASSERT_EQ (n9->key, 9);
  ASSERT_EQ (n9->value, "nine");
  ASSERT_EQ (n9->next, nullptr);

  const auto n7 { list.add (7, "seven") };
  ASSERT_EQ (n7->key, 7);
  ASSERT_EQ (n7->value, "seven");
  ASSERT_EQ (n7->next, n9);

  const auto n1 { list.add (1, "one") };
  ASSERT_EQ (n1->key, 1);
  ASSERT_EQ (n1->value, "one");
  ASSERT_EQ (n1->next, n3);

  const auto n4 { list.add (4, "four") };
  ASSERT_EQ (n4->key, 4);
  ASSERT_EQ (n4->value, "four");
  ASSERT_EQ (n4->next, n6);

  // check order
  const auto head { list.find (1) };
  ASSERT_EQ (head->next, n3);
  ASSERT_EQ (n3->next, n4);
  ASSERT_EQ (n4->next, n6);
  ASSERT_EQ (n6->next, n7);
  ASSERT_EQ (n7->next, n9);
  ASSERT_EQ (n9->next, nullptr);
}

// ----------------------------------------------------------------------------
// test_find
// ----------------------------------------------------------------------------
TEST_F (SortedListTest, test_find) {
  meminspect::SortedList<int, std::string_view, TestAllocator> list;

  ASSERT_EQ (list.find (0), nullptr);
  const auto three { list.add (3, "three") };
  const auto six { list.add (6, "six") };
  const auto nine { list.add (9, "nine") };

  ASSERT_EQ (list.find (3), three);
  ASSERT_EQ (list.find (6), six);
  ASSERT_EQ (list.find (9), nine);
  ASSERT_EQ (list.find (10), nullptr);

  ASSERT_TRUE (list.remove (6).has_value());
  ASSERT_EQ (list.find (6), nullptr);
}

// ----------------------------------------------------------------------------
// test_remove
// ----------------------------------------------------------------------------
TEST_F (SortedListTest, test_remove) {
  meminspect::SortedList<int, std::string_view, TestAllocator> list;

  ASSERT_FALSE (list.remove (99).has_value());

  const auto nine { list.add (9, "nine") };
  const auto six { list.add (6, "six") };
  const auto five { list.add (5, "five") };
  const auto three { list.add (3, "three") };
  const auto one { list.add (1, "one") };

  const auto head { list.find (1) };
  ASSERT_EQ (head, one);
  ASSERT_EQ (one->next, three);
  ASSERT_EQ (three->next, five);
  ASSERT_EQ (five->next, six);
  ASSERT_EQ (six->next, nine);
  ASSERT_EQ (nine->next, nullptr);

  // remove last (9)
  const auto v9 { list.remove (9) };
  ASSERT_TRUE (v9.has_value());
  ASSERT_EQ (*v9, "nine");

  ASSERT_EQ (list.find (9), nullptr);
  ASSERT_EQ (six->next, nullptr);

  // remove first (1)
  const auto v1 { list.remove (1) };
  ASSERT_TRUE (v1.has_value());
  ASSERT_EQ (*v1, "one");

  // remove middle (5)
  const auto v5 { list.remove (5) };
  ASSERT_TRUE (v5.has_value());
  ASSERT_EQ (*v5, "five");

  ASSERT_EQ (list.find (5), nullptr);
  ASSERT_EQ (three->next, six);

  // remove all
  const auto v6 { list.remove (6) };
  ASSERT_TRUE (v6.has_value());
  ASSERT_EQ (*v6, "six");
  ASSERT_EQ (three->next, nullptr);

  const auto v3 { list.remove (3) };
  ASSERT_TRUE (v3.has_value());
  ASSERT_EQ (*v3, "three");

  for (int i = 0; i < 10; ++i)
    ASSERT_EQ (list.find (i), nullptr);
}