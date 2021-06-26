/*
//@HEADER
// *****************************************************************************
//
//                               pmpi_component.h
//                       DARMA/vt => Virtual Transport
//
// Copyright 2019 National Technology & Engineering Solutions of Sandia, LLC
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

#if !defined INCLUDED_VT_PMPI_PMPI_COMPONENT_H
#define INCLUDED_VT_PMPI_PMPI_COMPONENT_H

#include "vt/config.h"

#if vt_check_enabled(mpi_access_guards)

#include "vt/configs/arguments/app_config.h"
#include "vt/runtime/component/component_pack.h"
#include "vt/runtime/mpi_access.h"

namespace vt { namespace pmpi {

/**
 * \struct PMPIComponent.
 *
 * \brief Component to support PMPI operations, as relevant.
 *
 * This provides an external entry point to some PMPI operations and
 * and infrastructure lifetimes such as registrations of events.
 */
struct PMPIComponent : runtime::component::Component<PMPIComponent> {
  PMPIComponent() = default;

  std::string name() override { return "PMPI"; }

  void startup() override {
    registerEventHandlers();

    trace_pmpi = theConfig()->vt_trace_pmpi;
  }

  void finalize() override {
    trace_pmpi = false;
  }

  /**
   * \internal
   * \brief Return true iff a PMPI call should be logged in the current context.
   */
  static bool shouldLogCall() {
    return trace_pmpi and runtime::ScopedMPIAccess::mpiCallsTraced();
  }

  template <typename Serializer>
  void serialize(Serializer& s) {
    s | trace_pmpi;
  }

private:

  // n.b. This is exepcted to be generated by the generated pmpi wrapper code.
  void registerEventHandlers();

  static bool trace_pmpi;

};

}} //end namespace vt::pmpi

namespace vt {
  extern pmpi::PMPIComponent* thePMPI();
}

#endif // vt_check_enabled(pmpi_access_guards)

#endif /*INCLUDED_VT_PMPI_PMPI_COMPONENT_H*/
