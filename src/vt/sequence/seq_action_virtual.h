/*
//@HEADER
// *****************************************************************************
//
//                             seq_action_virtual.h
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

#if !defined INCLUDED_VT_SEQUENCE_SEQ_ACTION_VIRTUAL_H
#define INCLUDED_VT_SEQUENCE_SEQ_ACTION_VIRTUAL_H

#include "vt/config.h"
#include "vt/sequence/seq_common.h"
#include "vt/termination/termination.h"

namespace vt { namespace seq {

template <typename MessageT, typename VcT>
struct ActionVirtual {
  using ActionType = std::function<void(MessageT*, VcT*)>;
  using CallableType = std::function<bool()>;

  SeqType const seq_id;
  ActionType const action;

  ActionVirtual(SeqType const& in_seq_id, ActionType const& in_action);

  void runAction(VcT* vc, MessageT* msg, bool const consume = true) const;
  CallableType generateCallable(MessageT* msg, VcT* vc) const;
};

}} //end namespace vt::seq

#include "vt/sequence/seq_action_virtual.impl.h"

#endif /* INCLUDED_VT_SEQUENCE_SEQ_ACTION_VIRTUAL_H*/
