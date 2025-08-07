// bslfmt_formatterpointer.h                                          -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERPOINTER
#define INCLUDED_BSLFMT_FORMATTERPOINTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a formatter customization for pointer types
//
//@CLASSES:
//  bsl::formatter<void *, t_CHAR>: formatter specialization for `void *`
//  bsl::formatter<const void *, t_CHAR>: specialization for `const void *`
//  bsl::formatter<bsl::nullptr_t, t_CHAR>: specialization for `bsl::nullptr_t`
//
//@CANONICAL_HEADER: bsl_format.h
//
//@DESCRIPTION: This component provides partial specializations of
// `bsl::formatter` catering for pointer types.
//
// This header is not intended to be included directly.  Please include
// `<bsl_format.h>` to be able to use specializations of the `bsl::formatter`
// for pointer types.
//
///Usage
///-----
// In this section we show the intended use of this component.
//
///Example: Formatting a pointer
///- - - - - - - - - - - - - - -
// We do not expect most users of `bsl::format` to interact with this type
// directly and instead use `bsl::format` or `bsl::vformat`, so this example is
// necessarily unrealistic.
//
// Suppose we want to test pointer formatter's ability to format a pointer with
// defined alignment and padding.
//
// ```
//  bslfmt::MockParseContext<char> mpc("*<6p", 1);
//
//  bsl::formatter<const void *, char> f;
//  mpc.advance_to(f.parse(mpc));
//
//  const void *value = 0;
//
//  bslfmt::MockFormatContext<char> mfc(value, 0, 0);
//
//  mfc.advance_to(bsl::as_const(f).format(value, mfc));
//
//  assert("0x0***" == mfc.finalString());
// ```

#include <bslscm_version.h>

#include <bslfmt_formaterror.h>
#include <bslfmt_formatterbase.h>
#include <bslfmt_formatterintegralbase.h>
#include <bslfmt_standardformatspecification.h>

#include <bslalg_numericformatterutil.h>

#include <bslmf_assert.h>

#include <bsls_nullptr.h>
#include <bsls_types.h>

#include <bslstl_iterator.h>

#include <locale>     // for `std::ctype`, `locale`

namespace BloombergLP {
namespace bslfmt {

                         // ===========================
                         // struct FormatterPointer_Imp
                         // ===========================

/// This struct is a specialization of the `bsl::formatter` template for the
/// pointer types.
template <class t_VALUE, class t_CHAR>
struct FormatterPointer_Imp : public FormatterIntegralBase<t_VALUE, t_CHAR> {
  public:
    // TRAITS
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;

    // ACCESSORS

    /// Create string representation of the specified `ptrValue`, customized in
    /// accordance with the requested format and the specified `formatContext`,
    /// and copy it to the output that the output iterator of the
    /// `formatContext` points to.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                        t_VALUE           ptrValue,
                                        t_FORMAT_CONTEXT& formatContext) const;
};


                         // ===============================
                         // struct FormatterPointer_Nullptr
                         // ===============================

/// This struct is a specialization of the `bsl::formatter` template for the
/// `bsl::nullptr_t`.
template <class t_CHAR>
struct FormatterPointer_Nullptr
: public FormatterIntegralBase<bsl::nullptr_t, t_CHAR> {
  public:
    // TRAITS
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;

    // ACCESSORS

    /// Create string representation of the `bsl::nullptr_t` object, customized
    /// in accordance with the requested format and the specified
    /// `formatContext`, and copy it to the output that the output iterator of
    /// the `formatContext` points to.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                        bsl::nullptr_t,
                                        t_FORMAT_CONTEXT& formatContext) const;
};
}  // close package namespace
}  // close enterprise namespace

namespace bsl {
// FORMATTER SPECIALIZATIONS

/// Partial specialization of the `bsl::formatter` template for the type
/// `void *`.
template <class t_CHAR>
struct formatter<void *, t_CHAR>
: BloombergLP::bslfmt::FormatterPointer_Imp<void *, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for the type
/// `const void *`.
template <class t_CHAR>
struct formatter<const void *, t_CHAR>
: BloombergLP::bslfmt::FormatterPointer_Imp<const void *, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for the type
/// `bsl::nullptr_t`.
template <class t_CHAR>
struct formatter<bsl::nullptr_t, t_CHAR>
: BloombergLP::bslfmt::FormatterPointer_Nullptr<t_CHAR> {
};

}  // close namespace bsl

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

