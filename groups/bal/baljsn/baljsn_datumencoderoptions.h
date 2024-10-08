// baljsn_datumencoderoptions.h                                       -*-C++-*-
#ifndef INCLUDED_BALJSN_DATUMENCODEROPTIONS
#define INCLUDED_BALJSN_DATUMENCODEROPTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an attribute class for specifying Datum<->JSON options.
//
//@CLASSES:
//  baljsn::DatumEncoderOptions: options for JSON encoding `Datum` objects
//
//@SEE_ALSO: baljsn_datumutil
//
//@DESCRIPTION: This component provides a single, simply constrained
// (value-semantic) attribute class, `baljsn::DatumEncoderOptions`, that is
// used to specify options for encoding `Datum` objects in the JSON format (see
// `baljsn::DatumUtil`).
//
///Attributes
///----------
// ```
// Name                Type           Default         Simple Constraints
// ------------------  -----------    -------         ------------------
// strictTypes         bool           false           none
// encodingStyle       EncodingStyle  e_COMPACT       none
// encodeQuotedDecimal64
//                     bool           true            none
// initialIndentLevel  int            0               >= 0
// spacesPerLevel      int            0               >= 0
// ```
// * `strictTypes`: whether type-checking is performed to make sure encoded
//   types conform to the strict set of types that JSON can represent (and can
//   thus be decoded back to the same types of `Datum` values) ([`string`,
//   `double`, `bool`, `null`, `array`, `map`]).
// * `encodingStyle`: encoding style used to encode the JSON data.
// * `encodeQuotedDecimal64`: option specifying a way to encode `Decimal64`
//                            values.  If the `encodeQuotedDecimal64`
//                            attribute in the `DatumEncoderOptions` is `true`
//                            (the default), the `Decimal64` values will be
//                            encoded as strings, otherwise they will be
//                            encoded as numbers. Encoding a Decimal64 as a
//                            JSON numbers will frequently result it being
//                            later decoded as binary floating point number,
//                            and in the process losing digits of precision
//                            that were the point of using the Decimal64 type
//                            in the first place.  Care should be taken when
//                            setting this option to `false` (though it may be
//                            useful when communicating with endpoints that
//                            are known to correctly handle high precision
//                            JSON numbers).
// * `initialIndentLevel`: Initial indent level for the topmost element.
// * `spacesPerLevel`: spaces per additional indent level.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating and Populating an Options Object
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// This component is designed to be used at a higher level to set the options
// for encoding `Datum` objects in the JSON format.  This example shows how to
// create and populate an options object.
//
// First, we default-construct a `baljsn::DatumEncoderOptions` object:
// ```
// const bool STRICT_TYPES             = true;
// const int  INITIAL_INDENT_LEVEL     = 1;
// const int  SPACES_PER_LEVEL         = 4;
// const bool ENCODE_QUOTED_DECIMAL64  = false;
//
// baljsn::DatumEncoderOptions options;
// assert(false == options.strictTypes());
// assert(0     == options.initialIndentLevel());
// assert(0     == options.spacesPerLevel());
// assert(baljsn::EncodingStyle::e_COMPACT == options.encodingStyle());
// assert(true  == options.encodeQuotedDecimal64());
// ```
// Next, we populate that object to check strict types and encode in a pretty
// format using a pre-defined initial indent level and spaces per level:
// ```
// options.setStrictTypes(STRICT_TYPES);
// assert(true == options.strictTypes());
//
// options.setEncodingStyle(baljsn::EncodingStyle::e_PRETTY);
// assert(baljsn::EncodingStyle::e_PRETTY == options.encodingStyle());
//
// options.setEncodeQuotedDecimal64(ENCODE_QUOTED_DECIMAL64);
// ASSERT(ENCODE_QUOTED_DECIMAL64 == options.encodeQuotedDecimal64());
//
// options.setInitialIndentLevel(INITIAL_INDENT_LEVEL);
// assert(INITIAL_INDENT_LEVEL == options.initialIndentLevel());
//
// options.setSpacesPerLevel(SPACES_PER_LEVEL);
// assert(SPACES_PER_LEVEL == options.spacesPerLevel());
// ```

