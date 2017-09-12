
#if ! defined __RUNTIME_TRANSPORT_RDMA_CHANNEL__
#define __RUNTIME_TRANSPORT_RDMA_CHANNEL__

#include "common.h"
#include "function.h"
#include "rdma_common.h"
#include "rdma_channel_lookup.h"
#include "rdma_handle.h"

#include <mpi.h>

#include <unordered_map>

namespace runtime { namespace rdma {

static constexpr ByteType const rdma_elm_size = sizeof(char);
static constexpr ByteType const rdma_empty_byte = 0;

struct Channel {
  using RDMA_HandleManagerType = HandleManager;
  using RDMA_TypeType = Type;
  using RDMA_GroupPosType = int;

  static constexpr RDMA_GroupPosType const no_group_pos = -1;

  Channel(
    RDMA_HandleType const& in_rdma_handle, RDMA_TypeType const& in_op_type,
    NodeType const& in_target, TagType const& in_channel_group_tag,
    NodeType const& in_non_target = uninitialized_destination,
    RDMA_PtrType const& in_ptr = nullptr, ByteType const& in_num_bytes = no_byte
  );

  void
  init_channel_group();

  void
  sync_channel_local();

  void
  sync_channel_global();

  void
  lock_channel_for_op();

  void
  unlock_channel_for_op();

  void
  write_data_to_channel(
    RDMA_PtrType const& ptr, ByteType const& ptr_num_bytes, ByteType const& offset
  );

  void
  free_channel();

  virtual ~Channel();

  NodeType
  get_target() const;

  NodeType
  get_non_target() const;

private:
  void
  init_channel_window();

private:
  bool is_target = false;

  bool initialized = false, locked = false, flushed = true;
  RDMA_HandleType const rdma_handle = no_rdma_handle;
  RDMA_GroupPosType target_pos = no_group_pos;
  RDMA_GroupPosType non_target_pos = no_group_pos;
  NodeType target = uninitialized_destination;
  NodeType my_node = uninitialized_destination;
  NodeType non_target = uninitialized_destination;
  ByteType num_bytes = no_byte;
  RDMA_PtrType ptr = no_rdma_ptr;
  RDMA_TypeType op_type = uninitialized_rdma_type;

  TagType channel_group_tag = no_tag;

  MPI_Win window;
  MPI_Group channel_group;
  MPI_Comm channel_comm;
};

}} //end namespace runtime::rdma

#endif /*__RUNTIME_TRANSPORT_RDMA_CHANNEL__*/