namespace BloombergLP {
namespace bslfmt {

                         // -----------------------
                         // struct FormatterPointer
                         // -----------------------

template <class t_VALUE, class t_CHAR>
template <class t_FORMAT_CONTEXT>
inline
typename t_FORMAT_CONTEXT::iterator
FormatterPointer_Imp<t_VALUE, t_CHAR>::format(
                                         t_VALUE           ptrValue,
                                         t_FORMAT_CONTEXT& formatContext) const
{
    typedef BloombergLP::bslalg::NumericFormatterUtil NFUtil;
    typedef StandardFormatSpecification<t_CHAR>       Specification;
    typedef bsls::Types::UintPtr                      UintPtr;

    const Specification& parsedSpec = this->specification();

    // prefix

    const int  maxPrefixSize = 2;
    char       prefixBuf[maxPrefixSize];
    char      *prefixBegin = prefixBuf;
    char      *prefixEnd = prefixBegin + maxPrefixSize;

    prefixBuf[0] = '0';

    switch (parsedSpec.formatType()) {
      case Specification::e_POINTER_HEX: {
        prefixBuf[1] = 'x';
      } break;
      case Specification::e_POINTER_HEX_UC: {
        prefixBuf[1] = 'X';
      } break;
      default: {
        BSLS_THROW(bsl::format_error("Invalid pointer format type"));
      }
    }

    // value

    // `formatValue` is universal function used for formatting integer values,
    // therefore it requires storage sufficient to accommodate any
    // representation of the value (even if for pointers hex format is used).
    // Binary representation takes up the most space.

    const int  maxValueSize = NFUtil::ToCharsMaxLength<UintPtr, 2>::k_VALUE;
    t_CHAR     valueBuf[maxValueSize];
    t_CHAR    *valueBegin = valueBuf;
    t_CHAR    *valueEnd   = valueBuf;

    const void *temp  = static_cast<const void *>(ptrValue);
    UintPtr     value = reinterpret_cast<UintPtr>(temp);
    valueEnd          = this->formatValue(valueBuf, maxValueSize, value);

    // output the result

    return this->outputValue(prefixBegin,
                             prefixEnd,
                             valueBegin,
                             valueEnd,
                             formatContext);
}

                         // -------------------------------
                         // struct FormatterPointer_Nullptr
                         // -------------------------------

template <class t_CHAR>
template <class t_FORMAT_CONTEXT>
inline
typename t_FORMAT_CONTEXT::iterator FormatterPointer_Nullptr<t_CHAR>::format(
                                         bsl::nullptr_t,
                                         t_FORMAT_CONTEXT& formatContext) const
{
    typedef StandardFormatSpecification<t_CHAR> Specification;

    const Specification& parsedSpec = this->specification();

    // prefix

    const int  maxPrefixSize = 2;
    char       prefixBuf[maxPrefixSize];
    char      *prefixBegin = prefixBuf;
    char      *prefixEnd = prefixBegin + maxPrefixSize;

    prefixBuf[0] = '0';

    switch (parsedSpec.formatType()) {
      case Specification::e_POINTER_HEX: {
        prefixBuf[1] = 'x';
      } break;
      case Specification::e_POINTER_HEX_UC: {
        prefixBuf[1] = 'X';
      } break;
      default: {
        BSLS_THROW(bsl::format_error("Invalid nullptr format type"));
      }
    }

    // value

    t_CHAR  value      = '0';
    t_CHAR *valueBegin = &value;
    t_CHAR *valueEnd   = valueBegin + 1;

    // output the result

    return this->outputValue(prefixBegin,
                             prefixEnd,
                             valueBegin,
                             valueEnd,
                             formatContext);
    }

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_BSLFMT_FORMATTERPOINTER

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
