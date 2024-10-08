// bdlde_sha1.h                                                       -*-C++-*-
#ifndef INCLUDED_BDLDE_SHA1
#define INCLUDED_BDLDE_SHA1

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide a value-semantic type encoding a message in a SHA-1 digest.
//
//@CLASSES:
//  bdlde::Sha1: value-semantic type representing a SHA-1 digest
//
//@SEE_ALSO: bdlde_md5, bdlde_sha2
//
//@DESCRIPTION: This component provides the class `bdlde::Sha1`, which
// implements a mechanism for computing and updating a SHA-1 digest (a
// cryptographic hash). The specification for this is based on FIPS-180, which
// can be found at
// ```
// https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf
// ```
//
// Note that a SHA-1 digest does not aid in error correction.
//
///Security
///--------
// Practical collision and chosen-prefix collision attacks are known against
// SHA-1.  Do not use SHA-1 to generate digital signatures under any
// circumstances, and do not use SHA-1 at all except when it is required for
// interoperation with legacy systems that use SHA-1.  SHA-2 (available in the
// `bdlde_sha2` component) and SHA-3 are more secure alternatives to SHA-1.
//
// You might think that your application doesn't require collision resistance.
// However, (1) you might be mistaken, (2) once you start using SHA-1, you
// prevent future versions of your application from being able to rely on
// collision resistance unless they break backward compatibility, (3) a
// maintainer of your application might accidentally make a change that
// implicitly assumes collision resistance, and (4) if you expose SHA-1 hashes
// to your users, they might assume that they are secure digital signatures,
// which will make their applications insecure.  In light of the foregoing
// considerations, and the availability of SHA-2 and SHA-3 as alternatives,
// there is no justification for using SHA-1 unless you absolutely have to.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// The `validatePassword` function below returns whether a specified password
// has a specified hash value.  The `assertPasswordIsExpected` function below
// has a sample password to hash and a hash value that matches it.  Note that
// the output of `loadDigest` is a binary representation.  When hashes are
// displayed for human consumption, they are typically converted to hex, but
// that would create unnecessary overhead here.  Also note that because SHA-1
// digests are inexpensive to compute, they are vulnerable to brute force
// attacks and should not be used for password hashing in real-world
// applications.  A function like `validatePassword` must only be used to
// validate passwords against previously computed SHA-1 hashes, and only during
// a transition period to a more secure password hashing function.
// ```
// /// Return `true` if the specified `password` concatenated with the
// /// specified `salt` has a SHA-1 hash equal to the specified `expected`,
// /// and `false` otherwise.
// bool validatePassword(const bsl::string_view&  password,
//                       const bsl::string_view&  salt,
//                       const unsigned char     *expected)
// {
//     bdlde::Sha1 hasher;
//     hasher.update(password.data(), password.length());
//     hasher.update(salt.data(), salt.length());
//
//     unsigned char digest[bdlde::Sha1::k_DIGEST_SIZE];
//     hasher.loadDigest(digest);
//     return bsl::equal(bsl::begin(digest), bsl::end(digest), expected);
// }
//
// /// Asserts that the constant string `pass` salted with `word` has the
// /// expected hash value.  In a real application, the expected hash would
// /// likely come from some sort of database.
// void assertPasswordIsExpected()
// {
//     const bsl::string   password = "pass";
//     const bsl::string   salt     = "word";
//     const unsigned char expected[bdlde::Sha1::k_DIGEST_SIZE] = {
//         0x5B, 0xAA, 0x61, 0xE4, 0xC9, 0xB9, 0x3F, 0x3F, 0x06, 0x82,
//         0x25, 0x0B, 0x6C, 0xF8, 0x33, 0x1B, 0x7E, 0xE6, 0x8F, 0xD8
//     };
//
//     ASSERT(validatePassword(password, salt, expected));
// }
// ```

#include <bdlscm_version.h>

#include <bsl_cstddef.h>
#include <bsl_cstdint.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace bdlde {

                                 // ==========
                                 // class Sha1
                                 // ==========

/// This `class` represents a SHA-1 digest that can be updated as additional
/// data is provided.
///
/// More generally, this class supports a complete set of *value*
/// *semantic* operations, including copy construction, assignment, equality
/// comparison, and `ostream` printing.  (A precise operational definition
/// of when two instances have the same value can be found in the
/// description of `operator==` for the class.)  This container is
/// *exception* *neutral* with no guarantee of rollback: if an exception is
/// thrown during the invocation of a method on a pre-existing instance, the
/// class is left in a valid state, but its value is undefined.  In no event
/// is memory leaked.  Finally, *aliasing* (e.g., using all or part of an
/// object as both source and destination) is supported in all cases.
class Sha1 {

