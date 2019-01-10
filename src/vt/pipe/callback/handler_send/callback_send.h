/*
//@HEADER
// ************************************************************************
//
//                          callback_send.h
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

#if !defined INCLUDED_PIPE_CALLBACK_HANDLER_SEND_CALLBACK_SEND_H
#define INCLUDED_PIPE_CALLBACK_HANDLER_SEND_CALLBACK_SEND_H

#include "vt/config.h"
#include "vt/pipe/pipe_common.h"
#include "vt/pipe/signal/signal.h"
#include "vt/pipe/callback/callback_base.h"
#include "vt/activefn/activefn.h"
#include "vt/context/context.h"
#include "vt/messaging/active.h"
#include "vt/messaging/envelope.h"
#include "vt/runnable/general.h"

#include <type_traits>

namespace vt { namespace pipe { namespace callback {

template <typename MsgT>
struct CallbackSend : CallbackBase<signal::Signal<MsgT>> {
  using SignalBaseType = typename signal::Signal<MsgT>;
  using SignalType     = typename CallbackBase<SignalBaseType>::SignalType;
  using SignalDataType = typename SignalType::DataType;
  using MessageType    = MsgT;
  using VoidSigType    = signal::SigVoidType;
  template <typename T, typename U=void>
  using IsVoidType     = std::enable_if_t<std::is_same<T,VoidSigType>::value,U>;
  template <typename T, typename U=void>
  using IsNotVoidType  = std::enable_if_t<!std::is_same<T,VoidSigType>::value,U>;

  CallbackSend() = default;
  CallbackSend(CallbackSend const&) = default;
  CallbackSend(CallbackSend&&) = default;

  CallbackSend(
    HandlerType const& in_handler, NodeType const& in_send_node
  );

  HandlerType getHandler() const { return handler_; }

  template <typename SerializerT>
  void serialize(SerializerT& s);

private:
  template <typename T>
  IsVoidType<T> triggerDispatch(SignalDataType* data, PipeType const& pid);

  template <typename T>
  IsNotVoidType<T> triggerDispatch(SignalDataType* data, PipeType const& pid);

  void trigger_(SignalDataType* data, PipeType const& pid) override;
  void trigger_(SignalDataType* data) override;

private:
  NodeType send_node_  = uninitialized_destination;
  HandlerType handler_ = uninitialized_handler;
};

}}} /* end namespace vt::pipe::callback */

#endif /*INCLUDED_PIPE_CALLBACK_HANDLER_SEND_CALLBACK_SEND_H*/
