/*
//@HEADER
// ************************************************************************
//
//                          group_collective.h
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

#if !defined INCLUDED_GROUP_GROUP_COLLECTIVE_H
#define INCLUDED_GROUP_GROUP_COLLECTIVE_H

#include "vt/config.h"
#include "vt/group/group_common.h"
#include "vt/group/group_manager.fwd.h"
#include "vt/group/group_info.fwd.h"
#include "vt/collective/tree/tree.h"
#include "vt/collective/reduce/reduce.h"

#include <memory>

namespace vt { namespace group {

struct GroupCollective {
  using TreeType = collective::tree::Tree;
  using TreePtrType = std::unique_ptr<TreeType>;
  using NodeListType = TreeType::NodeListType;
  using ReduceType = collective::reduce::Reduce;
  using ReducePtrType = std::unique_ptr<ReduceType>;

  explicit GroupCollective()
    : init_span_(
        std::make_unique<TreeType>(collective::tree::tree_cons_tag_t)
      )
  { }

  friend struct InfoColl;

protected:
  NodeType getInitialParent() const { return init_span_->getParent(); }
  NodeType getInitialChildren() const { return init_span_->getNumChildren();  }
  bool isInitialRoot() const { return init_span_->isRoot();  }
  NodeListType const& getChildren() const { return init_span_->getChildren(); }

private:
  TreePtrType span_           = nullptr;
  TreePtrType init_span_      = nullptr;
  NodeListType span_children_ = {};
  NodeType parent_            = uninitialized_destination;
  ReducePtrType reduce_       = nullptr;
};

}} /* end namespace vt::group */

#endif /*INCLUDED_GROUP_GROUP_COLLECTIVE_H*/
