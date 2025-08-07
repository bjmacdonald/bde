// baltzo_zoneinfobinaryheader.h                                      -*-C++-*-
#ifndef INCLUDED_BALTZO_ZONEINFOBINARYHEADER
#define INCLUDED_BALTZO_ZONEINFOBINARYHEADER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an attribute class for Zoneinfo binary-file header data.
//
//@CLASSES:
//   baltzo::ZoneinfoBinaryHeader: attribute class for Zoneinfo header data
//
//@SEE_ALSO: baltzo_zoneinfobinaryreader
//
//@DESCRIPTION: This component provides a simply constrained attribute class,
// `baltzo::ZoneinfoBinaryHeader`, representing the header data of a Zoneinfo
// binary data file.
//
///Attributes
///----------
// ```
// Name               Type  Default  Simple Constraints
// -----------------  ----  -------  ------------------------------
// version            char  '\0'     == '\0' || ( >= '2' && <= '4')
// numIsGmt           int    0       >= 0
// numIsStd           int    0       >= 0
// numLeaps           int    0       == 0
// numTransitions     int    0       >= 0
// numLocalTimeTypes  int    1       >= 1
// abbrevDataSize     int    1       >= 1
// ```
//
// * `version`: Zoneinfo file format version, as of 2025, either '\0', `2`, `3`
//    or `4`.
// * `numIsGmt`: number of encoded UTC/local indicators in the file,
//   indicating whether a transition time was originally specified as UTC in
//   the rule file.
// * `numIsStd`: number of encoded standard/wall indicators in the file,
//   indicating whether a transition time was originally specified as standard
//   time in the rule file.
// * `numLeaps`: number of leap corrections stored in the file.
// * `numTransitions`: number of local-time type transitions stored in the
//   file.
// * `numLocalTimeTypes`: number of local-time types stored in the file.
// * `abbrevDataSize`: length of the sequence of characters containing the
//   ('\0'-separated) abbreviation strings in the file.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating a `baltzo::ZoneinfoBinaryHeader` from User Input
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// We define the `getNextZoneinfoBinaryHeader` helper function, reads data from
// a stream, validates the data, and constructs a
// `baltzo::ZoneinfoBinaryHeader` object.
// ```
// /// Set to the specified `object` the value extracted from the
// /// specified `stream`.  Return 0 on success, and a non-zero value
// /// otherwise, with no change to `object`.  The `stream` contains
// /// white-space separated decimal representations of the attributes
// /// of `baltzo::ZoneinfoBinaryHeader` in the following order: `version`,
// /// `numIsGmt`, `numIsStd`, `numLeaps`, `numTransitions`,
// /// `numLocalTimeTypes`, and `abbrevDataSize`.
// int getNextZoneinfoBinaryHeader(baltzo::ZoneinfoBinaryHeader *object,
//                                 bsl::istream&                stream)
// {
//     int version;  // not 'char'
//     int numIsGmt;
//     int numIsStd;
//     int numLeaps;
//     int numTransitions;
//     int numLocalTimeTypes;
//     int abbrevDataSize;
//
//     if (!(stream >> version
//        && stream >> numIsGmt
//        && stream >> numIsStd
//        && stream >> numLeaps
//        && stream >> numTransitions
//        && stream >> numLocalTimeTypes
//        && stream >> abbrevDataSize)) {
//         return 1;                                                 // RETURN
//     }
//
//     if (!(baltzo::ZoneinfoBinaryHeader::isValidVersion(version)
//        && baltzo::ZoneinfoBinaryHeader::isValidNumIsGmt(numIsGmt)
//        && baltzo::ZoneinfoBinaryHeader::isValidNumIsStd(numIsStd)
//        && baltzo::ZoneinfoBinaryHeader::isValidNumLeaps(numLeaps)
//        && baltzo::ZoneinfoBinaryHeader::isValidNumTransitions(
//                                                             numTransitions)
//        && baltzo::ZoneinfoBinaryHeader::isValidNumLocalTimeTypes(
//                                                          numLocalTimeTypes)
//        && baltzo::ZoneinfoBinaryHeader::isValidAbbrevDataSize(
//                                                         abbrevDataSize))) {
//         return 2;                                                 // RETURN
//     }
//
//     object->setVersion(version);
//     object->setNumIsGmt(numIsGmt);
//     object->setNumIsStd(numIsStd);
//     object->setNumLeaps(numLeaps);
//     object->setNumTransitions(numTransitions);
//     object->setNumLocalTimeTypes(numLocalTimeTypes);
//     object->setAbbrevDataSize(abbrevDataSize);
//
//     return 0;
// }
// ```
// To use our helper function, we supply it with a stream of (decimal,
// whitespace-separated values).  The resulting object has the expected value.
// ```
// bsl::stringstream            input("50 1 2 0 3 4 5");
// baltzo::ZoneinfoBinaryHeader header;
// int rc;
//
// rc = getNextZoneinfoBinaryHeader(&header, input);
//
// assert( 0  == rc);
// assert('2' == header.version());
// assert( 1  == header.numIsGmt());
// assert( 2  == header.numIsStd());
// assert( 0  == header.numLeaps());
// assert( 3  == header.numTransitions());
// assert( 4  == header.numLocalTimeTypes());
// assert( 5  == header.abbrevDataSize());
// ```
// Since all of the data in the stream has now been consumed, another call to
// the function returns an error and leaves the object unchanged.
// ```
// header.setVersion(0);
// header.setNumIsGmt(10);
// header.setNumIsStd(20);
// header.setNumLeaps(0);
// header.setNumTransitions(30);
// header.setNumLocalTimeTypes(40);
// header.setAbbrevDataSize(50);
//
// rc = getNextZoneinfoBinaryHeader(&header, input);
//
// assert(  0  != rc);
// assert('\0' == header.version());
// assert( 10  == header.numIsGmt());
// assert( 20  == header.numIsStd());
// assert(  0  == header.numLeaps());
// assert( 30  == header.numTransitions());
// assert( 40  == header.numLocalTimeTypes());
// assert( 50  == header.abbrevDataSize());
// ```

