
#if !defined INCLUDED_PIPE_CALLBACK_CB_UNION_CB_RAW_BASE_IMPL_H
#define INCLUDED_PIPE_CALLBACK_CB_UNION_CB_RAW_BASE_IMPL_H

#include "config.h"
#include "pipe/callback/cb_union/cb_raw.h"
#include "pipe/callback/cb_union/cb_raw_base.h"

#include <cassert>

namespace vt { namespace pipe { namespace callback { namespace cbunion {

template <typename MsgT>
void CallbackRawBaseSingle::send(MsgT* msg) {
  switch (cb_.active_) {
  case CallbackEnum::SendMsgCB:
    cb_.u_.send_msg_cb_.trigger<MsgT>(msg);
    break;
  case CallbackEnum::BcastMsgCB:
    cb_.u_.bcast_msg_cb_.trigger<MsgT>(msg);
    break;
  default:
    assert(0 && "Should not be reachable");
  }
}

template <typename SerializerT>
void CallbackRawBaseSingle::serialize(SerializerT& s) {
  s | cb_ | pipe_;
}


}}}} /* end namespace vt::pipe::callback::cbunion */

#endif /*INCLUDED_PIPE_CALLBACK_CB_UNION_CB_RAW_BASE_IMPL_H*/
