// bsl_valarray.h                                                     -*-C++-*-
#ifndef INCLUDED_BSL_VALARRAY
#define INCLUDED_BSL_VALARRAY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@DESCRIPTION: Provide types, in the `bsl` namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the `std` namespace (if any) into the `bsl` namespace.

#include <valarray>

#include <bsls_libraryfeatures.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES


namespace bsl {
    // Import selected symbols into bsl namespace
    using std::abs;
    using std::acos;
    using std::asin;
    using std::atan2;
    using std::atan;
    using std::cos;
    using std::cosh;
    using std::exp;
    using std::gslice;
    using std::gslice_array;
    using std::indirect_array;
    using std::log10;
    using std::log;
    using std::mask_array;
    using std::pow;
    using std::sin;
    using std::sinh;
    using std::slice;
    using std::slice_array;
    using std::sqrt;
    using std::tan;
    using std::tanh;
    using std::valarray;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    using std::begin;
    using std::end;
#endif

}  // close package namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
