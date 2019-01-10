/*
//@HEADER
// ************************************************************************
//
//                          hierlb_msgs.h
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

#if !defined INCLUDED_VRT_COLLECTION_BALANCE_HIERARCHICALLB_HIERLB_MSGS_H
#define INCLUDED_VRT_COLLECTION_BALANCE_HIERARCHICALLB_HIERLB_MSGS_H

#include "vt/config.h"
#include "vt/vrt/collection/balance/hierarchicallb/hierlb_constants.h"
#include "vt/vrt/collection/balance/hierarchicallb/hierlb_types.h"
#include "vt/vrt/collection/balance/proc_stats.h"
#include "vt/messaging/message.h"

namespace vt { namespace vrt { namespace collection { namespace lb {

struct LBTreeUpMsg : HierLBTypes, ::vt::Message {
  using LoadType = double;

  LBTreeUpMsg() = default;
  LBTreeUpMsg(
    LoadType const in_child_load, NodeType const in_child,
    ObjSampleType in_load, NodeType const in_child_size
  ) : child_load_(in_child_load), child_(in_child), load_(in_load),
      child_size_(in_child_size)
  { }


  #if hierlb_use_parserdes
    template <typename SerializerT>
    void parserdes(SerializerT& s) {
      s & load_;
    }
  #else
    template <typename SerializerT>
    void serialize(SerializerT& s) {
      s | child_load_ | child_ | load_ | child_size_;
    }
  #endif

  LoadType getChildLoad() const { return child_load_; }
  NodeType getChild() const { return child_; }
  ObjSampleType const& getLoad() const { return load_; }
  ObjSampleType&& getLoadMove() { return std::move(load_); }
  NodeType getChildSize() const { return child_size_; }

private:
  LoadType child_load_ = 0.0f;
  NodeType child_ = uninitialized_destination;
  ObjSampleType load_;
  NodeType child_size_ = 0;
};

struct LBTreeDownMsg : HierLBTypes, ::vt::Message {
  using LoadType = double;

  LBTreeDownMsg() = default;
  LBTreeDownMsg(
    NodeType const in_from, ObjSampleType in_excess, bool const in_final_child
  ) : from_(in_from), excess_(in_excess), final_child_(in_final_child)
  { }


  #if hierlb_use_parserdes
    template <typename SerializerT>
    void parserdes(SerializerT& s) {
      s & excess_;
    }
  #else
    template <typename SerializerT>
    void serialize(SerializerT& s) {
      s | from_ | excess_ | final_child_;
    }
  #endif

  NodeType getFrom() const { return from_; }
  ObjSampleType const& getExcess() const { return excess_; }
  ObjSampleType&& getExcessMove() { return std::move(excess_); }
  bool getFinalChild() const { return final_child_; }

private:
  NodeType from_ = uninitialized_destination;
  ObjSampleType excess_;
  bool final_child_ = 0;
};

struct TransferMsg : HierLBTypes, ::vt::Message {
  using LoadType = double;

  TransferMsg() = default;
  TransferMsg(
    NodeType const in_from, std::vector<ObjIDType> const& in_transfer
  ) : from_(in_from), transfer_(in_transfer)
  { }

  #if hierlb_use_parserdes
    template <typename SerializerT>
    void parserdes(SerializerT& s) {
      s & transfer_;
    }
  #else
    template <typename SerializerT>
    void serialize(SerializerT& s) {
      s | from_ | transfer_;
    }
  #endif

  NodeType getFrom() const { return from_; }
  std::vector<ObjIDType> const& getTransfer() const { return transfer_; }
  std::vector<ObjIDType>&& getTransferMove() { return std::move(transfer_); }

private:
  NodeType from_ = uninitialized_destination;
  std::vector<ObjIDType> transfer_;
};

}}}} /* end namespace vt::vrt::collection::lb */

#endif /*INCLUDED_VRT_COLLECTION_BALANCE_HIERARCHICALLB_HIERLB_MSGS_H*/
