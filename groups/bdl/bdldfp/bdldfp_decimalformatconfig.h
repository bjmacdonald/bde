// bdldfp_decimalformatconfig.h                                       -*-C++-*-
#ifndef INCLUDED_BDLDFP_DECIMALFORMATCONFIG
#define INCLUDED_BDLDFP_DECIMALFORMATCONFIG

#include <bsls_ident.h>
BSLS_IDENT("$Id$")

//@PURPOSE: Provide an attribute class to configure decimal formatting.
//
//@CLASSES:
//  bdldfp::DecimalFormatConfig: configuration for formatting functions
//
//@SEE_ALSO: bdldfp_decimalutil
//
//@DESCRIPTION: This component provides a single, simply constrained
// (value-semantic) attribute class, `bdldfp::DecimalFormatConfig`, that is
// used to configure various aspects of decimal value formatting.
//
///Attributes
///----------
// ```
// Name        Type      Default          Simple Constraints
// ---------   ------    ---------------  ------------------
// style       Style     e_NATURAL        none
// precision   int       0                >= 0
// sign        Sign      e_NEGATIVE_ONLY  none
// infinity    string    "inf"            none
// nan         string    "nan"            none
// snan        string    "snan"           none
// point       char      '.'              none
// exponent    char      'e'              none
// showpoint   bool      false            none
// expwidth    int       2                >= 1, <= 4
// ```
// * `style`: control how the decimal number is written.  If `style` is
//   `e_SCIENTIFIC`, the number is written as its sign, then a single digit,
//   then the decimal point, then `precision` digits, then the `exponent`
//   character, then a `-` or `+`, then an exponent with no leading zeroes
//   (with a zero exponent written as `0`).  If `style` is `e_FIXED`, the
//   number is written as its sign, then one or more digits, then the decimal
//   point, then `precision` digits.  If the `precision` value equals `0` then
//   `precision` digits and the decimal point are not written.  If `style` is
//   `e_NATURAL`, the number is written according to the description of
//   `to-scientific-string` found in
//   http://speleotrove.com/decimal/decarith.pdf (and no other specified
//   formatting values are used, including precision).
// * `precision`: control how many digits are written after the decimal point
//   if the decimal number is rendered in `e_FIXED` and `e_SCIENTIFIC`
//   formats.  Note that `precision` attribute is ignored in `e_NATURAL`
//   format.
// * `sign`: control how the sign is output.  If a decimal value has its sign
//   bit set, a `-` is always written.  Otherwise, if `sign` is
//   `e_NEGATIVE_ONLY`, no sign is written.  If it is `e_ALWAYS`, a `+` is
//   written.
// * `infinity`: specify a string to output infinity value.
// * `nan`: specify a string to output NaN value.
// * `snan`: specify a string to output signaling NaN value.
// * `point`: specify the character to use for decimal points.
// * `exponent`: specify the character to use for exponent when `style` is
//   `e_SCIENTIFIC` or `e_NATURAL`.
// * `showpoint`: specify whether a decimal point is always displayed.
// * `expwidth`: control the minimum number of digits used to write the
//   exponent.

#include <bsl_cstring.h>

#include <bsls_assert.h>

namespace BloombergLP {
namespace bdldfp {

                        // =========================
                        // class DecimalFormatConfig
                        // =========================

/// This attribute class characterizes how to configure certain behavior of
/// `bdldfp::DecimalUtil::format` functions.
class DecimalFormatConfig {

  public:
    // TYPES
    enum Sign {
        e_NEGATIVE_ONLY,  // no sign output when sign bit is not set
        e_ALWAYS          // output '+' when sign bit is not set
    };

    enum Style {
        e_SCIENTIFIC,     // output number in scientific notation
        e_FIXED,          // output number in fixed-format
        e_NATURAL         // output number in "to-scientific-string" format
                          // described in
                          // {http://speleotrove.com/decimal/decarith.pdf}
    };

  private:
    // DATA
    int         d_precision;     // precision (number of digits after point)
    Style       d_style;         // formatting style
    Sign        d_sign;          // sign character
    const char *d_infinityText;  // infinity representation
    const char *d_nanText;       // NaN representation
    const char *d_sNanText;      // signaling NaN representation
    char        d_decimalPoint;  // decimal point character
    char        d_exponent;      // exponent character
    bool        d_showpoint;     // always show decimal
    int         d_expWidth;      // minimum digits in exponent

    // FRIENDS
    friend bool operator==(const DecimalFormatConfig&,
                           const DecimalFormatConfig&);
    friend bool operator!=(const DecimalFormatConfig&,
                           const DecimalFormatConfig&);

  public:
    // CREATORS

    /// Create an object of this class having the (default) attribute
    /// values:
    /// ```
    /// precision == 0
    /// style     == e_NATURAL
    /// sign      == e_NEGATIVE_ONLY
    /// infinity  == "inf"
    /// nan       == "nan"
    /// snan      == "snan"
    /// point     == '.'
    /// exponent  == 'e'
    /// expwidth  == 2
    /// showpoint == false
    /// ```
    DecimalFormatConfig();

