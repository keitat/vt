/*
//@HEADER
// *****************************************************************************
//
//                                  msg_rdma.h
//                           DARMA Toolkit v. 1.0.0
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

#if !defined INCLUDED_VT_MESSAGING_RDMA_MSG_RDMA_H
#define INCLUDED_VT_MESSAGING_RDMA_MSG_RDMA_H

#include "vt/config.h"
#include "vt/runtime/component/component_pack.h"
#include "vt/objgroup/proxy/proxy_objgroup.h"
#include "vt/rdmahandle/handle.h"

namespace vt { namespace messaging { namespace rdma {

struct MsgRDMA : runtime::component::PollableComponent<MsgRDMA> {

  MsgRDMA();

  void startup() override;

  std::string name() override { return "MsgRDMA"; }

  std::size_t writeBytesForGet(char* ptr, std::size_t len);

  void getBytes(NodeType get_node, char* ptr, int offset, std::size_t len);

  int progress() override;

private:
  void setProxy(objgroup::proxy::Proxy<MsgRDMA> in_proxy) {
    proxy_ = in_proxy;
  }

public:
  static std::unique_ptr<MsgRDMA> construct();

private:
  objgroup::proxy::Proxy<MsgRDMA> proxy_;
  std::size_t current_size_ = 0;
  int cur_offset_ = 0;
  vt::rdma::Handle<char> handle_;
  std::list<vt::rdma::RequestHolder> reqs_;
};

}}} /* end namespace vt::messaging::rdma */

#endif /*INCLUDED_VT_MESSAGING_RDMA_MSG_RDMA_H*/
