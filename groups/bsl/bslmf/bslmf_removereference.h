// bslmf_removereference.h                                            -*-C++-*-
#ifndef INCLUDED_BSLMF_REMOVEREFERENCE
#define INCLUDED_BSLMF_REMOVEREFERENCE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for stripping reference-ness from types.
//
//@CLASSES:
//  bsl::remove_reference: standard meta-function for stripping reference-ness
//  bsl::remove_reference_t: alias to the return type of the meta-function
//  bslmf::RemoveReference: meta-function for stripping reference-ness
//
//@SEE_ALSO: bslmf_addreference
//
//@DESCRIPTION: This component defines two meta-functions,
// `bsl::remove_reference` and `BloombergLP::bslmf::RemoveReference`, both of
// which may be used to strip reference-ness (including both lvalue and rvalue
// reference-ness, if the latter is supported by the compiler) from a type.
//
// `bsl::remove_reference` meets the requirements of the `remove_reference`
// template defined in the C++11 standard [meta.trans.ref], while
// `bslmf::RemoveReference` was devised before `remove_reference` was
// standardized.
//
// The two meta-functions are functionally equivalent.  The major difference
// between them is that the result for `bsl::remove_reference` is indicated by
// the class member `type`, while the result for `bslmf::RemoveReference` is
// indicated by the class member `Type`.
//
// Note that `bsl::remove_reference` should be preferred over
// `bslmf::RemoveReference`, and in general, should be used by new components.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Remove Reference-ness of Types
///- - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to remove the reference-ness of a set of types.
//
// Now, remove the reference-ness of a set of types using
// `bsl::remove_reference` and verify that the returned type has any
// reference-ness removed:
// ```
// assert(true  ==
//           (bsl::is_same<bsl::remove_reference<int& >::type, int >::value));
// assert(false ==
//           (bsl::is_same<bsl::remove_reference<int& >::type, int&>::value));
// assert(true  ==
//           (bsl::is_same<bsl::remove_reference<int  >::type, int >::value));
// #if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
// assert(true ==
//           (bsl::is_same<bsl::remove_reference<int&&>::type, int >::value));
// #endif
// ```
// Finally, if the current compiler supports alias templates C++11 feature, we
// remove reference-ness from a set of types using `bsl::remove_reference_t`
// and verify that the resulting type has any reference-ness removed:
// ```
// #ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
//   assert(true  ==
//             (bsl::is_same<bsl::remove_reference_t<int& >, int >::value));
//   assert(false ==
//             (bsl::is_same<bsl::remove_reference_t<int& >, int&>::value));
//   assert(true  ==
//             (bsl::is_same<bsl::remove_reference_t<int  >, int >::value));
// #if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
//   assert(true ==
//             (bsl::is_same<bsl::remove_reference_t<int&&>, int >::value));
// #endif
// #endif
// ```
// Note that rvalue reference is a feature introduced in the C++11 standard and
// may not be supported by all compilers.
//
// Also note, that the `bsl::remove_reference_t` avoids the `::type` suffix and
// `typename` prefix when we want to use the result of the
// `bsl::remove_reference` meta-function in templates.

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>

#if BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
#include <type_traits> // 'std::remove_reference', 'std_remove_reference_t'
#endif

                         // =======================
                         // struct remove_reference
                         // =======================

namespace bsl {

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
using std::remove_reference;
#else

/// This `struct` template implements the `remove_reference` meta-function
/// defined in the C++11 standard [meta.trans.ref], providing an alias,
/// `type`, that returns the result.  `type` has the same type as the
/// (template parameter) `t_TYPE` except with reference-ness removed.  Note
/// that this generic default template provides a `type` that is an alias to
/// `t_TYPE` for when `t_TYPE` is not a reference.  A template
/// specialization is provided (below) that removes reference-ness for when
/// `t_TYPE` is a reference.
template <class t_TYPE>
struct remove_reference {

    /// This `typedef` is an alias to the (template parameter) `t_TYPE`.
    typedef t_TYPE type;
};

/// This partial specialization of `bsl::remove_reference`, for when the
/// (template parameter) `t_TYPE` is an rvalue reference, provides a
/// `typedef`, `type`, that has reference-ness of `t_TYPE` removed.
template <class t_TYPE>
struct remove_reference<t_TYPE&> {

    /// This `typedef` is an alias to the same type as the (template
    /// parameter) `t_TYPE` except with the reference-ness removed.
    typedef t_TYPE type;
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

template <class t_TYPE>
struct remove_reference<t_TYPE&&> {
    // This partial specialization of 'bsl::remove_reference', for when the
    // (template parameter) 't_TYPE' is a reference, provides a 'typedef',
    // 'type', that has reference-ness of 't_TYPE' removed.

    typedef t_TYPE type;
        // This 'typedef' is an alias to the same type as the (template
        // parameter) 't_TYPE' except with the reference-ness removed.
};

#endif
#endif //  BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
using std::remove_reference_t;
#else
#if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES)
// ALIASES
template <class t_TYPE>
using remove_reference_t = typename remove_reference<t_TYPE>::type;
    // 'remove_reference_t' is an alias to the return type of the
    // 'bsl::remove_reference' meta-function.  Note, that the
    // 'remove_reference_t' avoids the '::type' suffix and 'typename' prefix
    // when we want to use the result of the meta-function in templates.
#endif // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

#endif // BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY

}  // close namespace bsl

namespace BloombergLP {
namespace bslmf {

                         // ======================
                         // struct RemoveReference
                         // ======================

/// This `struct` template implements a meta-function to remove the
/// reference-ness from the (template parameter) `t_TYPE`.  Note that
/// although this `struct` is functionally equivalent to
/// `bsl::remove_reference`, the use of `bsl::remove_reference` should be
/// preferred.
template <class t_TYPE>
struct RemoveReference {

    /// This `typedef` is an alias to the same type as the (template
    /// parameter) `t_TYPE` except with any reference-ness removed.
    typedef typename bsl::remove_reference<t_TYPE>::type Type;
};

}  // close package namespace
}  // close enterprise namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

#ifdef bslmf_RemoveReference
#undef bslmf_RemoveReference
#endif
/// This alias is defined for backward compatibility.
#define bslmf_RemoveReference bslmf::RemoveReference
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

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
