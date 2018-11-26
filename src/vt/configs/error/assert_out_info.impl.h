/*
//@HEADER
// ************************************************************************
//
//                          assert_out_info.impl.h
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

#if !defined INCLUDED_CONFIGS_ERROR_ASSERT_OUT_INFO_IMPL_H
#define INCLUDED_CONFIGS_ERROR_ASSERT_OUT_INFO_IMPL_H

#include "vt/configs/error/common.h"
#include "vt/configs/types/types_type.h"
#include "vt/configs/error/assert_out.h"
#include "vt/configs/error/assert_out_info.h"
#include "vt/configs/error/keyval_printer.h"

#include <cassert>
#include <tuple>
#include <type_traits>
#include <string>

#include <fmt/format.h>

namespace vt { namespace debug { namespace assert {

template <typename... Args, typename... Args2>
inline
std::enable_if_t<std::tuple_size<std::tuple<Args...>>::value == 0>
assertOutInfo(
  bool fail, std::string const cond, std::string const& str,
  std::string const& file, int const line, std::string const& func,
  ErrorCodeType error, std::tuple<Args2...> tup, Args... args
) {
  return assertOut(fail,cond,str,file,line,func,error,args...);
}

template <typename... Args, typename... Args2>
inline
std::enable_if_t<std::tuple_size<std::tuple<Args...>>::value != 0>
assertOutInfo(
  bool fail, std::string const cond, std::string const& str,
  std::string const& file, int const line, std::string const& func,
  ErrorCodeType error, std::tuple<Args2...> t1, Args... args
) {
  using KeyType = std::tuple<Args2...>;
  using ValueType = std::tuple<Args...>;
  static constexpr auto size = std::tuple_size<KeyType>::value;
  using PrinterType = util::error::PrinterNameValue<size-1,KeyType,ValueType>;

  // Output the standard assert message
  assertOut(false,cond,str,file,line,func,error);

  // Output each expression computed passed to the function along with the
  // computed value of that passed expression
  auto const t2 = std::make_tuple(args...);
  auto varlist = PrinterType::make(t1,t2);

  std::string state = ::fmt::format("{:*^80}\n\n", " DEBUG STATE ");
  for (auto&& var : varlist) {
    state += var + "\n";
  }
  state += "\n";
  state += ::fmt::format("{:*^80}\n", "");
  state += "\n";
  ::vt::output(state,error,false,false);

  if (fail) {
    assert(false);
  }
}

}}} /* end namespace vt::debug::assert */

#endif /*INCLUDED_CONFIGS_ERROR_ASSERT_OUT_INFO_IMPL_H*/
