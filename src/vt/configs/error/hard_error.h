/*
//@HEADER
// ************************************************************************
//
//                          hard_error.h
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

#if !defined INCLUDED_CONFIGS_ERROR_HARD_ERROR_H
#define INCLUDED_CONFIGS_ERROR_HARD_ERROR_H

/*
 *  A hard error is always checked and leads to failure in any mode if
 *  triggered
 */

#include "vt/configs/debug/debug_config.h"
#include "vt/configs/types/types_type.h"
#include "vt/configs/error/common.h"
#include "vt/configs/error/error.h"

#include <string>
#include <tuple>
#include <type_traits>

#if backend_check_enabled(production)
  #define vtAbort(str,...)                                            \
    ::vt::error::display(str,1,  outputArgsImpl(__VA_ARGS__));
  #define vtAbortCode(xy,str,...)                                     \
    ::vt::error::display(str,xy, outputArgsImpl(__VA_ARGS__));
#else
  #define vtAbort(str,...)                                            \
    ::vt::error::displayLoc(str,1, DEBUG_LOCATION, outputArgsImpl(__VA_ARGS__));
  #define vtAbortCode(xy,str,...)                                     \
    ::vt::error::displayLoc(str,xy,DEBUG_LOCATION, outputArgsImpl(__VA_ARGS__));
#endif

#define vtAbortIf(cond,str,...)                                         \
  do {                                                                  \
    if ((cond)) {                                                       \
      vtAbort(str,__VA_ARGS__);                                         \
    }                                                                   \
  } while (false)
#define vtAbortIfCode(code,cond,str,...)                                \
  do {                                                                  \
    if ((cond)) {                                                       \
      vtAbortCode(code,str,__VA_ARGS__);                                \
    }                                                                   \
  } while (false)

#define vtAbortIfNot(cond,str,...)                                    \
  vtAbortIf(INVERT_COND(cond),str,__VA_ARGS__)
#define vtAbortIfNotCode(code,cond,str,...)                           \
  vtAbortIfCode(code,INVERT_COND(cond),str,__VA_ARGS__)

#endif /*INCLUDED_CONFIGS_ERROR_HARD_ERROR_H*/
