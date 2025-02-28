/*
//@HEADER
// *****************************************************************************
//
//                                group_region.h
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

#if !defined INCLUDED_VT_GROUP_REGION_GROUP_REGION_H
#define INCLUDED_VT_GROUP_REGION_GROUP_REGION_H

#include "vt/config.h"
#include "vt/group/group_common.h"

#include <vector>
#include <cstdlib>
#include <memory>
#include <tuple>
#include <functional>

namespace vt { namespace group { namespace region {

struct Region {
  using BoundType = NodeType;
  using SizeType = size_t;
  using RegionPtr = Region*;
  using RegionUPtrType = std::unique_ptr<Region>;
  using SplitRegionType = std::tuple<RegionUPtrType, RegionUPtrType>;
  using ListType = std::vector<BoundType>;
  using ApplyFnType = std::function<void(RegionUPtrType)>;

  virtual ~Region(){}
  virtual SizeType getSize() const = 0;
  virtual void sort() = 0;
  virtual bool contains(NodeType const& node) = 0;
  virtual bool isList() const = 0;
  virtual ListType const& makeList() = 0;
  virtual RegionUPtrType copy() const = 0;
  virtual BoundType head() const = 0;
  virtual RegionUPtrType tail() const = 0;
  virtual SplitRegionType split() const = 0;
  virtual void splitN(int nsplits, ApplyFnType apply) const = 0;
};

struct List;
struct Range;
struct ShallowList;

}}} /* end namespace vt::group::region */

#endif /*INCLUDED_VT_GROUP_REGION_GROUP_REGION_H*/
