
#if !defined INCLUDED_VRT_COLLECTION_TYPES_TYPE_ATTORNEY_IMPL_H
#define INCLUDED_VRT_COLLECTION_TYPES_TYPE_ATTORNEY_IMPL_H

#include "config.h"
#include "vrt/collection/manager.fwd.h"

namespace vt { namespace vrt { namespace collection {

template <typename ColT>
/*static*/ void CollectionTypeAttorney::setSize(
  ColT const& col, VirtualElmCountType const& elms
) {
  return col->setSize(elms);
}

template <typename ColT, typename IndexT>
/*static*/ void CollectionTypeAttorney::setIndex(
  ColT const& col, IndexT const& index
) {
  return col->setIndex(index);
}

template <typename ColT>
/*static*/ void CollectionTypeAttorney::setProxy(
  ColT const& col, VirtualProxyType const& proxy
) {
  return col->setProxy(proxy);
}

}}} /* end namespace vt::vrt::collection */

#endif /*INCLUDED_VRT_COLLECTION_TYPES_TYPE_ATTORNEY_IMPL_H*/
