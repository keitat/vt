/*
//@HEADER
// ************************************************************************
//
//                          system_create.h
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

#if !defined INCLUDED_VRT_COLLECTION_MESSAGES_SYSTEM_CREATE_H
#define INCLUDED_VRT_COLLECTION_MESSAGES_SYSTEM_CREATE_H

#include "vt/config.h"
#include "vt/vrt/vrt_common.h"
#include "vt/vrt/collection/proxy_builder/elm_proxy_builder.h"
#include "vt/vrt/collection/types/headers.h"
#include "vt/messaging/message.h"
#include "vt/serialization/serialization.h"
#include "vt/collective/reduce/reduce.h"
#include "vt/vrt/proxy/collection_proxy.h"

namespace vt { namespace vrt { namespace collection {

template <
  typename RemoteInfo, typename ArgsTuple, typename CollectionT, typename IndexT
>
struct CollectionCreateMsg : ::vt::Message {
  using CollectionType = CollectionT;
  using IndexType = IndexT;
  using ArgsTupleType = ArgsTuple;

  RemoteInfo info;
  ArgsTuple tup;
  HandlerType map;

  CollectionCreateMsg() = default;
  CollectionCreateMsg(
    HandlerType const& in_han, ArgsTuple&& in_tup
  ) : ::vt::Message(), tup(std::forward<ArgsTuple>(in_tup)), map(in_han)
  { }

  template <typename SerializerT>
  void serialize(SerializerT& s) {
    s | info | tup | map;
  }
};

struct CollectionConsMsg : ::vt::collective::reduce::ReduceMsg {
  CollectionConsMsg() = default;
  explicit CollectionConsMsg(VirtualProxyType const& in_proxy)
    : proxy(in_proxy)
  { }

  VirtualProxyType getProxy() const { return proxy; }

  VirtualProxyType proxy = {};
};

// message type for collection view distributed construction.
template <typename ColT, typename IndexT, typename IndexU>
struct ViewCreateMsg : ::vt::collective::reduce::ReduceMsg {

  using CollectionType = ColT;
  using IndexOld = IndexT;
  using IndexNew = IndexU;

  VirtualProxyType old_proxy_    = no_vrt_proxy;
  VirtualProxyType new_proxy_    = no_vrt_proxy;
  IndexOld         old_range_    = {};
  IndexNew         new_range_    = {};
  HandlerType      old_view_han_ = uninitialized_handler;
  HandlerType      new_view_han_ = uninitialized_handler;
  HandlerType      col_map_id_   = uninitialized_handler;
  EpochType        epoch_        = no_epoch;
  TagType          tag_          = no_tag;

  ViewCreateMsg() = default;

  ViewCreateMsg(
    VirtualProxyType const& in_old_proxy,
    VirtualProxyType const& in_new_proxy,
    IndexOld         const& in_old_range,
    IndexNew         const& in_new_range,
    HandlerType      const& in_old_view_han,
    HandlerType      const& in_new_view_han,
    HandlerType      const& in_col_map,
    EpochType        const& in_epoch,
    TagType          const& in_tag
  ) : old_proxy_   (in_old_proxy),
      new_proxy_   (in_new_proxy),
      old_range_   (in_old_range),
      new_range_   (in_new_range),
      old_view_han_(in_old_view_han),
      new_view_han_(in_new_view_han),
      col_map_id_  (in_col_map),
      epoch_       (in_epoch),
      tag_         (in_tag)
  {}

  ~ViewCreateMsg() = default;

  template <typename SerializerT>
  void serialize(SerializerT& s) {
    s | old_proxy_ | new_proxy_
      | old_range_ | new_range_
      | old_view_han_ | new_view_han_ | col_map_id_;
  }
};

struct CollectionGroupMsg : CollectionConsMsg {
  CollectionGroupMsg() = default;
  CollectionGroupMsg(
    VirtualProxyType const& in_proxy, GroupType const& in_group
  ) : CollectionConsMsg(in_proxy), group_(in_group)
  { }

  GroupType getGroup() const { return group_; }

private:
  GroupType group_ = no_group;
};

struct ViewGroupMsg : ::vt::collective::reduce::ReduceMsg {

public:
  ViewGroupMsg() = delete;

  ViewGroupMsg(
    VirtualProxyType const& in_proxy, GroupType const& in_group = no_group
  )
    : ::vt::collective::reduce::ReduceMsg(),
      proxy_(in_proxy),
      group_(in_group)
  {}

  VirtualProxyType const& getProxy() const { return proxy_; }
  GroupType const& getGroup() const { return group_; }

private:
  GroupType group_ = no_group;
  VirtualProxyType proxy_ = no_vrt_proxy;
};

struct FinishedUpdateMsg : ::vt::collective::reduce::ReduceMsg {
  FinishedUpdateMsg() = default;
  explicit FinishedUpdateMsg(VirtualProxyType const& in_proxy)
    : proxy_(in_proxy)
  { }

  VirtualProxyType proxy_ = {};
};

struct CollectionPhaseMsg : ::vt::Message {};

template <typename ColT, typename IndexT>
struct InsertMsg : ::vt::Message {
  InsertMsg() = default;

  InsertMsg(
    CollectionProxy<ColT,IndexT> in_proxy,
    IndexT in_max, IndexT in_idx, NodeType in_construct_node,
    NodeType in_home_node, EpochType in_epoch, EpochType in_g_epoch
  ) : proxy_(in_proxy), max_(in_max), idx_(in_idx),
      construct_node_(in_construct_node), home_node_(in_home_node),
      epoch_(in_epoch), g_epoch_(in_g_epoch)
  { }

  CollectionProxy<ColT,IndexT> proxy_ = {};
  IndexT max_ = {}, idx_ = {};
  NodeType construct_node_ = uninitialized_destination;
  NodeType home_node_ = uninitialized_destination;
  EpochType epoch_ = no_epoch;
  EpochType g_epoch_ = no_epoch;
};

template <typename ColT, typename IndexT>
struct DoneInsertMsg : ::vt::Message {
  DoneInsertMsg() = default;

  DoneInsertMsg(
    CollectionProxy<ColT,IndexT> in_proxy,
    NodeType const& in_action_node = uninitialized_destination
  ) : action_node_(in_action_node), proxy_(in_proxy)
  { }

  NodeType action_node_ = uninitialized_destination;
  CollectionProxy<ColT,IndexT> proxy_ = {};
};

template <typename ColT, typename IndexT>
struct ActInsertMsg : ::vt::Message {
  ActInsertMsg() = default;

  explicit ActInsertMsg(CollectionProxy<ColT,IndexT> in_proxy)
    : proxy_(in_proxy)
  { }

  CollectionProxy<ColT,IndexT> proxy_ = {};
};

template <typename ColT, typename IndexT>
struct UpdateInsertMsg : ::vt::Message {
  UpdateInsertMsg() = default;

  UpdateInsertMsg(
    CollectionProxy<ColT,IndexT> in_proxy, EpochType const& in_epoch
  ) : proxy_(in_proxy), epoch_(in_epoch)
  { }

  CollectionProxy<ColT,IndexT> proxy_ = {};
  EpochType epoch_ = no_epoch;
};

}}} /* end namespace vt::vrt::collection */

#endif /*INCLUDED_VRT_COLLECTION_MESSAGES_SYSTEM_CREATE_H*/
