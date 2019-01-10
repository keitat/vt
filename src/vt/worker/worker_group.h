/*
//@HEADER
// ************************************************************************
//
//                          worker_group.h
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

#if !defined INCLUDED_WORKER_WORKER_GROUP_H
#define INCLUDED_WORKER_WORKER_GROUP_H

#include "vt/config.h"
#include "vt/worker/worker_common.h"
#include "vt/worker/worker.h"
#include "vt/worker/worker_group_counter.h"
#include "vt/worker/worker_group_comm.h"
#include "vt/utils/atomic/atomic.h"

#if backend_check_enabled(stdthread)
  #include "vt/worker/worker_stdthread.h"
#elif backend_no_threading
  #include "vt/worker/worker_seq.h"
#endif

#include <vector>
#include <memory>

namespace vt { namespace worker {

using ::vt::util::atomic::AtomicType;

template <typename WorkerT>
struct WorkerGroupAny : WorkerGroupCounter, WorkerGroupComm {
  using WorkerType = WorkerT;
  using WorkerPtrType = std::unique_ptr<WorkerT>;
  using WorkerContainerType = std::vector<WorkerPtrType>;

  WorkerGroupAny();
  WorkerGroupAny(WorkerCountType const& in_num_workers);

  virtual ~WorkerGroupAny();

  void initialize();
  void spawnWorkers();
  void spawnWorkersBlock(WorkerCommFnType fn);
  void joinWorkers();
  void progress();

  bool commScheduler();
  void enqueueCommThread(WorkUnitType const& work_unit);
  void enqueueAnyWorker(WorkUnitType const& work_unit);
  void enqueueForWorker(
    WorkerIDType const& worker_id, WorkUnitType const& work_unit
  );
  void enqueueAllWorkers(WorkUnitType const& work_unit);

private:
  WorkerFinishedFnType finished_fn_ = nullptr;
  bool initialized_ = false;
  WorkerCountType num_workers_ = 0;
  WorkerContainerType workers_;
};

#if backend_check_enabled(stdthread)
  using WorkerGroupSTD = WorkerGroupAny<StdThreadWorker>;
#elif backend_no_threading
  using WorkerGroupSeq = WorkerGroupAny<WorkerSeq>;
#endif /*backend_check_enabled(stdthread)*/

}} /* end namespace vt::worker */

#if backend_check_enabled(detector) && backend_check_enabled(stdthread)
  #include "vt/worker/worker_group_traits.h"

  namespace vt { namespace worker {

  static_assert(
    WorkerGroupTraits<WorkerGroupSTD>::is_worker,
    "WorkerGroupSTD must follow the WorkerGroup concept"
  );

  }} /* end namespace vt::worker */
#endif /*backend_check_enabled(detector) && backend_check_enabled(stdthread)*/

#include "vt/worker/worker_group.impl.h"

#endif /*INCLUDED_WORKER_WORKER_GROUP_H*/
