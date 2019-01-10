/*
//@HEADER
// ************************************************************************
//
//                          proxy_bits.h
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

#if !defined INCLUDED_CONTEXT_VRT_PROXY
#define INCLUDED_CONTEXT_VRT_PROXY

#include "vt/config.h"
#include "vt/vrt/context/context_vrt_fwd.h"

namespace vt { namespace vrt {

static constexpr BitCountType const virtual_is_collection_num_bits = 1;
static constexpr BitCountType const virtual_is_migratable_num_bits = 1;
static constexpr BitCountType const virtual_is_remote_num_bits = 1;
static constexpr BitCountType const virtual_node_num_bits =
    BitCounterType<NodeType>::value;
static constexpr BitCountType const virtual_id_num_bits =
    BitCounterType<VirtualIDType>::value;
static constexpr BitCountType const virtual_remote_id_num_bits =
    BitCounterType<VirtualRemoteIDType>::value;

enum eVirtualProxyBits {
  Collection = 0,
  Migratable = eVirtualProxyBits::Collection + virtual_is_collection_num_bits,
  Remote     = eVirtualProxyBits::Migratable + virtual_is_migratable_num_bits,
  Node       = eVirtualProxyBits::Remote     + virtual_is_remote_num_bits,
  ID         = eVirtualProxyBits::Node       + virtual_node_num_bits
};

enum eVirtualProxyRemoteBits {
  // The prelude is the same as eVirtualProxyBits. Starting with the ID field it
  // is different
  RemoteNode = eVirtualProxyBits::Node             + virtual_node_num_bits,
  RemoteID   = eVirtualProxyRemoteBits::RemoteNode + virtual_node_num_bits
};

struct VirtualProxyBuilder {
  static VirtualProxyType createProxy(
    VirtualIDType       const& id,
    NodeType            const& node,
    bool                const& is_coll        = false,
    bool                const& is_migratable  = false,
    bool                const& is_distributed = false
  );
  static VirtualProxyType createRemoteProxy(
    VirtualRemoteIDType const& id,
    NodeType            const& this_node,
    NodeType            const& target_node,
    bool                const& is_coll,
    bool                const& is_migratable
  );

  static void setIsCollection(VirtualProxyType& proxy, bool const& is_coll);
  static void setIsMigratable(VirtualProxyType& proxy, bool const& is_migratable);
  static void setIsRemote(VirtualProxyType& proxy, bool const& is_remote);
  static void setVirtualNode(VirtualProxyType& proxy, NodeType const& node);
  static void setVirtualID(VirtualProxyType& proxy, VirtualIDType const& id);
  static void setVirtualRemoteNode(VirtualProxyType& proxy, NodeType const& node);
  static void setVirtualRemoteID(VirtualProxyType& proxy, VirtualRemoteIDType const& id);
  static bool isCollection(VirtualProxyType const& proxy);
  static bool isMigratable(VirtualProxyType const& proxy);
  static bool isRemote(VirtualProxyType const& proxy);
  static NodeType getVirtualNode(VirtualProxyType const& proxy);
  static VirtualIDType getVirtualID(VirtualProxyType const& proxy);
};

}}  // end namespace vt::vrt

#endif  /*INCLUDED_CONTEXT_VRT_PROXY*/
