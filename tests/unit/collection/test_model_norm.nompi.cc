/*
//@HEADER
// *****************************************************************************
//
//                           test_model_norm.nompi.cc
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

#include <vt/vrt/collection/balance/model/load_model.h>
#include <vt/vrt/collection/balance/model/norm.h>

#include <gtest/gtest.h>

#include "test_harness.h"

#include <limits>
#include <memory>

namespace vt { namespace tests { namespace unit { namespace norm {

using TestModelNorm = TestHarness;

using vt::vrt::collection::balance::CommMapType;
using vt::vrt::collection::balance::ElementIDStruct;
using vt::vrt::collection::balance::LoadMapType;
using vt::vrt::collection::balance::LoadModel;
using vt::vrt::collection::balance::Norm;
using vt::vrt::collection::balance::ObjectIterator;
using vt::vrt::collection::balance::PhaseOffset;
using vt::vrt::collection::balance::SubphaseLoadMapType;
using vt::vrt::collection::balance::LoadMapObjectIterator;

using ProcLoadMap = std::unordered_map<PhaseType, LoadMapType>;
using ProcSubphaseLoadMap = std::unordered_map<PhaseType, SubphaseLoadMapType>;
using ProcCommMap = std::unordered_map<PhaseType, CommMapType>;

constexpr auto num_subphases = 3;

struct StubModel : LoadModel {

  StubModel() = default;
  virtual ~StubModel() = default;

  void setLoads(
    ProcLoadMap const* proc_load,
    ProcCommMap const*) override {
    proc_load_ = proc_load;
  }

  void updateLoads(PhaseType) override {}

  TimeType getWork(ElementIDStruct id, PhaseOffset phase) override {
    return proc_load_->at(0).at(id).subphase_loads.at(phase.subphase);
  }

  ObjectIterator begin() override {
    return {std::make_unique<LoadMapObjectIterator>(proc_load_->at(0).begin(), proc_load_->at(0).end())};
  }

  int getNumSubphases() override { return num_subphases; }

  // Not used in this test
  unsigned int getNumCompletedPhases() override { return 0; }
  unsigned int getNumPastPhasesNeeded(unsigned int look_back = 0) override { return look_back; }

private:
  ProcLoadMap const* proc_load_ = nullptr;
};

TEST_F(TestModelNorm, test_model_norm_1) {
  NodeType this_node = 0;
  ProcLoadMap proc_load = {
    {0,
     LoadMapType{
       {ElementIDStruct{1,this_node}, {TimeType{60}, {TimeType{10}, TimeType{20}, TimeType{30}}}},
       {ElementIDStruct{2,this_node}, {TimeType{150}, {TimeType{40}, TimeType{50}, TimeType{60}}}}}}};

  auto test_model = std::make_shared<Norm>(std::make_shared<StubModel>(), 3.0);
  test_model->setLoads(&proc_load, nullptr);
  test_model->updateLoads(0);

  for (unsigned int iter = 0; iter < num_subphases; ++iter) {
    int objects_seen = 0;
    for (auto&& obj : *test_model) {
      EXPECT_TRUE(obj.id == 1 || obj.id == 2);
      ++objects_seen;

      // offset.subphase != PhaseOffset::WHOLE_PHASE
      // expect work load value for given subphase
      auto work_val = test_model->getWork(obj, PhaseOffset{0, iter});
      EXPECT_EQ(work_val, proc_load[0][obj].subphase_loads[iter]);
    }

    EXPECT_EQ(objects_seen, 2);
  }
}

TEST_F(TestModelNorm, test_model_norm_2) {
  NodeType this_node = 0;
  ProcLoadMap proc_load = {
    {0,
     LoadMapType{
       {ElementIDStruct{1,this_node}, {TimeType{60}, {TimeType{10}, TimeType{20}, TimeType{30}}}},
       {ElementIDStruct{2,this_node}, {TimeType{150}, {TimeType{40}, TimeType{50}, TimeType{60}}}}}}};

  // finite 'power' value
  auto test_model = std::make_shared<Norm>(std::make_shared<StubModel>(), 3.0);
  test_model->setLoads(&proc_load, nullptr);
  test_model->updateLoads(0);

  std::array<TimeType, 2> expected_norms = {
    TimeType{33.019}, TimeType{73.986}};

  int objects_seen = 0;
  for (auto&& obj : *test_model) {
    EXPECT_TRUE(obj.id == 1 || obj.id == 2);
    ++objects_seen;

    auto work_val =
      test_model->getWork(obj, PhaseOffset{0, PhaseOffset::WHOLE_PHASE});
    EXPECT_NEAR(work_val, expected_norms[obj.id - 1], 0.001);
  }

  EXPECT_EQ(objects_seen, 2);
}

TEST_F(TestModelNorm, test_model_norm_3) {
  NodeType this_node = 0;
  ProcLoadMap proc_load = {
    {0,
     LoadMapType{
       {ElementIDStruct{1,this_node}, {TimeType{60}, {TimeType{10}, TimeType{20}, TimeType{30}}}},
       {ElementIDStruct{2,this_node}, {TimeType{150}, {TimeType{40}, TimeType{50}, TimeType{60}}}}}}};

  // infinite 'power' value
  auto test_model = std::make_shared<Norm>(
    std::make_shared<StubModel>(), std::numeric_limits<double>::infinity());
  test_model->setLoads(&proc_load, nullptr);
  test_model->updateLoads(0);

  std::array<TimeType, 2> expected_norms = {TimeType{30}, TimeType{60}};

  int objects_seen = 0;
  for (auto&& obj : *test_model) {
    EXPECT_TRUE(obj.id == 1 || obj.id == 2);
    ++objects_seen;

    auto work_val =
      test_model->getWork(obj, PhaseOffset{0, PhaseOffset::WHOLE_PHASE});
    EXPECT_EQ(work_val, expected_norms[obj.id - 1]);
  }

  EXPECT_EQ(objects_seen, 2);
}

}}}} // end namespace vt::tests::unit::norm
