/*
//@HEADER
// ************************************************************************
//
//                          args.cc
//                     vt (Virtual Transport)
//                  Copyright (C) 2018 NTESS, LLC
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

#include "vt/config.h"
#include "vt/configs/arguments/args.h"

#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "fmt/format.h"


namespace vt { namespace arguments {


//--- Initialization of static variables for vt::AnchorBase
/* static */ std::map<const context, const AnchorBase::orderCtxt> AnchorBase::emap_ =
         {{std::make_pair(context::dFault, AnchorBase::orderCtxt::dFault),
            std::make_pair(context::commandLine, AnchorBase::orderCtxt::commandLine),
            std::make_pair(context::thirdParty, AnchorBase::orderCtxt::thirdParty)
         }};

/* static */ std::map<const context, const std::string> AnchorBase::smap_ =
   {{std::make_pair(context::dFault, "Default Value"),
       std::make_pair(context::commandLine, "Command Line"),
       std::make_pair(context::thirdParty, "Third-Party Context")
    }};


//--- Initialization of static variables for vt::AnchorBase
/*static*/ ArgSetup Args::setup_ = {"vt"};
/*static*/ Configs Args::config = {};


/*static*/ void Args::initialize()
{

  /*
   * Flags for controlling the colorization of output from vt
   */
  auto quiet  = "Quiet the output from vt (only errors, warnings)";
  auto always = "Always colorize output";
  auto never  = "Never colorize output";
  auto maybe  = "Use isatty to determine colorization of output";
  auto a  = setup_.addFlag("vt_color",      config.vt_color,      always);
  auto b  = setup_.addFlag("vt_no_color",   config.vt_no_color,   never);
  auto c  = setup_.addFlag("vt_auto_color", config.vt_auto_color, maybe);
  auto a1 = setup_.addFlag("vt_quiet",      config.vt_quiet,      quiet);
  auto outputGroup = "Output Control";
  a->setGroup(outputGroup);
  b->setGroup(outputGroup);
  c->setGroup(outputGroup);
  a1->setGroup(outputGroup);
  b->excludes(a);
  b->excludes(c);

  b->setBannerMsg_OnOff("Disabling color output", "Color output enabled by default");
  c->setBannerMsg_On("Automatic TTY detection for color output",
		  [&]() { return (config.vt_no_color == false); });

  /*
   * Flags for controlling the signals that VT tries to catch
   */
  auto no_sigint      = "Do not register signal handler for SIGINT";
  auto no_sigsegv     = "Do not register signal handler for SIGSEGV";
  auto no_terminate   = "Do not register handler for std::terminate";
  auto d = setup_.addFlag("vt_no_SIGINT",    config.vt_no_sigint,    no_sigint);
  auto e = setup_.addFlag("vt_no_SIGSEGV",   config.vt_no_sigsegv,   no_sigsegv);
  auto f = setup_.addFlag("vt_no_terminate", config.vt_no_terminate, no_terminate);
  auto signalGroup = "Signal Handling";
  d->setGroup(signalGroup);
  e->setGroup(signalGroup);
  f->setGroup(signalGroup);

  d->setBannerMsg_OnOff("Disabling SIGINT signal handling", 
		  "SIGINT signal handling enabled by default");
  e->setBannerMsg_OnOff("Disabling SIGSEGV signal handling",
		  "SIGSEGV signal handling enabled by default");
  f->setBannerMsg_OnOff("Disabling std::terminate signal handling",
		  "std::terminate signal handling enabled by default");

  /*
   * Flags to control stack dumping
   */
  auto stack  = "Do not dump stack traces";
  auto warn   = "Do not dump stack traces when vtWarn(..) is invoked";
  auto assert = "Do not dump stack traces when vtAssert(..) is invoked";
  auto abort  = "Do not dump stack traces when vtAabort(..) is invoked";
  auto file   = "Dump stack traces to file instead of stdout";
  auto dir    = "Name of directory to write stack files";
  auto mod    = "Write stack dump if (node % vt_stack_mod) == 0";
  auto g = setup_.addFlag("vt_no_warn_stack",   config.vt_no_warn_stack,   warn);
  auto h = setup_.addFlag("vt_no_assert_stack", config.vt_no_assert_stack, assert);
  auto i = setup_.addFlag("vt_no_abort_stack",  config.vt_no_abort_stack,  abort);
  auto j = setup_.addFlag("vt_no_stack",        config.vt_no_stack,        stack);
  auto k = setup_.addOption("vt_stack_file",    config.vt_stack_file,      file);
  auto l = setup_.addOption("vt_stack_dir",     config.vt_stack_dir,       dir);
  auto m = setup_.addOption("vt_stack_mod",     config.vt_stack_mod,       mod);
  auto stackGroup = "Dump Stack Backtrace";
  g->setGroup(stackGroup);
  h->setGroup(stackGroup);
  i->setGroup(stackGroup);
  j->setGroup(stackGroup);
  k->setGroup(stackGroup);
  l->setGroup(stackGroup);
  m->setGroup(stackGroup);

  j->setBannerMsg_OnOff("Disabling all stack dumps", "Stack dumps enabled by default");
  g->setBannerMsg_On("Disabling all stack dumps on vtWarn(..)");
  h->setBannerMsg_On("Disabling all stack dumps on vtAssert(..)");
  i->setBannerMsg_On("Disabling all stack dumps on vtAbort(..)");
  k->setBannerMsg_On("Output stack dumps with file name {}");
  l->setBannerMsg_On("Output stack dumps to {}");
  m->setBannerMsg_On("Output stack dumps every {} files ");
  
  /*
   * Flags to control tracing output
   */
  auto trace  = "Enable tracing (must be compiled with trace_enabled)";
  auto tfile  = "Name of trace files";
  auto tdir   = "Name of directory for trace files";
  auto tmod   = "Output trace file if (node % vt_stack_mod) == 0";
  auto n = setup_.addFlag("vt_trace",        config.vt_trace,       trace);
  auto o = setup_.addOption("vt_trace_file", config.vt_trace_file,  tfile);
  auto p = setup_.addOption("vt_trace_dir",  config.vt_trace_dir,   tdir);
  auto q = setup_.addOption("vt_trace_mod",  config.vt_trace_mod,   tmod);
  auto traceGroup = "Tracing Configuration";
  n->setGroup(traceGroup);
  o->setGroup(traceGroup);
  p->setGroup(traceGroup);
  q->setGroup(traceGroup);

#if !backend_check_enabled(trace_enabled)
  n->setBannerMsg_Warning("trace_enabled");
#else
  n->setBannerMsg_On("Tracing enabled");
  {
    std::string msg_off = "";
    if (theTrace)
      msg_off = fmt::format("Trace file \"{}\"", theTrace->getTraceName());
    o->setBannerMsg_OnOff("Trace file name \"{}\"", msg_off,
		  [&]() { return (config.vt_trace == true); });
	//---
	msg_off = "";
    if (theTrace)
      msg_off = fmt::format("Trace directory \"{}\"", theTrace->getDirectory());
    p->setBannerMsg_OnOff("Directory \"{}\"", msg_off,
		  [&]() { return (config.vt_trace == true); });
	//---
	q->setBannerMsg_On("Output every {} files ", 
			[&]() { return (config.vt_trace == true); });
  }
#endif

  /*
   * Flags for enabling load balancing and configuring it
   */

  auto lb            = "Enable load balancing";
  auto lb_file       = "Enable reading LB configuration from file";
  auto lb_quiet      = "Silence load balancing output";
  auto lb_file_name  = "LB configuration file to read";
  auto lb_name       = "Name of the load balancer to use";
  auto lb_interval   = "Load balancing interval";
  auto lb_stats      = "Enable load balancing statistics";
  auto lb_stats_dir  = "Load balancing statistics output directory";
  auto lb_stats_file = "Load balancing statistics output file name";
  auto s  = setup_.addFlag("vt_lb",              config.vt_lb,            lb);
  auto t  = setup_.addFlag("vt_lb_file",         config.vt_lb_file,       lb_file);
  auto t1 = setup_.addFlag("vt_lb_quiet",        config.vt_lb_quiet,      lb_quiet);
  auto u  = setup_.addOption("vt_lb_file_name",  config.vt_lb_file_name,  lb_file_name);
  auto v  = setup_.addOption("vt_lb_name",       config.vt_lb_name,       lb_name);
  auto w  = setup_.addOption("vt_lb_interval",   config.vt_lb_interval,   lb_interval);
  auto ww = setup_.addFlag("vt_lb_stats",        config.vt_lb_stats,      lb_stats);
  auto wx = setup_.addOption("vt_lb_stats_dir",  config.vt_lb_stats_dir,  lb_stats_dir);
  auto wy = setup_.addOption("vt_lb_stats_file", config.vt_lb_stats_file, lb_stats_file);
  auto lbGroup = "Load Balancing";
  s->setGroup(lbGroup);
  t->setGroup(lbGroup);
  t1->setGroup(lbGroup);
  u->setGroup(lbGroup);
  v->setGroup(lbGroup);
  w->setGroup(lbGroup);
  ww->setGroup(lbGroup);
  wx->setGroup(lbGroup);
  wy->setGroup(lbGroup);

#if !backend_check_enabled(lblite)
  s->setBannerMsg_Warning("lblite");
  ww->setBannerMsg_Warning("lblite");
#else
  s->setBannerMsg_On("Load balancing enabled");
  {
    t->setBannerMsg_On("Reading LB config from file",
			[&]() { return ((config.vt_lb) && (config.vt_lb_file)); });
	u->setBannerMsg_On("Reading file \"{}\"", 
			[&]() { return ((config.vt_lb) && (config.vt_lb_file)); });
	v->setBannerMsg_On("Load balancer name: \"{}\"",
            [&]() { return ((config.vt_lb) && (!config.vt_lb_file)); });
	w->setBannerMsg_On("Load balancing interval ={}",
            [&]() { return ((config.vt_lb) && (!config.vt_lb_file)); });
  }
  ww->setBannerMsg_On("Load balancing statistics collection");
  {
    wy->setBannerMsg_On("LB stats file name \"{}.0.out\""
			[&]() { return (config.vt_lb_stats); });
	wx->setBannerMsg_On("LB stats directory \"{}\"", 
			[&]() { return (config.vt_lb_stats); });
  }
#endif

  /*
   * Flags for controlling termination
   */

  auto hang         = "Disable termination hang detection";
  auto hang_freq    = "The number of tree traversals before a hang is detected";
  auto ds           = "Force use of Dijkstra-Scholten (DS) algorithm for rooted epoch termination detection";
  auto wave         = "Force use of 4-counter algorithm for rooted epoch termination detection";
  auto x  = setup_.addFlag("vt_no_detect_hang",       config.vt_no_detect_hang,       hang);
  auto x1 = setup_.addFlag("vt_term_rooted_use_ds",   config.vt_term_rooted_use_ds,   ds);
  auto x2 = setup_.addFlag("vt_term_rooted_use_wave", config.vt_term_rooted_use_wave, wave);
  auto y = setup_.addOption("vt_hang_freq",           config.vt_hang_freq, hang_freq);
  auto termGroup = "Termination";
  x->setGroup(termGroup);
  x1->setGroup(termGroup);
  x2->setGroup(termGroup);
  y->setGroup(termGroup);

  //--- Set information for printing the startup banner
  x1->setBannerMsg_On(ds);
  x2->setBannerMsg_On(wave);

  auto hang_dfault = "Termination hang detection enabled by default";
  x->setBannerMsg_OnOff(hang, hang_dfault);
  y->setBannerMsg_On("Detecting hang every {} tree traversals ", 
		  [&]() { return (!config.vt_no_detect_hang); });

  /*
   * Flag for pausing
   */

  auto pause        = "Pause at startup so GDB/LLDB can be attached";
  auto z = setup_.addFlag("vt_pause", config.vt_pause, pause);
  auto launchTerm = "Debugging/Launch";
  z->setGroup(launchTerm);
  auto pause_msg = "Enabled debug pause at startup";
  z->setBannerMsg_On(pause_msg);

  /*
   * User option flags for convenience; VT will parse these and the app can use
   * them however the apps requires
   */

  auto user1    = "User Option 1a (boolean)";
  auto user2    = "User Option 2a (boolean)";
  auto user3    = "User Option 3a (boolean)";
  auto userint1 = "User Option 1b (int32_t)";
  auto userint2 = "User Option 2b (int32_t)";
  auto userint3 = "User Option 3b (int32_t)";
  auto userstr1 = "User Option 1c (std::string)";
  auto userstr2 = "User Option 2c (std::string)";
  auto userstr3 = "User Option 3c (std::string)";
  auto u1  = setup_.addFlag("vt_user_1", config.vt_user_1, user1);
  auto u2  = setup_.addFlag("vt_user_2", config.vt_user_2, user2);
  auto u3  = setup_.addFlag("vt_user_3", config.vt_user_3, user3);
  auto ui1 = setup_.addOption("vt_user_int_1", config.vt_user_int_1, userint1);
  auto ui2 = setup_.addOption("vt_user_int_2", config.vt_user_int_2, userint2);
  auto ui3 = setup_.addOption("vt_user_int_3", config.vt_user_int_3, userint3);
  auto us1 = setup_.addOption("vt_user_str_1", config.vt_user_str_1, userstr1);
  auto us2 = setup_.addOption("vt_user_str_2", config.vt_user_str_2, userstr2);
  auto us3 = setup_.addOption("vt_user_str_3", config.vt_user_str_3, userstr3);
  auto userOpts = "User Options";
  u1->setGroup(userOpts);
  u2->setGroup(userOpts);
  u3->setGroup(userOpts);
  ui1->setGroup(userOpts);
  ui2->setGroup(userOpts);
  ui3->setGroup(userOpts);
  us1->setGroup(userOpts);
  us2->setGroup(userOpts);
  us3->setGroup(userOpts);

  //---

  initialize_debug();

}

