// bslfmt_formattercharacter.cpp                                      -*-C++-*-

#include <bslfmt_formattercharacter.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslfmt_formattercharacter_cpp, "$Id$ $CSID$")

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)

#include <bslmf_assert.h>

BSLMF_ASSERT((!BloombergLP::bslfmt::FormatterBase_IsStdAliasingEnabled<
              bsl::formatter<char, char> >::value));

BSLMF_ASSERT((!BloombergLP::bslfmt::FormatterBase_IsStdAliasingEnabled<
              bsl::formatter<wchar_t, wchar_t> >::value));

BSLMF_ASSERT((!BloombergLP::bslfmt::FormatterBase_IsStdAliasingEnabled<
              bsl::formatter<char, wchar_t> >::value));

#endif

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
