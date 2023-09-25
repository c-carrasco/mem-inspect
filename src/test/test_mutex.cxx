// ----------------------------------------------------------------------------
// MIT License
//
// Copyright (c) 2023 Carlos Carrasco
// ----------------------------------------------------------------------------
#include <thread>

#include <gtest/gtest.h>

#include <meminspect/types.h>


struct Critical {
  Critical (meminspect::Mutex &m): mutex { m } {}

  void run () {
    for(int i = 0; i < 1000; i++) {
      mutex.lock();

      // dumb logic

      std::string str { "iteration number: " };
      if (i < 1000)
        str += "0";
      if (i < 100)
        str += "0";
      if (i < 10)
        str += "0";

      str += std::to_string (i);
      counter += actions.emplace_back (std::move (str)).size();

      mutex.unlock();
    }
  }

  meminspect::Mutex &mutex;
  int32_t counter { 0 };
  std::vector<std::string> actions;
};


// ----------------------------------------------------------------------------
// test_lock_unlock
// ----------------------------------------------------------------------------
TEST (Mutex, test_lock_unlock) {
  meminspect::Mutex mutex;

  Critical critical { mutex };

  std::thread t1 { [ &critical ] () { critical.run(); } };
  std::thread t2 { [ &critical ] () { critical.run(); } };

  t1.join();
  t2.join();

  EXPECT_EQ (22 * 1000 * 2, critical.counter); // 22=string-length, 1000=iteration, 2=threads
}
