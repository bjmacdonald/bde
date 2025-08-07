// bslfmt_formatterstring.h                                           -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERSTRING
#define INCLUDED_BSLFMT_FORMATTERSTRING

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a string formatter for use by bsl::format
//
//@CLASSES:
//  bsl::formatter<t_CHAR *, t_CHAR>: formatter specialization for `t_CHAR *`
//  bsl::formatter<const t_CHAR *, t_CHAR>: specialization for `const t_CHAR *`
//  bsl::formatter<t_CHAR[N], t_CHAR> : specialization for character arrays
//  bsl::formatter<std::basic_string, t_CHAR>: for `std::basic_string`
//  bsl::formatter<bsl::basic_string, t_CHAR>: for `bsl::basic_string`
//  bsl::formatter<bsl::basic_string_view, t_CHAR>: `bsl::basic_string_view`
//  bsl::formatter<bslstl:::StringRef, t_CHAR>: for `bslstl:::StringRef`
//
//@CANONICAL_HEADER: bsl_format.h
//
//@DESCRIPTION: This component provides partial specializations for the
// `bsl::formatter` type covering the case for string formatting.  Those types
// meet the requirements as specified in [formatter.requirements] and support
// all functionality supported by the C++20 `std::formatter` string type
// specializations with the following exceptions:
// * No support for escaped strings.
// * No support for non-`C` locales.
//
// This header is not intended to be included directly.  Please include
// `<bsl_format.h>` to be able to use specializations of the `bsl::formatter`
// for strings.
//
///Usage
///-----
// In this section we show the intended use of this component.
//
///Example: Formatting a basic string
/// - - - - - - - - - - - - - - - - -
// We do not expect most users of `bsl::format` to interact with this type
// directly and instead use `bsl::format` or `bsl::vformat`, so this example is
// necessarily unrealistic.
//
// Suppose we want to test this formatter's ability to a substring with padding
// and minimum width.
//
// ```
//  bslfmt::MockParseContext<char> mpc("*<5.3s", 1);
//
//  bsl::formatter<const char *, char> f;
//  mpc.advance_to(f.parse(mpc));
//
//  const char *value = "abcdefghij";
//
//  bslfmt::MockFormatContext<char> mfc(value, 0, 0);
//
//  mfc.advance_to(bsl::as_const(f).format(value, mfc));
//
//  assert("abc**" == mfc.finalString());
// ```
//

#include <bslscm_version.h>

#include <bslfmt_formaterror.h>
#include <bslfmt_formatterbase.h>
#include <bslfmt_standardformatspecification.h>

#include <bslalg_numericformatterutil.h>

#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>

#include <bslstl_string.h>
#include <bslstl_stringref.h>
#include <bslstl_stringview.h>

#include <limits>     // for 'std::numeric_limits'

namespace BloombergLP {
namespace bslfmt {

                        // =====================================
                        // class FormatterString_GraphemeCluster
                        // =====================================

/// [**PRIVATE**] This private implementation `class` characterizes a grapheme
/// cluster represented by a set of integral values.  This class is private to
/// this component and should not be used by clients.
class FormatterString_GraphemeCluster {
  private:
    // DATA
    bool              d_isValid;              // flag indicating whether this
                                              // object is successfully
                                              // initialized (without errors)

    int               d_numSourceBytes;       // number of bytes occupied by
                                              // this cluster

    int               d_numCodePoints;        // number of code points in this
                                              // cluster

    unsigned long int d_firstCodePointValue;  // value of the first code point
                                              // in this cluster

    int               d_firstCodePointWidth;  // width of the first code point
                                              // in this cluster

  public:
    // CREATORS

    /// Create an uninitialized cluster object (that is not valid).
    FormatterString_GraphemeCluster();

    // MANIPULATORS

