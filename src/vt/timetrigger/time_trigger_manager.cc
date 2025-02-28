/*
//@HEADER
// *****************************************************************************
//
//                           time_trigger_manager.cc
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

#include "vt/timetrigger/time_trigger_manager.h"
#include "vt/timing/timing.h"

namespace vt { namespace timetrigger {

int TimeTriggerManager::addTrigger(
  TimeType current_time, std::chrono::milliseconds period,
  ActionType action, bool fire_immediately
) {
  int const cur_id = next_trigger_id_++;
  Trigger trigger{period, action, cur_id};
  if (fire_immediately) {
    trigger.runAction(current_time);
  } else {
    trigger.setLastTriggerTime(current_time);
  }
  queue_.push(trigger);
  return cur_id;
}

void TimeTriggerManager::removeTrigger(int id) {
  removed_.insert(id);
}

void TimeTriggerManager::triggerReady(TimeType cur_time) {
  while (not queue_.empty()) {
    if (queue_.top().ready(cur_time)) {
      auto t = queue_.top();

      auto iter = removed_.find(t.getID());
      if (iter != removed_.end()) {
        queue_.pop();
        removed_.erase(iter);
        continue;
      }

      queue_.pop();
      t.runAction(cur_time);
      queue_.push(t);
    } else {
      // all other triggers will not be ready if this one isn't
      break;
    }
  }
}

int TimeTriggerManager::progress() {
  auto const cur_time = timing::getCurrentTime();
  triggerReady(cur_time);
  return 0;
}

}} /* end namespace vt::timetrigger */
