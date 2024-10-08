// bslstl_iserrorcodeenum.h                                           -*-C++-*-
#ifndef INCLUDED_BSLSTL_ISERRORCODEENUM
#define INCLUDED_BSLSTL_ISERRORCODEENUM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

// BDE_VERIFY pragma: -TP25  // CLASSES are not defined in C++11

//@PURPOSE: Provide a compliant standard `is_error_code_enum` trait.
//
//@CLASSES:
//  bsl::is_error_code_enum: standard compliant version of `is_error_code_enum`
//
//@MACROS:
//  BSL_IS_ERROR_CODE_ENUM_NAMESPACE:  namespace to specialize the trait
//
//@CANONICAL_HEADER: bsl_system_error.h
//
//@DESCRIPTION: This component defines a class template,
// `bsl::is_error_code_enum`, intended to be specialized for enumeration types
// that are designated as error codes for the `<system_error>` facility.  In
// C++11 mode, the vendor-supplied `<system_error>` implementation is used
// instead, and the corresponding names from `std` are imported into `bsl`.
// This component also defines a macro, `BSL_IS_ERROR_CODE_ENUM_NAMESPACE`, to
// be used as the namespace in which to write specializations of
// `is_error_code_enum`.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Dedicated Error Values
///- - - - - - - - - - - - - - - - -
// Suppose we have a dedicated system with a set of possible errors, and we
// want to be able to throw descriptive exceptions when an error occurs.  We
// need to work with the `<system_error>` facility to support this, starting by
// marking the enumeration type that defines the error literals as eligible to
// participate.  We can use `bsl::is_error_code_enum` to do this.
//
// First, we define the set of error values for our system.
// ```
// struct CarError {
//     // TYPES
//     enum Enum {
//         k_CAR_WHEELS_CAME_OFF = 1,
//         k_CAR_ENGINE_FELL_OUT = 2
//     };
// };
// ```
// Then, we enable the trait marking this as an error code.
// ```
// namespace BSL_IS_ERROR_CODE_ENUM_NAMESPACE {
// template <> struct is_error_code_enum<CarError::Enum>
// : bsl::public true_type { };
// }  // close namespace BSL_IS_ERROR_CODE_ENUM_NAMESPACE
// ```
// Finally, we verify that the trait marks our type as eligible.
// ```
// assert(is_error_code_enum<CarError::Enum>::value);
// ```

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

// BDE_VERIFY pragma: -SLM01  // Do not complain about macro leaking

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

#include <system_error>

#define BSL_IS_ERROR_CODE_ENUM_NAMESPACE std

namespace bsl {
using std::is_error_code_enum;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
using std::is_error_code_enum_v;
#elif defined BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class TYPE>
#ifdef BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES
inline
#endif
BSLS_KEYWORD_CONSTEXPR bool is_error_code_enum_v =
    is_error_code_enum<TYPE>::value;
#endif
}  // close namespace bsl

#else

#define BSL_IS_ERROR_CODE_ENUM_NAMESPACE bsl

namespace bsl {
                         // =========================
                         // struct is_error_code_enum
                         // =========================

/// This class template represents a trait defining whether the specified
/// enumeration type `TYPE` is to be treated as an error code by the
/// `error_code` template methods.
template <class TYPE>
struct is_error_code_enum : public bsl::false_type
{
};

}  // close namespace bsl

#endif
#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
