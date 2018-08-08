
#if !defined INCLUDED_TOPOS_LOCATION_LOCATION_IMPL_H
#define INCLUDED_TOPOS_LOCATION_LOCATION_IMPL_H

#include "config.h"
#include "topos/location/location_common.h"
#include "topos/location/location.h"
#include "topos/location/manager.h"
#include "topos/location/manager.fwd.h"
#include "topos/location/utility/entity.h"
#include "context/context.h"
#include "messaging/active.h"
#include "serialization/auto_dispatch/dispatch.h"
#include "runnable/general.h"

#include <cstdint>
#include <memory>
#include <unordered_map>

namespace vt { namespace location {

template <typename EntityID>
EntityLocationCoord<EntityID>::EntityLocationCoord()
  : EntityLocationCoord<EntityID>(theLocMan()->cur_loc_inst++)
{ }

template <typename EntityID>
EntityLocationCoord<EntityID>::EntityLocationCoord(
  collection_lm_tag_t, LocInstType identifier
) : EntityLocationCoord<EntityID>(identifier)
{ }

template <typename EntityID>
EntityLocationCoord<EntityID>::EntityLocationCoord(LocInstType const identifier)
  : this_inst(identifier), recs_(default_max_cache_size)
{
  debug_print(
    location, node,
    "EntityLocationCoord constructor: inst={}, this={}\n",
    this_inst, print_ptr(this)
  );

  LocationManager::insertInstance<EntityLocationCoord<EntityID>>(
    this_inst, this
  );
}

template <typename EntityID>
/*virtual*/ EntityLocationCoord<EntityID>::~EntityLocationCoord() {
}

template <typename EntityID>
void EntityLocationCoord<EntityID>::registerEntity(
  EntityID const& id, NodeType const& home, LocMsgActionType msg_action,
  bool const& migrated
) {
  auto const& this_node = theContext()->getNode();
  auto reg_iter = local_registered_.find(id);

  assert(
    reg_iter == local_registered_.end() &&
    "EntityLocationCoord entity should not already be registered"
  );

  debug_print(
    location, node,
    "EntityLocationCoord: registerEntity: inst={}, home={}, migrated={}, "
    "id={}\n",
    this_inst, home, migrated, id
  );

  local_registered_.insert(id);

  recs_.insert(id, LocRecType{id, eLocState::Local, this_node});

  if (msg_action != nullptr) {
    // assert(
    //   local_registered_msg_han_.find(id) == local_registered_msg_han_.end() &&
    //   "Entitiy should not exist in local registered msg handler"
    // );
    local_registered_msg_han_.emplace(
      std::piecewise_construct,
      std::forward_as_tuple(id),
      std::forward_as_tuple(LocEntityMsg{id, msg_action})
    );
  }

  // trigger any pending actions upon registration
  auto pending_lookup_iter = pending_lookups_.find(id);

  debug_print(
    location, node,
    "EntityLocationCoord: registerEntity: pending lookups size={}, this={}, "
    "id={}\n",
    pending_lookups_.size(), print_ptr(this), id
  );

  if (pending_lookup_iter != pending_lookups_.end()) {
    auto const& node = theContext()->getNode();
    int action = 0;
    for (auto&& pending_action : pending_lookup_iter->second) {
      debug_print(
        location, node,
        "EntityLocationCoord: registerEntity: running pending action {}\n",
        action
      );
      action++;
      pending_action(node);
    }
    pending_lookups_.erase(pending_lookup_iter);
  }

  if (!migrated) {
    /*
     *  This is the case where the entity is *not* migrated here but gets
     *  constructed in an alternative non-default location. Thus we need to
     *  inform the home so that messages can be forwarded.
     */
    assert(home != uninitialized_destination && "Must have home node info");
    if (home != this_node) {
      debug_print(
        location, node,
        "EntityLocationCoord: registerEntity: updating id={}, home={}: "
        "not migrated\n",
        id, home
      );

      auto const& ask_node = uninitialized_destination;
      auto msg = makeSharedMessage<LocMsgType>(
        this_inst, id, no_location_event_id, ask_node, home
      );
      msg->setResolvedNode(this_node);
      theMsg()->sendMsg<LocMsgType, updateLocation>(home, msg);
    }
  }
}

template <typename EntityID>
void EntityLocationCoord<EntityID>::unregisterEntity(EntityID const& id) {
  auto reg_iter = local_registered_.find(id);

  assert(
    reg_iter != local_registered_.end() &&
    "EntityLocationCoord entity must be registered"
  );

  debug_print(
    location, node,
    "EntityLocationCoord: unregisterEntity\n"
  );

  local_registered_.erase(reg_iter);

  bool const& rec_exists = recs_.exists(id);
  if (rec_exists) {
    recs_.remove(id);
  }

  auto reg_msg_han_iter = local_registered_msg_han_.find(id);
  if (reg_msg_han_iter != local_registered_msg_han_.end()) {
    local_registered_msg_han_.erase(reg_msg_han_iter);
  }
}

template <typename EntityID>
void EntityLocationCoord<EntityID>::entityMigrated(
  EntityID const& id, NodeType const& new_node
) {
  auto reg_iter = local_registered_.find(id);

  if (reg_iter != local_registered_.end()) {
    local_registered_.erase(reg_iter);
  }

  recs_.insert(id, LocRecType{id, eLocState::Remote, new_node});
}

template <typename EntityID>
void EntityLocationCoord<EntityID>::registerEntityMigrated(
  EntityID const& id, NodeType const& from, LocMsgActionType msg_action
) {
  // @todo: currently `from' is unused, but is passed to this method in case we
  // need it in the future
  return registerEntity(id, uninitialized_destination, msg_action, true);
}

template <typename EntityID>
void EntityLocationCoord<EntityID>::insertPendingEntityAction(
  EntityID const& id, NodeActionType action
) {
  debug_print(
    location, node,
    "EntityLocationCoord: insertPendingEntityAction, this={}, id={}\n",
    print_ptr(this), id
  );

  // this is the home node and there is no record on this entity
  auto pending_iter = pending_lookups_.find(id);
  if (pending_iter != pending_lookups_.end()) {
    pending_iter->second.push_back(action);
  } else {
    pending_lookups_.emplace(
      std::piecewise_construct,
      std::forward_as_tuple(id),
      std::forward_as_tuple(ActionListType{action})
    );
  }
}

template <typename EntityID>
template <typename MessageT>
void EntityLocationCoord<EntityID>::routeMsgEager(
  bool const serialize, EntityID const& id, NodeType const& home_node,
  MessageT *msg, ActionType action
) {
  auto const& this_node = theContext()->getNode();
  NodeType route_to_node = uninitialized_destination;

  auto reg_iter = local_registered_.find(id);
  bool const found = reg_iter != local_registered_.end();

  debug_print(
    location, node,
    "EntityLocationCoord: routeMsgEager: found={}, home_node={}, "
    "route_to_node={}, serialize={}, id={}\n",
    found, home_node, route_to_node, serialize, id
  );

  if (found) {
    recs_.insert(id, LocRecType{id, eLocState::Local, this_node});
    route_to_node = this_node;
  } else {
    bool const& rec_exists = recs_.exists(id);

    if (not rec_exists) {
      if (home_node != this_node) {
        route_to_node = home_node;
      } else {
        route_to_node = this_node;
      }
    } else {
      auto const& rec = recs_.get(id);

      if (rec.isLocal()) {
        route_to_node = this_node;
      } else if (rec.isRemote()) {
        route_to_node = rec.getRemoteNode();
      }
    }
  }

  assert(
    route_to_node != uninitialized_destination &&
    "Node to route to must be set by this point"
  );

  debug_print(
    location, node,
    "EntityLocationCoord: routeMsgEager: home_node={}, route_node={}, "
    "serialize={}, id={}\n",
    home_node, route_to_node, serialize, id
  );

  return routeMsgNode<MessageT>(serialize,id,home_node,route_to_node,msg,action);
}

template <typename EntityID>
void EntityLocationCoord<EntityID>::getLocation(
  EntityID const& id, NodeType const& home_node, NodeActionType const& action
) {
  auto const& this_node = theContext()->getNode();

  auto reg_iter = local_registered_.find(id);

  if (reg_iter != local_registered_.end()) {
    debug_print(
      location, node,
      "EntityLocationCoord: getLocation: entity is local\n"
    );

    action(this_node);
    recs_.insert(id, LocRecType{id, eLocState::Local, this_node});
    return;
  } else {
    bool const& rec_exists = recs_.exists(id);

    debug_print(
      location, node,
      "EntityLocationCoord: getLocation: home_node={}, rec_exists={}, "
      "msg size={}\n",
      home_node, print_bool(rec_exists), sizeof(LocMsgType)
    );

    if (not rec_exists) {
      if (home_node != this_node) {
        auto const& event_id = fst_location_event_id++;
        auto msg = makeSharedMessage<LocMsgType>(
          this_inst, id, event_id, this_node, home_node
        );
        theMsg()->sendMsg<LocMsgType, getLocationHandler>(home_node, msg);
        // save a pending action when information about location arrives
        pending_actions_.emplace(
          std::piecewise_construct,
          std::forward_as_tuple(event_id),
          std::forward_as_tuple(PendingType{id, action})
        );
      } else {
        // this is the home node and there is no record on this entity
        insertPendingEntityAction(id, action);
      }
    } else {
      auto const& rec = recs_.get(id);

      if (rec.isLocal()) {
        assert(0 && "Should be registered if this is the case!");
        action(this_node);
      } else if (rec.isRemote()) {
        debug_print(
          location, node,
          "EntityLocationCoord: getLocation: entity is remote\n"
        );

        action(rec.getRemoteNode());
      }
    }
  }
}

template <typename EntityID>
template <typename MessageT>
void EntityLocationCoord<EntityID>::routeMsgNode(
  bool const serialize, EntityID const& id, NodeType const& home_node,
  NodeType const& to_node, MessageT *msg, ActionType action
) {
  auto const& this_node = theContext()->getNode();

  debug_print(
    location, node,
    "EntityLocationCoord: routeMsgNode: to_node={}, this_node={}: inst={}, "
    "serialize={}, home_node={}, id={}\n",
    to_node, this_node, this_inst, serialize, home_node, id
  );

  if (to_node != this_node) {
    // set the instance on the message to deliver to the correct manager
    msg->setLocInst(this_inst);

    if (serialize) {
      using ::vt::serialization::auto_dispatch::RequiredSerialization;
      RequiredSerialization<MessageT,msgHandler>::sendMsg(
        to_node,msg,no_tag,action
      );
    } else {
      // send to the node discovered by the location manager
      theMsg()->sendMsg<MessageT, msgHandler>(to_node, msg, action);
    }
  } else {
    debug_print(
      location, node,
      "EntityLocationCoord: routeMsgNode: to_node={}, this_node={}: "
      "home_node={}, apply here\n",
      to_node, this_node, home_node
    );

    auto trigger_msg_handler_action = [=](EntityID const& id) {
      bool const& has_handler = msg->hasHandler();
      if (has_handler) {
        auto const& handler = msg->getHandler();
        auto const& from = msg->getFromNode();
        auto active_fn = auto_registry::getAutoHandler(handler);
        debug_print(
          location, node,
          "EntityLocationCoord: apply direct handler action: "
          "id={}, from={}, handler={}\n",
          id, handler
        );
        runnable::Runnable<MessageT>::run(handler, active_fn, msg, from);
      } else {
        auto reg_han_iter = local_registered_msg_han_.find(id);
        assert(
          reg_han_iter != local_registered_msg_han_.end() and
          "Message handler must exist for location manager routed msg"
        );
        debug_print(
          location, node,
          "EntityLocationCoord: no direct handler: id={}\n", id
        );
        reg_han_iter->second.applyRegisteredActionMsg(msg);
      }
    };

    auto reg_iter = local_registered_.find(id);

    debug_print(
      location, node,
      "EntityLocationCoord: routeMsgNode: size={}\n",
      local_registered_.size()
    );

    if (reg_iter != local_registered_.end()) {
      debug_print(
        location, node,
        "EntityLocationCoord: routeMsgNode: running actions\n"
      );

      trigger_msg_handler_action(id);
      if (action) {
        action();
      }
    } else {
      debug_print(
        location, node,
        "EntityLocationCoord: routeMsgNode: buffering\n"
      );

      messageRef(msg);

      EntityID id_ = id;
      // buffer the message here, the entity will be registered in the future
      insertPendingEntityAction(id_, [=](NodeType resolved) {
        auto const& this_node = theContext()->getNode();

        debug_print(
          location, node,
          "EntityLocationCoord: routeMsgNode: trigger action: resolved={}, "
          "this_node={}, id={}\n", resolved, this_node, id_
        );

        if (resolved == this_node) {
          trigger_msg_handler_action(id_);
          if (action) {
            action();
          }
        } else {
          /*
           *  Recurse with the new updated node information. This occurs
           *  typically when an non-migrated registration occurs off the home
           *  node and messages are buffered, awaiting forwarding information.
           */
          routeMsgNode<MessageT>(serialize,id_,home_node,resolved,msg,action);
        }
        messageDeref(msg);
      });
    }
  }
}

template <typename EntityID>
void EntityLocationCoord<EntityID>::routeNonEagerAction(
  EntityID const& id, NodeType const& home_node, ActionNodeType action
) {
  getLocation(id, home_node, [=](NodeType node) {
    action(node);
  });
}

template <typename EntityID>
template <typename MessageT, ActiveTypedFnType<MessageT> *f>
void EntityLocationCoord<EntityID>::routeMsgHandler(
  EntityID const& id, NodeType const& home_node, MessageT *m, ActionType action
) {
  auto const& handler = auto_registry::makeAutoHandler<MessageT,f>(nullptr);
  m->setHandler(handler);
  return routeMsg<MessageT>(id,home_node,m,action);
}

template <typename EntityID>
template <typename MessageT, ActiveTypedFnType<MessageT> *f>
void EntityLocationCoord<EntityID>::routeMsgSerializeHandler(
  EntityID const& id, NodeType const& home_node, MessageT *m, ActionType action
) {
  auto const& handler = auto_registry::makeAutoHandler<MessageT,f>(nullptr);
  m->setHandler(handler);
  return routeMsg<MessageT>(id,home_node,m,action,true);
}

template <typename EntityID>
template <typename MessageT>
void EntityLocationCoord<EntityID>::routeMsgSerialize(
  EntityID const& id, NodeType const& home_node, MessageT *m,
  ActionType action
) {
  return routeMsg<MessageT>(id,home_node,m,action,true);
}

template <typename EntityID>
template <typename MessageT>
void EntityLocationCoord<EntityID>::routeMsg(
  EntityID const& id, NodeType const& home_node, MessageT *msg, ActionType act,
  bool const serialize
) {
  auto const& from_node = theContext()->getNode();
  // set field for location routed message
  msg->setEntity(id);
  msg->setHomeNode(home_node);
  msg->setFromNode(from_node);
  msg->setSerialize(serialize);

  auto const& msg_size = sizeof(*msg);
  bool const& is_large_msg = msg_size > small_msg_max_size;
  bool const& use_eager = not is_large_msg;

  debug_print(
    location, node,
    "routeMsg: inst={}, home={}, msg_size={}, is_large_msg={}, eager={}, "
    "serialize={}\n",
    this_inst, home_node, msg_size, is_large_msg, use_eager, serialize
  );

  msg->setLocInst(this_inst);

  if (use_eager) {
    routeMsgEager<MessageT>(serialize, id, home_node, msg, act);
  } else {
    // non-eager protocol: get location first then send message after resolution
    getLocation(id, home_node, [=](NodeType node) {
      routeMsgNode<MessageT>(serialize, id, home_node, node, msg, act);
    });
  }
}

template <typename EntityID>
void EntityLocationCoord<EntityID>::updatePendingRequest(
  LocEventID const& event_id, EntityID const& id, NodeType const& node
) {

  debug_print(
    location, node,
    "EntityLocationCoord: updatePendingRequest: event_id={}, node={}\n",
    event_id, node
  );

  if (event_id != no_location_event_id) {
    auto pending_iter = pending_actions_.find(event_id);

    assert(
      pending_iter != pending_actions_.end() && "Event must exist in pending"
    );

    auto const& entity = pending_iter->second.entity_;

    recs_.insert(entity, LocRecType{entity, eLocState::Remote, node});

    pending_iter->second.applyNodeAction(node);

    pending_actions_.erase(pending_iter);
  } else {
    recs_.insert(id, LocRecType{id, eLocState::Remote, node});

    // trigger any pending actions upon registration
    auto pending_lookup_iter = pending_lookups_.find(id);

    debug_print(
      location, node,
      "EntityLocationCoord: updatePendingRequest: node={}\n", node
    );

    if (pending_lookup_iter != pending_lookups_.end()) {
      for (auto&& pending_action : pending_lookup_iter->second) {
        pending_action(node);
      }
      pending_lookups_.erase(pending_lookup_iter);
    }
  }
}

template <typename EntityID>
void EntityLocationCoord<EntityID>::printCurrentCache() const {
  recs_.printCache();
}

template <typename EntityID>
LocInstType EntityLocationCoord<EntityID>::getInst() const {
  return this_inst;
}

template <typename EntityID>
template <typename MessageT>
/*static*/ void EntityLocationCoord<EntityID>::msgHandler(MessageT *msg) {
  auto const& entity_id = msg->getEntity();
  auto const& home_node = msg->getHomeNode();
  auto const& inst = msg->getLocInst();
  auto const& serialize = msg->getSerialize();

  debug_print(
    location, node,
    "msgHandler: msg={}, ref={}, loc_inst={}, serialize={}, id={}\n",
    print_ptr(msg), envelopeGetRef(msg->env), inst, serialize, entity_id
  );

  messageRef(msg);
  LocationManager::applyInstance<EntityLocationCoord<EntityID>>(
    inst, [=](EntityLocationCoord<EntityID>* loc) {
      loc->routeMsg(entity_id, home_node, msg, nullptr, serialize);
      messageDeref(msg);
    }
  );
}

template <typename EntityID>
/*static*/ void EntityLocationCoord<EntityID>::getLocationHandler(LocMsgType *msg) {
  auto const& event_id = msg->loc_event;
  auto const& inst = msg->loc_man_inst;
  auto const& entity = msg->entity;
  auto const& home_node = msg->home_node;
  auto const& ask_node = msg->ask_node;

  messageRef(msg);
  LocationManager::applyInstance<EntityLocationCoord<EntityID>>(
    inst, [=](EntityLocationCoord<EntityID>* loc) {
      loc->getLocation(entity, home_node, [=](NodeType node) {
        auto msg = makeSharedMessage<LocMsgType>(
          inst, entity, event_id, ask_node, home_node
        );
        msg->setResolvedNode(node);
        theMsg()->sendMsg<LocMsgType, updateLocation>(ask_node, msg);
      });
      messageDeref(msg);
    }
  );
}

template <typename EntityID>
/*static*/ void EntityLocationCoord<EntityID>::updateLocation(LocMsgType *msg) {
  auto const& event_id = msg->loc_event;
  auto const& inst = msg->loc_man_inst;
  auto const& entity = msg->entity;

  debug_print(
    location, node,
    "updateLocation: event_id={}, resolved={}, id={}\n",
    event_id, msg->resolved_node, entity
  );

  messageRef(msg);
  LocationManager::applyInstance<EntityLocationCoord<EntityID>>(
    inst, [=](EntityLocationCoord<EntityID>* loc) {
      debug_print(
        location, node,
        "updateLocation: event_id={}, running pending: resolved={}, id={}\n",
        event_id, msg->resolved_node, entity
      );
      loc->updatePendingRequest(event_id, entity, msg->resolved_node);
      messageDeref(msg);
    }
  );
}

}}  // end namespace vt::location

#endif /*INCLUDED_TOPOS_LOCATION_LOCATION_IMPL_H*/
