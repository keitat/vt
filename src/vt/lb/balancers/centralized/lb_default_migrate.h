/*
//@HEADER
// ************************************************************************
//
//                          lb_default_migrate.h
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

#if !defined INCLUDED_LB_BALANCERS_CENTRALIZED_LB_DEFAULT_MIGRATE_H
#define INCLUDED_LB_BALANCERS_CENTRALIZED_LB_DEFAULT_MIGRATE_H

#include "vt/config.h"
#include "vt/lb/lb_types.h"
#include "vt/lb/lb_types_internal.h"
#include "vt/lb/migration/migrate.h"
#include "vt/lb/balancers/centralized/lb_interface.h"

namespace vt { namespace lb { namespace centralized {

struct CentralMigrate : CentralLB {

  CentralMigrate(NodeType const& node, NodeType const& central_node)
    : CentralLB(node, central_node)
  { }

  virtual void notifyMigration(
    NodeType const& from, NodeType const& to, LBEntityType const& entity
  ) override;
  virtual void notifyMigrationList(MigrateInfoType const& migrate_info) override;
  virtual void finishedMigrations() override;

protected:
  void migrate(NodeType const& to_node, LBEntityType const& entity);
};

}}} /* end namespace vt::lb::centralized */

#endif /*INCLUDED_LB_BALANCERS_CENTRALIZED_LB_DEFAULT_MIGRATE_H*/
