/*
//@HEADER
// ************************************************************************
//
//                          features_enableif.h
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

#if !defined INCLUDED_FEATURES_ENABLE_IF
#define INCLUDED_FEATURES_ENABLE_IF

#include "meld_headers.h"
#include "vt/configs/features/features_featureswitch.h"
#include "vt/configs/features/features_defines.h"

#define backend_static_assert_unreachable       \
  static_assert(false, "Configuration error: This should be unreachable");

#define debug_options_build(options...) options
#define debug_local_options_on(options...) options

// #define debug_resolve_op(option) debug_ ## option(option),
// #define debug_resolve_options(options...)             \
//   meld_meta_map(meld_map, debug_resolve_op, ##options)

#define backend_options_on(arg...) arg
#define backend_str_join(tok1,tok2) tok2 ## tok1

#define debug_options_on(options...)                              \
  debug_options_build(options)                                    \

#define backend_configuration debug_options_on(                 \
    backend_features,                                           \
    backend_debug_contexts,                                     \
    backend_debug_modes,                                        \
    backend_defaults                                            \
  )
#define backend_feature_configuration debug_options_on(         \
    backend_features,                                           \
  )
#define backend_debug_configuration debug_options_on(           \
    backend_debug_modes,                                        \
    backend_defaults                                            \
  )
#define backend(x) x

/* Test functions toc check if options are enabled */

#define debug_test_option_or(option, to_test)   \
  meld_token_compare(option, to_test),
#define debug_test_option_shortcut(opt1, opt2)  \
  meld_token_compare(opt1, opt2)

// Use the meld's logical or to determine if any option is on
#define check_enabled_or(test_option, options...)                       \
  meld_fold_or(                                                         \
    meld_eval(meld_map_with(debug_test_option, test_option, options))   \
  )

// Yield 1/0 and shortcut boolean check
#define check_enabled_shortcut(test_option, options...)                 \
  meld_eval(                                                            \
    meld_check_enabled(                                                 \
      debug_test_option_shortcut, test_option, options                  \
    )                                                                   \
  )

#define debug_check_enabled_shortcut_impl(test_option, options...)  \
  check_enabled_shortcut(test_option, options)
#define debug_check_enabled_or_impl(test_option, options...)  \
  check_enabled_shortcut(test_option, options)
#define debug_check_enabled_shortcut(config, test_option)               \
  debug_check_enabled_shortcut_impl(                                    \
    test_option, backend_str_join(_configuration, config)               \
  )
#define debug_check_enabled_or(config, test_option)                    \
  debug_check_enabled_or_impl(                                         \
    test_option, backend_str_join(_configuration, config)              \
  )

#define debug_compare(opt1, opt2)                       \
  check_enabled_shortcut(opt1, debug_local_options_on(opt2, none))
#define debug_cond_clause(condition, if_true, if_false) \
  meld_if_stmt(condition)(if_true)(if_false)
#define debug_cond_enabled(config, feature, if_true)                    \
  debug_cond_clause(                                                    \
    debug_check_enabled_shortcut(config,feature),if_true,               \
  )
#define debug_cond_enabled_else(config, feature, if_true, if_false)     \
  debug_cond_clause(                                                    \
    debug_check_enabled_shortcut(unconfig,feature),if_true,if_false       \
  )
#define debug_cond(condition, if_true)                          \
  debug_cond_clause(condition,if_true,)
#define debug_cond_else(condition, if_true, if_false) \
  debug_cond_clause(condition,if_true,if_false)
#define debug_cond_force(condition, if_true) do { if_true; } while (0);
#define debug_test(config, feature, if_true, if_false)                  \
  meld_if_stmt(                                                         \
    debug_check_enabled_shortcut(config, feature)                       \
  )(if_true)(if_false)

#define debug_cond_block(block, conds...)       \
  debug_cond_block_recur(block, conds)

#define debug_cond_block_recur(block, cond_fst, conds...)       \
  debug_cond_clause(cond_fst,                                   \
    meld_if_stmt(_meld_has_arguments(conds))(                   \
      debug_cond_block_recur(block, conds)                      \
    )(                                                          \
      block                                                     \
    )                                                           \
 ,)

/* External Interface Functions */
#define backend_check_enabled_options(config, test_option, options...)  \
  debug_check_enabled_shortcut_impl(config, test_option, options)
#define backend_check_enabled(test_option)                              \
  backend_check_enabled_options(                                        \
    test_option, backend_str_join(_configuration, backend)              \
  )
#define backend_debug_enabled(test_option)                              \
  backend_check_enabled_options(                                        \
    test_option, backend_str_join(_configuration, backend_debug)        \
  )

#define backend_enable_if_any(config, feature, eit) \
  debug_cond_enabled(config, feature, eit)
#define backend_enable_if_else_any(config, feature, eit, eif) \
  debug_test(config, feature, eit, eif)
#define backend_enable_if_not_any(feature, eif)  \
  debug_test(config, feature, /*do nothing*/ , eif)

#define backend_enable_if(feature, eit)         \
  backend_enable_if_any(backend, feature, eit)
#define backend_enable_if_else(feature, eit, eif) \
  backend_enable_if_else_any(backend, feature, eit, eif)
#define backend_enable_if_not(feature, eif)  \
  backend_enable_if_not_any(backend, feature, eif)

#endif  /*INCLUDED_FEATURES_ENABLE_IF*/