//---

/*static*/ void Args::initialize_debug()
{

  /*
   * Flags for controlling debug print output at runtime
   */

  std::unordered_map<config::CatEnum, std::tuple<bool*, std::string, std::string> > debug_flag;

  debug_flag[config::none] = std::make_tuple<bool*, std::string, std::string>(
         &config.vt_debug_none, 
         std::string("debug_none"), 
		 std::string(config::PrettyPrintCat<config::none>::str) 
		 );
  debug_flag[config::gen]  = std::make_tuple(
		 &config.vt_debug_gen,
         std::string("debug_gen"), 
		 std::string(config::PrettyPrintCat<config::gen>::str)
		 );
  debug_flag[config::runtime] = std::make_tuple(
		  &config.vt_debug_runtime, 
          std::string("debug_runtime"), 
		  std::string(config::PrettyPrintCat<config::runtime>::str)
		  );
  debug_flag[config::active]  = std::make_tuple(
          &config.vt_debug_active,
          std::string("debug_active"), 
		  std::string(config::PrettyPrintCat<config::active>::str)
		  );
  debug_flag[config::term]  = std::make_tuple(
          &config.vt_debug_term,
          std::string("debug_term"), 
		  std::string(config::PrettyPrintCat<config::term>::str)
		  );
  debug_flag[config::termds]  = std::make_tuple(
          &config.vt_debug_termds,
          std::string("debug_term_ds"), 
		  std::string(config::PrettyPrintCat<config::termds>::str)
		  );
  debug_flag[config::barrier]  = std::make_tuple(
          &config.vt_debug_barrier,
          std::string("debug_barrier"), 
		  std::string(config::PrettyPrintCat<config::barrier>::str)
		  );
  debug_flag[config::event]  = std::make_tuple(
          &config.vt_debug_event,
          std::string("debug_event"), 
		  std::string(config::PrettyPrintCat<config::event>::str)
		  );
  debug_flag[config::pipe]  = std::make_tuple(
          &config.vt_debug_pipe,
          std::string("debug_pipe"), 
		  std::string(config::PrettyPrintCat<config::pipe>::str)
		  );
  debug_flag[config::pool]  = std::make_tuple(
          &config.vt_debug_pool,
          std::string("debug_pool"), 
		  std::string(config::PrettyPrintCat<config::pool>::str)
		  );
  debug_flag[config::reduce]  = std::make_tuple(
          &config.vt_debug_reduce,
          std::string("debug_reduce"), 
		  std::string(config::PrettyPrintCat<config::reduce>::str)
		  );
  debug_flag[config::rdma]  = std::make_tuple(
          &config.vt_debug_rdma,
          std::string("debug_rdma"), 
		  std::string(config::PrettyPrintCat<config::rdma>::str)
		  );
  debug_flag[config::rdma_channel]  = std::make_tuple(
          &config.vt_debug_rdma_channel,
          std::string("debug_rdma_channel"), 
		  std::string(config::PrettyPrintCat<config::rdma_channel>::str)
		  );
  debug_flag[config::rdma_state]  = std::make_tuple(
          &config.vt_debug_rdma_state,
          std::string("debug_rdma_state"), 
		  std::string(config::PrettyPrintCat<config::rdma_state>::str)
		  );
  debug_flag[config::param]  = std::make_tuple(
          &config.vt_debug_param,
          std::string("debug_param"), 
		  std::string(config::PrettyPrintCat<config::param>::str)
		  );
  debug_flag[config::handler]  = std::make_tuple(
          &config.vt_debug_handler,
          std::string("debug_handler"), 
		  std::string(config::PrettyPrintCat<config::handler>::str)
		  );
  debug_flag[config::hierlb]  = std::make_tuple(
          &config.vt_debug_hierlb,
          std::string("debug_hierlb"), 
		  std::string(config::PrettyPrintCat<config::hierlb>::str)
		  );
  debug_flag[config::scatter]  = std::make_tuple(
          &config.vt_debug_scatter,
          std::string("debug_scatter"), 
		  std::string(config::PrettyPrintCat<config::scatter>::str)
		  );
  debug_flag[config::sequence]  = std::make_tuple(
          &config.vt_debug_sequence,
          std::string("debug_sequence"), 
		  std::string(config::PrettyPrintCat<config::sequence>::str)
		  );
  debug_flag[config::sequence_vrt]  = std::make_tuple(
          &config.vt_debug_sequence_vrt,
          std::string("debug_sequence_vrt"), 
		  std::string(config::PrettyPrintCat<config::sequence_vrt>::str)
		  );
  debug_flag[config::serial_msg]  = std::make_tuple(
          &config.vt_debug_serial_msg,
          std::string("debug_serial_msg"), 
		  std::string(config::PrettyPrintCat<config::serial_msg>::str)
		  );
  debug_flag[config::trace]  = std::make_tuple(
          &config.vt_debug_trace,
          std::string("debug_trace"), 
		  std::string(config::PrettyPrintCat<config::trace>::str)
		  );
  debug_flag[config::location]  = std::make_tuple(
          &config.vt_debug_location,
          std::string("debug_location"), 
		  std::string(config::PrettyPrintCat<config::location>::str)
		  );
  debug_flag[config::lb]  = std::make_tuple(
          &config.vt_debug_lb,
          std::string("debug_lb"), 
		  std::string(config::PrettyPrintCat<config::lb>::str)
		  );
  debug_flag[config::vrt]  = std::make_tuple(
          &config.vt_debug_vrt,
          std::string("debug_vrt"), 
		  std::string(config::PrettyPrintCat<config::vrt>::str)
		  );
  debug_flag[config::vrt_coll]  = std::make_tuple(
          &config.vt_debug_vrt_coll,
          std::string("debug_vrt_coll"), 
		  std::string(config::PrettyPrintCat<config::vrt_coll>::str)
		  );
  debug_flag[config::worker]  = std::make_tuple(
          &config.vt_debug_worker,
          std::string("debug_worker"), 
		  std::string(config::PrettyPrintCat<config::worker>::str)
		  );
  debug_flag[config::group]  = std::make_tuple(
          &config.vt_debug_group,
          std::string("debug_group"), 
		  std::string(config::PrettyPrintCat<config::group>::str)
		  );
  debug_flag[config::broadcast] = std::make_tuple(
         &config.vt_debug_broadcast, 
         std::string("debug_broadcast"), 
		 std::string(config::PrettyPrintCat<config::broadcast>::str)
		 );
  debug_flag[config::objgroup]  = std::make_tuple(
		 &config.vt_debug_objgroup,
         std::string("debug_objgroup"), 
		 std::string(config::PrettyPrintCat<config::objgroup>::str)
		 );

  auto debugGroup = "Debug Print Configuration (must be compile-time enabled)";

  auto r  = setup_.addFlag("vt_debug_all", config.vt_debug_all, 
		  "Enable all debug prints");
  r->setBannerMsg_On("All debug prints are on (if enabled compile-time)");
  r->setGroup(debugGroup);

  auto r1 = setup_.addFlag("vt_debug_verbose", config.vt_debug_verbose,
		  "Enable verbose debug prints");
  r1->setGroup(debugGroup);

  for (const auto dcase : debug_flag) {
    auto f3 = dcase.second;
	auto ff = std::string("Enable ") + std::get<1>(f3) 
		+ std::string(" = \"") + std::get<2>(f3) + std::string("\"");
	auto &fbool = *(std::get<0>(f3));
	auto fname = std::string("vt_") + std::get<1>(f3);
	auto fr = setup_.addFlag(fname, fbool, ff);
	fr->setGroup(debugGroup);
	if (!((vt::config::DefaultConfig::category & dcase.first) != 0)) {
      if (fbool)
	    fr->setBannerMsg_Warning(std::get<1>(f3));
	}
  }

}

