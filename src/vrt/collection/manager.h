
#if !defined INCLUDED_VRT_COLLECTION_MANAGER_H
#define INCLUDED_VRT_COLLECTION_MANAGER_H

#include "config.h"
#include "vrt/vrt_common.h"
#include "vrt/collection/manager.fwd.h"
#include "vrt/collection/proxy_builder/elm_proxy_builder.h"
#include "vrt/collection/messages/system_create.h"
#include "vrt/collection/types/headers.h"
#include "vrt/collection/holders/holder.h"
#include "vrt/collection/holders/entire_holder.h"
#include "vrt/collection/traits/cons_detect.h"
#include "vrt/collection/traits/cons_dispatch.h"
#include "vrt/collection/constructor/coll_constructors.h"
#include "vrt/collection/migrate/manager_migrate_attorney.fwd.h"
#include "vrt/collection/migrate/migrate_status.h"
#include "vrt/collection/destroy/manager_destroy_attorney.fwd.h"
#include "vrt/proxy/collection_wrapper.h"
#include "topos/mapping/mapping_headers.h"
#include "messaging/message.h"
#include "topos/location/location_headers.h"
#include "collective/collective_alg.h"
#include "collective/reduce/reduce_msg.h"
#include "collective/reduce/reduce_hash.h"

#include <memory>
#include <vector>
#include <tuple>
#include <utility>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <vector>
#include <list>
#include <cstdlib>

namespace vt { namespace vrt { namespace collection {

struct CollectionManager {
  template <typename ColT, typename IndexT>
  using CollectionType = typename Holder<ColT, IndexT>::Collection;
  template <typename ColT, typename IndexT>
  using VirtualPtrType = typename Holder<ColT, IndexT>::VirtualPtrType;
  using ActionProxyType = std::function<void(VirtualProxyType)>;
  using ActionFinishedLBType = std::function<void()>;
  using NoElementActionType = std::function<void()>;
  using ActionContainerType = std::vector<ActionProxyType>;
  using BufferedActionType = std::unordered_map<
    VirtualProxyType, ActionContainerType
  >;
  template <typename ColT, typename IndexT>
  using CollectionProxyWrapType = CollectionIndexProxy<ColT, IndexT>;
  using ReduceIDType = ::vt::collective::reduce::ReduceEpochLookupType;

  CollectionManager() = default;

  virtual ~CollectionManager() { destroyCollections<>(); }

  template <typename=void>
  void destroyCollections();

  /*
   *         CollectionManager::constructMap<ColT, Args...>
   *
   *  Construct virtual context collection with an initial pre-registered map
   *  function.
   */
  template <typename ColT, typename... Args>
  CollectionProxyWrapType<ColT, typename ColT::IndexType>
  constructMap(
    typename ColT::IndexType range, HandlerType const& map,
    Args&&... args
  );

  /*
   *      CollectionManager::construct<ColT, MapFnT, Args...>
   *
   *  Construct virtual context collection with an explicit templated map
   *  function, causing registration to occur.
   */
  template <
    typename ColT, mapping::ActiveMapTypedFnType<typename ColT::IndexType> fn,
    typename... Args
  >
  CollectionProxyWrapType<ColT, typename ColT::IndexType>
  construct(typename ColT::IndexType range, Args&&... args);

  /*
   *      CollectionManager::construct<ColT, Args...>
   *
   *  Construct virtual context collection using the default map for the given
   *  index. Found by looking up a vrt::collection::DefaultMap<...>
   *  specialization for the Index type.
   */
  template <typename ColT, typename... Args>
  CollectionProxyWrapType<ColT, typename ColT::IndexType>
  construct(typename ColT::IndexType range, Args&&... args);

  template <typename SysMsgT>
  static void distConstruct(SysMsgT* msg);

