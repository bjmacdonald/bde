// bslalg_typetraitpair.h                                             -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITPAIR
#define INCLUDED_BSLALG_TYPETRAITPAIR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a trait to mark classes as similar to `std::pair`.
//
//@DEPRECATED: Use `bslmf_ispair` instead.
//
//@CLASSES:
//  bslalg::TypeTraitPair: for `std::pair`-like classes
//
//@SEE_ALSO: bslalg_typetraits
//
//@DESCRIPTION: This component provides a single traits class,
// `bslalg::TypeTraitPair`, which allows the trait `bslmf::IsPair` to be
// declared using the (deprecated) `BSLALG_DECLARE_NESTED_TRAITS` macro.  See
// the `bslmf_ispair` component for details of this trait.
//
///Usage
///-----
// No Usage example is provided since this component is deprecated.

#include <bslscm_version.h>

#include <bslmf_ispair.h>
#include <bslmf_nestedtraitdeclaration.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslmf_detectnestedtrait.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace BloombergLP {

namespace bslalg {

                        // ====================
                        // struct TypeTraitPair
                        // ====================

/// A `TYPE` with this trait has two data members, `first` and `second`
/// of types `T::first_type` and `T::second_type`, respectively.
struct TypeTraitPair {

    /// This class template ties the `bslalg::TypeTraitPair` trait tag to
    /// the `bslmf::IsPair` trait metafunction.
    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE, bslmf::IsPair>
    {
    };

    template <class TYPE>
    struct Metafunction : bslmf::IsPair<TYPE>::type { };
};

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

/// This alias is defined for backward compatibility.
typedef bslalg::TypeTraitPair bslalg_TypeTraitPair;
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
