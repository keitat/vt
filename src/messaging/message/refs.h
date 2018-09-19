
#if !defined INCLUDED_MESSAGING_MESSAGE_REFS_H
#define INCLUDED_MESSAGING_MESSAGE_REFS_H

#include "config.h"
#include "messaging/envelope.h"
#include "context/context.h"

namespace vt {

template <template <typename> class MsgPtrT, typename MsgT>
void messageRef(MsgPtrT<MsgT> msg);

template <typename MessageT>
void messageRef(MessageT* msg);

template <template <typename> class MsgPtrT, typename MsgT>
void messageDeref(MsgPtrT<MsgT> msg);

template <typename MessageT>
void messageDeref(MessageT* msg);

template <typename MessageT>
bool isSharedMessage(MessageT* msg);

} /* end namespace vt */

#include "messaging/message/refs.impl.h"

#endif /*INCLUDED_MESSAGING_MESSAGE_REFS_H*/
