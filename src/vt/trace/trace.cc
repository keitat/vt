/*
//@HEADER
// *****************************************************************************
//
//                                   trace.cc
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
// Questions? Contact darma@sandia.gov
//
// *****************************************************************************
//@HEADER
*/

#include "vt/collective/collective_alg.h"
#include "vt/config.h"
#include "vt/scheduler/scheduler.h"
#include "vt/timing/timing.h"
#include "vt/trace/trace.h"
#include "vt/utils/demangle/demangle.h"

#include <fstream>
#include <cinttypes>
#include <sys/stat.h>
#include <unistd.h>

#include <zlib.h>

namespace vt { namespace trace {

using ArgType = vt::arguments::ArgConfig;

Trace::Trace(std::string const& in_prog_name, std::string const& in_trace_name)
  : prog_name_(in_prog_name), trace_name_(in_trace_name),
    start_time_(getCurrentTime()),
    log_file_()
{ }

Trace::Trace() { }

/*static*/ void Trace::traceBeginIdleTrigger() {
  #if backend_check_enabled(trace_enabled)
    if (not theTrace()->inIdleEvent()) {
      theTrace()->beginIdle();
    }
  #endif
}

void Trace::initializeTracing(
  std::string const& in_prog_name,
  std::string const& in_trace_name,
  std::string const& in_dir_name
) {
  traces_.reserve(trace_reserve_count);

  setupNames(in_prog_name, in_trace_name, in_dir_name);

  theSched()->registerTrigger(
    sched::SchedulerEvent::BeginIdle, traceBeginIdleTrigger
  );
}

bool Trace::inIdleEvent() const {
  return idle_begun_;
}

void Trace::setupNames(
  std::string const& in_prog_name,
  std::string const& in_trace_name,
  std::string const& in_dir_name
) {
  auto const node = theContext()->getNode();

  prog_name_ = in_prog_name;
  trace_name_ = in_trace_name;
  start_time_ = getCurrentTime();

  std::string dir_name = (in_dir_name.empty()) ? prog_name_ + "_trace" : in_dir_name;

  char cur_dir[1024];
  if (getcwd(cur_dir, sizeof(cur_dir)) == nullptr) {
    vtAssert(false, "Must have current directory");
  }

  if (ArgType::vt_trace_dir.empty()) {
    full_dir_name_ = std::string(cur_dir) + "/" + dir_name;
  }
  else {
    full_dir_name_ = ArgType::vt_trace_dir;
  }

  if (full_dir_name_[full_dir_name_.size() - 1] != '/')
    full_dir_name_ = full_dir_name_ + "/";

  if (theContext()->getNode() == 0) {
    int flag = mkdir(full_dir_name_.c_str(), S_IRWXU);
    if ((flag < 0) && (errno != EEXIST)) {
      vtAssert(flag >= 0, "Must be able to make directory");
    }
  }

  auto const tc = util::demangle::DemanglerUtils::splitString(trace_name_, '/');
  auto const pc = util::demangle::DemanglerUtils::splitString(prog_name_, '/');
  auto const trace_name = tc[tc.size()-1];
  auto const prog_name = pc[pc.size()-1];

  auto const node_str = "." + std::to_string(node) + ".log.gz";
  if (ArgType::vt_trace_file.empty()) {
    full_trace_name_ = full_dir_name_ + trace_name;
    full_sts_name_   = full_dir_name_ + prog_name + ".sts";
  } else {
    full_trace_name_ = full_dir_name_ + ArgType::vt_trace_file + node_str;
    full_sts_name_   = full_dir_name_ + ArgType::vt_trace_file + ".sts";
  }
}

/*virtual*/ Trace::~Trace() {
  writeTracesFile(true);
}

void Trace::addUserNote(std::string const& note) {
  debug_print(
    trace, node,
    "Trace::addUserNote: note={}\n",
    note
  );

  auto const type = TraceConstantsType::UserSuppliedNote;
  auto const time = getCurrentTime();

  LogPtrType log = new LogType(time, type);
  logEvent(log);

  editLastEntry([note](LogPtrType entry) {
    entry->setUserNote(note);
  });
}

void Trace::addUserData(int32_t data) {
  debug_print(
    trace, node,
    "Trace::addUserData: data={}\n",
    data
  );

  auto const type = TraceConstantsType::UserSupplied;
  auto const time = getCurrentTime();

  LogPtrType log = new LogType(time, type);
  logEvent(log);

  editLastEntry([data](LogPtrType entry) {
    entry->setUserData(data);
  });
}

void Trace::addUserBracketedNote(
  double const begin, double const end, std::string const& note,
  TraceEventIDType const event
) {
  debug_print(
    trace, node,
    "Trace::addUserBracketedNote: begin={}, end={}, note={}, event={}\n",
    begin, end, note, event
  );

  auto const type = TraceConstantsType::UserSuppliedBracketedNote;
  LogPtrType log = new LogType(begin, end, type, note, event);
  logEvent(log);
}

UserEventIDType Trace::registerUserEventColl(std::string const& name) {
  return user_event_.collective(name);
}

UserEventIDType Trace::registerUserEventRoot(std::string const& name) {
  return user_event_.rooted(name);
}

UserEventIDType Trace::registerUserEventHash(std::string const& name) {
  return user_event_.hash(name);
}

void Trace::registerUserEventManual(
  std::string const& name, UserSpecEventIDType id
) {
  user_event_.user(name, id);
}

void insertNewUserEvent(UserEventIDType event, std::string const& name) {
  #if backend_check_enabled(trace_enabled)
    theTrace()->user_event_.insertEvent(event, name);
  #endif
}

void Trace::addUserEvent(UserEventIDType event) {
  debug_print(
    trace, node,
    "Trace::addUserEvent: event={:x}\n",
    event
  );

  auto const type = TraceConstantsType::UserEvent;
  auto const time = getCurrentTime();

  LogPtrType log = new LogType(time, type, event, true);
  log->node = theContext()->getNode();
  logEvent(log);
}

void Trace::addUserEventManual(UserSpecEventIDType event) {
  debug_print(
    trace, node,
    "Trace::addUserEventManual: event={:x}\n",
    event
  );

  auto id = user_event_.createEvent(true, false, 0, event);
  addUserEvent(id);
}

void Trace::addUserEventBracketed(UserEventIDType event, double begin, double end) {
  debug_print(
    trace, node,
    "Trace::addUserEventBracketed: event={:x}, begin={}, end={}\n",
    event, begin, end
  );

  auto const type = TraceConstantsType::UserEventPair;

  LogPtrType log_b = new LogType(begin, type, event, true);
  log_b->node = theContext()->getNode();
  logEvent(log_b);

  LogPtrType log_e = new LogType(end, type, event, false);
  log_e->node = theContext()->getNode();
  logEvent(log_e);
}

void Trace::addUserEventBracketedBegin(UserEventIDType event) {
  debug_print(
    trace, node,
    "Trace::addUserEventBracketedBegin: event={:x}\n",
    event
  );

  auto const type = TraceConstantsType::BeginUserEventPair;
  auto const time = getCurrentTime();

  LogPtrType log = new LogType(time, type, event, true);
  log->node = theContext()->getNode();
  logEvent(log);
}

void Trace::addUserEventBracketedEnd(UserEventIDType event) {
  debug_print(
    trace, node,
    "Trace::addUserEventBracketedEnd: event={:x}\n",
    event
  );

  auto const type = TraceConstantsType::EndUserEventPair;
  auto const time = getCurrentTime();

  LogPtrType log = new LogType(time, type, event, false);
  log->node = theContext()->getNode();
  logEvent(log);
}

void Trace::addUserEventBracketedManualBegin(UserSpecEventIDType event) {
  auto id = user_event_.createEvent(true, false, 0, event);
  addUserEventBracketedBegin(id);
}

void Trace::addUserEventBracketedManualEnd(UserSpecEventIDType event) {
  auto id = user_event_.createEvent(true, false, 0, event);
  addUserEventBracketedEnd(id);
}

void Trace::addUserEventBracketedManual(
  UserSpecEventIDType event, double begin, double end
) {
  debug_print(
    trace, node,
    "Trace::addUserEventBracketedManual: event={:x}, begin={}, end={}\n",
    event, begin, end
  );

  auto id = user_event_.createEvent(true, false, 0, event);
  addUserEventBracketed(id, begin, end);
}

void Trace::beginProcessing(
  TraceEntryIDType const ep, TraceMsgLenType const len,
  TraceEventIDType const event, NodeType const from_node, double const time,
  uint64_t const idx1, uint64_t const idx2, uint64_t const idx3,
  uint64_t const idx4
) {
  auto const type = TraceConstantsType::BeginProcessing;
  LogPtrType log = new LogType(time, ep, type);

  debug_print(
    trace, node,
    "event_start: ep={}, event={}, time={}, from={}\n",
    ep, event, time, from_node
  );

  log->node = from_node;
  log->msg_len = len;
  log->event = event;
  log->idx1 = idx1;
  log->idx2 = idx2;
  log->idx3 = idx3;
  log->idx4 = idx4;

  logEvent(log);
}

void Trace::endProcessing(
  TraceEntryIDType const ep, TraceMsgLenType const len,
  TraceEventIDType const event, NodeType const from_node, double const time,
  uint64_t const idx1, uint64_t const idx2, uint64_t const idx3,
  uint64_t const idx4
) {
  auto const type = TraceConstantsType::EndProcessing;
  LogPtrType log = new LogType(time, ep, type);

  debug_print(
    trace, node,
    "event_stop: ep={}, event={}, time={}, from_node={}\n",
    ep, event, time, from_node
  );

  log->node = from_node;
  log->msg_len = len;
  log->event = event;
  log->idx1 = idx1;
  log->idx2 = idx2;
  log->idx3 = idx3;
  log->idx4 = idx4;

  logEvent(log);

  if (open_events_.empty()) {
    cur_stop_ = traces_.size();
  }
}

void Trace::beginIdle(double const time) {
  auto const type = TraceConstantsType::BeginIdle;
  LogPtrType log = new LogType(time, no_trace_entry_id, type);

  debug_print(
    trace, node, "begin_idle: time={}\n", time
  );

  log->node = theContext()->getNode();

  logEvent(log);

  idle_begun_ = true;
}

void Trace::endIdle(double const time) {
  auto const type = TraceConstantsType::EndIdle;
  LogPtrType log = new LogType(time, no_trace_entry_id, type);

  debug_print(
    trace, node, "end_idle: time={}\n", time
  );

  log->node = theContext()->getNode();

  logEvent(log);

  idle_begun_ = false;
}

TraceEventIDType Trace::messageCreation(
  TraceEntryIDType const ep, TraceMsgLenType const len, double const time
) {
  auto const type = TraceConstantsType::Creation;
  LogPtrType log = new LogType(time, ep, type);

  log->node = theContext()->getNode();
  log->msg_len = len;

  return logEvent(log);
}

TraceEventIDType Trace::messageCreationBcast(
  TraceEntryIDType const ep, TraceMsgLenType const len, double const time
) {
  auto const type = TraceConstantsType::CreationBcast;
  LogPtrType log = new LogType(time, ep, type);

  log->node = theContext()->getNode();
  log->msg_len = len;

  return logEvent(log);
}

TraceEventIDType Trace::messageRecv(
  TraceEntryIDType const ep, TraceMsgLenType const len,
  NodeType const from_node, double const time
) {
  auto const type = TraceConstantsType::MessageRecv;
  LogPtrType log = new LogType(time, ep, type);

  log->node = from_node;

  return logEvent(log);
}

void Trace::editLastEntry(std::function<void(LogPtrType)> fn) {
  if (not enabled_ || not traceWritingEnabled(theContext()->getNode())) {
    return;
  }
  if (traces_.empty()) {
    return;
  }
  //---
  auto const trace_cont_size = traces_.size();
  fn(traces_.at(trace_cont_size - 1));
}

TraceEventIDType Trace::logEvent(LogPtrType log) {
  if (not enabled_ || not traceWritingEnabled(theContext()->getNode())) {
    return 0;
  }

  // close any idle event as soon as we encounter any other type of event
  if (idle_begun_ and
      log->type != TraceConstantsType::BeginIdle and
      log->type != TraceConstantsType::EndIdle) {
    endIdle();
  }

  auto grouped_begin = [&]() -> TraceEventIDType {
    if (not open_events_.empty()) {
      traces_.push_back(
        new LogType(
          log->time,
          open_events_.top()->ep,
          TraceConstantsType::EndProcessing,
          open_events_.top()->event,
          open_events_.top()->msg_len,
          open_events_.top()->node,
          open_events_.top()->idx1,
          open_events_.top()->idx2,
          open_events_.top()->idx3,
          open_events_.top()->idx4
        )
      );
    }

    // push on open stack.
    open_events_.push(log);
    traces_.push_back(log);

    return log->event;
  };

  auto grouped_end = [&]() -> TraceEventIDType {
    vtAssert(
      not open_events_.empty(), "Stack should be empty"
    );

    vtAssert(
      open_events_.top()->ep == log->ep and
      open_events_.top()->type == TraceConstantsType::BeginProcessing,
      "Top event should be correct type and event"
    );

    // match event with the one that this ends
    log->event = open_events_.top()->event;

    // set up begin/end links
    open_events_.pop();

    traces_.push_back(log);

    if (not open_events_.empty()) {
      traces_.push_back(
        new LogType(
          log->time,
          open_events_.top()->ep,
          TraceConstantsType::BeginProcessing,
          open_events_.top()->event,
          open_events_.top()->msg_len,
          open_events_.top()->node,
          open_events_.top()->idx1,
          open_events_.top()->idx2,
          open_events_.top()->idx3,
          open_events_.top()->idx4
        )
      );
    }

    return log->event;
  };

  auto basic_new_event_create = [&]() -> TraceEventIDType {
    traces_.push_back(log);

    log->event = cur_event_++;

    return log->event;
  };

  auto basic_no_event_create = [&]() -> TraceEventIDType {
    traces_.push_back(log);

    log->event = no_trace_event;

    return log->event;
  };

  auto basic_cur_event = [&]() -> TraceEventIDType {
    traces_.push_back(log);

    log->event = cur_event_;

    return log->event;
  };

  auto basic_create = [&]() -> TraceEventIDType {
    traces_.push_back(log);
    return log->event;
  };

  switch (log->type) {
  case TraceConstantsType::BeginProcessing:
    return grouped_begin();
  case TraceConstantsType::EndProcessing:
    return grouped_end();
  case TraceConstantsType::Creation:
  case TraceConstantsType::CreationBcast:
  case TraceConstantsType::MessageRecv:
    return basic_new_event_create();
  case TraceConstantsType::BeginIdle:
  case TraceConstantsType::EndIdle:
    return basic_no_event_create();
  case TraceConstantsType::UserSupplied:
  case TraceConstantsType::UserSuppliedNote:
  case TraceConstantsType::UserSuppliedBracketedNote:
    return basic_create();
  case TraceConstantsType::UserEvent:
  case TraceConstantsType::UserEventPair:
    return log->user_start ? basic_cur_event() : basic_new_event_create();
    break;
  case TraceConstantsType::BeginUserEventPair:
  case TraceConstantsType::EndUserEventPair:
    return basic_new_event_create();
  default:
    vtAssert(0, "Not implemented");
    return 0;
  }
}

/*static*/ bool Trace::traceWritingEnabled(NodeType node) {
  if (ArgType::vt_trace) {
    return ((ArgType::vt_trace_mod == 0)
            or (node % ArgType::vt_trace_mod == 1));
  }
  else {
    return false;
  }
}

void Trace::enableTracing() {
  enabled_ = true;
}

void Trace::disableTracing() {
  enabled_ = false;
}

void Trace::flushTracesFile(bool useGlobalSync) {
  if (ArgType::vt_trace_flush_size == 0) {
    // Flush the traces at the end only
    return;
  }
  if (useGlobalSync) {
    // Synchronize all the nodes before flushing the traces
    theCollective()->barrier();
  }
  if (traces_.size() > cur_ + ArgType::vt_trace_flush_size) {
    writeTracesFile(false);
  }
}

void Trace::writeTracesFile(bool finalizeTracing) {
  auto const node = theContext()->getNode();

  if (tracing_closed_) {
    return;
  }

  debug_print(
    trace, node,
    "write_traces_file: traces.size={}, "
    "event_type_container.size={}, event_container.size={}\n",
    traces_.size(),
    TraceContainersType::event_type_container.size(),
    TraceContainersType::event_container.size()
  );

  if (finalizeTracing) {
    disableTracing(); // Accept no more traces.

    if (not open_events_.empty()) {
      // During an abort there might be open events.
      // This is not ideal; maybe a different footer can be written?
      debug_print(
        trace, node,
        "Trying to dump traces with open events - abnormal termination?"
      );
    }

    //--- Dump everything into an output file
    cur_stop_ = traces_.size();
  }

  if (traceWritingEnabled(theContext()->getNode())) {
    auto path = full_trace_name_;

    gzFile file = static_cast<gzFile>(log_file_);
    if (not file) {
      file = gzopen(path.c_str(), "wb");
      log_file_ = static_cast<void*>(file);

      outputHeader(node, start_time_, file);
    }

    writeTracesToLogFile(file);

    if (finalizeTracing) {
      // Closing zip stream/file permantently.
      outputFooter(node, start_time_, file);

      gzclose(file);
      log_file_ = nullptr;
      tracing_closed_ = true;
    } else {
      // Z_FULL_FLUSH is used/useful if previous data is corruptable;
      // however, it comes with severe compression penalities when used often.
      // Z_SYNC_FLUSH is sufficient to ensure data-until-here can be decompressed.
      gzflush(file, Z_SYNC_FLUSH);
    }
  }

  // STS file is written on first write request, and then (possibly again)
  // during finalization to ensure at least partial data and ultimately
  // full summary data: eg. new event counts, new event types, renames..
  if (node == designated_root_node
      and (not wrote_sts_file_ or finalizeTracing)) {
    std::ofstream file;
    auto name = full_sts_name_;
    file.open(name);
    outputControlFile(file);
    file.close();
    wrote_sts_file_ = true;
  }
}

void Trace::writeTracesToLogFile(void* fileVP) {
  gzFile file = static_cast<gzFile>(fileVP);
  TraceContainerType const& traces = traces_;

  size_t stop_point = cur_stop_;
  for (size_t i = cur_; i < stop_point; i++) {
    auto& log = traces[i];
    auto const& converted_time = timeToInt(log->time - start_time_);

    auto const type = static_cast<
      std::underlying_type<decltype(log->type)>::type
    >(log->type);

    auto event_iter = TraceContainersType::getEventContainer().find(log->ep);

    vtAssert(
      log->ep == no_trace_entry_id or
      event_iter != TraceContainersType::getEventContainer().end(),
      "Event must exist that was logged"
    );

    TraceEntryIDType event_seq_id;
    if (event_iter == TraceContainersType::getEventContainer().end()) {
      event_seq_id = 0;
    } else {
      event_seq_id = log->ep == no_trace_entry_id ?
        no_trace_entry_id : event_iter->second.theEventSeq();
    }

    auto const num_nodes = theContext()->getNumNodes();

    switch (log->type) {
    case TraceConstantsType::BeginProcessing:
      gzprintf(
        file,
        "%d %d %lu %lld %d %d %d 0 %" PRIu64 " %" PRIu64 " %" PRIu64 " %" PRIu64 " 0\n",
        type,
        eTraceEnvelopeTypes::ForChareMsg,
        event_seq_id,
        converted_time,
        log->event,
        log->node,
        log->msg_len,
        log->idx1,
        log->idx2,
        log->idx3,
        log->idx4
      );
      break;
    case TraceConstantsType::EndProcessing:
      gzprintf(
        file,
        "%d %d %lu %lld %d %d %d 0 %" PRIu64 " %" PRIu64 " %" PRIu64 " %" PRIu64 " 0\n",
        type,
        eTraceEnvelopeTypes::ForChareMsg,
        event_seq_id,
        converted_time,
        log->event,
        log->node,
        log->msg_len,
        log->idx1,
        log->idx2,
        log->idx3,
        log->idx4
      );
      break;
    case TraceConstantsType::BeginIdle:
      gzprintf(
        file,
        "%d %lld %d\n",
        type,
        converted_time,
        log->node
      );
      break;
    case TraceConstantsType::EndIdle:
      gzprintf(
        file,
        "%d %lld %d\n",
        type,
        converted_time,
        log->node
      );
      break;
    case TraceConstantsType::CreationBcast:
      gzprintf(
        file,
        "%d %d %lu %lld %d %d %d %d %d\n",
        type,
        eTraceEnvelopeTypes::ForChareMsg,
        event_seq_id,
        converted_time,
        log->event,
        log->node,
        log->msg_len,
        0,
        num_nodes
      );
      break;
    case TraceConstantsType::Creation:
      gzprintf(
        file,
        "%d %d %lu %lld %d %d %d 0\n",
        type,
        eTraceEnvelopeTypes::ForChareMsg,
        event_seq_id,
        converted_time,
        log->event,
        log->node,
        log->msg_len
      );
      break;
    case TraceConstantsType::UserEvent:
    case TraceConstantsType::UserEventPair:
    case TraceConstantsType::BeginUserEventPair:
    case TraceConstantsType::EndUserEventPair:
      gzprintf(
        file,
        "%d %lld %lld %d %d %d\n",
        type,
        log->user_event,
        converted_time,
        log->event,
        log->node,
        0
      );
      break;
    case TraceConstantsType::UserSupplied:
      gzprintf(
        file,
        "%d %d %lld\n",
        type,
        log->user_supplied_data,
        converted_time
      );
      break;
    case TraceConstantsType::UserSuppliedNote:
      gzprintf(
        file,
        "%d %lld %zu %s\n",
        type,
        converted_time,
        log->user_supplied_note.length(),
        log->user_supplied_note.c_str()
      );
      break;
    case TraceConstantsType::UserSuppliedBracketedNote: {
      auto const converted_end_time = timeToInt(log->end_time - start_time_);
      gzprintf(
        file,
        "%d %lld %lld %d %zu %s\n",
        type,
        converted_time,
        converted_end_time,
        log->event,
        log->user_supplied_note.length(),
        log->user_supplied_note.c_str()
      );
      break;
    }
    case TraceConstantsType::MessageRecv:
      vtAssert(false, "Message receive log type unimplemented");
      break;
    default:
      vtAssertInfo(false, "Unimplemented log type", converted_time, log->node);
    }

    delete log;
  }

  cur_ = stop_point;
}

/*static*/ double Trace::getCurrentTime() {
  return timing::Timing::getCurrentTime();
}

void Trace::outputControlFile(std::ofstream& file) {
  auto const num_nodes = theContext()->getNumNodes();

  auto const num_event_types =
    TraceContainersType::getEventTypeContainer().size();
  auto const num_events = TraceContainersType::getEventContainer().size();
  auto const num_user_events = user_event_.getEvents().size();

  file << "PROJECTIONS_ID\n"
       << "VERSION 7.0\n"
       << "TOTAL_PHASES 1\n"
       << "MACHINE vt\n"
       << "PROCESSORS " << num_nodes << "\n"
       << "TOTAL_CHARES " << num_event_types << "\n"
       << "TOTAL_EPS " << num_events << "\n"
       << "TOTAL_MSGS 1\n"
       << "TOTAL_PSEUDOS 0\n"
       << "TOTAL_EVENTS " << num_user_events
       << std::endl;

  ContainerEventSortedType sorted_event;
  ContainerEventTypeSortedType sorted_event_type;

  for (auto&& elem : TraceContainersType::getEventContainer()) {
    sorted_event.emplace(
      std::piecewise_construct,
      std::forward_as_tuple(&elem.second),
      std::forward_as_tuple(true)
    );
  }

  for (auto&& elem : TraceContainersType::getEventTypeContainer()) {
    sorted_event_type.emplace(
      std::piecewise_construct,
      std::forward_as_tuple(&elem.second),
      std::forward_as_tuple(true)
    );
  }

  for (auto&& event : sorted_event_type) {
    auto const name = event.first->theEventName();
    auto const id = event.first->theEventSeq();

    auto const out_name = std::string("::" + name);

    file << "CHARE "
         << id << " "
         << out_name << " "
         << std::endl;
  }

  for (auto&& event : sorted_event) {
    auto const name = event.first->theEventName();
    auto const type = event.first->theEventTypeSeq();
    auto const id = event.first->theEventSeq();

    file << "ENTRY CHARE "
         << id << " "
         << name << " "
         << type << " "
         << 0 << " "
         << std::endl;
  }

  file << "MESSAGE 0 0\n"
       << "TOTAL_STATS 0\n";

  for (auto&& elm : user_event_.getEvents()) {
    auto const id = elm.first;
    auto const name = elm.second;

    file << "EVENT "
         << id << " "
         << name << " "
         << std::endl;
  }

  file << "TOTAL_FUNCTIONS 0\n"
       << "END\n"
       << std::endl;
}

/*static*/ void Trace::outputHeader(
  NodeType const node, double const start, void* fileVP
) {
  gzFile file = static_cast<gzFile>(fileVP);
  // Output header for projections file
  gzprintf(file, "PROJECTIONS-RECORD 0\n");
  // '6' means COMPUTATION_BEGIN to Projections: this starts a trace
  gzprintf(file, "6 0\n");
}

/*static*/ void Trace::outputFooter(
  NodeType const node, double const start, void* fileVP
) {
  gzFile file = static_cast<gzFile>(fileVP);
  // Output footer for projections file,
  // '7' means COMPUTATION_END to Projections
  gzprintf(file, "7 %lld\n", timeToInt(getCurrentTime() - start));
}

/*static*/ Trace::TimeIntegerType Trace::timeToInt(double const time) {
  return static_cast<TimeIntegerType>(time * 1e6);
}

}} //end namespace vt::trace
