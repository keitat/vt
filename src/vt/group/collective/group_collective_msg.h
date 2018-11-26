/*
//@HEADER
// ************************************************************************
//
//                          group_collective_msg.h
//                                VT
//              Copyright (C) 2017 NTESS, LLC
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

#if !defined INCLUDED_GROUP_GROUP_COLLECTIVE_MSG_H
#define INCLUDED_GROUP_GROUP_COLLECTIVE_MSG_H

#include "vt/config.h"
#include "vt/group/group_common.h"
#include "vt/group/msg/group_msg.h"
#include "vt/messaging/message.h"

#include <cstdlib>

namespace vt { namespace group {

template <typename MsgT>
struct GroupCollectiveInfoMsg : MsgT {
  using CountType = int32_t;

  GroupCollectiveInfoMsg() = default;
  GroupCollectiveInfoMsg(
    GroupType const& in_group, RemoteOperationIDType in_op, bool in_is_in_group,
    NodeType const& in_subtree,
    NodeType const& in_child_node = uninitialized_destination,
    CountType const& level = 0, CountType const& extra_nodes = 0
  ) : MsgT(in_group, in_op), is_in_group(in_is_in_group),
      child_node_(in_child_node), subtree_size_(in_subtree),
      extra_nodes_(extra_nodes), level_(level)
  { }

  NodeType getChild() const { return child_node_; }
  NodeType getSubtreeSize() const { return subtree_size_; }
  bool isStatic() const { return is_static_; }
  bool isInGroup() const { return is_in_group; }
  CountType getExtraNodes() const { return extra_nodes_; }
  CountType getLevel() const { return level_; }

private:
  bool is_in_group = false;
  bool is_static_ = true;
  NodeType child_node_ = uninitialized_destination;
  NodeType subtree_size_ = 0;
  CountType extra_nodes_ = 0;
  CountType level_ = 0;
};

using GroupCollectiveMsg = GroupCollectiveInfoMsg<GroupMsg<::vt::Message>>;

}} /* end namespace vt::group */

#endif /*INCLUDED_GROUP_GROUP_COLLECTIVE_MSG_H*/
