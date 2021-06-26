/*
//@HEADER
// *****************************************************************************
//
//                               seq_node.impl.h
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

#if !defined INCLUDED_VT_SEQUENCE_SEQ_NODE_IMPL_H
#define INCLUDED_VT_SEQUENCE_SEQ_NODE_IMPL_H

#include <list>
#include <memory>
#include <cassert>
#include <cstdint>

#include "vt/config.h"
#include "vt/sequence/seq_common.h"
#include "vt/sequence/seq_helpers.h"
#include "vt/sequence/seq_closure.h"
#include "vt/sequence/seq_node.h"

namespace vt { namespace seq {

template <typename... Args>
/*static*/ SeqNodePtrType SeqNode::makeNode(
  SeqType const& id, SeqNodePtrType parent, Args&&... args
) {
  return std::make_shared<SeqNode>(id, parent, std::forward<Args...>(args...));
}

template <typename... Args>
/*static*/ SeqNodePtrType SeqNode::makeParallelNode(
  SeqType const& id, Args&&... args
) {
  ActionType const act = nullptr;
  auto par = new SeqParallel(id, act, std::forward<Args>(args)...);
  return std::make_shared<SeqNode>(seq_node_parallel_tag_t, id, par);
}

/*static*/ inline SeqNodePtrType SeqNode::makeParallelNode(
  SeqType const& id, SeqFuncContainerType const& funcs
) {
  ActionType const act = nullptr;
  auto par = new SeqParallel(id, act, funcs);
  return std::make_shared<SeqNode>(seq_node_parallel_tag_t, id, par);
}

template <typename... FnT>
SeqNode::SeqNode(SeqType const& id, SeqNodeLeafTag, FnT&&... fns)
  : SeqNode(seq_node_leaf_tag_t, id)
{
  auto vec = {fns...};
  for (auto&& elm : vec) {
    payload_.funcs->push_back(elm);
  }
}

}} //end namespace vt::seq

#endif /* INCLUDED_VT_SEQUENCE_SEQ_NODE_IMPL_H*/