    /// Extract a grapheme cluster from no more than the specified `maxBytes`
    /// of the byte stream at the specified `bytes` location in the specified
    /// `encoding`.  Behavior is undefined if the input bytes are not in the
    /// specified encoding.  Unicode Byte Order Markers are not supported and
    /// behavior is undefined if the input data contains an embedded BOM.
    /// Endianness is assumed to be that of the type pointed to by `bytes`.
    ///
    /// For UTF-8, behavior is undefined if `bytes` is not a valid pointer to
    /// an array of `numBytes` `unsigned char` types in contiguous memory.
    ///
    /// For UTF-16, behavior is undefined if `bytes` is not a valid pointer to
    /// an array of `numBytes/2` `wchar_t` types in contiguous memory.
    /// Behavior is undefined if `2 != sizeof(wchar_t)`.  Endianness is
    /// assumed to be the same as for the `wchar_t` type and Byte Order Markers
    /// are not supported.
    ///
    /// For UTF-32, behavior is undefined if `bytes` is not a valid pointer to
    /// an array of `numBytes/4` `wchar_t` types in contiguous memory.
    /// Behavior is undefined if `4 != sizeof(wchar_t)`.  Endianness is
    /// assumed to be the same as for the `wchar_t` type and Byte Order Markers
    /// are not supported.
    void extract(UnicodeCodePoint::UtfEncoding  encoding,
                 const void                    *bytes,
                 size_t                         maxBytes);

    /// Reset this object to an uninitialized state.
    void reset();

    // ACCESSORS

    /// Return `true` if this object was successfully extracted and contains
    /// valid code point data, and `false` otherwise.
    bool isValid() const;

    /// Return the number of bytes occupied by this cluster.
    int numSourceBytes() const;

    /// Return the number of code points this cluster contains.
    int numCodePoints() const;

    /// Return the value of the first code point in this cluster.
    unsigned long int firstCodePointValue() const;

    /// Return the width of the first code point in this cluster.
    int firstCodePointWidth() const;
};

                        // ==========================
                        // struct FormatterString_Imp
                        // ==========================

/// This component-private class provides the implementations for parsing
/// string formatting specifications and for formatting strings according to
/// that specification.
template <class t_CHAR>
struct FormatterString_Imp {
  private:
    // PRIVATE TYPES
    typedef StandardFormatSpecification<t_CHAR> Specification;

    // DATA
    Specification d_spec;  // parsed specification.

    // PRIVATE CLASS METHODS

    /// Given the specified `inputString` and `maxTotalDisplayWidth` find the
    /// maximal initial substring of `inputString` whose unicode display width
    /// (calculated using the rules in [format.string.std] does not exceed
    /// `maxTotalDisplayWidth`.  Update the specified `charactersUsed` with the
    /// number of characters of type `t_CHAR` in the maximal substring and
    /// update the specified `widthUsed` with the unicode display width of the
    /// maximal substring.  Throw an exception of type `bsl::format_error` in
    /// the event of failure.
    static void findPrecisionLimitedString(
                   size_t                               *charactersUsed,
                   int                                  *widthUsed,
                   const bsl::basic_string_view<t_CHAR>  inputString,
                   int                                   maxTotalDisplayWidth);
  protected:
    // PROTECTED ACCESSORS

    /// Format the specified `value` according to the specification stored as a
    /// result of a previous call to the `parse` method, and write the result
    /// to the iterator accessed by calling the `out()` method on the specified
    /// `formatContext` parameter.  Return an end iterator of the output range.
    /// Throw an exception of type `bsl::format_error` in the event of failure.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator formatImpl(
                     const bsl::basic_string_view<t_CHAR> value,
                     t_FORMAT_CONTEXT&                    formatContext) const;

  public:
    // MANIPULATORS

    /// Parse and validate the specification string stored in the specified
    /// `parseContext`.  Return an end iterator of the parsed range.  Throw an
    /// exception of type `bsl::format_error` in the event of failure.
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator parse(
                                                t_PARSE_CONTEXT& parseContext);
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                        // ==================================
                        // struct formatter<t_CHAR *, t_CHAR>
                        // ==================================

/// This component-private class provides the partial specialization of the
/// `bsl::formatter` type.  It implements formatting for `char *` and
/// `wchar_t *` types.
template <class t_CHAR>
struct formatter<t_CHAR *, t_CHAR>
: BloombergLP::bslfmt::FormatterString_Imp<t_CHAR> {
  public:
    // TRAITS

    // There will already be a standard library formatter taking a raw
    // character pointer, so do not alias this into `std`.
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;

    // ACCESSORS

    /// Format the value in the specified `value` parameter according to the
    /// specification stored as a result of a previous call to the `parse`
    /// method, and write the result to the iterator accessed by calling the
    /// `out()` method on the specified `formatContext`.  Return an end
    /// iterator of the output range.  Throw an exception of type
    /// `bsl::format_error` in the event of failure.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                       const t_CHAR      *value,
                                       t_FORMAT_CONTEXT&  formatContext) const;
};