/* ------------------------------------------------- */
//--- Utility functions
/* ------------------------------------------------- */

template< typename T>
std::string display(const T &val) { std::string res; return res; }

template<>
std::string display<bool>(const bool &val) {
   return val ? std::string("ON") : std::string("OFF");
}

template<>
std::string display<int>(const int &val) {
   return std::to_string(val);
}

template<>
std::string display<std::string>(const std::string &val) {
   std::string res = std::string("\"") + val + std::string("\"");
   return res;
}

//---

template< typename T>
T t_zero() { return {}; }

template<>
bool t_zero<bool>() { return false; }

template<>
int t_zero<int>() { return 0; }

template<>
long long t_zero<long long>() { return 0; }

template<>
std::string t_zero<std::string>() { return std::string(""); }

/* ------------------------------------------------- */
//--- Utility printing classes
/* ------------------------------------------------- */

class Printer {
public:
  virtual void output() = 0;
};

//---

template < typename T >
class Print_On : public Printer {
public:

  Print_On(
    Anchor<T> *opt,
    const std::string &msg_str
  );

  Print_On(
    Anchor<T> *opt,
    const std::string &msg_str,
    std::function<bool()> fun
  );

  void output() override;

protected:
  Anchor<T> *option_ = nullptr;
  std::string msg_str_ ;
  std::function<bool()> condition_ = nullptr;
};