#include <balscm_version.h>

#include <baljsn_encodingstyle.h>

#include <bslalg_typetraits.h>

#include <bsl_limits.h>
#include <bsl_iosfwd.h>

#include <bsls_assert.h>
#include <bsls_objectbuffer.h>
#include <bsls_review.h>

namespace BloombergLP {

namespace baljsn { class DatumEncoderOptions; }
namespace baljsn {

                         // =========================
                         // class DatumEncoderOptions
                         // =========================

/// This simply constrained (value-semantic) attribute class specifies
/// options for encoding `Datum` objects in the JSON format.  See the
/// [](#Attributes) section for information on the class attributes.  Note that
/// the class invariants are identically the constraints on the individual
/// attributes.
class DatumEncoderOptions {

    // INSTANCE DATA

    // whether strict type validation is performed
    bool                          d_strictTypes;

    // option specifying a way to encode `Decimal64` values.  If the option
    // value is `true` then the `Decimal64` value is encoded quoted
    // { "dec": "1.2e-5" }, and unquoted { "dec": 1.2e-5 } otherwise.
    bool                          d_encodeQuotedDecimal64;

    // initial indentation level for the topmost element
    int                           d_initialIndentLevel;

    // spaces per additional level of indentation
    int                           d_spacesPerLevel;

    // encoding style used to encode values
    baljsn::EncodingStyle::Value  d_encodingStyle;

  public:
    // CONSTANTS
    static const bool s_DEFAULT_INITIALIZER_STRICT_TYPES;

    static const bool s_DEFAULT_INITIALIZER_ENCODE_QUOTED_DECIMAL64;

    static const int  s_DEFAULT_INITIALIZER_INITIAL_INDENT_LEVEL;

    static const int  s_DEFAULT_INITIALIZER_SPACES_PER_LEVEL;

    static const baljsn::EncodingStyle::Value
                      s_DEFAULT_INITIALIZER_ENCODING_STYLE;

  public:
    // CREATORS

    /// Create an object of type `DatumEncoderOptions` having the default
    /// value.
    DatumEncoderOptions();

    /// Create an object of type `DatumEncoderOptions` having the value of
    /// the specified `original` object.
    DatumEncoderOptions(const DatumEncoderOptions& original);

    /// Destroy this object.
    ~DatumEncoderOptions();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    DatumEncoderOptions& operator=(const DatumEncoderOptions& rhs);

    /// Reset this object to the default value (i.e., its value upon
    /// default construction).
    void reset();

    /// Set the "StrictTypes" attribute of this object to the specified
    /// `value`.
    void setStrictTypes(bool value);

    /// Set the "InitialIndentLevel" attribute of this object to the
    /// specified `value`.  The behavior is undefined unless `0 <= value`.
    void setInitialIndentLevel(int value);

    /// Set the "SpacesPerLevel" attribute of this object to the specified
    /// `value`.  The behavior is undefined unless `0 <= value`.
    void setSpacesPerLevel(int value);

    /// Set the "EncodingStyle" attribute of this object to the specified
    /// `value`.
    void setEncodingStyle(baljsn::EncodingStyle::Value value);

    /// Set the "EncodeQuotedDecimal64" attribute of this object to the
    /// specified `value`.
    void setEncodeQuotedDecimal64(bool value);

    // ACCESSORS

    /// Return the "StrictTypes" attribute of this object.
    bool strictTypes() const;

    /// Return the "InitialIndentLevel" attribute of this object.
    int initialIndentLevel() const;

    /// Return the "SpacesPerLevel" attribute of this object.
    int spacesPerLevel() const;

    /// Return the "EncodingStyle" attribute of this object.
    baljsn::EncodingStyle::Value encodingStyle() const;

    /// Return the value of the "EncodeQuotedDecimal64" attribute of this
    /// object.
    bool encodeQuotedDecimal64() const;

                                  // Aspects