                     // ========================================
                     // struct formatter<const t_CHAR *, t_CHAR>
                     // ========================================

/// This component-private class provides the partial specialization of the
/// `bsl::formatter` type.  It implements formatting for `const char *` and
/// `const wchar_t *` types.
template <class t_CHAR>
struct formatter<const t_CHAR *, t_CHAR>
: BloombergLP::bslfmt::FormatterString_Imp<t_CHAR> {
  public:
    // TRAITS

    // There will already be a standard library formatter taking a raw
    // character pointer, so do not alias this into `std`.
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;

    // ACCESSORS

    /// Format the value in the specified `value` parameter according to the
    /// specification stored as a result of a previous call to the `parse`
    /// method, and write the result to the iterator accessed by calling the
    /// `out()` method on the specified `formatContext` parameter.  Return an
    /// end iterator of the output range.  Throw an exception of type
    /// `bsl::format_error` in the event of failure.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                       const t_CHAR      *value,
                                       t_FORMAT_CONTEXT&  formatContext) const;
};

                     // ===================================
                     // struct formatter<t_CHAR[N], t_CHAR>
                     // ===================================

/// This component-private class provides the partial specialization of the
/// `bsl::formatter` type.  It implements formatting for `char[]` and
/// `wchar_t[]` types.
template <class t_CHAR, size_t t_SIZE>
struct formatter<t_CHAR[t_SIZE], t_CHAR>
: BloombergLP::bslfmt::FormatterString_Imp<t_CHAR> {
  public:
    // TRAITS

    // There will already be a standard library formatter taking a raw
    // character pointer, so do not alias this into `std`.
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;

    // ACCESSORS

    /// Format the value in the specified `value` parameter according to the
    /// specification stored as a result of a previous call to the `parse`
    /// method, and write the result to the iterator accessed by calling the
    /// `out()` method on the specified `formatContext` parameter.  Return an
    /// end iterator of the output range.  Throw an exception of type
    /// `bsl::format_error` in the event of failure.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                       const t_CHAR      *value,
                                       t_FORMAT_CONTEXT&  formatContext) const;
};

                // ===========================================
                // struct formatter<std::basic_string, t_CHAR>
                // ===========================================

/// This component-private class provides the partial specialization of the
/// `bsl::formatter` type.  It implements formatting for `std::string` and
/// `std::wstring` types.
template <class t_CHAR>
struct formatter<std::basic_string<t_CHAR>, t_CHAR>
: BloombergLP::bslfmt::FormatterString_Imp<t_CHAR> {
  public:
    // TRAITS

    // There will already be a standard library formatter taking a `std` string
    // view, so do not alias this into `std`.
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;

    // ACCESSORS

    /// Format the value in the specified `value` parameter according to the
    /// specification stored as a result of a previous call to the `parse`
    /// method, and write the result to the iterator accessed by calling the
    /// `out()` method on the specified `formatContext` parameter.  Return an
    /// end iterator of the output range.  Throw an exception of type
    /// `bsl::format_error` in the event of failure.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                         const std::basic_string<t_CHAR>& value,
                         t_FORMAT_CONTEXT&                formatContext) const;
};

              // ===========================================
              // struct formatter<bsl::basic_string, t_CHAR>
              // ===========================================