template< typename T>
Print_On<T>::Print_On(
  Anchor<T> *opt,
  const std::string &msg_str
) : option_(opt), msg_str_(msg_str), condition_(nullptr)
{ }

template< typename T>
Print_On<T>::Print_On(
  Anchor<T> *opt,
  const std::string &msg_str,
  std::function<bool()> fun
) : option_(opt), msg_str_(msg_str), condition_(fun)
{ }

template< typename T>
void Print_On<T>::output() {

  if (!option_->isResolved())
    return;

  T setValue = option_->getValue();
  if (setValue == t_zero<T>())
    return;

  if ((condition_) && (!condition_()))
    return;

  auto green    = debug::green();
  auto red      = debug::red();
  auto reset    = debug::reset();
  auto bd_green = debug::bd_green();
  auto magenta  = debug::magenta();
  auto vt_pre   = debug::vtPre();
//  auto emph     = [=](std::string s) -> std::string { return debug::emph(s); };
//  auto reg      = [=](std::string s) -> std::string { return debug::reg(s);  };

  std::string cli_name = std::string("--") + option_->getName();
  auto f8 = fmt::format(msg_str_, setValue);

  auto f9 = fmt::format(
      "{}Option:{} flag {}{}{} on: {}{}\n",
      green, reset, magenta, cli_name, reset, f8, reset
    );
  fmt::print("{}\t{}{}", vt_pre, f9, reset);

}

