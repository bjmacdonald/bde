// bslfmt_formatparsecontext.h                                        -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATPARSECONTEXT
#define INCLUDED_BSLFMT_FORMATPARSECONTEXT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provides access to formatting parsing string and parsing state.
//
//@CLASSES:
//  bslfmt::basic_format_parse_context: compliant format spec parse context
//
//@CANONICAL_HEADER: bsl_format.h
//
//@DESCRIPTION: This component provides an implementation of the C++20 Standard
// Library's `std::basic_format_parse_context`, which provides access to
// formatting specification string, the parsing state, and the auto-indexing
// argument counter (where applicable).
//
// This header is not intended to be included directly.  Please include
// `<bsl_format.h>` to be able to use `bsl::basic_format_parse_context`.
//

#include <bslscm_version.h>

#include <bslfmt_formaterror.h>

#include <bsls_exceptionutil.h>

#include <bslstl_stringview.h>


namespace BloombergLP {
namespace bslfmt {

                 // ================================
                 // class basic_format_parse_context
                 // ================================

/// A format context for use by `formatter::parse()` partial specialization.
/// It provides access to iterators for the start and end of the remaining
/// (unparsed) format string, as well as argument counters.
template <class t_CHAR>
class basic_format_parse_context {
  public:
    // TYPES
    typedef
        typename bsl::basic_string_view<t_CHAR>::const_iterator const_iterator;
    typedef const_iterator                                      iterator;

  private:
    // PRIVATE TYPES
    enum IndexingMode { e_UNKNOWN, e_MANUAL, e_AUTOMATIC };

    // DATA
    iterator     d_origBegin;  // start of format string
    iterator     d_unparsed;   // start of unparsed portion of format string
    iterator     d_end;        // end of format string
    IndexingMode d_indexing;   // current indexing mode
    size_t       d_nextArgId;  // next arg id for automatic indexing
    size_t       d_numArgs;    // number of args

  private:
    // NOT IMPLEMENTED
    basic_format_parse_context(
                       const basic_format_parse_context&) BSLS_KEYWORD_DELETED;
    basic_format_parse_context& operator=(
                       const basic_format_parse_context&) BSLS_KEYWORD_DELETED;

  public:
    // TYPES
    typedef t_CHAR char_type;

    // CREATORS

    /// Create an instance of this type with the specified `fmt` parse string
    /// and number of arguments set to zero.  The string is stored by reference,
    /// so `fmt` must outlive this object.
    BSLS_KEYWORD_CONSTEXPR_CPP20 explicit basic_format_parse_context(
                     bsl::basic_string_view<t_CHAR> fmt) BSLS_KEYWORD_NOEXCEPT;

    /// Create an instance of this type with the specified `fmt` parse string
    /// and number of arguments set to the specified `numArgs`.  The string is
    /// stored by reference, so `fmt` must outlive this object.
    BSLS_KEYWORD_CONSTEXPR_CPP20 explicit basic_format_parse_context(
                 bsl::basic_string_view<t_CHAR> fmt,
                 size_t                         numArgs) BSLS_KEYWORD_NOEXCEPT;

    // MANIPULATORS

    /// Update the held iterator to the unparsed portion of the format string
    /// to be the specified `it`.  Subsequent calls to `begin` will return this
    /// value.  The behavior is undefined unless `it` is a valid iterator to
    /// the format string used in construction of this context.
    BSLS_KEYWORD_CONSTEXPR_CPP20 void advance_to(const_iterator it);

    /// Return the index of the next-unused argument.  If the indexing mode is
    /// unset, the indexing mode is set to `e_AUTOMATIC`.  If the indexing mode
    /// is `e_MANUAL`, caused by a previous call to `check_arg_id` then an
    /// exception of type `format_error` is thrown.  If there are no remaining
    /// unused arguments then an exception of type `format_error` is thrown.
    BSLS_KEYWORD_CONSTEXPR_CPP20 size_t next_arg_id();

    /// Check that the specified `id` is a valid argument index.  If the
    /// indexing mode is unset, the indexing mode is set to `e_MANUAL`.  If the
    /// indexing mode is `e_AUTOMATIC`, caused by a previous call to
    /// `next_arg_id` then an exception of type `format_error` is thrown.  If
    /// `id` is not less than the number of arguments then an exception of type
    /// `format_error` is thrown.
    BSLS_KEYWORD_CONSTEXPR_CPP20 void check_arg_id(size_t id);

