// bdlb_cstringhash.h                                                 -*-C++-*-
#ifndef INCLUDED_BDLB_CSTRINGHASH
#define INCLUDED_BDLB_CSTRINGHASH

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a functor enabling C-strings as unordered-container keys.
//
//@CLASSES:
//  bdlb::CStringHash: functor enabling C-strings as unordered-container keys
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a `struct`, `bdlb::CStringHash`, that
// defines a functor to generate a hash code for a null-terminated string,
// rather than simply generating a hash code for the address of the string, as
// the `std::hash` functor would do.  This hash functor is suitable for
// supporting C-strings as keys in unordered associative containers.  Note that
// the container behavior would be undefined if the strings referenced by such
// pointers were to change value.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of `bdlb::CStringHash`
///- - - - - - - - - - - - - - - - - - - - - -
// Suppose we need an associative container to store some objects, uniquely
// identified by C-strings.   The following code illustrates how to use
// `bdlb::CStringHash` as a hash function for the standard container
// `unordered_map` taking C-string as a key.
//
// First, let us define our mapped type class:
// ```
// /// This class implements a value semantic type that represents
// /// ownership of a security.
// class Security
// {
//
//     // DATA
//     char         *d_name_p;      // Security name
//     unsigned int  d_sharesOwned; // The number of owned shares
//
//   public:
//     // CREATORS
//
//     /// Create a `Security` object having the specified `name` and
//     /// `sharesOwned`.
//     Security(const char *name, unsigned int sharesOwned);
//
//     /// Create a `Security` object having the value of the specified
//     /// `original` security.
//     Security(const Security& original);
//
//     /// Destroy this security object.
//     ~Security();
//
//
//     // ACCESSORS
//
//     /// Return the value of the `sharesOwned` attribute of this security
//     /// object.
//     unsigned int sharesOwned() const;
//
//     // MANIPULATORS
//
//     /// Assign to this security object the value of the specified
//     /// `other` object, and return a reference providing modifiable
//     /// access to this object.
//     Security& operator=(Security other);
//
//     /// Efficiently exchange the value of this security with the value
//     /// of the specified `other` security.
//     void swap(Security& other);
// };
//
// // CREATORS
// inline
// Security::Security(const char *name, unsigned int sharesOwned)
// : d_sharesOwned(sharesOwned)
// {
//     d_name_p = new char [strlen(name) + 1];
//     strncpy(d_name_p, name, strlen(name) + 1);
// }
//
// inline
// Security::Security(const Security& original)
// : d_sharesOwned(original.d_sharesOwned)
// {
//     if (this != &original)
//     {
//         d_name_p = new char [strlen(original.d_name_p) + 1];
//         strncpy(d_name_p,
//                 original.d_name_p,
//                 strlen(original.d_name_p) + 1);
//     }
// }
//
// inline
// Security::~Security()
// {
//     delete [] d_name_p;
// }
//
// // ACCESSORS
//
// inline    unsigned int Security::sharesOwned() const
// {
//     return d_sharesOwned;
// }
//
// // MANIPULATORS
// inline
// Security& Security::operator=(Security other)
// {
//     this->swap(other);
//     return *this;
// }
//
// inline
// void Security::swap(Security& other)
// {
//     char * tempPtr = d_name_p;
//     d_name_p = other.d_name_p;
//     other.d_name_p = tempPtr;
//     unsigned int tempInt = d_sharesOwned;
//     d_sharesOwned = other.d_sharesOwned;
//     other.d_sharesOwned = tempInt;
// }
// ```
// Next, we define container type using `bdlb::CStringHash` as a hash function
// and `bdlb::CstringEqualTo` as a comparator:
// ```
// typedef unordered_map<const char *,
//                       Security,
//                       bdlb::CStringHash,
//                       bdlb::CStringEqualTo> SecuritiesUM;
// ```
// This container stores objects of `Security` class and allow access to them
// by their names.
//
// Then, we create several C-strings with security names:
// ```
// const char *ibm  = "IBM";
// const char *msft = "Microsoft";
// const char *goog = "Google";
// ```
// Now, we create a container for securities and fill it:
// ```
// SecuritiesUM securities;
//
// securities.insert(
//           std::make_pair<const char *, Security>(ibm, Security(ibm, 616)));
// securities.insert(
//     std::make_pair<const char *, Security>(msft, Security(msft, 6150000)));
// ```
// Finally, we make sure, that we able to access securities by their names:
// ```
// SecuritiesUM::iterator it = securities.find(ibm);
// assert(616 == it->second.sharesOwned());
// it = securities.find(msft);
// assert(6150000 == it->second.sharesOwned());
// it = securities.find(goog);
// assert(securities.end() == it);
// ```

#include <bdlscm_version.h>

#include <bslh_spookyhashalgorithm.h>

#include <bslmf_integralconstant.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_assert.h>
#include <bsls_review.h>

#include <bsl_cstring.h>
#include <bsl_cstddef.h>

namespace BloombergLP {
namespace bdlb {

                           // ==================
                           // struct CStringHash
                           // ==================

/// This `struct` defines a hash operation for null-terminated character
/// strings enabling them to be used as keys in the standard unordered
/// associative containers such as `bsl::unordered_map` and
/// `bsl::unordered_set`.  Note that this class is an empty POD type.
struct CStringHash {

    // STANDARD TYPEDEFS
    typedef const char  *argument_type;
    typedef bsl::size_t  result_type;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(CStringHash, bsl::is_trivially_copyable);
    BSLMF_NESTED_TRAIT_DECLARATION(CStringHash,
                                   bsl::is_trivially_default_constructible);

    /// Create a `CStringHash` object.
    //! CStringHash() = default;

    /// Create a `CStringHash` object.  Note that as `CStringHash` is an
    /// empty (stateless) type, this operation will have no observable
    /// effect.
    //! CStringHash(const CStringHash& original) = default;

    /// Destroy this object.
    //! ~CStringHash() = default;

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// a return a reference providing modifiable access to this object.
    /// Note that as `CStringHash` is an empty (stateless) type, this
    /// operation will have no observable effect.
    //! CStringHash& operator=(const CStringHash& rhs) = default;

    // ACCESSORS

    /// Return a hash code generated from the contents of the specified
    /// null-terminated `argument` string.  The behavior is undefined
    /// unless `argument` points to a null-terminated string.
    bsl::size_t operator()(const char *argument) const;
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // ------------------
                           // struct CStringHash
                           // ------------------

// ACCESSORS
inline
bsl::size_t bdlb::CStringHash::operator()(const char *argument) const
{
    BSLS_ASSERT(argument);

    bslh::SpookyHashAlgorithm hash;
    hash(argument, bsl::strlen(argument));
    return static_cast<bsl::size_t>(hash.computeHash());
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