//---

template< typename T>
class Print_OnOff : public Printer {
public:
  Print_OnOff(
    Anchor<T> *opt,
    const std::string &msg_on,
    const std::string &msg_off
  );
  Print_OnOff(
    Anchor<T> *opt,
    const std::string &msg_on,
    const std::string &msg_off,
    std::function<bool()> fun
  );
  void output() override;
protected:
  Anchor<T> *option_;
  std::string msg_on_;
  std::string msg_off_;
  std::function<bool()> condition_ = nullptr;
};

template< typename T>
Print_OnOff<T>::Print_OnOff(
  Anchor<T> *opt,
  const std::string &msg_on,
  const std::string &msg_off
) : option_(opt), msg_on_(msg_on), msg_off_(msg_off), condition_(nullptr)
{ }

template< typename T>
Print_OnOff<T>::Print_OnOff(
  Anchor<T> *opt,
  const std::string &msg_on,
  const std::string &msg_off,
  std::function<bool()> fun
) : option_(opt), msg_on_(msg_on), msg_off_(msg_off), condition_(fun)
{ }

template< typename T>
void Print_OnOff<T>::output() {

  if (!option_->isResolved())
    return;

  auto green    = debug::green();
  auto red      = debug::red();
  auto reset    = debug::reset();
  auto bd_green = debug::bd_green();
  auto magenta  = debug::magenta();
  auto vt_pre   = debug::vtPre();
//  auto emph     = [=](std::string s) -> std::string { return debug::emph(s); };
//  auto reg      = [=](std::string s) -> std::string { return debug::reg(s);  };

  std::string cli_name = std::string("--") + option_->getName();
  T setValue = option_->getValue();

  if ((condition_) && (!condition_()))
    return;

  if (setValue != t_zero<T>()) {
	auto f_on = fmt::format(msg_on_, setValue);
    auto f9 = fmt::format(
      "{}Option:{} flag {}{}{} on: {}{}\n",
      green, reset, magenta, cli_name, reset, f_on, reset
    );
    fmt::print("{}\t{}{}", vt_pre, f9, reset);
  }
  else {
	auto f_off = fmt::format(msg_off_, setValue);
    auto f12 = fmt::format(
      "{}Default:{} {}, use {}{}{} to disable{}\n",
      green, reset, f_off, magenta, cli_name, reset, reset
    );
    fmt::print("{}\t{}{}", vt_pre, f12, reset);
  }

}

