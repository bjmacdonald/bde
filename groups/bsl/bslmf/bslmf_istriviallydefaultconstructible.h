// bslmf_istriviallydefaultconstructible.h                            -*-C++-*-
#ifndef INCLUDED_BSLMF_ISTRIVIALLYDEFAULTCONSTRUCTIBLE
#define INCLUDED_BSLMF_ISTRIVIALLYDEFAULTCONSTRUCTIBLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for trivially default-constructible.
//
//@CLASSES:
//  bsl::is_trivially_default_constructible: trait meta-function
//  bsl::is_trivially_default_constructible_v: the result value
//
//@SEE_ALSO: bslmf_integerconstant, bslmf_nestedtraitdeclaration
//
//@DESCRIPTION: This component defines a meta-function,
// `bsl::is_trivially_default_constructible` and a template variable
// `bsl::is_trivially_default_constructible_v`, that represents the result
// value of the `bsl::is_trivially_default_constructible` meta-function, that
// may be used to query whether a type has a trivial default constructor as
// defined in section 12.1.5 of the C++11 standard [class.ctor].
//
// `bsl::is_trivially_default_constructible` has the same syntax as the
// `is_trivially_default_constructible` template from the C++11 standard
// [meta.unary.prop].  However, unlike the template defined in the C++11
// standard, which can determine the correct value for all types without
// requiring specialization, `bsl::is_trivially_default_constructible` can, by
// default, determine the value for the following type categories only:
// ```
// Type Category        Has Trivial Default Constructor
// -------------------  -------------------------------
// reference types      false
// fundamental types    true
// enums                true
// pointers             true
// pointers to members  true
// ```
// For all other types, `bsl::is_trivially_default_constructible` returns
// `false`, unless the type is explicitly specified to be trivially
// default-constructible, which can be done in two ways:
//
// 1. Define a template specialization for
//    `bsl::is_trivially_default_constructible` having the type as the template
//    parameter that inherits directly from `bsl::true_type`.
// 2. Use the `BSLMF_NESTED_TRAIT_DECLARATION` macro to define
//    `bsl::is_trivially_default_constructible` as the trait in the class
//    definition of the type.
//
// Note that the template variable `is_trivially_default_constructible_v` is
// defined in the C++17 standard as an inline variable.  If the current
// compiler supports the inline variable C++17 compiler feature,
// `bsl::is_trivially_default_constructible_v` is defined as an
// `inline constexpr bool` variable.  Otherwise, if the compiler supports the
// variable templates C++14 compiler feature,
// `bsl::is_trivially_default_constructible_v` is defined as a non-inline
// `constexpr bool` variable.  See
// `BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES` and
// `BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES` macros in
// bsls_compilerfeatures component for details.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Whether Types are Trivially Default-Constructible
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a type is trivially
// default-constructible.
//
// First, we define a set of types to evaluate:
// ```
// typedef int MyFundamentalType;
// typedef int& MyFundamentalTypeReference;
//
// class MyTriviallyDefaultConstructibleType {
// };
//
// struct MyNonTriviallyDefaultConstructibleType {
//
//     int d_data;
//
//     MyNonTriviallyDefaultConstructibleType()
//     : d_data(1)
//     {
//     }
// };
// ```
// Then, since user-defined types cannot be automatically evaluated by
// `is_trivially_default_constructible`, we define a template specialization to
// specify that `MyTriviallyDefaultConstructibleType` is trivially
// default-constructible:
// ```
// namespace bsl {
//
// template <>
// struct is_trivially_default_constructible<
//                     MyTriviallyDefaultConstructibleType> : bsl::true_type {
//     // This template specialization for
//     // 'is_trivially_default_constructible' indicates that
//     // 'MyTriviallyDefaultConstructibleType' is a trivially
//     // default-constructible type.
// };
//
// }  // close namespace bsl
// ```
// Now, we verify whether each type is trivially default-constructible using
// `bsl::is_trivially_default_constructible`:
// ```
// assert(true  ==
//         bsl::is_trivially_default_constructible<MyFundamentalType>::value);
// assert(false ==
//     bsl::is_trivially_default_constructible<
//                                        MyFundamentalTypeReference>::value);
// assert(true  ==
//     bsl::is_trivially_default_constructible<
//                               MyTriviallyDefaultConstructibleType>::value);
// assert(false ==
//     bsl::is_trivially_default_constructible<
//                            MyNonTriviallyDefaultConstructibleType>::value);
// ```
// Note that if the current compiler supports the variable templates C++14
// feature, then we can re-write the snippet of code above as follows:
// ```
// #ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
//   assert(true  ==
//           bsl::is_trivially_default_constructible_v<MyFundamentalType>);
//   assert(false ==
//       bsl::is_trivially_default_constructible_v<MyFundamentalTypeReference>);
//   assert(true  ==
//       bsl::is_trivially_default_constructible_v<
//                                        MyTriviallyDefaultConstructibleType>);
//   assert(false ==
//       bsl::is_trivially_default_constructible_v<
//                                     MyNonTriviallyDefaultConstructibleType>);
// #endif
// ```