  /*
   *  Send message to an element of a collection
   */
  template <
    typename MsgT,
    ActiveColTypedFnType<MsgT, typename MsgT::CollectionType> *f
  >
  void sendMsg(
    VirtualElmProxyType<
      typename MsgT::CollectionType, typename MsgT::CollectionType::IndexType
    > const& toProxy,
    MsgT *const msg, ActionType act
  );

  template <typename CoLT, typename IndexT>
  static void collectionMsgHandler(BaseMessage* msg);

  /*
   *  Reduce all elements of a collection
   */
  template <typename ColT, typename MsgT, ActiveTypedFnType<MsgT> *f>
  EpochType reduceMsg(
    CollectionProxyWrapType<ColT, typename ColT::IndexType> const& toProxy,
    MsgT *const msg, EpochType const& epoch = no_epoch,
    TagType const& tag = no_tag
  );

  /*
   *  Broadcast message to all elements of a collection
   */
  template <
    typename MsgT,
    ActiveColTypedFnType<MsgT, typename MsgT::CollectionType> *f
  >
  void broadcastMsg(
    CollectionProxyWrapType<
      typename MsgT::CollectionType, typename MsgT::CollectionType::IndexType
    > const& toProxy,
    MsgT *const msg, ActionType act = nullptr, bool instrument = true
  );

  template <typename ColT, typename IndexT, typename MsgT>
  void broadcastFromRoot(MsgT* msg);

  template <typename ColT, typename IndexT, typename MsgT>
  static void collectionBcastHandler(MsgT* msg);
  template <typename ColT, typename IndexT, typename MsgT>
  static void broadcastRootHandler(MsgT* msg);
  template <typename=void>
  static void collectionConstructHan(CollectionConsMsg* msg);
  template <typename=void>
  static void collectionFinishedHan(CollectionConsMsg* msg);

  /*
   * Traits version of running the constructor based on the detected available
   * constructor types
   */

  template <
    typename ColT, typename IndexT, typename Tuple, typename... Args,
    size_t... I,
    typename = typename std::enable_if_t<
      ConstructorType<ColT,IndexT,Args...>::use_no_index
    >::type
  >
  static VirtualPtrType<ColT, IndexT> detectConstructorNoIndex(
    VirtualElmCountType const& elms, IndexT const& idx, Tuple* tup,
    std::index_sequence<I...>
  );

  template <
    typename ColT, typename IndexT, typename Tuple, typename... Args,
    size_t... I,
    typename = typename std::enable_if_t<
      ConstructorType<ColT,IndexT,Args...>::use_index_fst
    >::type
  >
  static VirtualPtrType<ColT, IndexT> detectConstructorIndexFst(
    VirtualElmCountType const& elms, IndexT const& idx, Tuple* tup,
    std::index_sequence<I...>
  );

  /*
   * Non-traits version of running the constructor: does not require the
   * detection idiom to dispatch to constructor.
   */

  template <typename ColT, typename IndexT, typename Tuple, size_t... I>
  static VirtualPtrType<ColT, IndexT> runConstructor(
    VirtualElmCountType const& elms, IndexT const& idx, Tuple* tup,
    std::index_sequence<I...>
  );

  template <typename ColT, typename IndexT>
  bool insertCollectionElement(
    VirtualPtrType<ColT, IndexT> vc, IndexT const& idx, IndexT const& max_idx,
    HandlerType const& map_han, VirtualProxyType const& proxy,
    bool const& is_migrated_in = false,
    NodeType const& migrated_from = uninitialized_destination
  );

  /*
   *  LB-related operations on the collection
   */
  template <typename ColT>
  void nextPhase(
    CollectionProxyWrapType<ColT, typename ColT::IndexType> const& proxy,
    PhaseType const& cur_phase, ActionFinishedLBType continuation = nullptr
  );

  template <typename ColT>
  void computeStats(
    CollectionProxyWrapType<ColT, typename ColT::IndexType> const& proxy,
    PhaseType const& cur_phase
  );