#include <balscm_version.h>

#include <bsls_assert.h>
#include <bsls_review.h>

#include <bsl_algorithm.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace baltzo {

                         // ==========================
                         // class ZoneinfoBinaryHeader
                         // ==========================

/// This simply constrained (value-semantic) attribute class represents the
/// header information found at the start of a Zoneinfo (binary) database
/// file, which describes the contents of the file.
///
/// See the [](#Attributes) section for information on the class attributes.
/// Note that the class invariants are identically the constraints on the
/// individual attributes.
///
/// This class:
/// * supports a complete set of *value* *semantic* operations
///   - except for `bdex` serialization
/// * is *exception-neutral*
/// * is *alias-safe*
/// * is `const` *thread-safe*
/// For terminology see `bsldoc_glossary`.
class ZoneinfoBinaryHeader {

    // DATA
    char d_version;            // file format version of the Zoneinfo, as of
                               // 2013, it can be either '\0', '2', or '3'.

    int  d_numIsGmt;           // number of encoded UTC/local indicators in the
                               // file, indicating whether a transition time
                               // was originally specified as UTC in the rule
                               // file.

    int  d_numIsStd;           // number of encoded standard/wall indicators in
                               // the file, indicating whether a transition
                               // time was originally specified as standard the
                               // in the rule file.

    int  d_numLeaps;           // number of leap corrections stored in the file

    int  d_numTransitions;     // number of local-time type transitions stored
                               // in the file

    int  d_numLocalTimeTypes;  // number of local-time types stored in the file

    int  d_abbrevDataSize;     // length of the sequence of characters
                               // containing the ('\0' separated) abbreviation
                               // strings in the file

  public:
    // CLASS METHODS

    /// Return `true` if the specified value equals '\0', `2`, `3` or `4`, and
    /// `false` otherwise.
    static bool isValidVersion(char value);

