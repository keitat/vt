/*
//@HEADER
// ************************************************************************
//
//                          callback_proxy_bcast_tl.h
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

#if !defined INCLUDED_PIPE_CALLBACK_PROXY_BCAST_CALLBACK_PROXY_BCAST_TL_H
#define INCLUDED_PIPE_CALLBACK_PROXY_BCAST_CALLBACK_PROXY_BCAST_TL_H

#include "vt/config.h"
#include "vt/pipe/pipe_common.h"
#include "vt/pipe/callback/callback_base_tl.h"
#include "vt/registry/auto/auto_registry_common.h"

namespace vt { namespace pipe { namespace callback {

struct CallbackProxyBcastTypeless : CallbackBaseTL<CallbackProxyBcastTypeless> {
  CallbackProxyBcastTypeless() = default;

  template <typename SerializerT>
  void serialize(SerializerT& s);

  bool operator==(CallbackProxyBcastTypeless const& other) const {
    return true;
  }

public:
  template <typename MsgT>
  void trigger(MsgT* msg, PipeType const& pipe);

  void triggerVoid(PipeType const& pipe) {
    vtAssert(0, "Must not be void");
  }
};

struct CallbackProxyBcastDirect : CallbackBaseTL<CallbackProxyBcastDirect> {
  using AutoHandlerType = auto_registry::AutoHandlerType;

  CallbackProxyBcastDirect() = default;
  CallbackProxyBcastDirect(
    HandlerType const& in_han, AutoHandlerType const& in_vrt,
    bool const& in_member, VirtualProxyType const& in_proxy
  ) : vrt_dispatch_han_(in_vrt), handler_(in_han), proxy_(in_proxy),
      member_(in_member)
  { }

  template <typename SerializerT>
  void serialize(SerializerT& s);

  bool operator==(CallbackProxyBcastDirect const& other) const {
    return
      other.handler_ == handler_ &&
      other.vrt_dispatch_han_ == vrt_dispatch_han_ &&
      other.proxy_ == proxy_ &&
      other.member_ == member_;
  }

public:
  template <typename MsgT>
  void trigger(MsgT* msg, PipeType const& pipe);

  void triggerVoid(PipeType const& pipe) {
    vtAssert(0, "Must not be void");
  }

private:
  AutoHandlerType vrt_dispatch_han_ = uninitialized_handler;
  HandlerType handler_              = uninitialized_handler;
  VirtualProxyType proxy_           = no_vrt_proxy;
  bool member_                      = false;
};

}}} /* end namespace vt::pipe::callback */

#endif /*INCLUDED_PIPE_CALLBACK_PROXY_BCAST_CALLBACK_PROXY_BCAST_TL_H*/
