/*
//@HEADER
// ************************************************************************
//
//                       test_objgroup.cc
//                     vt (Virtual Transport)
//                  Copyright (C) 2018 NTESS, LLC
//
// Under the terms of Contract DE-NA-0003525 with NTESS, LLC,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact darma@sandia.gov
//
// ************************************************************************
//@HEADER
*/

#include "test_objgroup_common.h"

namespace vt { namespace tests { namespace unit {

struct TestObjGroup : TestParallelHarness {

  void SetUp() override {
    TestParallelHarness::SetUp();
    auto const nb_nodes = vt::theContext()->getNumNodes();
    vtAssert(nb_nodes > 1, "Failure: require two or more nodes");
  }

  void TearDown() override {
    TestParallelHarness::TearDown();
    MyObjA::next_id = 0;
    MyObjB::next_id = 0;
  }
};

TEST_F(TestObjGroup, test_proxy_construct_send) {

  auto const my_node = vt::theContext()->getNode();
  auto const epoch = vt::theTerm()->makeEpochCollective();

  // create object group using object constructor or a raw pointer.
  // EDIT: cannot create group using a smart pointer (unique, shared)
  // due to overload resolution issue related to variadic constructor.
  MyObjB obj(0xFFFFFFFF);
  auto proxy1 = vt::theObjGroup()->makeCollective<MyObjA>();
  auto proxy2 = vt::theObjGroup()->makeCollective<MyObjB>(&obj);
  auto proxy3 = vt::theObjGroup()->makeCollective<MyObjA>();
  //auto obj_ptr = std::make_shared<MyObjB>(tag);
  //auto proxy3 = vt::theObjGroup()->makeCollective<MyObjB>(obj_ptr); // fails
  //auto proxy4 = vt::theObjGroup()->makeCollective<MyObjB>(tag); // fails

  if (my_node == 0) {
    proxy1[0].send<MyMsg, &MyObjA::handler>();
    proxy1[0].send<MyMsg, &MyObjA::handler>();
    proxy1[1].send<MyMsg, &MyObjA::handler>();
  } else if (my_node == 1) {
    proxy2.broadcast<MyMsg, &MyObjB::handler>();
    proxy3[0].send<MyMsg, &MyObjA::handler>();
  }

  // check received messages for each group
  vt::theTerm()->addAction(epoch, [=]{
    auto obj1 = proxy1.get();
    auto obj2 = proxy2.get();
    auto obj3 = proxy3.get();

    switch (my_node) {
      case 0:  EXPECT_EQ(obj1->recv_, 2); break;
      case 1:  EXPECT_EQ(obj1->recv_, 1); break;
      default: EXPECT_EQ(obj1->recv_, 0); break;
    }
    EXPECT_EQ(obj2->recv_, 1);
    EXPECT_EQ(obj3->recv_, my_node == 0 ? 1 : 0);
  });

  vt::theCollective()->barrier();
  vt::theTerm()->finishedEpoch(epoch);
}

TEST_F(TestObjGroup, test_proxy_reduce) {

  auto const my_node = vt::theContext()->getNode();
  auto const epoch = vt::theTerm()->makeEpochCollective();

  // create two proxy instances of a same object group type
  auto proxy1 = vt::theObjGroup()->makeCollective<MyObjA>();
  auto proxy2 = vt::theObjGroup()->makeCollective<MyObjA>();
  auto proxy3 = vt::theObjGroup()->makeCollective<MyObjA>();

  // the three reductions should not interfere each other, even if
  // performed by the same subset of nodes within the same epoch.
  using namespace vt::collective;
  auto msg1 = vt::makeMessage<SysMsg>(my_node);
  vt::theObjGroup()->reduce<
    MyObjA,
    SysMsg,
    SysMsg::msgHandler<SysMsg, PlusOp<int>, Verify<1> >
  >(proxy1, msg1, epoch, vt::no_tag);
  //proxy1.reduce<SysMsg, Verify<1>>(msg1, epoch, vt::no_tag); // bad construct ?

  auto msg2 = vt::makeMessage<SysMsg>(4);
  vt::theObjGroup()->reduce<
    MyObjA,
    SysMsg,
    SysMsg::msgHandler<SysMsg, PlusOp<int>, Verify<2> >
  >(proxy2, msg2, epoch, vt::no_tag);

  auto msg3 = vt::makeMessage<SysMsg>(my_node);
  vt::theObjGroup()->reduce<
    MyObjA,
    SysMsg,
    SysMsg::msgHandler<SysMsg, MaxOp<int>, Verify<3> >
  >(proxy3, msg3, epoch, vt::no_tag);

  vt::theTerm()->finishedEpoch(epoch);
}

}}} // end namespace vt::tests::unit