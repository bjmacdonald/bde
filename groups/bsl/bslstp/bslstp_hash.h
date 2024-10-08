// bslstp_hash.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLSTP_HASH
#define INCLUDED_BSLSTP_HASH

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace for hash functions.
//
//@INTERNAL_DEPRECATED: Do not use directly.
//
//@CLASSES:
//          bslstp::Hash: hash function for primitive types
//   bslstp::HashCString: hash function pointers to null-terminated strings
//  bslstp::HashSelector: metafunction to select a preferred hash functor type
//
//@SEE_ALSO: bslalg_hashutil
//
//@DESCRIPTION: This component provides a namespace for hash functions used by
// `hash_map` and `hash_set`.
//
// Note that the hash functions here are based on STLPort's implementation,
// with copyright notice as follows:
// ```
// -----------------------------------------------------------------------------
//  Copyright (c) 1996-1998
//  Silicon Graphics Computer Systems, Inc.
//
//  Permission to use, copy, modify, distribute and sell this software
//  and its documentation for any purpose is hereby granted without fee,
//  provided that the above copyright notice appear in all copies and
//  that both that copyright notice and this permission notice appear
//  in supporting documentation.  Silicon Graphics makes no
//  representations about the suitability of this software for any
//  purpose.  It is provided "as is" without express or implied warranty.
//
//
//  Copyright (c) 1994
//  Hewlett-Packard Company
//
//  Permission to use, copy, modify, distribute and sell this software
//  and its documentation for any purpose is hereby granted without fee,
//  provided that the above copyright notice appear in all copies and
//  that both that copyright notice and this permission notice appear
//  in supporting documentation.  Hewlett-Packard Company makes no
//  representations about the suitability of this software for any
//  purpose.  It is provided "as is" without express or implied warranty.
// -----------------------------------------------------------------------------
// ```
//
///Usage
///-----
// This component is for internal use only.

#include <bslscm_version.h>

#include <bslmf_istriviallycopyable.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_platform.h>

#include <bslstl_hash.h>

#include <cstddef>  // for 'std::size_t'

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslalg_typetraits.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#endif // BDE_OMIT_INTERNAL_DEPRECATED

namespace BloombergLP {
namespace bslstp {

                          // ==================
                          // class bslst::hash
                          // ==================

/// Empty base class for hashing.  No general hash struct defined, each type
/// requires a specialization.  Leaving this struct declared but undefined
/// will generate error messages that are more clear when someone tries to
/// use a key that does not have a corresponding hash function.
template <class HASH_KEY> struct Hash;

template <> struct Hash<char>;
template <> struct Hash<signed char>;
template <> struct Hash<unsigned char>;
template <> struct Hash<short>;
template <> struct Hash<unsigned short>;
template <> struct Hash<int>;
template <> struct Hash<unsigned int>;
template <> struct Hash<long>;
template <> struct Hash<unsigned long>;
template <> struct Hash<long long>;
template <> struct Hash<unsigned long long>;

struct HashCString;

                       // ==========================
                       // class bslstp::HashSelector
                       // ==========================

/// This meta-function selects the appropriate implementation for comparing
/// the parameterized `TYPE`.  This generic template uses the
/// `std::equal_to` functor.
template <class HASH_KEY>
struct HashSelector {

    // TYPES
    typedef ::bsl::hash<HASH_KEY> Type;
};

/// Partial specialization to treat `const` qualified types in exactly the
/// same way as the non-`const` qualified type.  Users should rarely, if
/// ever, need this specialization but would be surprised by their results
/// if used accidentally, and it were not supplied..
template <class HASH_KEY>
struct HashSelector<const HASH_KEY> {

    // TYPES
    typedef typename HashSelector<HASH_KEY>::Type Type;
};

template <>
struct HashSelector<const char *> {
    typedef HashCString Type;
};

template <>
struct HashSelector<char> {
    typedef Hash<char> Type;
};

template <>
struct HashSelector<signed char> {
    typedef Hash<signed char> Type;
};

template <>
struct HashSelector<unsigned char> {
    typedef Hash<unsigned char> Type;
};

template <>
struct HashSelector<short> {
    typedef Hash<short> Type;
};

template <>
struct HashSelector<unsigned short> {
    typedef Hash<unsigned short> Type;
};

template <>
struct HashSelector<int> {
    typedef Hash<int> Type;
};

template <>
struct HashSelector<unsigned int> {
    typedef Hash<unsigned int> Type;
};

template <>
struct HashSelector<long> {
    typedef Hash<long> Type;
};

template <>
struct HashSelector<unsigned long> {
    typedef Hash<unsigned long> Type;
};

template <>
struct HashSelector<long long> {
    typedef Hash<long long> Type;
};

template <>
struct HashSelector<unsigned long long> {
    typedef Hash<unsigned long long> Type;
};

