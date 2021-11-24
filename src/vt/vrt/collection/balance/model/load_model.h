/*
//@HEADER
// *****************************************************************************
//
//                                 load_model.h
//                       DARMA/vt => Virtual Transport
//
// Copyright 2019-2021 National Technology & Engineering Solutions of Sandia, LLC
// (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact darma@sandia.gov
//
// *****************************************************************************
//@HEADER
*/

#if !defined INCLUDED_VT_VRT_COLLECTION_BALANCE_MODEL_LOAD_MODEL_H
#define INCLUDED_VT_VRT_COLLECTION_BALANCE_MODEL_LOAD_MODEL_H

#include "vt/config.h"
#include "vt/vrt/collection/balance/lb_common.h"
#include "vt/vrt/collection/balance/lb_comm.h"

namespace vt { namespace vrt { namespace collection { namespace balance {

class ObjectIterator;
struct EndObjectIterator {
  // Take rhs by reference to enable virtual dispatch and avoid slicing
  bool operator==(const ObjectIterator& rhs) const;
  bool operator!=(const ObjectIterator& rhs) const;
};

struct ObjectIteratorImpl
{
  using value_type = ElementIDStruct;

  ObjectIteratorImpl() = default;
  virtual ~ObjectIteratorImpl() = default;

  virtual void operator++() = 0;
  virtual value_type operator*() const = 0;
  virtual bool operator==(EndObjectIterator rhs) const = 0;
  virtual bool operator!=(EndObjectIterator rhs) const = 0;
};

class LoadMapObjectIterator : public ObjectIteratorImpl
{
  using map_iterator_type = LoadMapType::const_iterator;
  using iterator_category = std::iterator_traits<map_iterator_type>::iterator_category;
  map_iterator_type i, end;

public:
  LoadMapObjectIterator(map_iterator_type in, map_iterator_type in_end) : i(in), end(in_end) { }
  void operator++() override { ++i; }
  value_type operator*() const override { return i->first; }
  bool operator==(EndObjectIterator rhs) const override { return i == end; }
  bool operator!=(EndObjectIterator rhs) const override { return i != end; }
};

class ObjectIterator {
  std::unique_ptr<ObjectIteratorImpl> impl;
public:
  ObjectIterator(std::unique_ptr<ObjectIteratorImpl>&& in_impl)
    : impl(std::move(in_impl))
  { }
  void operator++() { ++(*impl); }
  ElementIDStruct operator*() const { return **impl; }
  bool operator==(EndObjectIterator rhs) const { return *impl == rhs; }
  bool operator!=(EndObjectIterator rhs) const { return *impl != rhs; }
};

inline bool EndObjectIterator::operator==(const ObjectIterator& rhs) const { return rhs == *this; }
inline bool EndObjectIterator::operator!=(const ObjectIterator& rhs) const { return rhs != *this; }

/**
 * \brief Interface for transforming measurements of past object loads
 * into predictions of future object load for load balancing
 * strategies
 */
class LoadModel
{
public:
  LoadModel() = default;
  virtual ~LoadModel() = default;

  /**
   * \brief Initialize the model instance with pointers to the measured load data
   *
   * This would typically be called by LBManager when the user has
   * passed a new model instance for a collection
   */
  virtual void setLoads(std::unordered_map<PhaseType, LoadMapType> const* proc_load,
                        std::unordered_map<PhaseType, CommMapType> const* proc_comm) = 0;

  /**
   * \brief Signals that load data for a new phase is available
   *
   * For models that want to do pre-computation based on measured
   * loads before being asked to provide predictions from them
   *
   * This would typically be called by LBManager collectively inside
   * an epoch that can be used for global communication in advance of
   * any calls to getWork()
   *
   * The `setLoads` method must have been called before any call to
   * this.
   */
  virtual void updateLoads(PhaseType last_completed_phase) = 0;

  /**
   * \brief Provide an estimate of the given object's load during a specified interval
   *
   * \param[in] object The object whose load is desired
   * \param[in] when The interval in which the estimated load is desired
   *
   * \return How much computation time the object is estimated to require
   *
   * The `updateLoads` method must have been called before any call to
   * this.
   */
  virtual TimeType getWork(ElementIDStruct object, PhaseOffset when) = 0;

  /**
   * \brief Compute how many phases of past load statistics need to be
   * kept availble for the model to use
   *
   * \param[in] look_back How many phases into the past the caller
   * intends to query
   *
   * \return How many phases of past load statistics will be needed to
   * satisfy the requested history
   */
  virtual unsigned int getNumPastPhasesNeeded(unsigned int look_back = 0) = 0;

  /**
   * Object enumeration, to abstract away access to the underlying structures from NodeStats
   *
   * The `updateLoads` method must have been called before any call to
   * this.
   */
  virtual ObjectIterator begin() = 0;
  /**
   * Object enumeration, to abstract away access to the underlying structures from NodeStats
   *
   * The `updateLoads` method must have been called before any call to
   * this.
   */
  EndObjectIterator end() { return EndObjectIterator{}; }

  /**
   * Object enumeration, to abstract away access to the underlying structures from NodeStats
   *
   * The `updateLoads` method must have been called before any call to
   * this.
   */
  virtual int getNumObjects() {
    int count = 0;
    for (auto it = begin(); it != end(); ++it, ++count) {}
    return count;
  }

  /**
   * Returns the number of phases of history available
   *
   * The `updateLoads` method must have been called before any call to
   * this.
   */
  virtual unsigned int getNumCompletedPhases() = 0;

  /**
   * Returns the number of subphases recorded in the most recent
   * completed phase
   *
   * The `updateLoads` method must have been called before any call to
   * this.
   */
  virtual int getNumSubphases() = 0;

  template <typename Serializer>
  void serialize(Serializer& s) {}
}; // class LoadModel

}}}} // namespaces

#endif
