
#if ! defined __RUNTIME_TRANSPORT_RDMA_TYPES__
#define __RUNTIME_TRANSPORT_RDMA_TYPES__

#include "common.h"

#include <sstream>
#include <iostream>

namespace runtime { namespace rdma {

using UnderlyingNodeType = NodeType;

struct Endpoint {
  Endpoint(
    bool const& in_is_target, UnderlyingNodeType const& in_node
  ) : is_target(in_is_target), value(in_node)
  { }

  operator UnderlyingNodeType() const { return get(); }

  UnderlyingNodeType get() const { return value; }

  bool target() const { return is_target; }

private:
  bool is_target = false;
  UnderlyingNodeType value = uninitialized_destination;
};

struct Target : Endpoint {
  explicit Target(UnderlyingNodeType const& in_node)
    : Endpoint(true, in_node)
  { }
};

struct NonTarget : Endpoint {
  explicit NonTarget(UnderlyingNodeType const& in_node)
    : Endpoint(false, in_node)
  { }
};

}} //end namespace runtime::rdma

namespace runtime {

using RDMA_TargetType = rdma::Target;
using RDMA_NonTargetType = rdma::NonTarget;

struct from_s {
  RDMA_TargetType operator=(rdma::UnderlyingNodeType val) {
    return RDMA_TargetType(val);
  }
};

struct to_s {
  RDMA_NonTargetType operator=(rdma::UnderlyingNodeType val) {
    return RDMA_NonTargetType(val);
  }
};

extern from_s rdma_from;
extern to_s rdma_to;

} //end namespace runtime

#endif /*__RUNTIME_TRANSPORT_RDMA_TYPES__*/