                           // ==================
                           // struct HashCString
                           // ==================

/// Hash functor to generate a hash for a pointer to a null-terminated
/// string.
struct HashCString {

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(HashCString, bsl::is_trivially_copyable);

    // ACCESSORS

    /// Return a hash value computed using the specified `s`.
    std::size_t operator()(const char *s) const
    {
        unsigned long result = 0;

        for (; *s; ++s) {
            result = 5 * result + *s;
        }

        return std::size_t(result);
    }
};

                 // =============================================
                 // explicit class bslstp::Hash<> specializations
                 // =============================================

/// Specialization of `Hash` for `char` values.
template <>
struct Hash<char> {

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Hash, bsl::is_trivially_copyable);

    // ACCESSORS

    /// Return a hash value computed using the specified `x`.
    std::size_t operator()(char x) const
    {
        return x;
    }
};

/// Specialization of `Hash` for `unsigned` `char` values.
template <>
struct Hash<unsigned char> {

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Hash, bsl::is_trivially_copyable);

    // ACCESSORS

    /// Return a hash value computed using the specified `x`.
    std::size_t operator()(unsigned char x) const
    {
        return x;
    }
};

/// Specialization of `Hash` for `signed` `char` values.
template <>
struct Hash<signed char> {

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Hash, bsl::is_trivially_copyable);

    // ACCESSORS

    /// Return a hash value computed using the specified `x`.
    std::size_t operator()(signed char x) const
    {
        return x;
    }
};

/// Specialization of `Hash` for `short` values.
template <>
struct Hash<short> {

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Hash, bsl::is_trivially_copyable);

    // ACCESSORS

    /// Return a hash value computed using the specified `x`.
    std::size_t operator()(short x) const
    {
        return x;
    }
};

/// Specialization of `Hash` for `unsigned` `short` values.
template <>
struct Hash<unsigned short> {

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Hash, bsl::is_trivially_copyable);

    // ACCESSORS

    /// Return a hash value computed using the specified `x`.
    std::size_t operator()(unsigned short x) const
    {
        return x;
    }
};

/// Specialization of `Hash` for `int` values.
template <>
struct Hash<int> {

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Hash, bsl::is_trivially_copyable);

    // ACCESSORS

    /// Return a hash value computed using the specified `x`.
    std::size_t operator()(int x) const
    {
        return x;
    }
};

/// Specialization of `Hash` for `unsigned` `int` values.
template <>
struct Hash<unsigned int> {

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Hash, bsl::is_trivially_copyable);

    // ACCESSORS

    /// Return a hash value computed using the specified `x`.
    std::size_t operator()(unsigned int x) const
    {
        return x;
    }
};

/// Specialization of `Hash` for `long` values.
template <>
struct Hash<long> {

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Hash, bsl::is_trivially_copyable);

    // ACCESSORS

    /// Return a hash value computed using the specified `x`.
    std::size_t operator()(long x) const
    {
        return x;
    }
};

/// Specialization of `Hash` for `unsigned` `long` values.
template <>
struct Hash<unsigned long> {

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Hash, bsl::is_trivially_copyable);

    // ACCESSORS

    /// Return a hash value computed using the specified `x`.
    std::size_t operator()(unsigned long x) const
    {
        return x;
    }
};

#ifdef BSLS_PLATFORM_CPU_64_BIT
/// Specialization of `Hash` for `long long` values.
template <>
struct Hash<long long> {

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Hash, bsl::is_trivially_copyable);

    // ACCESSORS

    /// Return a hash value computed using the specified `x`.
    std::size_t operator()(long long x) const
    {
        return x;
    }
};

/// Specialization of `Hash` for `unsigned` `long long` values.
template <>
struct Hash<unsigned long long> {

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Hash, bsl::is_trivially_copyable);

    // ACCESSORS

    /// Return a hash value computed using the specified `x`.
    std::size_t operator()(unsigned long long x) const
    {
        return x;
    }
};

#else  // BSLS_PLATFORM_CPU_32_BIT

template <>
struct Hash<long long> {
    // Specialization of 'Hash' for 'long long' values.

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Hash, bsl::is_trivially_copyable);

    // ACCESSORS
    std::size_t operator()(unsigned long long x) const
        // Return a hash value computed using the specified 'x'.
    {
        return (std::size_t)(x ^ (x >> 32));
    }
};

template <>
struct Hash<unsigned long long> {
    // Specialization of 'Hash' for 'unsigned' 'long long' values.

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Hash, bsl::is_trivially_copyable);

    // ACCESSORS
    std::size_t operator()(unsigned long long x) const
        // Return a hash value computed using the specified 'x'.
    {
        return (std::size_t)(x ^ (x >> 32));
    }
};
#endif   // BSLS_PLATFORM_CPU_64_BIT

}  // close package namespace
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