//---

class Warning : public Printer {
public:
  Warning(Anchor<bool> *opt, const std::string &compile);
  Warning(Anchor<bool> *opt, const std::string &compile,
    std::function<bool()> fun
  );
  void output() override;
protected:
  Anchor<bool> *option_;
  std::string compile_;
  std::function<bool()> condition_ = nullptr;
};

Warning::Warning(
  Anchor<bool> *opt,
  const std::string &compile
) : option_(opt), compile_(compile), condition_(nullptr)
{ }

Warning::Warning(
  Anchor<bool> *opt,
  const std::string &compile,
  std::function<bool()> fun
) : option_(opt), compile_(compile), condition_(fun)
{ }

void Warning::output() {

  if (!option_->isResolved())
    return;

  if (!option_->getValue())
    return;

  if ((condition_) && (!condition_()))
    return;

  auto green    = debug::green();
  auto red      = debug::red();
  auto reset    = debug::reset();
  auto bd_green = debug::bd_green();
  auto magenta  = debug::magenta();
  auto vt_pre   = debug::vtPre();
//  auto emph     = [=](std::string s) -> std::string { return debug::emph(s); };
//  auto reg      = [=](std::string s) -> std::string { return debug::reg(s);  };

  std::string cli_name = std::string("--") + option_->getName();
  auto fcompile = fmt::format(compile_, option_->getValue());
  auto f9 = fmt::format(
    "{}Warning:{} {}{}{} has no effect: compile-time"
    " feature {}{}{} is disabled{}\n",
    red, reset, magenta, cli_name, reset,
    magenta, fcompile, reset, reset
  );
  fmt::print("{}\t{}{}", vt_pre, f9, reset);

}

/* ------------------------------------------------- */
// --- Member functions for class Anchor<T>
/* ------------------------------------------------- */


template< typename T >
Anchor<T>::Anchor(
   T& var,
   const std::string &name,
   const std::string &desc
) : AnchorBase(name, desc),
    value_(var),
    specifications_(),
    hasNewDefault_(false),
    isResolved_(false),
    resolved_ctxt_(context::dFault),
    resolved_instance_(),
    resolved_to_default_(false),
    always_print_(false),
    always_print_startup_(false),
    print_value_(nullptr),
    print_condition_(nullptr),
    azerty()
{
   auto myCase = Instance<T>(var, this);
   specifications_.insert(std::make_pair(context::dFault, myCase));
   ordering_[context::dFault] = orderCtxt::dFault;
}

//-----

template< typename T >
std::string Anchor<T>::valueToString() const {
   std::string val;
   if (!isResolved_)
      return val;
   //
   val = display<T>(value_);
   //
   return val;
}

//-----

template< typename T>
void Anchor<T>::print() {
  if (azerty.get())
    azerty->output();
}

//-----

template< typename T>
std::string Anchor<T>::resolved_Context() const {
   std::string val;
   if (!isResolved_)
      return val;
   return smap_[resolved_ctxt_];
}

//-----

