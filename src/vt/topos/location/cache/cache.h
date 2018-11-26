/*
//@HEADER
// ************************************************************************
//
//                          cache.h
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

#if !defined INCLUDED_TOPOS_LOCATION_CACHE_CACHE_H
#define INCLUDED_TOPOS_LOCATION_CACHE_CACHE_H

#include "vt/config.h"
#include "vt/topos/location/location_common.h"
#include "vt/context/context.h"

#include <list>
#include <tuple>
#include <unordered_map>

namespace vt { namespace location {

template <typename KeyT, typename ValueT>
struct LocationCache {
  using LookupType = std::tuple<KeyT, ValueT>;
  using CacheOrderedType = std::list<LookupType>;
  using ValueIter = typename CacheOrderedType::iterator;
  using LookupContainerType = std::unordered_map<KeyT, ValueIter>;

  explicit LocationCache(LocationSizeType const& in_max_size);

  LocationCache(LocationCache const&) = delete;
  LocationCache(LocationCache&&) = delete;
  LocationCache& operator=(LocationCache const&) = delete;

  bool exists(KeyT const& key) const;
  LocationSizeType getSize() const;
  ValueT const& get(KeyT const& key);
  void remove(KeyT const& key);
  void insert(KeyT const& key, ValueT const& value);
  void printCache() const;

 private:
  // container for quick lookup
  LookupContainerType lookup_;

  // the location records sorted in LRU cache
  CacheOrderedType cache_;

  // the maximum size the cache is allowed to grow
  LocationSizeType max_size_;
};

}}  // end namespace vt::location

#include "vt/topos/location/cache/cache.impl.h"

#endif /*INCLUDED_TOPOS_LOCATION_CACHE_CACHE_H*/