/// This component-private class provides the partial specialization of the
/// `bsl::formatter` type.  It implements formatting for `bsl::string` and
/// `bsl::wstring` types.
template <class t_CHAR>
struct formatter<bsl::basic_string<t_CHAR>, t_CHAR>
: BloombergLP::bslfmt::FormatterString_Imp<t_CHAR> {
  public:
    // TRAITS

    // We should propagate this formatter to the `std` namespace, so do *not*
    // define the `BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20` trait.

    // ACCESSORS

    /// Format the value in the specified `value` parameter according to the
    /// specification stored as a result of a previous call to the `parse`
    /// method, and write the result to the iterator accessed by calling the
    /// `out()` method on the specified `formatContext` parameter.  Return an
    /// end iterator of the output range.  Throw an exception of type
    /// `bsl::format_error` in the event of failure.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                         const bsl::basic_string<t_CHAR>& value,
                         t_FORMAT_CONTEXT&                formatContext) const;
};

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

              // ================================================
              // struct formatter<std::basic_string_view, t_CHAR>
              // ================================================

/// This component-private class provides the partial specialization of the
/// `bsl::formatter` type.  It implements formatting for `std::string_view`
/// and `std::wstring_view` types.
template <class t_CHAR>
struct formatter<std::basic_string_view<t_CHAR>, t_CHAR>
: BloombergLP::bslfmt::FormatterString_Imp<t_CHAR> {
  public:
    // TRAITS

    // There will already be a standard library formatter taking a `std`
    // string view, so do *not* alias this class into `std`.
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;

    // ACCESSORS

    /// Format the value in the specified `value` parameter according to the
    /// specification stored as a result of a previous call to the `parse`
    /// method, and write the result to the iterator accessed by calling the
    /// `out()` method on the specified `formatContext` parameter.  Return an
    /// end iterator of the output range.  Throw an exception of type
    /// `bsl::format_error` in the event of failure.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                           std::basic_string_view<t_CHAR> value,
                           t_FORMAT_CONTEXT&              formatContext) const;
};
#endif

#ifndef BSLSTL_STRING_VIEW_IS_ALIASED

              // ================================================
              // struct formatter<bsl::basic_string_view, t_CHAR>
              // ================================================

/// This component-private class provides the partial specialization of the
/// `bsl::formatter` type.  It implements formatting for `bsl::string_view` and
/// `bsl::wstring_view` types.
template <class t_CHAR>
struct formatter<bsl::basic_string_view<t_CHAR>, t_CHAR>
: BloombergLP::bslfmt::FormatterString_Imp<t_CHAR> {
  public:
    // TRAITS

    // If we do not alias `string_view` then this formatter must be promoted to
    // the `std` namespace, so do *not* define the
    // `BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20` trait.

    // ACCESSORS

    /// Format the value in the specified `value` parameter according to the
    /// specification stored as a result of a previous call to the `parse`
    /// method, and write the result to the iterator accessed by calling the
    /// `out()` method on the specified `formatContext` parameter.  Return an
    /// end iterator of the output range.  Throw an exception of type
    /// `bsl::format_error` in the event of failure.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                           bsl::basic_string_view<t_CHAR> value,
                           t_FORMAT_CONTEXT&              formatContext) const;
};

#endif

                // ===========================================
                // struct formatter<bslstl::StringRef, t_CHAR>
                // ===========================================

/// This component-private class provides the partial specialization of the
/// `bsl::formatter` type.  It implements formatting for `bslstl::StringRef`
/// and `bslstl::StringRefWide` types.
template <class t_CHAR>
struct formatter<BloombergLP::bslstl::StringRefImp<t_CHAR>, t_CHAR>
: BloombergLP::bslfmt::FormatterString_Imp<t_CHAR> {
  public:
    // TRAITS

    // This formatter specialization must be promoted to the `std` namespace so
    //  do *not* define the `BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20`
    //  trait.

    // ACCESSORS

    /// Format the value in the specified `value` parameter according to the
    /// specification stored as a result of a previous call to the `parse`
    /// method, and write the result to the iterator accessed by calling the

    /// `out()` method on the specified `formatContext` parameter.  Return an
    /// end iterator of the output range.  Throw an exception of type
    /// `bsl::format_error` in the event of failure.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                BloombergLP::bslstl::StringRefImp<t_CHAR> value,
                t_FORMAT_CONTEXT&                         formatContext) const;
};

}  // close namespace bsl

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

namespace BloombergLP {
namespace bslfmt {

