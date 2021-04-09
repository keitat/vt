/*
//@HEADER
// *****************************************************************************
//
//                           test_gossiplb.nompi.cc
//                           DARMA Toolkit v. 1.0.0
//                       DARMA/vt => Virtual Transport
//
// Copyright 2019 National Technology & Engineering Solutions of Sandia, LLC
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

#include <vt/transport.h>
#include <vt/vrt/collection/balance/lb_common.h>
#include <vt/vrt/collection/balance/baselb/baselb.h>
#include <vt/vrt/collection/balance/gossiplb/gossiplb.h>

#include "test_harness.h"

namespace vt { namespace tests { namespace unit {

using TestGossipLB = TestHarness;
using ElementIDStruct = vt::vrt::collection::balance::ElementIDStruct;
using ElementIDType = vt::vrt::collection::balance::ElementIDType;
using ObjectOrdering = vt::vrt::collection::lb::ObjectOrderEnum;

void setupProblem(
  std::unordered_map<ElementIDStruct, TimeType> &cur_objs,
  TimeType &my_load
) {
  cur_objs.emplace(ElementIDStruct{3,0,0}, 4.0);
  cur_objs.emplace(ElementIDStruct{5,0,0}, 5.0);
  cur_objs.emplace(ElementIDStruct{2,0,0}, 9.0);
  cur_objs.emplace(ElementIDStruct{0,0,0}, 2.0);
  cur_objs.emplace(ElementIDStruct{1,0,0}, 6.0);
  cur_objs.emplace(ElementIDStruct{4,0,0}, 3.0);

  my_load = 0;
  for (auto &obj : cur_objs) {
    my_load += vt::vrt::collection::lb::BaseLB::loadMilli(obj.second);
  }
}

TEST_F(TestGossipLB, test_gossiplb_ordering_elmid) {
  std::unordered_map<ElementIDStruct, TimeType> cur_objs;
  TimeType my_load = 0;
  setupProblem(cur_objs, my_load);

  ObjectOrdering order = ObjectOrdering::ElmID;
  TimeType over_avg = vt::vrt::collection::lb::BaseLB::loadMilli(4.5);
  TimeType target_load = my_load - over_avg;
  std::vector<ElementIDType> soln = {0, 1, 2, 3, 4, 5};

  auto ordered_objs = vt::vrt::collection::lb::GossipLB::orderObjects(
    order, cur_objs, my_load, target_load
  );

  int i = 0;
  for (auto obj_id : ordered_objs) {
    EXPECT_EQ(obj_id.id, soln[i++]);
  }
}

TEST_F(TestGossipLB, test_gossiplb_ordering_leastmigrations) {
  std::unordered_map<ElementIDStruct, TimeType> cur_objs;
  TimeType my_load = 0;
  setupProblem(cur_objs, my_load);

  ObjectOrdering order = ObjectOrdering::LeastMigrations;
  TimeType over_avg = vt::vrt::collection::lb::BaseLB::loadMilli(4.5);
  TimeType target_load = my_load - over_avg;
  // single_obj_load will be 5.0
  // load order will be 5.0, 4.0, 3.0, 2.0, 6.0, 9.0
  std::vector<ElementIDType> soln = {5, 3, 4, 0, 1, 2};

  auto ordered_objs = vt::vrt::collection::lb::GossipLB::orderObjects(
    order, cur_objs, my_load, target_load
  );

  int i = 0;
  for (auto obj_id : ordered_objs) {
    EXPECT_EQ(obj_id.id, soln[i++]);
  }
}

TEST_F(TestGossipLB, test_gossiplb_ordering_leastmigrations_largest) {
  std::unordered_map<ElementIDStruct, TimeType> cur_objs;
  TimeType my_load = 0;
  setupProblem(cur_objs, my_load);

  ObjectOrdering order = ObjectOrdering::LeastMigrations;
  TimeType over_avg = vt::vrt::collection::lb::BaseLB::loadMilli(12.5);
  TimeType target_load = my_load - over_avg;
  // single_obj_load will be 9.0
  // load order will be 9.0, 6.0, 5.0, 4.0, 3.0, 2.0
  std::vector<ElementIDType> soln = {2, 1, 5, 3, 4, 0};

  auto ordered_objs = vt::vrt::collection::lb::GossipLB::orderObjects(
    order, cur_objs, my_load, target_load
  );

  int i = 0;
  for (auto obj_id : ordered_objs) {
    EXPECT_EQ(obj_id.id, soln[i++]);
  }
}

TEST_F(TestGossipLB, test_gossiplb_ordering_leastmigrations_smallest) {
  std::unordered_map<ElementIDStruct, TimeType> cur_objs;
  TimeType my_load = 0;
  setupProblem(cur_objs, my_load);

  ObjectOrdering order = ObjectOrdering::LeastMigrations;
  TimeType over_avg = vt::vrt::collection::lb::BaseLB::loadMilli(1.5);
  TimeType target_load = my_load - over_avg;
  // single_obj_load will be 2.0
  // load order will be 2.0, 3.0, 4.0, 5.0, 6.0, 9.0
  std::vector<ElementIDType> soln = {0, 4, 3, 5, 1, 2};

  auto ordered_objs = vt::vrt::collection::lb::GossipLB::orderObjects(
    order, cur_objs, my_load, target_load
  );

  int i = 0;
  for (auto obj_id : ordered_objs) {
    EXPECT_EQ(obj_id.id, soln[i++]);
  }
}

TEST_F(TestGossipLB, test_gossiplb_ordering_smallestobjects) {
  std::unordered_map<ElementIDStruct, TimeType> cur_objs;
  TimeType my_load = 0;
  setupProblem(cur_objs, my_load);

  ObjectOrdering order = ObjectOrdering::SmallestObjects;
  TimeType over_avg = vt::vrt::collection::lb::BaseLB::loadMilli(4.5);
  TimeType target_load = my_load - over_avg;
  // marginal_obj_load will be 3.0
  // load order will be 3.0, 2.0, 4.0, 5.0, 6.0, 9.0
  std::vector<ElementIDType> soln = {4, 0, 3, 5, 1, 2};

  auto ordered_objs = vt::vrt::collection::lb::GossipLB::orderObjects(
    order, cur_objs, my_load, target_load
  );

  int i = 0;
  for (auto obj_id : ordered_objs) {
    EXPECT_EQ(obj_id.id, soln[i++]);
  }
}

TEST_F(TestGossipLB, test_gossiplb_ordering_smallestobjects_largest) {
  std::unordered_map<ElementIDStruct, TimeType> cur_objs;
  TimeType my_load = 0;
  setupProblem(cur_objs, my_load);

  ObjectOrdering order = ObjectOrdering::SmallestObjects;
  TimeType target_load = vt::vrt::collection::lb::BaseLB::loadMilli(0.5);
  // marginal_obj_load will be 9.0
  // load order will be 9.0, 6.0, 5.0, 4.0, 3.0, 2.0
  std::vector<ElementIDType> soln = {2, 1, 5, 3, 4, 0};

  auto ordered_objs = vt::vrt::collection::lb::GossipLB::orderObjects(
    order, cur_objs, my_load, target_load
  );

  int i = 0;
  for (auto obj_id : ordered_objs) {
    EXPECT_EQ(obj_id.id, soln[i++]);
  }
}

TEST_F(TestGossipLB, test_gossiplb_ordering_smallestobjects_smallest) {
  std::unordered_map<ElementIDStruct, TimeType> cur_objs;
  TimeType my_load = 0;
  setupProblem(cur_objs, my_load);

  ObjectOrdering order = ObjectOrdering::SmallestObjects;
  TimeType over_avg = vt::vrt::collection::lb::BaseLB::loadMilli(1.5);
  TimeType target_load = my_load - over_avg;
  // marginal_obj_load will be 2.0
  // load order will be 2.0, 3.0, 4.0, 5.0, 6.0, 9.0
  std::vector<ElementIDType> soln = {0, 4, 3, 5, 1, 2};

  auto ordered_objs = vt::vrt::collection::lb::GossipLB::orderObjects(
    order, cur_objs, my_load, target_load
  );

  int i = 0;
  for (auto obj_id : ordered_objs) {
    EXPECT_EQ(obj_id.id, soln[i++]);
  }
}

}}} // end namespace vt::tests::unit
