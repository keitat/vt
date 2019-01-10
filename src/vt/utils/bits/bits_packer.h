/*
//@HEADER
// ************************************************************************
//
//                          bits_packer.h
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

#if !defined INCLUDED_BITS_PACKER
#define INCLUDED_BITS_PACKER

#include <cassert>
#include <cstdlib>

namespace vt { namespace utils {

struct BitPacker {
  using FieldType         = int64_t;
  using FieldUnsignedType = uint64_t;

  template <FieldType start, FieldType len, typename BitType, typename BitField>
  static inline BitType getField(BitField const& field);

  template <FieldType start, FieldType len, typename BitType, typename BitField>
  static inline void setField(BitField& field, BitType const& segment);

  template <FieldType start, FieldType len = 1, typename BitField>
  static inline void boolSetField(BitField& field, bool const& set_value);

  template <FieldType start, FieldType len = 1, typename BitField>
  static inline bool boolGetField(BitField const& field);

public:
  template <typename BitType, typename BitField>
  static inline BitType getFieldDynamic(
    FieldType start, FieldType len, BitField const& field
  );

  template <typename BitType, typename BitField>
  static inline void setFieldDynamic(
    FieldType start, FieldType len, BitField& field, BitType const& segment
  );

private:
  template <typename BitField>
  static inline FieldUnsignedType getMsbBit(BitField const& field);

  template <FieldType start, FieldType len, typename BitType, typename BitField>
  static inline void bitorSetField(BitField& field, BitType const& segment);

};

}}  // end namespace vt::utils

#include "vt/utils/bits/bits_packer.impl.h"

#endif  /*INCLUDED_BITS_PACKER*/
