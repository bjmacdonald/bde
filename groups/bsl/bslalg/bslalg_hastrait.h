// bslalg_hastrait.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLALG_HASTRAIT
#define INCLUDED_BSLALG_HASTRAIT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function to detect if a type has a given trait.
//
//@DEPRECATED: Do not use.
//
//@CLASSES:
//  bslalg::HasTrait: legacy mechanism for detecting type traits
//
//@SEE_ALSO: bslalg_typetraits, bslmf_integralconstant
//
//@DESCRIPTION: This component provides a meta-function, `bslalg::HasTrait`,
// that is a legacy mechanism for detecting traits.  `bslalg::HasTrait` takes
// two template parameters, `TYPE` and `TRAIT`.
// `bslalg::HasTrait<TYPE, TRAIT>` inherits from `bsl::true_type` if the
// (template parameter) `TYPE` has the (template parameter) `TRAIT`, and
// inherits from `bsl::false_type` otherwise.
//
// `bslalg::HasTrait` is *not* a general-purpose trait detection facility.  It
// is intended to be used in conjunction with the legacy (deprecated) `bslalg`
// traits only.  In particular, the type supplied to `bslalg::HasTrait` as the
// `TRAIT` template parameter *must* be a `bslalg` trait, for example,
// `bslalg::TypeTraitUsesBslmaAllocator`.  Supplying anything else for `TRAIT`
// will (more than likely) fail to compile.  For example, the following errant
// use of `bslalg::HasTrait` will definitely fail to compile:
// ```
// bslalg::HasTrait<Foo, bsl::is_trivially_default_constructible>'
// ```
// See `bslalg_typetraits` for a complete list of the `bslalg` traits that are
// supported by `bslalg::HasTrait`.
//
// In lieu of the deprecated `bslalg` traits, the newer traits intended to
// replace them should be used instead.  These successor traits are typically
// defined in `bslmf` (two exceptions are `bslma::UsesBslmaAllocator` and
// `bslalg::HasStlIterators`).  `bslalg::HasTrait` and the `bslalg` traits that
// it supports are written in such a way that `bslalg::HasTrait` will correctly
// detect whether a type has a given trait regardless of whether the trait is
// ascribed to the type using the deprecated `BSLALG_DECLARE_NESTED_TRAITS*` or
// `BSLMF_NESTED_TRAIT_DECLARATION` macros (see `bslalg_typetraits` and
// `bslmf_nestedtraitdeclaration`, respectively), or whether the (preferred)
// C++11 idiom for defining traits is used (see `bslmf_detectnestedtrait`).  In
// this way, `bslalg::HasTrait` provides a "bridge" between legacy `bslalg`
// type traits and their replacements that facilitates the migration to the
// newer traits.
//
///Usage
///-----
// No Usage example is provided since this component is deprecated.

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>
#include <bslmf_removecv.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslmf_isconvertible.h>
#include <bslmf_removecvq.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace BloombergLP {
namespace bslalg {

                       // ===============
                       // struct HasTrait
                       // ===============

/// This meta-function evaluates to `bsl::true_type` if the (template
/// parameter) `TYPE` has the (template parameter) `TRAIT`, and to
/// `bsl::false_type` otherwise.
template <class TYPE, class TRAIT>
struct HasTrait : public bsl::integral_constant<
                      bool,
                      TRAIT::template Metafunction<
                          typename bsl::remove_cv<TYPE>::type>::value> {
};

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

#ifdef bslalg_HasTrait
#undef bslalg_HasTrait
#endif
/// This alias is defined for backward compatibility.
#define bslalg_HasTrait bslalg::HasTrait
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

}  // close enterprise namespace

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
