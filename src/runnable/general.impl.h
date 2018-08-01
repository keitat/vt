
#if !defined INCLUDED_RUNNABLE_GENERAL_IMPL_H
#define INCLUDED_RUNNABLE_GENERAL_IMPL_H

#include "config.h"
#include "runnable/general.h"
#include "registry/auto/auto_registry_common.h"
#include "registry/auto/auto_registry_interface.h"
#include "registry/auto/auto_registry_general.h"
#include "trace/trace_common.h"
#include "messaging/envelope.h"
#include "handler/handler.h"

#include <cassert>

namespace vt { namespace runnable {

template <typename MsgT>
/*static*/ void Runnable<MsgT>::run(
  HandlerType handler, ActiveClosureFnType func, MsgT* msg, NodeType from_node
) {
  using HandlerManagerType = HandlerManager;

  #if backend_check_enabled(trace_enabled)
    trace::TraceEntryIDType trace_id = auto_registry::theTraceID(
      handler, auto_registry::RegistryTypeEnum::RegGeneral
    );
    trace::TraceEventIDType trace_event = envelopeGetTraceEvent(msg->env);
  #endif

  #if backend_check_enabled(trace_enabled)
    theTrace()->beginProcessing(trace_id, sizeof(*msg), trace_event, from_node);
  #endif

  if (func == nullptr) {
    bool const& is_auto = HandlerManagerType::isHandlerAuto(handler);
    bool const& is_functor = HandlerManagerType::isHandlerFunctor(handler);

    if (is_auto && is_functor) {
      func = auto_registry::getAutoHandlerFunctor(handler);
    } else if (is_auto) {
      func = auto_registry::getAutoHandler(handler);
    } else {
      assert(0 && "Must be auto handler");
    }
  }

  func(msg);

  #if backend_check_enabled(trace_enabled)
    theTrace()->endProcessing(trace_id, sizeof(*msg), trace_event, from_node);
  #endif
}

}} /* end namespace vt::runnable */

#endif /*INCLUDED_RUNNABLE_GENERAL_IMPL_H*/