#include <bslscm_version.h>

#include <bslmf_detectnestedtrait.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isenum.h>
#include <bslmf_isfundamental.h>
#include <bslmf_ismemberpointer.h>
#include <bslmf_ispointer.h>
#include <bslmf_isreference.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>

#include <stddef.h>

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
# include <type_traits>
#endif

namespace bsl {

template <class t_TYPE>
struct is_trivially_default_constructible;

}  // close namespace bsl

namespace BloombergLP {
namespace bslmf {

           // ==================================================
           // struct IsTriviallyDefaultConstructible_DetectTrait
           // ==================================================

/// This `struct` template implements a meta-function to determine whether
/// the (non-cv-qualified) (template parameter) `t_TYPE` has been explicitly
/// tagged with the trivially default constructible trait.  If the flag
/// `t_K_INTRINSIC` is `true` then the compiler has already determined that
/// `t_TYPE` is trivially default constructible without user intervention,
/// and the check for nested traits can be optimized away.
template <class t_TYPE, bool t_K_INTRINSIC = false>
struct IsTriviallyDefaultConstructible_DetectTrait
: DetectNestedTrait<t_TYPE, bsl::is_trivially_default_constructible>::type {
};

/// This `struct` template implements a meta-function to determine whether
/// the (non-cv-qualified) (template parameter) `t_TYPE` is trivially
/// default constructible.
template <class t_TYPE>
struct IsTriviallyDefaultConstructible_DetectTrait<t_TYPE, true>
: bsl::true_type {
};

