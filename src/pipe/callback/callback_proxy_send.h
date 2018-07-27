
#if !defined INCLUDED_PIPE_CALLBACK_CALLBACK_PROXY_SEND_H
#define INCLUDED_PIPE_CALLBACK_CALLBACK_PROXY_SEND_H

#include "config.h"
#include "pipe/pipe_common.h"
#include "pipe/signal/signal.h"
#include "pipe/callback/callback_base.h"
#include "vrt/collection/active/active_funcs.h"
#include "vrt/proxy/collection_elm_proxy.h"

#include <cassert>

namespace vt { namespace pipe { namespace callback {

template <
  typename ColT,
  typename MsgT,
  vrt::collection::ActiveColTypedFnType<MsgT,ColT>* f
>
struct CallbackProxySend : CallbackBase<signal::Signal<MsgT>> {
  using SignalBaseType   = typename signal::Signal<MsgT>;
  using SignalType       = typename CallbackBase<SignalBaseType>::SignalType;
  using SignalDataType   = typename SignalType::DataType;
  using IndexedProxyType = typename ColT::ProxyType;
  using ProxyType        = typename ColT::CollectionProxyType;
  using IndexType        = typename ColT::IndexType;
  using MessageType      = MsgT;

  explicit CallbackProxySend(IndexedProxyType const& in_proxy)
    : proxy_(in_proxy.getCollectionProxy()),
      idx_(in_proxy.getElementProxy().getIndex())
  { }

  CallbackProxySend(ProxyType const& in_proxy, IndexType const& in_idx)
    : proxy_(in_proxy), idx_(in_idx)
  { }

  template <typename SerializerT>
  void serialize(SerializerT& s) {
    CallbackBase<SignalBaseType>::serializer(s);
    s | proxy_ | idx_;
  }

private:
  void trigger_(SignalDataType* data) override {
    proxy_[idx_].template send<ColT,MsgT,f>(data);
  }

private:
  ProxyType proxy_ = {};
  IndexType idx_   = {};
};

}}} /* end namespace vt::pipe::callback */

#endif /*INCLUDED_PIPE_CALLBACK_CALLBACK_PROXY_SEND_H*/
