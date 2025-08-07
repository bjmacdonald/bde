// bslfmt_formatterintegral.cpp                                       -*-C++-*-

#include <bslfmt_formatterintegral.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslfmt_formatterintegral_cpp, "$Id$ $CSID$")

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)

BSLMF_ASSERT((!BloombergLP::bslfmt::FormatterBase_IsStdAliasingEnabled<
              bsl::formatter<int, char> >::value));

BSLMF_ASSERT((!BloombergLP::bslfmt::FormatterBase_IsStdAliasingEnabled<
              bsl::formatter<int, wchar_t> >::value));

BSLMF_ASSERT((!BloombergLP::bslfmt::FormatterBase_IsStdAliasingEnabled<
              bsl::formatter<unsigned, char> >::value));

BSLMF_ASSERT((!BloombergLP::bslfmt::FormatterBase_IsStdAliasingEnabled<
              bsl::formatter<unsigned, wchar_t> >::value));

BSLMF_ASSERT((!BloombergLP::bslfmt::FormatterBase_IsStdAliasingEnabled<
              bsl::formatter<long long, char> >::value));

BSLMF_ASSERT((!BloombergLP::bslfmt::FormatterBase_IsStdAliasingEnabled<
              bsl::formatter<long long, wchar_t> >::value));

BSLMF_ASSERT((!BloombergLP::bslfmt::FormatterBase_IsStdAliasingEnabled<
              bsl::formatter<unsigned long long, char> >::value));

BSLMF_ASSERT((!BloombergLP::bslfmt::FormatterBase_IsStdAliasingEnabled<
              bsl::formatter<unsigned long long, wchar_t> >::value));

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