    // ACCESSORS

    /// Return an iterator to the start of the unparsed portion of the format
    /// string.
    BSLS_KEYWORD_CONSTEXPR_CPP20 const_iterator
    begin() const BSLS_KEYWORD_NOEXCEPT;

    /// Return an iterator to the start of the unparsed portion of the format
    /// string.
    BSLS_KEYWORD_CONSTEXPR_CPP20 const_iterator
    end() const BSLS_KEYWORD_NOEXCEPT;

};

// TYPEDEFS
typedef basic_format_parse_context<char>     format_parse_context;
typedef basic_format_parse_context<wchar_t> wformat_parse_context;

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                 // ---------------------------------
                  // class basic_format_parse_context
                  // --------------------------------

// CREATORS
template <class t_CHAR>
inline
BSLS_KEYWORD_CONSTEXPR_CPP20
basic_format_parse_context<t_CHAR>::basic_format_parse_context(
                      bsl::basic_string_view<t_CHAR> fmt) BSLS_KEYWORD_NOEXCEPT
: d_origBegin(fmt.begin())
, d_unparsed(fmt.begin())
, d_end(fmt.end())
, d_indexing(e_UNKNOWN)
, d_nextArgId(0)
, d_numArgs(0)
{
}

template <class t_CHAR>
inline
BSLS_KEYWORD_CONSTEXPR_CPP20
basic_format_parse_context<t_CHAR>::basic_format_parse_context(
                  bsl::basic_string_view<t_CHAR> fmt,
                  size_t                         numArgs) BSLS_KEYWORD_NOEXCEPT
: d_origBegin(fmt.begin())
, d_unparsed(fmt.begin())
, d_end(fmt.end())
, d_indexing(e_UNKNOWN)
, d_nextArgId(0)
, d_numArgs(numArgs)
{
}

// MANIPULATORS
template <class t_CHAR>
inline
BSLS_KEYWORD_CONSTEXPR_CPP20
void basic_format_parse_context<t_CHAR>::advance_to(const_iterator it)
{
    // This check is safe as, in our implementation, it is guaranteed that
    // const_iterator is a random access iterator.
    if (!((it >= d_origBegin) && (it <= d_end))) {
        BSLS_THROW(format_error(
               "Parse error: attempt to advance to an out-of-range iterator"));
    }
    d_unparsed = it;
}

template <class t_CHAR>
inline
BSLS_KEYWORD_CONSTEXPR_CPP20
size_t basic_format_parse_context<t_CHAR>::next_arg_id()
{
    if (d_indexing == e_MANUAL) {
        BSLS_THROW(format_error("Mixing of automatic and manual indexing"));
    }
    if (d_nextArgId >= d_numArgs) {
        BSLS_THROW(format_error("Number of conversion specifiers exceeds "
        "number of arguments"));
    }
    if (d_indexing == e_UNKNOWN) {
        d_indexing = e_AUTOMATIC;
    }
    return d_nextArgId++;
}

template <class t_CHAR>
inline
BSLS_KEYWORD_CONSTEXPR_CPP20
void basic_format_parse_context<t_CHAR>::check_arg_id(size_t id)
{
    if (d_indexing == e_AUTOMATIC) {
        BSLS_THROW(format_error("Mixing of automatic and manual indexing"));
    }
    if (id >= d_numArgs) {
        BSLS_THROW(format_error("Invalid argument index"));
    }
    if (d_indexing == e_UNKNOWN) {
        d_indexing = e_MANUAL;
    }
}

// ACCESSORS
template <class t_CHAR>
inline
BSLS_KEYWORD_CONSTEXPR_CPP20
typename basic_format_parse_context<t_CHAR>::const_iterator
basic_format_parse_context<t_CHAR>::begin() const BSLS_KEYWORD_NOEXCEPT
{
    return d_unparsed;
}

template <class t_CHAR>
inline
BSLS_KEYWORD_CONSTEXPR_CPP20
typename basic_format_parse_context<t_CHAR>::const_iterator
basic_format_parse_context<t_CHAR>::end() const BSLS_KEYWORD_NOEXCEPT
{
    return d_end;
}

}  // close package namespace
}  // close enterprise namespace


#endif  // INCLUDED_BSLFMT_FORMATPARSECONTEXT

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