                    // -------------------------------------
                    // class FormatterString_GraphemeCluster
                    // -------------------------------------

// CREATORS
inline
FormatterString_GraphemeCluster::FormatterString_GraphemeCluster()
{
    reset();
}

// MANIPULATORS
inline
void FormatterString_GraphemeCluster::reset()
{
    d_isValid             = false;
    d_numSourceBytes      = 0;
    d_numCodePoints       = 0;
    d_firstCodePointValue = 0;
    d_firstCodePointWidth = 0;
}

// ACCESSSORS
inline
bool FormatterString_GraphemeCluster::isValid() const
{
    return d_isValid;
}

inline
int FormatterString_GraphemeCluster::numSourceBytes() const
{
    return d_numSourceBytes;
}

inline
int FormatterString_GraphemeCluster::numCodePoints() const
{
    return d_numCodePoints;
}

inline
unsigned long int FormatterString_GraphemeCluster::firstCodePointValue() const
{
    return d_firstCodePointValue;
}

inline
int FormatterString_GraphemeCluster::firstCodePointWidth() const
{
    return d_firstCodePointWidth;
}

                        // --------------------------
                        // struct FormatterString_Imp
                        // --------------------------

template <class t_CHAR>
void FormatterString_Imp<t_CHAR>::findPrecisionLimitedString(
                    size_t                               *charactersUsed,
                    int                                  *widthUsed,
                    const bsl::basic_string_view<t_CHAR>  inputString,
                    int                                   maxTotalDisplayWidth)
{
    *widthUsed      = 0;
    *charactersUsed = 0;

    const t_CHAR *current   = inputString.data();
    size_t        bytesLeft = inputString.size() * sizeof(t_CHAR);

    while (bytesLeft > 0) {
        FormatterString_GraphemeCluster cluster;

        switch (sizeof(t_CHAR)) {
          case 1: {
            cluster.extract(UnicodeCodePoint::e_UTF8, current, bytesLeft);
          } break;
          case 2: {
            cluster.extract(UnicodeCodePoint::e_UTF16, current, bytesLeft);
          } break;
          case 4: {
            cluster.extract(UnicodeCodePoint::e_UTF32, current, bytesLeft);
          } break;
          default: {
            BSLS_THROW(bsl::format_error("Invalid character width"));
          }
        }
        if (!cluster.isValid()) {
            BSLS_THROW(bsl::format_error("Invalid unicode stream"));
        }

        if (*widthUsed + cluster.firstCodePointWidth() > maxTotalDisplayWidth)
            break;

        bytesLeft -= cluster.numSourceBytes();
        current += cluster.numSourceBytes() / sizeof(t_CHAR);

        *charactersUsed += cluster.numSourceBytes() / sizeof(t_CHAR);

        *widthUsed += cluster.firstCodePointWidth();
    }
}

// PROTECTED ACCESSORS
template <class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator FormatterString_Imp<t_CHAR>::formatImpl(
                      const bsl::basic_string_view<t_CHAR> value,
                      t_FORMAT_CONTEXT&                    formatContext) const
{
    Specification finalSpec(d_spec);

    finalSpec.postprocess(formatContext);

    typedef FormatterSpecificationNumericValue NumericValue;

    NumericValue finalWidth(finalSpec.postprocessedWidth());

    NumericValue finalPrecision(finalSpec.postprocessedPrecision());

    int maxDisplayWidth = 0;
    switch (finalPrecision.category()) {
      case NumericValue::e_DEFAULT: {
        maxDisplayWidth = std::numeric_limits<int>::max();
      } break;
      case NumericValue::e_VALUE: {
        maxDisplayWidth = finalPrecision.value();
      } break;
      default: {
        BSLS_THROW(bsl::format_error("Invalid precision specifier"));  // THROW
      }
    }

    int    displayWidthUsedByInputString = std::numeric_limits<int>::min();
    size_t charactersOfInputUsed         = value.size();

    // Only do an analysis of the string if there is a possibility of
    // truncation or padding.
    if ((maxDisplayWidth < static_cast<int>(value.size()) * 2) ||
        (finalWidth.category() != NumericValue::e_DEFAULT)) {
        findPrecisionLimitedString(&charactersOfInputUsed,
                                   &displayWidthUsedByInputString,
                                   value,
                                   maxDisplayWidth);
    }

    int totalPadDisplayWidth = 0;

    switch (finalWidth.category()) {
      case NumericValue::e_DEFAULT: {
        totalPadDisplayWidth = 0;
      } break;
      case NumericValue::e_VALUE: {
        totalPadDisplayWidth =
               bsl::max(0, finalWidth.value() - displayWidthUsedByInputString);
      } break;
      default: {
        BSLS_THROW(bsl::format_error("Invalid precision specifier"));
      }
    }

    BSLS_ASSERT(totalPadDisplayWidth >= 0);

    int leftPadFillerCopies  = 0;
    int rightPadFillerCopies = 0;

    // Note that, per the C++ spec, the fill character is always assumed to
    // have a field width of one, regardless of its actual field width.
    switch (d_spec.alignment()) {
      case Specification::e_ALIGN_DEFAULT:
      case Specification::e_ALIGN_LEFT: {
        leftPadFillerCopies  = 0;
        rightPadFillerCopies = totalPadDisplayWidth;
      } break;
      case Specification::e_ALIGN_MIDDLE: {
        leftPadFillerCopies  = (totalPadDisplayWidth / 2);
        rightPadFillerCopies = ((totalPadDisplayWidth + 1) / 2);
      } break;
      case Specification::e_ALIGN_RIGHT: {
        leftPadFillerCopies  = totalPadDisplayWidth;
        rightPadFillerCopies = 0;
      } break;
      default: {
        BSLS_THROW(bsl::format_error("Invalid alignment"));
      }
    }

    typename t_FORMAT_CONTEXT::iterator outIterator = formatContext.out();

    for (int i = 0; i < leftPadFillerCopies; ++i) {
        outIterator = bsl::copy(
                          finalSpec.filler(),
                          finalSpec.filler() + finalSpec.numFillerCharacters(),
                          outIterator);
    }

    outIterator = bsl::copy(value.begin(),
                            value.begin() + charactersOfInputUsed,
                            outIterator);

    for (int i = 0; i < rightPadFillerCopies; ++i) {
        outIterator = bsl::copy(
                          finalSpec.filler(),
                          finalSpec.filler() + finalSpec.numFillerCharacters(),
                          outIterator);
    }

    return outIterator;
}

// MANIPULATORS
template <class t_CHAR>
template <class t_PARSE_CONTEXT>
BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator
FormatterString_Imp<t_CHAR>::parse(t_PARSE_CONTEXT& parseContext)
{
    d_spec.parse(&parseContext, Specification::e_CATEGORY_STRING);

    if (d_spec.sign() != Specification::e_SIGN_DEFAULT)
        BSLS_THROW(bsl::format_error(
                      "Formatting sign specifier not valid for string types"));

    if (d_spec.alternativeFlag())
        BSLS_THROW(bsl::format_error(
                         "Formatting # specifier not valid for string types"));

    if (d_spec.zeroPaddingFlag())
        BSLS_THROW(bsl::format_error(
                         "Formatting 0 specifier not valid for string types"));

    if (d_spec.localeSpecificFlag())
        BSLS_THROW(bsl::format_error("Formatting L specifier not supported"));

    if (d_spec.formatType() == Specification::e_STRING_ESCAPED)
        BSLS_THROW(bsl::format_error("String escaping not supported"));

    return parseContext.begin();
}

}  // close package namespace
}  // close enterprise namespace


