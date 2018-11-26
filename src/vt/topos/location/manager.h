/*
//@HEADER
// ************************************************************************
//
//                          manager.h
//                                VT
//              Copyright (C) 2017 NTESS, LLC
//
// Under the terms of Contract DE-NA-0003525 with NTESS, LLC,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact darma@sandia.gov
//
// ************************************************************************
//@HEADER
*/

#if !defined INCLUDED_TOPOS_LOCATION_MANAGER_H
#define INCLUDED_TOPOS_LOCATION_MANAGER_H

#include "vt/config.h"
#include "vt/topos/location/location_common.h"
#include "vt/topos/location/location.fwd.h"
#include "vt/topos/location/manager.fwd.h"
#include "vt/topos/location/utility/coord.h"
#include "vt/vrt/vrt_common.h"

#include <unordered_map>
#include <functional>

namespace vt { namespace location {

struct LocationManager {
  template <typename LocType>
  using PtrType = std::unique_ptr<LocType>;
  using LocCoordPtrType = LocationCoord*;
  using LocInstContainerType = std::unordered_map<LocInstType, LocCoordPtrType>;
  using VrtLocType = EntityLocationCoord<int32_t>;
  using VrtLocProxyType = EntityLocationCoord<VirtualProxyType>;

  template <typename ColT, typename IndexT>
  using CollectionProxyType = ::vt::vrt::VirtualElmProxyType<ColT, IndexT>;
  template <typename ColT, typename IndexT>
  using VrtColl = EntityLocationCoord<CollectionProxyType<ColT, IndexT>>;
  template <typename ColT, typename IndexT>
  using CollectionLocType = PtrType<VrtColl<ColT, IndexT>>;
  using LocErasureType = LocationCoord;
  using LocDeleterType = std::function<void(LocErasureType*)>;
  using CollectionLocErasedType = std::unique_ptr<
    LocErasureType, LocDeleterType
  >;
  using CollectionContainerType = std::unordered_map<
    VirtualProxyType, CollectionLocErasedType
  >;
  template <typename LocType>
  using ActionLocInstType = std::function<void(LocType*)>;
  template <typename LocType>
  using PendingContainerType = std::vector<ActionLocInstType<LocType>>;

  LocationManager() = default;

  virtual ~LocationManager();

  static LocInstType cur_loc_inst;

  PtrType<VrtLocType> virtual_loc = std::make_unique<VrtLocType>();;
  PtrType<VrtLocProxyType> vrtContextLoc = std::make_unique<VrtLocProxyType>();

  template <typename ColT, typename IndexT>
  VrtColl<ColT, IndexT>* getCollectionLM(VirtualProxyType const& proxy);
  template <typename ColT, typename IndexT>
  void insertCollectionLM(VirtualProxyType const& proxy);

public:
  // Manage different instances of individually managed entities
  template <typename LocType>
  static void insertInstance(LocInstType const i, LocType* ptr);
  static LocCoordPtrType getInstance(LocInstType const inst);

  template <typename LocType>
  static void applyInstance(LocInstType const inst, ActionLocInstType<LocType> action);

  template <typename LocType>
  static std::unordered_map<
    LocInstType, PendingContainerType<LocType>
  > pending_inst_;

protected:
  CollectionContainerType collectionLoc;

 private:
  static LocInstContainerType loc_insts;
};

}} /* end namespace vt::location */

#include "vt/topos/location/manager.impl.h"

namespace vt {

extern location::LocationManager* theLocMan();

}  // end namespace vt

#endif /*INCLUDED_TOPOS_LOCATION_MANAGER_H*/
