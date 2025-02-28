/*
//@HEADER
// *****************************************************************************
//
//                             envelope_ref.impl.h
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

#if !defined INCLUDED_VT_MESSAGING_ENVELOPE_ENVELOPE_REF_IMPL_H
#define INCLUDED_VT_MESSAGING_ENVELOPE_ENVELOPE_REF_IMPL_H

#include "vt/config.h"
#include "vt/messaging/envelope/envelope_ref.h"

namespace vt {

template <typename Env>
inline void envelopeRef(Env& env) {
  Envelope* envp = reinterpret_cast<Envelope*>(&env);

  vtAssert(
    envp->ref != not_shared_message,
    "'Not shared message' encountered on message ref-increment. "
    "This is can be caused by explicitly using 'new Message(..)' "
    "instead of a MsgPtr/makeMessage/makeSharedMessage construct."
  );
  vtAssertInfo(
    envp->ref >= 0 and envp->ref < 100,
    "Bad ref-count on message ref-increment. "
    "Message ref-count must never be negative and cannnot exceed limit (100).",
    static_cast<RefType>(envp->ref)
  );

  envp->ref++;
}

template <typename Env>
inline RefType envelopeDeref(Env& env) {
  Envelope* envp = reinterpret_cast<Envelope*>(&env);

  vtAssertInfo(
    envp->ref >= 1,
    "Bad ref-count on message ref-decrement.",
    static_cast<RefType>(envp->ref)
  );

  return --(envp->ref);
}

} /* end namespace vt */

#endif /*INCLUDED_VT_MESSAGING_ENVELOPE_ENVELOPE_REF_IMPL_H*/
