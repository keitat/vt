
#if !defined INCLUDED_PIPE_PIPE_MANAGER_TL_H
#define INCLUDED_PIPE_PIPE_MANAGER_TL_H

#include "config.h"
#include "pipe/pipe_common.h"
#include "pipe/pipe_manager_base.h"
#include "pipe/callback/cb_union/cb_raw_base.h"
#include "activefn/activefn.h"

namespace vt { namespace pipe {

struct PipeManagerTL : virtual PipeManagerBase {
  using CallbackType = callback::cbunion::CallbackRawBaseSingle;

  /*
   *  Untyped variants of callbacks: uses union to dispatch
   */

  // Single active message function-handler
  template <typename MsgT, ActiveTypedFnType<MsgT>* f>
  CallbackType makeCallbackSingleSend(NodeType const& node);

  template <typename MsgT, ActiveTypedFnType<MsgT>* f>
  CallbackType makeCallbackSingleBcast(bool const& inc);

  // Single active message functor-handler
  template <typename FunctorT>
  CallbackType makeCallbackFunctorSend(NodeType const& node);

  template <typename FunctorT>
  CallbackType makeCallbackFunctorBcast(bool const& inc);

  // Single active message functor-handler void param
  template <typename FunctorT>
  CallbackType makeCallbackFunctorSendVoid(NodeType const& node);

  template <typename FunctorT>
  CallbackType makeCallbackFunctorBcastVoid(bool const& inc);

  // Single active message anon func-handler
  template <typename=void>
  CallbackType makeCallbackSingleAnonVoid(FuncVoidType fn);

  template <typename MsgT>
  CallbackType makeCallbackSingleAnon(FuncMsgType<MsgT> fn);

  // Multi-staged callback
  template <typename=void>
  CallbackType makeCallback();

  template <typename MsgT, ActiveTypedFnType<MsgT>* f>
  void addListener(CallbackType const& cb, NodeType const& node);

  template <typename MsgT, ActiveTypedFnType<MsgT>* f>
  void addListenerBcast(CallbackType const& cb, bool const& inc);

  template <typename FunctorT>
  void addListenerFunctor(CallbackType const& cb, NodeType const& node);

  template <typename FunctorT>
  void addListenerFunctorVoid(CallbackType const& cb, NodeType const& node);

  template <typename FunctorT>
  void addListenerFunctorBcast(CallbackType const& cb, bool const& inc);
};

}} /* end namespace vt::pipe */

#endif /*INCLUDED_PIPE_PIPE_MANAGER_TL_H*/
