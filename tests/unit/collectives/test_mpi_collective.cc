/*
//@HEADER
// *****************************************************************************
//
//                           test_mpi_collective.cc
//                           DARMA Toolkit v. 1.0.0
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
// Questions? Contact darma@sandia.go
//
// *****************************************************************************
//@HEADER
*/

#include <gtest/gtest.h>
#include <vt/transport.h>

#include "test_parallel_harness.h"

namespace vt { namespace tests { namespace unit {

using TestMPICollective = TestParallelHarness;

TEST_F(TestMPICollective, test_mpi_collective_1) {
  bool done = false;

  theCollective()->mpiCollective([&done]{
    auto comm = theContext()->getComm();
    MPI_Barrier(comm);
    done = true;
  });

  theTerm()->addAction([&done]{
    EXPECT_TRUE(done);
  });

  while (not vt::rt->isTerminated()) {
    runScheduler();
  }
}

TEST_F(TestMPICollective, test_mpi_collective_2) {
  int done = 0;

  // These three collective can execute in any order, but it will always be
  // consistent across all the nodes
  theCollective()->mpiCollective([&done]{
    auto comm = theContext()->getComm();
    vt_print(barrier, "run MPI_Barrier\n");
    MPI_Barrier(comm);
    done++;
  });

  theCollective()->mpiCollective([&done]{
    auto comm = theContext()->getComm();
    int val = 0;
    vt_print(barrier, "run MPI_Bcast\n");
    MPI_Bcast(&val, 1, MPI_INT, 0, comm);
    done++;
  });

  theCollective()->mpiCollective([&done]{
    auto comm = theContext()->getComm();
    int val_in = 1;
    int val_out = 0;
    vt_print(barrier, "run MPI_Reduce\n");
    MPI_Reduce(&val_in, &val_out, 1, MPI_INT, MPI_SUM, 0, comm);
    done++;
  });

  theTerm()->addAction([&done]{
    EXPECT_EQ(done, 3);
  });

  while (not vt::rt->isTerminated()) {
    runScheduler();
  }
}

TEST_F(TestMPICollective, test_mpi_collective_3) {
  int done = 0;

  // These three collective can execute in any order, but it will always be
  // consistent across all the nodes
  auto tag = theCollective()->mpiCollective([&done]{
    auto comm = theContext()->getComm();
    int val = 0;
    vt_print(barrier, "run MPI_Bcast\n");
    MPI_Bcast(&val, 1, MPI_INT, 0, comm);
    done++;
  });

  theCollective()->waitCollective(tag);

  EXPECT_EQ(done, 1);

  theCollective()->mpiCollectiveWait([&done]{
    auto comm = theContext()->getComm();
    int val_in = 1;
    int val_out = 0;
    vt_print(barrier, "run MPI_Reduce\n");
    MPI_Reduce(&val_in, &val_out, 1, MPI_INT, MPI_SUM, 0, comm);
    done++;
  });

  EXPECT_EQ(done, 2);
}


}}} // end namespace vt::tests::unit