    /// Return `true` if the specified `value` is greater than or equal to 0,
    /// and `false` otherwise'.
    static bool isValidNumIsGmt(int value);

    /// Return `true` if the specified `value` is greater than or equal to 0,
    /// and `false` otherwise'.
    static bool isValidNumIsStd(int value);

    /// Return `true` if the specified `value` is greater than or equal to 0,
    /// and `false` otherwise'.
    static bool isValidNumLeaps(int value);

    /// Return `true` if the specified `value` is greater than or equal to 0,
    /// and `false` otherwise'.
    static bool isValidNumTransitions(int value);

    /// Return `true` if the specified `value` is greater than or equal to 1,
    /// and `false` otherwise'.
    static bool isValidNumLocalTimeTypes(int value);

    /// Return `true` if the specified `value` is greater than or equal to 1,
    /// and `false` otherwise'.
    static bool isValidAbbrevDataSize(int value);

    // CREATORS

    /// Create a `ZoneinfoBinaryHeader` object having the (default) attribute
    /// values:
    /// ```
    /// version()           == 0
    /// numIsGmt()          == 0
    /// numIsStd()          == 0
    /// numLeaps()          == 0
    /// numTransitions()    == 0
    /// numLocalTimeTypes() == 1
    /// abbrevDataSize()    == 1
    /// ```
    ZoneinfoBinaryHeader();

    /// Create a `ZoneInfoBinaryHeader` object with the same value as the
    /// specified `original`.
    ZoneinfoBinaryHeader(const ZoneinfoBinaryHeader& original);

    /// Create a `ZoneinfoBinaryHeader` having the specified `version`,
    /// `numIsGmt`, `numIsStd`, `numLeaps`, `numTransitions`,
    /// `numLocalTimeTypes`, and `abbrevDataSize` values.  The behavior is
    /// undefined unless ('0' <= version && version <= '4')`, `0 <= numIsGmt`,
    /// `0 <= numIsStd`, `0 <= numLeaps`, `0 <= numTransitions`,
    /// `1 <= numLocalTimeTypes`, and `1 <= abbrevDataSize`.
    ZoneinfoBinaryHeader(char version,
                         int  numIsGmt,
                         int  numIsStd,
                         int  numLeaps,
                         int  numTransitions,
                         int  numLocalTimeTypes,
                         int  abbrevDataSize);

    /// Destroy this object.
    ~ZoneinfoBinaryHeader();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.
    ZoneinfoBinaryHeader& operator=(const ZoneinfoBinaryHeader& rhs);

    /// Set the `version` attribute of this object to the specified `value`.
    /// The behavior is undefined unless
    /// '0 == value || (50 >= value || 52 <= value)'.  Note that 50 is the
    /// value of ASCII character `2` and 52 is the value of ASCII character
    /// `4`.
    void setVersion(char value);

    /// Set the `numIsGmt` attribute of this object to the specified `value`.
    /// The behavior is undefined unless `0 <= value`.
    void setNumIsGmt(int value);

    /// Set the `numIsStd` attribute of this object to the specified `value`.
    /// The behavior is undefined unless `0 <= value`.
    void setNumIsStd(int value);

    /// Set the `numLeaps` attribute of this object to the specified `value`.
    /// The behavior is undefined unless `0 <= value`.  While we do not provide
    /// parsed leap second transitions to clients, the requirement to be able
    /// to handle version `4` of the Zoneinfo binary data files as well forces
    /// us to allow a non-zero number of such transactions to be accepted.
    void setNumLeaps(int value);

    /// Set the `numTransitions` attribute of this object to the specified
    /// `value`.  The behavior is undefined unless `0 <= value`.
    void setNumTransitions(int value);

    /// Set the `numLocalTimeTypes` attribute of this object to the specified
    /// `value`.  The behavior is undefined unless `1 <= value`.
    void setNumLocalTimeTypes(int value);

