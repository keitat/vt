/*
//@HEADER
// ************************************************************************
//
//                          rdma_handle.h
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

#if !defined INCLUDED_RDMA_RDMA_HANDLE_H
#define INCLUDED_RDMA_RDMA_HANDLE_H

#include "vt/config.h"
#include "vt/rdma/rdma_common.h"

namespace vt { namespace rdma {

static_assert(
  sizeof(RDMA_HandleType) == sizeof(RDMA_HandlerType),
  "RDMA Handle, RDMA Handler IDs must be the same size"
);

struct HandleManager {
  using RDMA_BitsType = Bits;
  using RDMA_TypeType = Type;
  using RDMA_UniversalIdType = RDMA_HandleType;

  static void setIsSized(RDMA_UniversalIdType& handle, bool const& is_sized);
  static void setIsCollective(RDMA_UniversalIdType& handle, bool const& is_collective);
  static void setIsHandler(
    RDMA_UniversalIdType& handle, bool const& is_handler
  );
  static void setOpType(
    RDMA_UniversalIdType& handle, RDMA_TypeType const& rdma_type
  );
  static void setRdmaNode(RDMA_UniversalIdType& handle, NodeType const& node);
  static void setRdmaIdentifier(
    RDMA_UniversalIdType& handle, RDMA_IdentifierType const& ident
  );
  static NodeType getRdmaNode(RDMA_UniversalIdType const& handle);
  static RDMA_IdentifierType getRdmaIdentifier(RDMA_UniversalIdType const& handle);
  static bool isSized(RDMA_UniversalIdType const& handle);
  static bool isCollective(RDMA_UniversalIdType const& handle);
  static bool isHandler(RDMA_UniversalIdType const& handle);
  static RDMA_TypeType getOpType(RDMA_UniversalIdType const& handle);
  static RDMA_UniversalIdType createNewHandler();
};

using RDMA_HandleManagerType = HandleManager;

}} //end namespace vt::rdma

#endif /*INCLUDED_RDMA_RDMA_HANDLE_H*/