template< typename T>
std::string Anchor<T>::anchorDefault() const {
   std::string val;
   for (auto item : specifications_) {
      if (item.first == context::dFault) {
         val = display<T>(item.second.getValue());
         break;
      }
   }
   return val;
}

//-----

template< typename T>
void Anchor<T>::addGeneralInstance(
      context ctxt,
      const T &value)
{
   //---
   // Does not allow to specify a 'dFault' instance
   if (ctxt == context::dFault) {
      std::string code = std::string(__func__)
                         + std::string("::")
                         + std::string(" Default for ") + smap_[ctxt]
                         + std::string(" Can Not Be Added ");
      throw std::runtime_error(code);
   }
   //---
   if (specifications_.count(ctxt) > 0) {
      std::string code = std::string(__func__)
                         + std::string(" Context ") + smap_[ctxt]
                         + std::string(" Already Inserted ");
      throw std::runtime_error(code);
   }
   //----
   auto myCase = Instance<T>(value, this);
   specifications_.insert(std::make_pair(ctxt, myCase));
   if ((ordering_.count(ctxt) == 0) || (ordering_[ctxt] != orderCtxt::MAX))
      ordering_[ctxt] = emap_[ctxt];
}

//-----

template< typename T>
void Anchor<T>::checkExcludes() const {
   for(auto opt_ex : excludes_) {
      // Check whether the 'excluded' options have specifications
      // in addition to their default ones.
      if ((opt_ex->count() > 1) && (this->count() > 1)) {
         std::string code = std::string(__func__)
                            + std::string("::") + name_
                            + std::string(" excludes ")
                            + opt_ex->getName();
         throw std::runtime_error(code);
      }
   }
}

//-----

template< typename T>
void Anchor<T>::resolve() {
   if (isResolved_)
      return;
   //
   try {
      checkExcludes();
   }
   catch (const std::exception &e) {
      throw;
   }
   //
   int cmax = 0;
   context resolved_ctxt = context::dFault;
   for (const auto &iter : specifications_) {
      auto my_ctxt = iter.first;
      if ((ordering_.count(my_ctxt) > 0) &&
          (static_cast<int>(ordering_[my_ctxt]) > cmax)) {
         resolved_ctxt = my_ctxt;
         resolved_instance_ = iter.second;
         cmax = static_cast<int>(ordering_[my_ctxt]);
      }
   }
   //
   value_ = resolved_instance_.getValue();
   if ((cmax <= 1) || (resolved_ctxt == context::dFault))
      resolved_to_default_ = true;
   //
   isResolved_ = true;
}

//---

template< typename T>
void Anchor<T>::setBannerMsg_On(
  std::string msg_on, std::function<bool()> fun
)
{
  azerty = std::make_unique<Print_On<T>>(this, msg_on, fun);
}

//---

template< typename T>
void Anchor<T>::setBannerMsg_OnOff(
  std::string msg_on,
  std::string msg_off,
  std::function<bool()> fun
)
{
  azerty = std::make_unique<Print_OnOff<T>>(this, msg_on, msg_off, fun);
}

//---

template< typename T>
void Anchor<T>::setBannerMsg_Warning(
  std::string msg_compile,
  std::function<bool()> fun
)
{
  azerty = std::make_unique<Warning>(this, msg_compile, fun);
}

/* ------------------------------------------------- */
// --- Member functions for class ArgSetup
/* ------------------------------------------------- */

std::string ArgSetup::verifyName(const std::string &name) const
{
  int ipos = 0;
  while (name[ipos] == '-') { ipos++; }
  std::string tmpName = name.substr(ipos);
  //
  if (!CLI::detail::valid_name_string(tmpName)) {
     std::string code = std::string(" Invalid Name ") + name;
     throw std::invalid_argument(code);
  }
  //
  return tmpName;
}

//------

template<typename T>
std::shared_ptr<Anchor<T> > ArgSetup::getOption(const std::string &name) const
{
   std::string sname;
   try {
      sname = verifyName(name);
   }
   catch (const std::exception &e) {
      throw;
   }
   auto iter = options_.find(sname);
   if (iter == options_.end()) {
     std::string code = std::string(__func__)
		 + std::string("::") + std::string(" Name ")
		 + sname + std::string(" Does Not Exist ");
     throw std::runtime_error(code);
   }
   auto base = iter->second;
   auto anchor = std::static_pointer_cast<Anchor<T>>(base);
   return anchor;
}

//------

template <typename T>
std::shared_ptr<Anchor<T>> ArgSetup::addOption(
    const std::string &name,
    T& anchor_value,
    const std::string &desc
)
{
   std::string sname;
   try {
      sname = verifyName(name);
   }
   catch (const std::exception &e) {
      throw;
   }
   auto iter = options_.find(sname);
   if (iter == options_.end()) {
      // @todo: stop using default for warn_override and get this from runtime?
	  // @todo: Ask JL
      auto anchor = std::make_shared<Anchor<T> >(anchor_value, sname, desc);
      options_[sname] = anchor;
      //
      // Insert the option into CLI app
      //
      CLI::callback_t fun = [&anchor,&anchor_value](CLI::results_t res) {
         bool myFlag = CLI::detail::lexical_cast(res[0], anchor_value);
         anchor->addGeneralInstance(context::commandLine, anchor_value);
         return myFlag;
      };
      std::string cli_name = std::string("--") + sname;
      auto opt = app_.add_option(cli_name, fun, desc, true);
      opt->type_name(CLI::detail::type_name<T>());
      //
      std::stringstream out;
      out << anchor_value;
      opt->default_str(out.str());
      //
      return anchor;
   } else {
      auto base = iter->second;
      auto anchor = std::static_pointer_cast<Anchor<T>>(base);
      return anchor;
   }
}