namespace bsl {

                     // ----------------------------------
                     // struct formatter<t_CHAR *, t_CHAR>
                     // ----------------------------------

// ACCESSORS
template <class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator formatter<t_CHAR *, t_CHAR>::format(
                                        const t_CHAR      *value,
                                        t_FORMAT_CONTEXT&  formatContext) const
{
    bsl::basic_string_view<t_CHAR> view(value);
    return BloombergLP::bslfmt::FormatterString_Imp<t_CHAR>::formatImpl(
                                                                view,
                                                                formatContext);
}

                  // ----------------------------------------
                  // struct formatter<const t_CHAR *, t_CHAR>
                  // ----------------------------------------

// ACCESSORS
template <class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator formatter<const t_CHAR *, t_CHAR>::format(
                                        const t_CHAR      *value,
                                        t_FORMAT_CONTEXT&  formatContext) const
{
    bsl::basic_string_view<t_CHAR> view(value);
    return BloombergLP::bslfmt::FormatterString_Imp<t_CHAR>::formatImpl(
                                                                view,
                                                                formatContext);
}

                    // -----------------------------------
                    // struct formatter<t_CHAR[N], t_CHAR>
                    // -----------------------------------

// ACCESSORS
template <class t_CHAR, size_t t_SIZE>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator formatter<t_CHAR[t_SIZE], t_CHAR>::format(
                                        const t_CHAR      *value,
                                        t_FORMAT_CONTEXT&  formatContext) const
{
    bsl::basic_string_view<t_CHAR> view(value, t_SIZE);
    return BloombergLP::bslfmt::FormatterString_Imp<t_CHAR>::formatImpl(
                                                                view,
                                                                formatContext);
}