    /// Create an object of this class having the specified `precision` to
    /// control how many digits are written after a decimal point.  The
    /// behavior is undefined if `precision` is negative.  Optionally
    /// specify `style` to control how the number is written.  If it is not
    /// specified, `e_NATURAL` is used.  Optionally specify `sign` to
    /// control how the sign is output.  If is not specified,
    /// `e_NEGATIVE_ONLY` is used.  Optionally specify `infinity` as a
    /// string to output infinity value.  If it is not specified, "inf" is
    /// used.  Optionally specify `nan` as a string to output NaN value.  If
    /// it is not specified, "nan" is used.  Optionally specify `snan` as a
    /// string to output signaling NaN value.  If it is not specified,
    /// "snan" is used.  The behavior is undefined unless the pointers to
    /// `infinity`, `nan` and `snan` remain valid for the lifetime of this
    /// object.  Optionally specify `point` as the character to use for
    /// decimal points.  If it is not specified, `.` is used.  Optionally
    /// specify `exponent` as the character to use for exponent.  If it is
    /// not specified, `e` is used.  Optionally specify `showpoint` to force
    /// a decimal point to always be written.  Optionally specify `expWidth`
    /// to force at least that many digits to be written for an exponent, up
    /// to the number of digits in the largest supported exponent.  If it is
    /// not specified, 2 is used.  The behavior is undefined unless
    /// `expWidth` is 1, 2, 3, or 4.  See the Attributes section under
    /// @DESCRIPTION in the component-level documentation for information on
    /// the class attributes.
    explicit
    DecimalFormatConfig(int         precision,
                        Style       style     = e_NATURAL,
                        Sign        sign      = e_NEGATIVE_ONLY,
                        const char *infinity  = "inf",
                        const char *nan       = "nan",
                        const char *snan      = "snan",
                        char        point     = '.',
                        char        exponent  = 'e',
                        bool        showpoint = false,
                        int         expWidth  = 2);

    // MANIPULATORS

    /// Set the `precision` attribute of this object to the specified
    /// `value`.  Behavior is undefined if `value` is negative.
    void setPrecision (int value);

    /// Set the `style` attribute of this object to the specified `value`.
    void setStyle(Style value);

    /// Set the `sign` attribute of this object to the specified `value`.
    void setSign(Sign  value);

    /// Set the `infinity` attribute of this object to the specified
    /// `value`.  The behavior is undefined unless the pointer to the
    /// `value` remains valid for the lifetime of this object.
    void setInfinity(const char *value);

    /// Set the `nan` attribute of this object to the specified `value`.
    /// The behavior is undefined unless the pointer to the `value` remains
    /// valid for the lifetime of this object.
    void setNan(const char *value);

    /// Set the `snan` attribute of this object to the specified `value`.
    /// The behavior is undefined unless the pointer to the `value` remains
    /// valid for the lifetime of this object.
    void setSNan(const char *value);

    /// Set the `point` attribute of this object to the specified `value`.
    void setDecimalPoint(char value);

    /// Set the `exponent` attribute of this object to the specified
    /// `value`.
    void setExponent(char value);

    /// Set the `showpoint` attribute of this object to the specified
    /// `value`.
    void setShowpoint(bool value);

    /// Set the `expwidth` attribute of this object to the specified
    /// `value`.  The behavior is undefined unless `value` is 1, 2, 3, or 4.
    void setExpWidth(int value);

    // ACCESSORS

    /// Return the number of digits of precision in the outputs.
    int precision() const;

    /// Return the style of output format.
    Style style() const;

    /// Return the sign attribute.
    Sign sign() const;

    /// Return infinity string representation.
    const char *infinity() const;

    /// Return NaN string representation.
    const char *nan() const;

    /// Return sNaN string representation.
    const char *sNan() const;

    /// Return point character.
    char decimalPoint() const;

    /// Return exponent character.
    char exponent() const;

    /// Return the `showpoint` attribute.
    bool showpoint() const;