    /// Format this object to the specified output `stream` at the
    /// optionally specified indentation `level` and return a reference to
    /// the modifiable `stream`.  If `level` is specified, optionally
    /// specify `spacesPerLevel`, the number of spaces per indentation level
    /// for this and all of its nested objects.  Each line is indented by
    /// the absolute value of `level * spacesPerLevel`.  If `level` is
    /// negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, suppress line breaks and format the
    /// entire output on one line.  If `stream` is initially invalid, this
    /// operation has no effect.  Note that a trailing newline is provided
    /// in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` attribute objects have
/// the same value, and `false` otherwise.  Two attribute objects have the
/// same value if each respective attribute has the same value.
inline
bool operator==(const DatumEncoderOptions& lhs,
                const DatumEncoderOptions& rhs);

/// Return `true` if the specified `lhs` and `rhs` attribute objects do not
/// have the same value, and `false` otherwise.  Two attribute objects do
/// not have the same value if one or more respective attributes differ in
/// values.
inline
bool operator!=(const DatumEncoderOptions& lhs,
                const DatumEncoderOptions& rhs);

/// Format the specified `rhs` to the specified output `stream` and return a
/// reference to the modifiable `stream`.
inline
bsl::ostream& operator<<(bsl::ostream& stream, const DatumEncoderOptions& rhs);

}  // close package namespace

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace baljsn {

                           // -------------------------
                           // class DatumEncoderOptions
                           // -------------------------

inline
void DatumEncoderOptions::setEncodingStyle(baljsn::EncodingStyle::Value value)
{
    d_encodingStyle = value;
}

inline
void DatumEncoderOptions::setInitialIndentLevel(int value)
{
    BSLS_ASSERT(0 <= value);

    d_initialIndentLevel = value;
}

inline
void DatumEncoderOptions::setSpacesPerLevel(int value)
{
    BSLS_ASSERT(0 <= value);

    d_spacesPerLevel = value;
}

inline
void DatumEncoderOptions::setStrictTypes(bool value)
{
    d_strictTypes = value;
}

inline
void DatumEncoderOptions::setEncodeQuotedDecimal64(bool value)
{
    d_encodeQuotedDecimal64 = value;
}

// ACCESSORS
inline
baljsn::EncodingStyle::Value DatumEncoderOptions::encodingStyle() const
{
    return static_cast<baljsn::EncodingStyle::Value>(d_encodingStyle);
}

inline
int DatumEncoderOptions::initialIndentLevel() const
{
    return d_initialIndentLevel;
}

inline
int DatumEncoderOptions::spacesPerLevel() const
{
    return d_spacesPerLevel;
}

inline
bool DatumEncoderOptions::strictTypes() const
{
    return d_strictTypes;
}

inline
bool DatumEncoderOptions::encodeQuotedDecimal64() const
{
    return d_encodeQuotedDecimal64;
}

}  // close package namespace

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

inline
bool baljsn::operator==(const baljsn::DatumEncoderOptions& lhs,
                        const baljsn::DatumEncoderOptions& rhs)
{
    return  lhs.strictTypes()           == rhs.strictTypes()
         && lhs.initialIndentLevel()    == rhs.initialIndentLevel()
         && lhs.spacesPerLevel()        == rhs.spacesPerLevel()
         && lhs.encodingStyle()         == rhs.encodingStyle()
         && lhs.encodeQuotedDecimal64() == rhs.encodeQuotedDecimal64();
}

inline
bool baljsn::operator!=(const baljsn::DatumEncoderOptions& lhs,
                        const baljsn::DatumEncoderOptions& rhs)
{
    return  lhs.strictTypes()           != rhs.strictTypes()
         || lhs.initialIndentLevel()    != rhs.initialIndentLevel()
         || lhs.spacesPerLevel()        != rhs.spacesPerLevel()
         || lhs.encodingStyle()         != rhs.encodingStyle()
         || lhs.encodeQuotedDecimal64() != rhs.encodeQuotedDecimal64();
}

inline
bsl::ostream& baljsn::operator<<(bsl::ostream&                      stream,
                                 const baljsn::DatumEncoderOptions& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace
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