               // ==========================================
               // struct IsTriviallyDefaultConstructible_Imp
               // ==========================================

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
/// This `struct` template implements a meta-function to determine whether
/// the (non-cv-qualified) (template parameter) `t_TYPE` is trivially
/// default constructible.
template <class t_TYPE>
struct IsTriviallyDefaultConstructible_Imp
: IsTriviallyDefaultConstructible_DetectTrait<
      t_TYPE,
      ::std::is_trivially_default_constructible<t_TYPE>::value>::type {
};
#else
/// This `struct` template implements a meta-function to determine whether
/// the (non-cv-qualified) (template parameter) `t_TYPE` is trivially
/// default-constructible.
template <class t_TYPE>
struct IsTriviallyDefaultConstructible_Imp
: IsTriviallyDefaultConstructible_DetectTrait<
      t_TYPE,
      bsl::is_fundamental<t_TYPE>::value ||
      bsl::is_enum<t_TYPE>::value ||
      bsl::is_pointer<t_TYPE>::value ||
      bsl::is_member_pointer<t_TYPE>::value>::type {
};


/// This explicit specialization reports that `void` is not a trivially
/// default constructible type, despite being a fundamental type.
template <>
struct IsTriviallyDefaultConstructible_Imp<void> : bsl::false_type {
};
#endif


}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                // =========================================
                // struct is_trivially_default_constructible
                // =========================================

/// This `struct` template implements a meta-function to determine whether
/// the (template parameter) `t_TYPE` is trivially default-constructible.
/// This `struct` derives from `bsl::true_type` if the `t_TYPE` is trivially
/// default-constructible, and from `bsl::false_type` otherwise.  This
/// meta-function has the same syntax as the
/// `is_trivially_default_constructible` meta-function defined in the C++11
/// standard [meta.unary.prop]; however, this meta-function can
/// automatically determine the value for the following types only:
/// reference types, fundamental types, enums, pointers to members, and
/// types declared to have the `bsl::is_trivially_default_constructible`
/// trait using the `BSLMF_NESTED_TRAIT_DECLARATION` macro (and the value
/// for other types defaults to `false`).  To support other trivially
/// default-constructible types, this template must be specialized to
/// inherit from `bsl::true_type` for them.
template <class t_TYPE>
struct is_trivially_default_constructible
: BloombergLP::bslmf::IsTriviallyDefaultConstructible_Imp<t_TYPE>::type {
};

/// This partial specialization ensures that const-qualified types have the
/// same result as their element type.
template <class t_TYPE>
struct is_trivially_default_constructible<const t_TYPE>
: is_trivially_default_constructible<t_TYPE>::type {
};

/// This partial specialization ensures that volatile-qualified types have
/// the same result as their element type.
template <class t_TYPE>
struct is_trivially_default_constructible<volatile t_TYPE>
: is_trivially_default_constructible<t_TYPE>::type {
};

/// This partial specialization ensures that const-volatile-qualified types
/// have the same result as their element type.
template <class t_TYPE>
struct is_trivially_default_constructible<const volatile t_TYPE>
: is_trivially_default_constructible<t_TYPE>::type {
};

/// This partial specialization ensures that array types have the same
/// result as their element type.
template <class t_TYPE, size_t t_LEN>
struct is_trivially_default_constructible<t_TYPE[t_LEN]>
: is_trivially_default_constructible<t_TYPE>::type {
};

/// This partial specialization ensures that const-qualified array types
/// have the same result as their element type.
template <class t_TYPE, size_t t_LEN>
struct is_trivially_default_constructible<const t_TYPE[t_LEN]>
: is_trivially_default_constructible<t_TYPE>::type {
};

/// This partial specialization ensures that volatile-qualified array types
/// have the same result as their element type.
template <class t_TYPE, size_t t_LEN>
struct is_trivially_default_constructible<volatile t_TYPE[t_LEN]>
: is_trivially_default_constructible<t_TYPE>::type {
};

/// This partial specialization ensures that const-volatile-qualified array
/// types have the same result as their element type.
template <class t_TYPE, size_t t_LEN>
struct is_trivially_default_constructible<const volatile t_TYPE[t_LEN]>
: is_trivially_default_constructible<t_TYPE>::type {
};

#if !defined(BSLS_PLATFORM_CMP_IBM)
// Last checked with the xlC 12.1 compiler.  The IBM xlC compiler has problems
// correctly handling arrays of unknown bound as template parameters.

/// This partial specialization ensures that array-of-unknown-bound types
/// have the same result as their element type.
template <class t_TYPE>
struct is_trivially_default_constructible<t_TYPE[]>
: is_trivially_default_constructible<t_TYPE>::type {
};

/// This partial specialization ensures that const-qualified
/// array-of-unknown-bound types have the same result as their element type.
template <class t_TYPE>
struct is_trivially_default_constructible<const t_TYPE[]>
: is_trivially_default_constructible<t_TYPE>::type {
};

/// This partial specialization ensures that volatile-qualified
/// array-of-unknown-bound types have the same result as their element type.
template <class t_TYPE>
struct is_trivially_default_constructible<volatile t_TYPE[]>
: is_trivially_default_constructible<t_TYPE>::type {
};

/// This partial specialization ensures that const-volatile-qualified
/// array-of-unknown-bound types have the same result as their element type.
template <class t_TYPE>
struct is_trivially_default_constructible<const volatile t_TYPE[]>
: is_trivially_default_constructible<t_TYPE>::type {
};
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
/// This template variable represents the result value of the
/// `bsl::is_trivially_default_constructible` meta-function.
template <class t_TYPE>
BSLS_KEYWORD_INLINE_VARIABLE constexpr bool
    is_trivially_default_constructible_v =
                             is_trivially_default_constructible<t_TYPE>::value;
#endif

}  // close namespace bsl

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
