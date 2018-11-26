/*
//@HEADER
// ************************************************************************
//
//                          pipe_id.h
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

#if !defined INCLUDED_PIPE_ID_PIPE_ID_H
#define INCLUDED_PIPE_ID_PIPE_ID_H

#include "vt/config.h"
#include "vt/pipe/pipe_common.h"

namespace vt { namespace pipe {

static constexpr BitCountType const pipe_send_back_num_bits = 1;
static constexpr BitCountType const pipe_persist_num_bits = 1;
static constexpr BitCountType const pipe_node_num_bits =
    BitCounterType<NodeType>::value;
static constexpr BitCountType const pipe_id_num_bits =
    BitCounterType<PipeIDType>::value;

enum ePipeIDBits {
  Sendback   = 0,
  Persist    = ePipeIDBits::Sendback + pipe_send_back_num_bits,
  Node       = ePipeIDBits::Persist  + pipe_persist_num_bits,
  ID         = ePipeIDBits::Node     + pipe_node_num_bits
};

struct PipeIDBuilder {
  static PipeType createPipeID(
    PipeIDType const& id, NodeType const& node,
    bool const& is_send_back = false, bool const& is_persist = true
  );

  static void setIsSendback(PipeType& pipe, bool const& is_send_back);
  static void setIsPersist(PipeType& pipe, bool const& is_persist);
  static void setNode(PipeType& pipe, NodeType const& node);
  static void setID(PipeType& pipe, PipeIDType const& id);
  static bool isSendback(PipeType const& pipe);
  static bool isPersist(PipeType const& pipe);
  static NodeType getNode(PipeType const& pipe);
  static PipeIDType getPipeID(PipeType const& pipe);
};

}} /* end namespace vt::pipe */

#endif /*INCLUDED_PIPE_ID_PIPE_ID_H*/
