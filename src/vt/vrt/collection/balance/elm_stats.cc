/*
//@HEADER
// *****************************************************************************
//
//                                 elm_stats.cc
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

#include "vt/config.h"
#include "vt/vrt/collection/balance/elm_stats.h"
#include "vt/timing/timing.h"

#include <cassert>

namespace vt { namespace vrt { namespace collection { namespace balance {

void ElementStats::startTime() {
  auto const start_time = timing::Timing::getCurrentTime();
  cur_time_ = start_time;
  cur_time_started_ = true;

  debug_print_verbose(
    lb, node,
    "ElementStats: startTime: time={}\n",
    start_time
  );
}

void ElementStats::stopTime() {
  auto const stop_time = timing::Timing::getCurrentTime();
  auto const total_time = stop_time - cur_time_;
  //vtAssert(cur_time_started_, "Must have started time");
  auto const started = cur_time_started_;
  if (started) {
    cur_time_started_ = false;
    addTime(total_time);
  }

  debug_print_verbose(
    lb, node,
    "ElementStats: stopTime: time={}, total={}, started={}\n",
    stop_time, total_time, started
  );
}

void ElementStats::recvObjData(
  ElementIDType pto, ElementIDType tto,
  ElementIDType pfrom, ElementIDType tfrom, double bytes, bool bcast
) {
  LBCommKey key(LBCommKey::CollectionTag{}, pfrom, tfrom, pto, tto, bcast);
  comm_[cur_phase_][key] += bytes;
}

void ElementStats::recvFromNode(
  ElementIDType pto, ElementIDType tto, NodeType from,
  double bytes, bool bcast
) {
  LBCommKey key(LBCommKey::NodeToCollectionTag{}, from, pto, tto, bcast);
  comm_[cur_phase_][key] += bytes;
}

void ElementStats::recvToNode(
  NodeType to, ElementIDType pfrom, ElementIDType tfrom,
  double bytes, bool bcast
) {
  LBCommKey key(LBCommKey::CollectionToNodeTag{}, pfrom, tfrom, to, bcast);
  comm_[cur_phase_][key] += bytes;
}

void ElementStats::setModelWeight(TimeType const& time) {
  cur_time_started_ = false;
  addTime(time);

  debug_print(
    lb, node,
    "ElementStats: setModelWeight: time={}, cur_load={}\n",
    time, phase_timings_[cur_phase_]
  );
}

void ElementStats::addTime(TimeType const& time) {
  phase_timings_[cur_phase_] += time;

  subphase_timings_[cur_phase_].resize(cur_subphase_ + 1);
  subphase_timings_[cur_phase_].at(cur_subphase_) += time;

  debug_print(
    lb, node,
    "ElementStats: addTime: time={}, cur_load={}\n",
    time, phase_timings_.at(cur_phase_)
  );
}

void ElementStats::updatePhase(PhaseType const& inc) {
  debug_print(
    lb, node,
    "ElementStats: updatePhase: cur_phase_={}, inc={}\n",
    cur_phase_, inc
  );

  cur_phase_ += inc;
}

void ElementStats::resetPhase() {
  cur_phase_ = fst_lb_phase;
}

PhaseType ElementStats::getPhase() const {
  return cur_phase_;
}

TimeType ElementStats::getLoad(PhaseType const& phase) const {
  auto iter = phase_timings_.find(phase);
  if (iter != phase_timings_.end()) {
    debug_print(
      lb, node,
      "ElementStats: getLoad: load={}, phase={}, size={}\n",
      iter->second, phase, phase_timings_.size()
    );

    return iter->second;
  } else {
    return 0.;
  }
}

TimeType ElementStats::getLoad(PhaseType phase, SubphaseType subphase) const {
  if (subphase == no_subphase)
    return getLoad(phase);

  auto iter = subphase_timings_.find(phase);
  if (iter != subphase_timings_.end()) {
    auto const& subphase_loads = iter->second;

    auto total_load = subphase_loads.at(subphase);

    debug_print(
      lb, node,
      "ElementStats: getLoad: load={}, phase={}, subphase={}\n",
      total_load, phase, subphase
    );

    return total_load;
  } else {
    return 0.;
  }
}

CommMapType const&
ElementStats::getComm(PhaseType const& phase) {
  auto const& phase_comm = comm_[phase];

  debug_print(
    lb, node,
    "ElementStats: getComm: comm size={}, phase={}\n",
    phase_comm.size(), phase
  );

  return phase_comm;
}

void ElementStats::setSubPhase(SubphaseType subphase) {
  vtAssert(subphase < no_subphase, "subphase must be less than sentinel");
  cur_subphase_ = subphase;
}

typename ElementStats::SubphaseType ElementStats::getSubPhase() const {
  return cur_subphase_;
}

/*static*/
void ElementStats::setFocusedSubPhase(VirtualProxyType collection, SubphaseType subphase) {
  focused_subphase_[collection] = subphase;
}

/*static*/
ElementStats::SubphaseType ElementStats::getFocusedSubPhase(VirtualProxyType collection) {
  auto i = focused_subphase_.find(collection);
  if (i != focused_subphase_.end())
    return i->second;
  else
    return no_subphase;
}

/*static*/ std::unordered_map<VirtualProxyType,ElementStats::SubphaseType> ElementStats::focused_subphase_;

void ElementStats::clear() {
  phase_timings_.clear();
  comm_.clear();
  subphase_timings_.clear();
}

}}}} /* end namespace vt::vrt::collection::balance */
