/*
//@HEADER
// *****************************************************************************
//
//                             worker_stdthread.cc
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

#if backend_check_enabled(stdthread)

#include "vt/context/context.h"
#include "vt/context/context_attorney.h"
#include "vt/collective/collective.h"
#include "vt/worker/worker_common.h"
#include "vt/worker/worker_stdthread.h"

#include <thread>
#include <memory>
#include <functional>

namespace vt { namespace worker {

StdThreadWorker::StdThreadWorker(
  WorkerIDType const& in_worker_id_, WorkerCountType const&,
  WorkerFinishedFnType finished_fn
) : worker_id_(in_worker_id_), finished_fn_(finished_fn)
{ }

void StdThreadWorker::enqueue(WorkUnitType const& work_unit) {
  work_queue_.pushBack(work_unit);
}

void StdThreadWorker::progress() {
  // Noop
}

void StdThreadWorker::scheduler() {
  using ::vt::ctx::ContextAttorney;

  // For now, all workers to have direct access to the runtime
  // TODO: this needs to change
  CollectiveOps::setCurrentRuntimeTLS();

  // Set the thread-local worker in the Context
  ContextAttorney::setWorker(worker_id_comm_thread);

  while (not should_terminate_.load()) {
    if (work_queue_.size() > 0) {
      auto elm = work_queue_.popGetBack();
      elm();
      finished_fn_(worker_id_, 1);
    }
  }
}

void StdThreadWorker::sendTerminateSignal() {
  should_terminate_.store(true);
}

void StdThreadWorker::spawn() {
  debug_print(
    worker, node,
    "StdThreadWorker: spawn: spawning worker: id={}\n", worker_id_
  );

  auto sched_fn = std::bind(&StdThreadWorker::scheduler, this);
  thd_ = std::make_unique<ThreadType>(sched_fn);
}

void StdThreadWorker::join() {
  debug_print(
    worker, node,
    "StdThreadWorker: join: spawning worker: id={}\n", worker_id_
  );

  // tell the worker to return from the scheduler loop
  sendTerminateSignal();

  // join the std::thread
  thd_->join();
}

void StdThreadWorker::dispatch(WorkerFunType fun) {
  enqueue(fun);
}

}} /* end namespace vt::worker */

#endif /*backend_check_enabled(stdthread)*/