//-----

std::vector<std::string> ArgSetup::getGroupList() const {
   std::vector<std::string> groups;
   for(auto opt : options_) {
      // Add group if it is not already in there
      auto gval = opt.second->getGroup();
      if(std::find(groups.begin(), groups.end(), gval) == groups.end()) {
         groups.push_back(gval);
      }
   }
   std::sort(groups.begin(), groups.end());
   return groups;
}

//-----

std::map< std::string, std::shared_ptr<AnchorBase> > ArgSetup::getGroupOptions(
   const std::string &gname
) const {
   std::map< std::string, std::shared_ptr<AnchorBase> > options;
   for (auto opt : options_) {
      // Add group if it is not already in there
      auto gval = opt.second->getGroup();
      if (gval == gname) {
         options[opt.first] = opt.second;
      }
   }
   return options;
}

//-----

void ArgSetup::parseResolve(int &argc, char** &argv) {
  try {
    parse(argc, argv);
	resolveOptions();
  }
  catch (const std::exception &e) {
    throw;
  }
}

//-----

void ArgSetup::printBanner() const
{
  auto groupList = getGroupList();
  for (auto gname : groupList) {
    auto goptions = getGroupOptions(gname);
    for (const auto &opt : goptions) {
      opt.second->print();
    }
  }
}

//-----

std::string ArgSetup::to_config(
  bool default_also,
  bool write_description,
  std::string prefix
) const
{
  auto groupList = getGroupList();
  std::stringstream out;
  for (auto gname : groupList) {
    auto goptions = getGroupOptions(gname);
    for (const auto &opt : goptions) {
      std::string name = prefix + opt.first;
      std::string value;
      //
      auto option = opt.second;
      //
      if (static_cast<int>(out.tellp()) != 0)
         out << std::endl;
      //
      out << "; Group [" << option->getGroup() << "]" << std::endl;
      //
      if (write_description) {
         out << "; "
             << CLI::detail::fix_newlines("; ", option->getDescription())
             << std::endl;
      }
      //
      if (default_also) {
         out << "; Default Value = " << option->anchorDefault()
            << std::endl;
      }
      //
      if (option->isResolved()) {
          out << "; Specified by " << option->resolved_Context() << std::endl;
      }
      else {
        out << "; Option/Flag Not Resolved " << std::endl;
      }
      //
      out << option->getName() << " = " << option->valueToString();
      out << std::endl;
      //
    }
  }
  return out.str();
}

//-----

template <typename T>
std::shared_ptr<Anchor<T>> ArgSetup::setNewDefaultValue(
   const std::string &name,
   const T& anchor_value
) {
   auto optPtr = this->getOption<T>(name);
   if (optPtr == nullptr) {
      std::string code = std::string(__func__)
      + std::string(" Name ") + name
      + std::string(" Can Not Be Found ");
      throw std::runtime_error(code);
   }
   optPtr->setNewDefaultValue(anchor_value);
   return optPtr;
}

//-----

int ArgSetup::parse(int& argc, char**& argv)
{

  if (parsed)
    return 0;

  std::vector<std::string> args;
  for (auto i = 0; i < argc; i++) {
    args.push_back(std::string(argv[i]));
  }

  // fmt::print("argc={}, argv={}\n", argc, print_ptr(argv));

  /*
   * Run the parser!
   */
  app_.allow_extras(true);
  try {
    app_.parse(args);
  } catch (CLI::Error &ex) {
    return app_.exit(ex);
  }

  /*
   * Put the arguments back into argc, argv, but properly order them based on
   * the input order by comparing between the current args
   */
  std::vector<std::string> ret_args;
  std::vector<std::size_t> ret_idx;

  // Reverse iterate (CLI11 reverses the order when they modify the args)
  for (auto iter = args.rbegin(); iter != args.rend(); ++iter) {
    for (auto ii = 0; ii < argc; ii++) {
      if (std::string(argv[ii]) == *iter) {
        ret_idx.push_back(ii);
        break;
      }
    }
    ret_args.push_back(*iter);
  }

  // Use the saved index to setup the new_argv and new_argc
  int new_argc = ret_args.size();
  char** new_argv = new char*[new_argc];
  for (auto ii = 0; ii < new_argc; ii++) {
    new_argv[ii] = argv[ret_idx[ii]];
  }

  // Set them back with all vt arguments elided
  argc = new_argc;
  argv = new_argv;

  parsed = true;

  return 0;

}


}} /* end namespace vt::arguments */
