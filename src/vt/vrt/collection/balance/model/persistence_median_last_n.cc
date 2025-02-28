/*
//@HEADER
// *****************************************************************************
//
//                         persistence_median_last_n.cc
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

#include "vt/config.h"
#include "vt/vrt/collection/balance/model/persistence_median_last_n.h"
#include <utility>

namespace vt { namespace vrt { namespace collection { namespace balance {

PersistenceMedianLastN::PersistenceMedianLastN(std::shared_ptr<LoadModel> base, unsigned int n)
  : ComposedModel(base)
  , n_(n)
{
  vtAssert(n > 0, "Cannot take a median over no phases");
}

TimeType PersistenceMedianLastN::getWork(ElementIDStruct object, PhaseOffset when)
{
  // Retrospective queries don't call for a prospective calculation
  if (when.phases < 0)
    return ComposedModel::getWork(object, when);

  unsigned int phases = std::min(n_, getNumCompletedPhases());
  std::vector<TimeType> times(phases);
  for (unsigned int i = 1; i <= phases; ++i) {
    PhaseOffset p{-1*static_cast<int>(i), when.subphase};
    TimeType t = ComposedModel::getWork(object, p);
    times[i-1] = t;
  }

  std::sort(times.begin(), times.end());

  if (phases % 2 == 1)
    return times[phases / 2];
  else
    return (times[phases / 2 - 1] + times[phases / 2]) / 2;
}

unsigned int PersistenceMedianLastN::getNumPastPhasesNeeded(unsigned int look_back)
{
  return ComposedModel::getNumPastPhasesNeeded(std::max(n_, look_back));
}

}}}}