    /// Return the minimum exponent width.
    int expWidth() const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` objects have the same
/// value, and `false` otherwise.  Two `DecimalFormatConfig` objects have
/// the same value if each of their attributes (respectively) have the same
/// value.  Note that comparison of two string type attributes are done via
/// 'bsl::strcmp() function.
bool operator==(const DecimalFormatConfig& lhs,
                const DecimalFormatConfig& rhs);

/// Return `true` if the specified `lhs` and `rhs` objects do not have the
/// same value, and `false` otherwise.  Two `DecimalFormatConfig` objects
/// do not have the same value if any of their attributes (respectively) do
/// not have the same value.  Note that comparison of two string type
/// attributes are done via 'bsl::strcmp() function.
bool operator!=(const DecimalFormatConfig& lhs,
                const DecimalFormatConfig& rhs);


// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                          // -------------------------
                          // class DecimalFormatConfig
                          // -------------------------

// CREATORS
inline
DecimalFormatConfig::DecimalFormatConfig()
    : d_precision(0)
    , d_style(e_NATURAL)
    , d_sign(e_NEGATIVE_ONLY)
    , d_infinityText("inf")
    , d_nanText("nan")
    , d_sNanText("snan")
    , d_decimalPoint('.')
    , d_exponent('e')
    , d_showpoint(false)
    , d_expWidth(2)
{
}

inline
DecimalFormatConfig::DecimalFormatConfig(int         precision,
                                         Style       style,
                                         Sign        sign,
                                         const char *infinity,
                                         const char *nan,
                                         const char *snan,
                                         char        point,
                                         char        exponent,
                                         bool        showpoint,
                                         int         expWidth)
    : d_precision(precision)
    , d_style(style)
    , d_sign(sign)
    , d_infinityText(infinity)
    , d_nanText(nan)
    , d_sNanText(snan)
    , d_decimalPoint(point)
    , d_exponent(exponent)
    , d_showpoint(showpoint)
    , d_expWidth(expWidth)
{
    BSLS_ASSERT(precision >= 0);
    BSLS_ASSERT(infinity);
    BSLS_ASSERT(nan);
    BSLS_ASSERT(snan);
    BSLS_ASSERT(expWidth >= 1);
    BSLS_ASSERT(expWidth <= 4);
}

// MANIPULATORS
inline
void DecimalFormatConfig::setPrecision (int value)
{
    BSLS_ASSERT(value >= 0);
    d_precision = value;
}

inline
void DecimalFormatConfig::setStyle(Style value)
{
    d_style = value;
}

inline
void DecimalFormatConfig::setSign(Sign  value)
{
    d_sign = value;
}

inline
void DecimalFormatConfig::setInfinity(const char *value)
{
    BSLS_ASSERT(value);
    d_infinityText = value;
}

inline
void DecimalFormatConfig::setNan(const char *value)
{
    BSLS_ASSERT(value);
    d_nanText = value;
}

inline
void DecimalFormatConfig::setSNan(const char *value)
{
    BSLS_ASSERT(value);
    d_sNanText = value;
}

inline
void DecimalFormatConfig::setDecimalPoint(char value)
{
    d_decimalPoint = value;
}

inline
void DecimalFormatConfig::setExponent(char value)
{
    d_exponent = value;
}

inline
void DecimalFormatConfig::setShowpoint(bool value)
{
    d_showpoint = value;
}


inline
void DecimalFormatConfig::setExpWidth(int value)
{
    BSLS_ASSERT(value >= 1);
    BSLS_ASSERT(value <= 4);

    d_expWidth = value;
}

// ACCESSORS
inline
int DecimalFormatConfig::precision() const
{
    return d_precision;
}

inline
DecimalFormatConfig::Style DecimalFormatConfig::style() const
{
    return d_style;
}

inline
DecimalFormatConfig::Sign DecimalFormatConfig::sign() const
{
    return d_sign;
}

inline
const char *DecimalFormatConfig::infinity() const
{
    return d_infinityText;
}

inline
const char *DecimalFormatConfig::nan() const
{
    return d_nanText;
}

inline
const char *DecimalFormatConfig::sNan() const
{
    return d_sNanText;
}

inline
char DecimalFormatConfig::decimalPoint() const
{
    return d_decimalPoint;
}

inline
char DecimalFormatConfig::exponent() const
{
    return d_exponent;
}

inline
bool DecimalFormatConfig::showpoint() const
{
    return d_showpoint;
}

inline
int DecimalFormatConfig::expWidth() const
{
    return d_expWidth;
}
}  // close package namespace

// FREE OPERATORS
inline
bool bdldfp::operator==(const DecimalFormatConfig& lhs,
                        const DecimalFormatConfig& rhs)
{
    return lhs.d_precision              == rhs.d_precision          &&
           lhs.d_style                  == rhs.d_style              &&
           lhs.d_sign                   == rhs.d_sign               &&
           bsl::strcmp(lhs.d_infinityText, rhs.d_infinityText) == 0 &&
           bsl::strcmp(lhs.d_nanText,      rhs.d_nanText)      == 0 &&
           bsl::strcmp(lhs.d_sNanText,     rhs.d_sNanText)     == 0 &&
           lhs.d_decimalPoint           == rhs.d_decimalPoint       &&
           lhs.d_exponent               == rhs.d_exponent           &&
           lhs.d_showpoint              == rhs.d_showpoint          &&
           lhs.d_expWidth               == rhs.d_expWidth;
}

inline
bool bdldfp::operator!=(const DecimalFormatConfig& lhs,
                        const DecimalFormatConfig& rhs)
{
    return lhs.d_precision              != rhs.d_precision     ||
           lhs.d_style                  != rhs.d_style         ||
           lhs.d_sign                   != rhs.d_sign          ||
           bsl::strcmp(lhs.d_infinityText, rhs.d_infinityText) ||
           bsl::strcmp(lhs.d_nanText,      rhs.d_nanText)      ||
           bsl::strcmp(lhs.d_sNanText,     rhs.d_sNanText)     ||
           lhs.d_decimalPoint           != rhs.d_decimalPoint  ||
           lhs.d_exponent               != rhs.d_exponent      ||
           lhs.d_showpoint              != rhs.d_showpoint     ||
           lhs.d_expWidth               != rhs.d_expWidth;
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
