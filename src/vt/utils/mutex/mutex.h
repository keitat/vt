/*
//@HEADER
// *****************************************************************************
//
//                                   mutex.h
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

#if !defined INCLUDED_VT_UTILS_MUTEX_MUTEX_H
#define INCLUDED_VT_UTILS_MUTEX_MUTEX_H

#include "vt/config.h"
#include "vt/utils/mutex/lock_guard.h"
#include "vt/utils/mutex/null_mutex.h"

#include <mutex>

#if vt_check_enabled(openmp)
  #include "vt/utils/mutex/omp_mutex.h"
#elif vt_check_enabled(stdthread)
  #include "vt/utils/mutex/std_mutex.h"
#else
  #include "vt/utils/mutex/null_mutex.h"
#endif

namespace vt { namespace util { namespace mutex {

#if vt_check_enabled(openmp)
  using MutexType = OMPMutex;
#elif vt_check_enabled(stdthread)
  using MutexType = STDMutex;
#else
  using MutexType = NullMutex;
#endif

using NullMutexType = NullMutex;

using LockGuardType = LockGuardAnyType<MutexType>;
using LockGuardPtrType = LockGuardAnyPtrType<MutexType>;

}}} /* end namespace vt::util::mutex */

#endif /*INCLUDED_VT_UTILS_MUTEX_MUTEX_H*/
