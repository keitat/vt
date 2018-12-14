
#if !defined INCLUDED_COLLECTIVE_REDUCE_OPERATORS_DEFAULT_OP_H
#define INCLUDED_COLLECTIVE_REDUCE_OPERATORS_DEFAULT_OP_H

#include "vt/config.h"
#include "vt/collective/reduce/reduce_msg.h"
#include "vt/collective/reduce/operators/default_msg.h"
#include "vt/collective/reduce/operators/functors/none_op.h"
#include "vt/collective/reduce/operators/functors/and_op.h"
#include "vt/collective/reduce/operators/functors/or_op.h"
#include "vt/collective/reduce/operators/functors/plus_op.h"
#include "vt/collective/reduce/operators/functors/max_op.h"
#include "vt/collective/reduce/operators/functors/min_op.h"
#include "vt/collective/reduce/operators/functors/bit_and_op.h"
#include "vt/collective/reduce/operators/functors/bit_or_op.h"
#include "vt/collective/reduce/operators/functors/bit_xor_op.h"

#include <algorithm>

namespace vt { namespace collective { namespace reduce { namespace operators {

template <typename T = void>
struct ReduceCallback {
  void operator()(T* t) const { /* do nothing */ }
};

template <typename T = void>
struct ReduceCombine {
  ReduceCombine() = default;
private:
  template <typename MsgT, typename Op, typename ActOp>
  static void combine(MsgT* m1, MsgT* m2) {
    Op()(m1->getVal(), m2->getConstVal());
  }
public:
  template <typename MsgT, typename Op, typename ActOp>
  static void msgHandler(MsgT* msg) {
    if (msg->isRoot()) {
      auto cb = msg->getCallback();
      debug_print(
        reduce, node,
        "ROOT: reduce root: valid={}, ptr={}\n", cb.valid(), print_ptr(msg)
      );
      if (cb.valid()) {
        cb.template send<MsgT>(msg);
      } else {
        ActOp()(msg);
      }
    } else {
      MsgT* fst_msg = msg;
      MsgT* cur_msg = msg->template getNext<MsgT>();
      debug_print(
        reduce, node,
        "leaf: fst valid={}, ptr={}\n", fst_msg->getCallback().valid(),
        print_ptr(fst_msg)
      );
      while (cur_msg != nullptr) {
        ReduceCombine<>::combine<MsgT,Op,ActOp>(fst_msg, cur_msg);
        cur_msg = cur_msg->template getNext<MsgT>();
      }
    }
  }
};

}}}} /* end namespace vt::collective::reduce::operators */

#endif /*INCLUDED_COLLECTIVE_REDUCE_OPERATORS_DEFAULT_OP_H*/