    /// Set the `abbrevDataSize` attribute of this object to the specified
    /// `value`.  The behavior is undefined unless `1 <= value`.
    void setAbbrevDataSize(int value);

    /// Efficiently exchange the value of this object with the value of the
    /// specified `other` object.  This method provides the no-throw
    /// exception-safety guarantee.
    void swap(ZoneinfoBinaryHeader& other);

    // ACCESSORS

    /// Return the value of the `version` attribute of this object.
    char version() const;

    /// Return the value of the `numIsGmt` attribute of this object.
    int numIsGmt() const;

    /// Return the value of the `numIsStd` attribute of this object.
    int numIsStd() const;

    /// Return the value of the `numLeaps` attribute of this object.
    int numLeaps() const;

    /// Return the value of the `numTransitions` attribute of this object.
    int numTransitions() const;

    /// Return the value of the `numLocalTimeTypes` attribute of this object.
    int numLocalTimeTypes() const;

    /// Return the value of the `abbrevDataSize` attribute of this object.
    int abbrevDataSize() const;

                        // Aspects

    /// Write the value of this object to the specified output `stream` in a
    /// human-readable format, and return a reference to `stream`.  Optionally
    /// specify an initial indentation `level`, whose absolute value is
    /// incremented recursively for nested objects.  If `level` is specified,
    /// optionally specify `spacesPerLevel`, whose absolute value indicates the
    /// number of spaces per indentation level for this and all of its nested
    /// objects.  If `level` is negative, suppress indentation of the first
    /// line.  If `spacesPerLevel` is negative, format the entire output on one
    /// line, suppressing all but the initial indentation (as governed by
    /// `level`).  If `stream` is not valid on entry, this operation has no
    /// effect.  Note that the format is not fully specified, and can change
    /// without notice.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` objects have the same value,
/// and `false` otherwise.  Two `ZoneinfoBinaryHeader` objects have the same
/// value if the corresponding values of their `version`, `numIsGmt`,
/// `numIsStd`, `numLeaps`, `numTransitions`, `numLocalTimeTypes`, and
/// `abbrevDataSize` attributes are the same.
bool operator==(const ZoneinfoBinaryHeader& lhs,
                const ZoneinfoBinaryHeader& rhs);

/// Return `true` if the specified `lhs` and `rhs` objects have the same value,
/// and `false` otherwise.  Two `ZoneinfoBinaryHeader` objects do not have the
/// same value if the corresponding values of their `version`, `numIsGmt`,
/// `numIsStd`, `numLeaps`, `numTransitions`, `numLocalTimeTypes`, or
/// `abbrevDataSize` attributes are not the same.
bool operator!=(const ZoneinfoBinaryHeader& lhs,
                const ZoneinfoBinaryHeader& rhs);

/// Write the value of the specified `object` to the specified output `stream`
/// in a single-line format, and return a reference to `stream`.  If `stream`
/// is not valid on entry, this operation has no effect.  Note that this
/// human-readable format is not fully specified and can change without notice.
/// Also note that this method has the same behavior as
/// `object.print(stream, 0, -1)`.
bsl::ostream& operator<<(bsl::ostream&               stream,
                         const ZoneinfoBinaryHeader& object);

// FREE FUNCTIONS

/// Efficiently exchange the values of the specified `a` and `b` objects.  This
/// function provides the no-throw exception-safety guarantee.
void swap(baltzo::ZoneinfoBinaryHeader& a, baltzo::ZoneinfoBinaryHeader& b);

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                         // --------------------------
                         // class ZoneinfoBinaryHeader
                         // --------------------------

// CLASS METHODS
inline
bool ZoneinfoBinaryHeader::isValidVersion(char value)
{
    return '\0' == value || ('2' <= value && '4' >= value);
}

inline
bool ZoneinfoBinaryHeader::isValidNumIsGmt(int value)
{
    return value >= 0;
}

inline
bool ZoneinfoBinaryHeader::isValidNumIsStd(int value)
{
    return value >= 0;
}

inline
bool ZoneinfoBinaryHeader::isValidNumLeaps(int value)
{
    return value >= 0;
}

inline
bool ZoneinfoBinaryHeader::isValidNumTransitions(int value)
{
    return value >= 0;
}

inline
bool ZoneinfoBinaryHeader::isValidNumLocalTimeTypes(int value)
{
    return value >= 1;
}

inline
bool ZoneinfoBinaryHeader::isValidAbbrevDataSize(int value)
{
    return value >= 1;
}

// CREATORS
inline
ZoneinfoBinaryHeader::ZoneinfoBinaryHeader()
: d_version(0)
, d_numIsGmt(0)
, d_numIsStd(0)
, d_numLeaps(0)
, d_numTransitions(0)
, d_numLocalTimeTypes(1)
, d_abbrevDataSize(1)
{
}

inline
ZoneinfoBinaryHeader::ZoneinfoBinaryHeader(
                                          const ZoneinfoBinaryHeader& original)
: d_version(original.d_version)
, d_numIsGmt(original.d_numIsGmt)
, d_numIsStd(original.d_numIsStd)
, d_numLeaps(original.d_numLeaps)
, d_numTransitions(original.d_numTransitions)
, d_numLocalTimeTypes(original.d_numLocalTimeTypes)
, d_abbrevDataSize(original.d_abbrevDataSize)
{
}

inline
ZoneinfoBinaryHeader::ZoneinfoBinaryHeader(char version,
                                           int  numIsGmt,
                                           int  numIsStd,
                                           int  numLeaps,
                                           int  numTransitions,
                                           int  numLocalTimeTypes,
                                           int  abbrevDataSize)
: d_version(version)
, d_numIsGmt(numIsGmt)
, d_numIsStd(numIsStd)
, d_numLeaps(numLeaps)
, d_numTransitions(numTransitions)
, d_numLocalTimeTypes(numLocalTimeTypes)
, d_abbrevDataSize(abbrevDataSize)
{
    BSLS_ASSERT(isValidVersion(version));
    BSLS_ASSERT(isValidNumIsGmt(numIsGmt));
    BSLS_ASSERT(isValidNumIsStd(numIsStd));
    BSLS_ASSERT(isValidNumLeaps(numLeaps));
    BSLS_ASSERT(isValidNumTransitions(numTransitions));
    BSLS_ASSERT(isValidNumLocalTimeTypes(numLocalTimeTypes));
    BSLS_ASSERT(isValidAbbrevDataSize(abbrevDataSize));
}

inline
ZoneinfoBinaryHeader::~ZoneinfoBinaryHeader()
{
    BSLS_ASSERT(isValidVersion(d_version));
    BSLS_ASSERT(isValidNumIsGmt(d_numIsGmt));
    BSLS_ASSERT(isValidNumIsStd(d_numIsStd));
    BSLS_ASSERT(isValidNumLeaps(d_numLeaps));
    BSLS_ASSERT(isValidNumTransitions(d_numTransitions));
    BSLS_ASSERT(isValidNumLocalTimeTypes(d_numLocalTimeTypes));
    BSLS_ASSERT(isValidAbbrevDataSize(d_abbrevDataSize));
}

// MANIPULATORS
inline
ZoneinfoBinaryHeader& baltzo::ZoneinfoBinaryHeader::operator=(
                                               const ZoneinfoBinaryHeader& rhs)
{
    d_version           = rhs.d_version;
    d_numIsGmt          = rhs.d_numIsGmt;
    d_numIsStd          = rhs.d_numIsStd;
    d_numLeaps          = rhs.d_numLeaps;
    d_numTransitions    = rhs.d_numTransitions;
    d_numLocalTimeTypes = rhs.d_numLocalTimeTypes;
    d_abbrevDataSize    = rhs.d_abbrevDataSize;
    return *this;
}
inline
void ZoneinfoBinaryHeader::setVersion(char value)
{
    BSLS_ASSERT(isValidVersion(value));

    d_version = value;
}

inline
void ZoneinfoBinaryHeader::setNumIsGmt(int value)
{
    BSLS_ASSERT(isValidNumIsGmt(value));

    d_numIsGmt = value;
}

inline
void ZoneinfoBinaryHeader::setNumIsStd(int value)
{
    BSLS_ASSERT(isValidNumIsStd(value));

    d_numIsStd = value;
}

inline
void ZoneinfoBinaryHeader::setNumLeaps(int value)
{
    BSLS_ASSERT(isValidNumLeaps(value));

    d_numLeaps = value;
}

inline
void ZoneinfoBinaryHeader::setNumTransitions(int value)
{
    BSLS_ASSERT(isValidNumTransitions(value));

    d_numTransitions = value;
}

inline
void ZoneinfoBinaryHeader::setNumLocalTimeTypes(int value)
{
    BSLS_ASSERT(isValidNumLocalTimeTypes(value));

    d_numLocalTimeTypes = value;
}

inline
void ZoneinfoBinaryHeader::setAbbrevDataSize(int value)
{
    BSLS_ASSERT(isValidAbbrevDataSize(value));

    d_abbrevDataSize = value;
}

inline
void ZoneinfoBinaryHeader::swap(ZoneinfoBinaryHeader& other)
{
    bsl::swap(d_version,           other.d_version);
    bsl::swap(d_numIsGmt,          other.d_numIsGmt);
    bsl::swap(d_numIsStd,          other.d_numIsStd);
    bsl::swap(d_numLeaps,          other.d_numLeaps);
    bsl::swap(d_numTransitions,    other.d_numTransitions);
    bsl::swap(d_numLocalTimeTypes, other.d_numLocalTimeTypes);
    bsl::swap(d_abbrevDataSize,    other.d_abbrevDataSize);
}

// ACCESSORS
inline
char ZoneinfoBinaryHeader::version() const
{
    return d_version;
}

inline
int ZoneinfoBinaryHeader::numIsGmt() const
{
    return d_numIsGmt;
}

inline
int ZoneinfoBinaryHeader::numIsStd() const
{
    return d_numIsStd;
}

inline
int ZoneinfoBinaryHeader::numLeaps() const
{
    return d_numLeaps;
}

inline
int ZoneinfoBinaryHeader::numTransitions() const
{
    return d_numTransitions;
}

inline
int ZoneinfoBinaryHeader::numLocalTimeTypes() const
{
    return d_numLocalTimeTypes;
}

inline
int ZoneinfoBinaryHeader::abbrevDataSize() const
{
    return d_abbrevDataSize;
}

}  // close package namespace

