
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "test_parallel_harness.h"
#include "test_collection_common.h"
#include "data_message.h"

#include "vt/transport.h"

#include <cstdint>

namespace vt { namespace tests { namespace unit {

using namespace vt;
using namespace vt::collective;
using namespace vt::tests::unit;

struct WorkMsg;

static int32_t num_inserted = 0;
static int32_t num_work = 0;

struct InsertTest : InsertableCollection<InsertTest,Index1D> {
  InsertTest() : InsertableCollection<InsertTest,Index1D>() {
    num_inserted++;
    // ::fmt::print(
    //   "{}: inserting on node {}\n", idx.x(), theContext()->getNode()
    // );
  }

  void work(WorkMsg* msg);
};

void InsertTest::work(WorkMsg* msg) {
  // ::fmt::print("node={}: num_work={}\n", theContext()->getNode(), num_work);
  num_work++;
}

struct WorkMsg : CollectionMessage<InsertTest> {};
using ColProxyType = CollectionIndexProxy<InsertTest,Index1D>;

struct TestInsert : TestParallelHarness { };

static constexpr int32_t const num_elms_per_node = 8;

TEST_F(TestInsert, test_insert_dense_1) {
  auto const& root = 0;
  auto const& this_node = theContext()->getNode();
  auto const& num_nodes = theContext()->getNumNodes();
  if (this_node == 0) {
    auto const& range = Index1D(num_nodes * num_elms_per_node);
    auto proxy = theCollection()->construct<InsertTest>(range);
    for (auto i = 0; i < range.x(); i++) {
      proxy[i].insert();
    }
  }
  theTerm()->addAction([]{
    /// ::fmt::print("num inserted={}\n", num_inserted);
    // Relies on default mapping equally distributing
    EXPECT_EQ(num_inserted, num_elms_per_node);
    num_inserted = 0;
  });
}

TEST_F(TestInsert, test_insert_sparse_1) {
  auto const& root = 0;
  auto const& this_node = theContext()->getNode();
  auto const& num_nodes = theContext()->getNumNodes();
  if (this_node == 0) {
    auto const& range = Index1D(num_nodes * num_elms_per_node * 16);
    auto proxy = theCollection()->construct<InsertTest>(range);
    for (auto i = 0; i < range.x(); i+=16) {
      proxy[i].insert();
    }
  }
  theTerm()->addAction([]{
    /// ::fmt::print("num inserted={}\n", num_inserted);
    // Relies on default mapping equally distributing
    EXPECT_EQ(num_inserted, num_elms_per_node);
    num_inserted = 0;
  });
}

TEST_F(TestInsert, test_insert_dense_node_1) {
  auto const& root = 0;
  auto const& this_node = theContext()->getNode();
  auto const& num_nodes = theContext()->getNumNodes();
  if (this_node == 0) {
    auto const& range = Index1D(num_nodes * num_elms_per_node);
    auto proxy = theCollection()->construct<InsertTest>(range);
    for (auto i = 0; i < range.x(); i++) {
      proxy[i].insert(this_node);
    }
  }
  theTerm()->addAction([=]{
    /// ::fmt::print("num inserted={}\n", num_inserted);
    // Relies on default mapping equally distributing
    if (this_node == 0) {
      EXPECT_EQ(num_inserted, num_elms_per_node * num_nodes);
    }
    num_inserted = 0;
  });
}

TEST_F(TestInsert, test_insert_sparse_node_1) {
  auto const& root = 0;
  auto const& this_node = theContext()->getNode();
  auto const& num_nodes = theContext()->getNumNodes();
  if (this_node == 0) {
    auto const& range = Index1D(num_nodes * num_elms_per_node * 16);
    auto proxy = theCollection()->construct<InsertTest>(range);
    for (auto i = 0; i < range.x(); i+=16) {
      proxy[i].insert(this_node);
    }
  }
  theTerm()->addAction([=]{
    /// ::fmt::print("num inserted={}\n", num_inserted);
    // Relies on default mapping equally distributing
    if (this_node == 0) {
      EXPECT_EQ(num_inserted, num_elms_per_node * num_nodes);
    }
    num_inserted = 0;
  });
}

TEST_F(TestInsert, test_insert_send_dense_node_1) {
  auto const& root = 0;
  auto const& this_node = theContext()->getNode();
  auto const& num_nodes = theContext()->getNumNodes();
  if (this_node == 0) {
    auto const& range = Index1D(num_nodes * num_elms_per_node);
    auto proxy = theCollection()->construct<InsertTest>(range);
    for (auto i = 0; i < range.x(); i++) {
      proxy[i].insert((this_node + 1) % num_nodes);
      auto msg = makeSharedMessage<WorkMsg>();
      proxy[i].send<WorkMsg,&InsertTest::work>(msg);
      // ::fmt::print("sending to {}\n", i);
    }
  }
  theTerm()->addAction([=]{
    /// ::fmt::print("num inserted={}\n", num_inserted);
    // Relies on default mapping equally distributing
    if (this_node == 1 || (this_node == 0 && num_nodes == 1)) {
      EXPECT_EQ(num_inserted, num_elms_per_node * num_nodes);
      EXPECT_EQ(num_work, num_elms_per_node * num_nodes);
    }
    num_inserted = 0;
    num_work = 0;
  });
}

TEST_F(TestInsert, test_insert_send_sparse_node_1) {
  auto const& root = 0;
  auto const& this_node = theContext()->getNode();
  auto const& num_nodes = theContext()->getNumNodes();
  if (this_node == 0) {
    auto const& range = Index1D(num_nodes * num_elms_per_node * 16);
    auto proxy = theCollection()->construct<InsertTest>(range);
    for (auto i = 0; i < range.x(); i+=16) {
      proxy[i].insert((this_node + 1) % num_nodes);
      auto msg = makeSharedMessage<WorkMsg>();
      proxy[i].send<WorkMsg,&InsertTest::work>(msg);
    }
  }
  theTerm()->addAction([=]{
    /// ::fmt::print("num inserted={}\n", num_inserted);
    // Relies on default mapping equally distributing
    if (this_node == 1 || (this_node == 0 && num_nodes == 1)) {
      EXPECT_EQ(num_inserted, num_elms_per_node * num_nodes);
      EXPECT_EQ(num_work, num_elms_per_node * num_nodes);
    }
    num_inserted = 0;
    num_work = 0;
  });
}

}}} // end namespace vt::tests::unit