  template <typename=void>
  static void releaseLBPhase(CollectionPhaseMsg* msg);

  template <typename=void>
  std::size_t numCollections();

  template <typename=void>
  std::size_t numReadyCollections();

  template <typename=void>
  bool readyNextPhase();

  template <typename=void>
  void resetReadyPhase();

  template <typename=void>
  void releaseLBContinuation();

  template <typename=void>
  void makeCollectionReady(VirtualProxyType const coll);

  template <typename=void>
  void checkReduceNoElements();

private:
  template <typename ColT, typename IndexT>
  CollectionHolder<ColT, IndexT>* findColHolder(VirtualProxyType const& proxy);
  template <typename ColT, typename IndexT>
  Holder<ColT, IndexT>* findElmHolder(VirtualProxyType const& proxy);

public:
  template <typename ColT, typename IndexT>
  void destroy(CollectionProxyWrapType<ColT,IndexT> const& proxy);

private:
  template <typename ColT, typename IndexT>
  void incomingDestroy(CollectionProxyWrapType<ColT,IndexT> const& proxy);

  template <typename ColT, typename IndexT>
  void destroyMatching(CollectionProxyWrapType<ColT,IndexT> const& proxy);

protected:
  VirtualProxyType makeNewCollectionProxy();
  void insertCollectionInfo(
    VirtualProxyType const& proxy, HandlerType const& map
  );

public:
  template <typename ColT>
  MigrateStatus migrate(
    VrtElmProxy<ColT, typename ColT::IndexType>, NodeType const& dest
  );

  template <typename ColT, typename IndexT>
  static void insertHandler(InsertMsg<ColT,IndexT>* msg);

  template <typename ColT, typename IndexT = typename ColT::IndexType>
  void insert(
    CollectionProxyWrapType<ColT,IndexT> const& proxy, IndexT idx,
    IndexT max_idx, NodeType const& node = uninitialized_destination
  );

private:
  template <typename ColT, typename IndexT>
  friend struct CollectionElmAttorney;

  template <typename ColT, typename IndexT>
  friend struct CollectionElmDestroyAttorney;

  friend struct balance::ElementStats;

  template <typename ColT, typename IndexT>
  MigrateStatus migrateOut(
    VirtualProxyType const& proxy, IndexT const& idx, NodeType const& dest
  );

  template <typename ColT, typename IndexT>
  MigrateStatus migrateIn(
    VirtualProxyType const& proxy, IndexT const& idx, NodeType const& from,
    VirtualPtrType<ColT, IndexT> vrt_elm_ptr, IndexT const& range,
    HandlerType const& map_han
  );

private:
  template <typename=void>
  static VirtualIDType curIdent_;

  BufferedActionType buffered_sends_;
  BufferedActionType buffered_bcasts_;
  std::unordered_set<VirtualProxyType> constructed_;
  std::unordered_map<ReduceIDType,EpochType> reduce_cur_epoch_;
  std::vector<ActionFinishedLBType> lb_continuations_ = {};
  std::unordered_map<VirtualProxyType,NoElementActionType> lb_no_elm_ = {};
};

}}} /* end namespace vt::vrt::collection */

namespace vt {

extern vrt::collection::CollectionManager* theCollection();

}  // end namespace vt

#include "vrt/collection/manager.impl.h"
#include "vrt/collection/migrate/manager_migrate_attorney.impl.h"
#include "vrt/collection/send/sendable.impl.h"
#include "vrt/collection/insert/insertable.impl.h"
#include "vrt/collection/destroy/destroyable.impl.h"
#include "vrt/collection/destroy/manager_destroy_attorney.impl.h"
#include "vrt/collection/broadcast/broadcastable.impl.h"
#include "vrt/collection/balance/elm_stats.impl.h"
#include "vrt/collection/types/insertable.impl.h"

#endif /*INCLUDED_VRT_COLLECTION_MANAGER_H*/
