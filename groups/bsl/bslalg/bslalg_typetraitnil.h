// bslalg_typetraitnil.h                                              -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITNIL
#define INCLUDED_BSLALG_TYPETRAITNIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a trait to mark classes as having no other traits.
//
//@DEPRECATED: Do not use.
//
//@CLASSES:
//  bslalg::TypeTraitNil: trait possessed by all types
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a single traits class,
// `bslalg::TypeTraitNil`.  All types have this trait.
//
///Usage
///-----
// No Usage example is provided since this component is deprecated.

#include <bslscm_version.h>

#include <bslmf_integralconstant.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslmf_nil.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace BloombergLP {

namespace bslalg {

                            //=============
                            // TypeTraitNil
                            //=============

/// Nil trait -- every type has this trait.
struct TypeTraitNil
{

    /// This meta-function returns `true_type` for any type that is queried
    /// for the nil trait.
    template <class TYPE>
    struct NestedTraitDeclaration
    {
    };

    template <class TYPE>
    struct Metafunction : bsl::true_type { };
};

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

#ifdef bslalg_TypeTraitNil
#undef bslalg_TypeTraitNil
#endif
/// This alias is defined for backward compatibility.
#define bslalg_TypeTraitNil bslalg::TypeTraitNil
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