                // -------------------------------------------
                // struct formatter<std::basic_string, t_CHAR>
                // -------------------------------------------

// ACCESSORS
template <class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
formatter<std::basic_string<t_CHAR>, t_CHAR>::format(
                          const std::basic_string<t_CHAR>& value,
                          t_FORMAT_CONTEXT&                formatContext) const
{
    bsl::basic_string_view<t_CHAR> view(value);
    return BloombergLP::bslfmt::FormatterString_Imp<t_CHAR>::formatImpl(
                                                                view,
                                                                formatContext);
}

                // -------------------------------------------
                // struct formatter<bsl::basic_string, t_CHAR>
                // -------------------------------------------

// ACCESSORS
template <class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
formatter<bsl::basic_string<t_CHAR>, t_CHAR>::format(
                          const bsl::basic_string<t_CHAR>& value,
                          t_FORMAT_CONTEXT&                formatContext) const
{
    return BloombergLP::bslfmt::FormatterString_Imp<t_CHAR>::formatImpl(
                                                                value,
                                                                formatContext);
}


#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

              // ------------------------------------------------
              // struct formatter<std::basic_string_view, t_CHAR>
              // ------------------------------------------------

// ACCESSORS
template <class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
formatter<std::basic_string_view<t_CHAR>, t_CHAR>::format(
                            std::basic_string_view<t_CHAR> value,
                            t_FORMAT_CONTEXT&              formatContext) const
{
    bsl::basic_string_view<t_CHAR> view(value);
    return BloombergLP::bslfmt::FormatterString_Imp<t_CHAR>::formatImpl(
                                                                view,
                                                                formatContext);
}

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY


#ifndef BSLSTL_STRING_VIEW_IS_ALIASED

              // ------------------------------------------------
              // struct formatter<bsl::basic_string_view, t_CHAR>
              // ------------------------------------------------

// ACCESSORS
template <class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
formatter<bsl::basic_string_view<t_CHAR>, t_CHAR>::format(
                            bsl::basic_string_view<t_CHAR> value,
                            t_FORMAT_CONTEXT&              formatContext) const
{
    return BloombergLP::bslfmt::FormatterString_Imp<t_CHAR>::formatImpl(
                                                                value,
                                                                formatContext);
}

#endif  // BSLSTL_STRING_VIEW_IS_ALIASED

              // ----------------------------------------------
              // struct formatter<bslstl::StringRefImp, t_CHAR>
              // ----------------------------------------------

// ACCESSORS
template <class t_CHAR>
template <class t_FORMAT_CONTEXT>
typename t_FORMAT_CONTEXT::iterator
formatter<BloombergLP::bslstl::StringRefImp<t_CHAR>, t_CHAR>::format(
                 BloombergLP::bslstl::StringRefImp<t_CHAR> value,
                 t_FORMAT_CONTEXT&                         formatContext) const
{
    bsl::basic_string_view<t_CHAR> view(value.data(), value.length());
    return BloombergLP::bslfmt::FormatterString_Imp<t_CHAR>::formatImpl(
                                                                view,
                                                                formatContext);
}

}  // close namespace bsl

#endif  // INCLUDED_BSLFMT_FORMATTERSTRING

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
