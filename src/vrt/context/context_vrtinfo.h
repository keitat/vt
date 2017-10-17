
#if !defined INCLUDED_CONTEXT_VRT_INFO
#define INCLUDED_CONTEXT_VRT_INFO

#include <memory>
#include <cassert>

#include "config.h"
#include "context_vrt.h"

namespace vt { namespace vrt {

struct VirtualInfo {
  using VirtualPtrType = std::unique_ptr<VirtualContext>;

  VirtualInfo(VirtualPtrType in_vrt_ptr, VirtualProxyType const& proxy_in)
    : proxy_(proxy_in), vrt_ptr_(std::move(in_vrt_ptr)) {}

  VirtualInfo(VirtualInfo&&) = default;
  VirtualInfo(VirtualInfo const&) = delete;

  VirtualContext *get() const {
    assert(vrt_ptr_ != nullptr and "Must have a valid context");
    return vrt_ptr_.get();
  }

  VirtualProxyType getProxy() const { return proxy_; }

  CoreType getCore() const { return default_core_; }
  NodeType getNode() const { return default_node_; }
  void mapToCore(CoreType const& core) { default_core_ = core; }
  void setCoreMap(HandlerType const han) { core_map_handle_ = han; }
  void setNodeMap(HandlerType const han) { node_map_handle_ = han; }
  bool hasCoreMap() const { return core_map_handle_ != uninitialized_handler; }
  bool hasNodeMap() const { return node_map_handle_ != uninitialized_handler; }

 private:
  HandlerType core_map_handle_ = uninitialized_handler;
  HandlerType node_map_handle_ = uninitialized_handler;

  CoreType default_core_ = uninitialized_destination;
  NodeType default_node_ = uninitialized_destination;
  VirtualProxyType proxy_ = no_vrt_proxy;
  VirtualPtrType vrt_ptr_ = nullptr;
};

}}  // end namespace vt::vrt

#endif /*INCLUDED_CONTEXT_VRT_INFO*/
