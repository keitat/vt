/*
//@HEADER
// ************************************************************************
//
//                          tutorial_1a.h
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

#include "vt/transport.h"

namespace vt { namespace tutorial {

static inline void context() {
  /*
   *  ::vt::theContext() can be used to obtain the node and number of nodes:
   *  these correlate exactly to the MPI ranks and MPI size.
   *
   *  By default if you initialize VT without passing an MPI communication, VT
   *  will initialize MPI and use an internal communicator. If you want VT to
   *  interoperate, you can pass a communicator to the initialize invocation.
   *
   */

  // Equivalent to: MPI_Comm_rank(...)
  NodeType const this_node = ::vt::theContext()->getNode();

  // Equivalent to: MPI_Comm_size(...)
  NodeType const num_nodes = ::vt::theContext()->getNumNodes();

  // The header-only library fmt is used for printing throughout VT. You can use
  // it because the headers are included by default
  ::fmt::print("this_node={}, num_ndoes={}\n", this_node, num_nodes);
}

}} /* end namespace vt::tutorial */
