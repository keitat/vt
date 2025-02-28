/*
//@HEADER
// *****************************************************************************
//
//                           test_safe_union.nompi.cc
//                       DARMA/vt => Virtual Transport
//
// Copyright 2019-2021 National Technology & Engineering Solutions of Sandia, LLC
// (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact darma@sandia.gov
//
// *****************************************************************************
//@HEADER
*/

#include <gtest/gtest.h>

#include <vt/utils/adt/union.h>
#include "test_harness.h"

namespace vt { namespace tests { namespace unit {

using TestSafeUnion = TestHarness;

TEST_F(TestSafeUnion, test_safe_union_1) {

  vt::adt::SafeUnion<int64_t> x1;
  vt::adt::SafeUnion<int64_t, int64_t> x2;
  vt::adt::SafeUnion<int64_t, int64_t, int64_t> x3;
  vt::adt::SafeUnion<int64_t, int64_t, int64_t, int64_t> x4;

  static_assert(
    sizeof(x1) == sizeof(x2) and
    sizeof(x2) == sizeof(x3) and
    sizeof(x3) == sizeof(x4),
    "As a union these must all be the same size!"
  );

  vt::adt::SafeUnion<int64_t> y1;
  vt::adt::SafeUnion<int64_t, char> y2;

  // This makes an assertion about alignment that will hold across these two
  // types
  static_assert(
    sizeof(y1) == sizeof(y2),
    "As a union these must be the same size!"
  );
}

static int destroy_counter = 0;

TEST_F(TestSafeUnion, test_safe_union_2) {

  destroy_counter = 0;

  struct MyTest {
    struct Tag { };
    explicit MyTest(Tag) { }
    MyTest(MyTest const&) = delete;
    MyTest(MyTest&&) = delete;
    ~MyTest() { destroy_counter++; }
    int v = 0;
  };

  vt::adt::SafeUnion<int, float, MyTest> x;

  EXPECT_FALSE(x.is<int>());
  EXPECT_FALSE(x.is<float>());
  EXPECT_FALSE(x.is<MyTest>());

  x.init<int>(100);

  EXPECT_TRUE(x.is<int>());
  EXPECT_FALSE(x.is<float>());
  EXPECT_FALSE(x.is<MyTest>());

  EXPECT_EQ(x.get<int>(), 100);

  x.reset();

  EXPECT_FALSE(x.is<int>());
  EXPECT_FALSE(x.is<float>());
  EXPECT_FALSE(x.is<MyTest>());

  x.init<float>(29.33f);

  EXPECT_FALSE(x.is<int>());
  EXPECT_TRUE(x.is<float>());
  EXPECT_FALSE(x.is<MyTest>());

  EXPECT_EQ(x.get<float>(), 29.33f);

  x.reset();

  x.init<MyTest>(MyTest::Tag{});

  EXPECT_EQ(x.get<MyTest>().v, 0);

  x.get<MyTest>().v = 129;

  EXPECT_EQ(x.get<MyTest>().v, 129);

  EXPECT_EQ(destroy_counter, 0);

  x.reset();

  EXPECT_EQ(destroy_counter, 1);

}

static int copy_counter = 0;
static int move_counter = 0;
static int destroy_counter_2 = 0;

TEST_F(TestSafeUnion, test_safe_union_3) {

  destroy_counter = destroy_counter_2 = 0;
  copy_counter = 0;
  move_counter = 0;

  struct MyTest {
    struct Tag { };
    explicit MyTest(Tag) { }
    MyTest(MyTest const& other) {
      copy_counter++;
      v = other.v;
    }
    MyTest(MyTest&& other) : v(std::move(other.v)) {
      move_counter++;
    }
    ~MyTest() { destroy_counter++; }
    bool operator==(MyTest const& other) const { return v == other.v; }
    int v = 0;
  };

  struct MyTest2 {
    ~MyTest2() { destroy_counter_2++; }
    bool operator==(MyTest2 const& other) const { return true; }
  };

  vt::adt::SafeUnion<float, int, MyTest, double, MyTest2> x;

  x.init<MyTest>(MyTest::Tag{});

  EXPECT_TRUE(x.is<MyTest>());

  x.get<MyTest>().v = 234;

  x.reset();
  x.init<MyTest2>();
  x.reset();
  x.init<int>();
  x.reset();
  x.init<float>();
  x.reset();
  x.init<double>();
  x.reset();

  EXPECT_EQ(destroy_counter, 1);
  EXPECT_EQ(destroy_counter_2, 1);
  EXPECT_EQ(copy_counter, 0);

  x.init<MyTest>(MyTest::Tag{});
  x.get<MyTest>().v = 235;

  EXPECT_EQ(x.get<MyTest>().v, 235);

  vt::adt::SafeUnion<float, int, MyTest, double, MyTest2> y(x);

  EXPECT_TRUE(y == x);
  x.get<MyTest>().v = 236;
  EXPECT_FALSE(y == x);

  EXPECT_EQ(copy_counter, 1);
  EXPECT_TRUE(y.is<MyTest>());
  EXPECT_EQ(y.get<MyTest>().v, 235);

  vt::adt::SafeUnion<float, int, MyTest, double, MyTest2> z;
  z = y;

  EXPECT_EQ(copy_counter, 2);

  vt::adt::SafeUnion<float, int, MyTest, double, MyTest2> u;
  u = std::move(z);

  EXPECT_EQ(copy_counter, 2);
  EXPECT_EQ(move_counter, 1);

  u.reset();
  EXPECT_EQ(destroy_counter, 2);
  y.reset();
  EXPECT_EQ(destroy_counter, 3);
  x.reset();
  EXPECT_EQ(destroy_counter, 4);
  z.reset();
  EXPECT_EQ(destroy_counter, 5);
}

struct MyTest4 { };

struct TestFunctor2 {
  template <typename T>
  int apply(vt::adt::SafeUnion<float, int, MyTest4>& in);
};

template <>
int TestFunctor2::apply<float>(vt::adt::SafeUnion<float, int, MyTest4>& in) {
  EXPECT_FALSE(in.template is<int>());
  EXPECT_FALSE(in.template is<MyTest4>());

  EXPECT_TRUE(in.template is<float>());
  EXPECT_GT(in.template get<float>(), 29.3);
  EXPECT_LT(in.template get<float>(), 29.5);
  return 1;
}

template <>
int TestFunctor2::apply<int>(vt::adt::SafeUnion<float, int, MyTest4>& in) {
  EXPECT_FALSE(in.template is<float>());
  EXPECT_FALSE(in.template is<MyTest4>());

  EXPECT_TRUE(in.template is<int>());
  EXPECT_EQ(in.template get<int>(), 10);
  return 2;
}

template <>
int TestFunctor2::apply<MyTest4>(vt::adt::SafeUnion<float, int, MyTest4>& in) {
  // never should happen
  EXPECT_FALSE(true);
  return 3;
}

// this gets triggered when no type is selected
template <>
int TestFunctor2::apply<void>(vt::adt::SafeUnion<float, int, MyTest4>& in) {
  EXPECT_FALSE(in.template is<float>());
  EXPECT_FALSE(in.template is<int>());
  EXPECT_FALSE(in.template is<MyTest4>());
  return 0;
}

TEST_F(TestSafeUnion, test_safe_union_switch_4) {

  TestFunctor2 fn;

  vt::adt::SafeUnion<float, int, MyTest4> x;

  EXPECT_EQ(x.switchOn(fn), 0);

  x.init<int>();
  x.get<int>() = 10;

  EXPECT_EQ(x.switchOn(fn), 2);

  x.reset();

  x.init<float>();
  x.get<float>() = 29.4;

  EXPECT_EQ(x.switchOn(fn), 1);
}

}}} /* end namespace vt::tests::unit */