    // PRIVATE TYPES

    /// Alias for the word type of the SHA-1 algorithm.
    typedef bsl::uint32_t Word;

    /// Alias for the internal state type of the SHA-1 algorithm, an array
    /// of 5 words that is updated after each message block is ingested and
    /// eventually converted into the 160-bit message digest.
    typedef Word          State[5];

    // CLASS DATA

    // Size (in bytes) of the blocks into which the message is divided
    // before being ingested into the SHA-1 state.
    static const bsl::size_t k_BLOCK_SIZE = 512 / 8;

    // DATA
    bsl::uint64_t d_totalSize;   // length of the entire message in bytes

    bsl::uint64_t d_bufferSize;  // bytes currently used for 'd_buffer'

    // buffer for storing remaining part of message that is not yet
    // incorporated into `d_state`
    unsigned char d_buffer[k_BLOCK_SIZE];

    State         d_state;       // current state of the SHA-1 algorithm
                                 // instance represented by this 'Sha1' object

    // FRIENDS
    friend bool operator==(const Sha1&, const Sha1&);

  public:
    // PUBLIC CLASS DATA

    /// The size (in bytes) of the output
    static const bsl::size_t k_DIGEST_SIZE = 160 / 8;

    // CREATORS

    /// Construct a SHA-1 digest having the value corresponding to no data
    /// having been provided.
    Sha1();

    /// Construct a SHA-1 digest corresponding to the specified `data`
    /// having the specified `length` (in bytes).  Note that if `data` is
    /// null, then `length` must be 0.
    Sha1(const void *data, bsl::size_t length);

    // MANIPULATORS

    /// Load the current value of this SHA-1 digest into the specified
    /// `result` and reset this `Sha1` object to the default-constructed
    /// state.
    void loadDigestAndReset(unsigned char *result);

    /// Reset the value of this SHA-1 digest to the value provided by the
    /// default constructor.
    void reset();

    /// Update the value of this SHA-1 digest to incorporate the specified
    /// `data` having the specified `length` in bytes.  If the current
    /// state is the default state, the resultant value of this SHA-1
    /// digest is the application of the SHA-1 algorithm upon the currently
    /// given `data` of the given `length`.  If this digest has been
    /// previously provided data and has not been subsequently reset, the
    /// current state is not the default state and the resultant value is
    /// equivalent to applying the SHA-1 algorithm upon the concatenation of
    /// all the provided data.  The behavior is undefined unless the range
    /// `[data, data + length)` is a valid range.  Note that if `data` is
    /// null, then `length` must be 0.
    void update(const void *data, bsl::size_t length);

    // ACCESSORS

    /// Load the current value of this SHA-1 digest into the specified
    /// `result`.
    void loadDigest(unsigned char *result) const;

    /// Format the current value of this SHA-1 digest to the specified
    /// output `stream` and return a reference to the modifiable `stream`.
    bsl::ostream& print(bsl::ostream& stream) const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` SHA-1 digests have the
/// same value, and `false` otherwise.  Two digests have the same value if,
/// after applying any number of equivalent updates to both (possibly
/// including no updates), the values obtained from their respective
/// `loadDigest` methods are identical.
bool operator==(const Sha1& lhs, const Sha1& rhs);

/// Return `true` if the specified `lhs` and `rhs` SHA-1 digests do not have
/// the same value, and `false` otherwise.  Two digests do not have the same
/// value if there exists a set of updates (possibly including the empty
/// set) that, if applied to both, lead to different values being obtained
/// from their respective `loadDigest` methods.
inline
bool operator!=(const Sha1& lhs, const Sha1& rhs);

/// Write to the specified output `stream` the specified SHA-1 `digest` and
/// return a reference to the modifiable `stream`.
inline
bsl::ostream& operator<<(bsl::ostream& stream, const Sha1& digest);

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

}  // close package namespace

// FREE OPERATORS
inline
bool bdlde::operator!=(const Sha1& lhs, const Sha1& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& bdlde::operator<<(bsl::ostream& stream, const Sha1& digest)
{
    return digest.print(stream);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