// FREE OPERATORS
inline
bool baltzo::operator==(const ZoneinfoBinaryHeader& lhs,
                        const ZoneinfoBinaryHeader& rhs)
{
    return lhs.version()           == rhs.version()
        && lhs.numIsGmt()          == rhs.numIsGmt()
        && lhs.numIsStd()          == rhs.numIsStd()
        && lhs.numLeaps()          == rhs.numLeaps()
        && lhs.numTransitions()    == rhs.numTransitions()
        && lhs.numLocalTimeTypes() == rhs.numLocalTimeTypes()
        && lhs.abbrevDataSize()    == rhs.abbrevDataSize();
}

inline
bool baltzo::operator!=(const ZoneinfoBinaryHeader& lhs,
                        const ZoneinfoBinaryHeader& rhs)
{
    return !(lhs == rhs);
}

/// Write a single line description of the specified `description` to the
/// specified `stream` and a reference to the modifiable `stream`.
inline
bsl::ostream& baltzo::operator<<(bsl::ostream&               stream,
                                 const ZoneinfoBinaryHeader& object)
{
    return object.print(stream, 0, -1);
}

// FREE FUNCTIONS
inline
void baltzo::swap(ZoneinfoBinaryHeader& a, ZoneinfoBinaryHeader& b)
{
    a.swap(b);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
