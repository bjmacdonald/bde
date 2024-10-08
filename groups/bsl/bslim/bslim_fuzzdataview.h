// bslim_fuzzdataview.h                                               -*-C++-*-
#ifndef INCLUDED_BSLIM_FUZZDATAVIEW
#define INCLUDED_BSLIM_FUZZDATAVIEW

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a view of a buffer of fuzz data bytes.
//
//@CLASSES:
//  bslim::FuzzDataView: reference-semantic type for fuzz data
//
//@SEE_ALSO: bslim_fuzzutil
//
//@DESCRIPTION: This component defines a reference-semantic class,
// `bslim::FuzzDataView`, providing a view to a non-modifiable buffer of fuzz
// data obtained from a fuzz testing harness (e.g., `libFuzzer`).
//
// See {http://bburl/BDEFuzzTesting} for details on how to build and run with
// fuzz testing enabled.
//
// Typically, this class is intended to be employed by a utility that takes an
// object of this class as an in-out parameter, consumes the bytes (updating
// the view so that the bytes are not used again), and returns objects of the
// type requested.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating a `bsl::string`
///- - - - - - - - - - - - - - - - - -
// The following example demonstrates how to create a `bsl::string` object from
// a `FuzzDataView`.
//
// First, we construct a `FuzzDataView` object, `view0`, from an array of
// bytes:
// ```
// const bsl::uint8_t data[] = {0x8A, 0x19, 0x0D, 0x44, 0x37, 0x0D,
//                              0x38, 0x5E, 0x9B, 0xAA, 0xF3, 0xDA};
//
// bslim::FuzzDataView view0(data, sizeof(data));
//
// assert(12 == view0.length());
// ```
// Next, we take the first 3 bytes from `view0` and store them in a new
// `FuzzDataView` object, `view1`:
// ```
// bslim::FuzzDataView view1 = view0.removePrefix(3);
//
// assert(3 == view1.length());
// assert(9 == view0.length());
// ```
// We confirm that `removePrefix(3)` removed 3 bytes from `view0` and that
// `view1` has length 3.
//
// Then, we create a `bsl::string` object from `view1`:
// ```
// bsl::string s1(view1.begin(), view1.end());
//
// assert(3 == s1.length());
// ```
// Finally, we create another `bsl::string` with the remaining bytes of
// `view0`:
// ```
// bsl::string s2(view0.begin(), view0.end());
//
// assert(9 == s2.length());
// ```

#include <bslscm_version.h>

#include <bsls_assert.h>

#include <bsl_algorithm.h>  // min
#include <bsl_cstddef.h>    // size_t
#include <bsl_cstdint.h>    // uint8_t

namespace BloombergLP {
namespace bslim {

                             // ==================
                             // class FuzzDataView
                             // ==================

/// This type represents a view of a buffer of bytes provided by a fuzz
/// testing harness.
class FuzzDataView {

  private:
    // DATA
    const bsl::uint8_t *d_data_p;  // pointer to the data
    bsl::size_t         d_length;  // length of the view

  public:
    // CREATORS

    /// Create a `FuzzDataView` object from the specified fuzz `data` and
    /// `size`. The behavior is undefined unless `data || (0 == size)`.
    FuzzDataView(const bsl::uint8_t *data, bsl::size_t size);

    /// Create a `FuzzDataView` having the value of the specified `original`
    /// `FuzzDataView`.
    //! FuzzDataView(const FuzzDataView& original)  = default;

    /// Destroy this object.
    //! ~FuzzDataView() = default;

    // MANIPULATORS

    /// Assign to this FuzzDataView the value of the specified `rhs`
    /// `FuzzDataView`, and return a reference providing modifiable access to
    /// this object.  Note that this trivial assignment operation is generated
    /// by the compiler.
    // FuzzDataView &operator=(const FuzzDataView& rhs) = default;

    /// Remove the specified initial `numBytes` from this view if
    /// `numBytes <= length()` and remove `length()` bytes otherwise; return
    /// a view to the bytes that were removed.  Note that this method will
    /// decrease the length of this view by `min(numBytes, length())` bytes.
    FuzzDataView removePrefix(bsl::size_t numBytes);

    /// Remove the specified last `numBytes` from this view if
    /// `numBytes <= length()` and remove `length()` bytes otherwise; return
    /// a view to the bytes that were removed.  Note that this method will
    /// decrease the length of this view by `min(numBytes, length())` bytes.
    FuzzDataView removeSuffix(bsl::size_t numBytes);

    // ACCESSORS

    /// Return a const pointer to the beginning of the buffer.
    const bsl::uint8_t *begin() const;

    /// Return a const pointer to the end of the buffer.
    const bsl::uint8_t *end() const;

    /// Return the length in bytes of the buffer.
    bsl::size_t length() const;

    /// Return a pointer to the beginning of the buffer.
    const bsl::uint8_t *data() const;
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                            // ------------------
                            // class FuzzDataView
                            // ------------------

// CREATORS
inline
FuzzDataView::FuzzDataView(const bsl::uint8_t *data, bsl::size_t size)
: d_data_p(data)
, d_length(size)
{
    BSLS_ASSERT(data || (0 == size));
}

// MANIPULATORS
inline
FuzzDataView FuzzDataView::removePrefix(bsl::size_t numBytes)
{
    FuzzDataView prefix(d_data_p, bsl::min(numBytes, length()));

    d_data_p += prefix.length();
    d_length -= prefix.length();

    return prefix;
}

inline
FuzzDataView FuzzDataView::removeSuffix(bsl::size_t numBytes)
{
    bsl::size_t num = bsl::min(numBytes, length());

    FuzzDataView suffix(end() - num, num);
    d_length -= num;

    return suffix;
}

// ACCESSORS
inline
const bsl::uint8_t* FuzzDataView::begin() const
{
    return d_data_p;
}

inline
const bsl::uint8_t* FuzzDataView::end() const
{
    return d_data_p + d_length;
}

inline
bsl::size_t FuzzDataView::length() const
{
    return d_length;
}

inline
const bsl::uint8_t *FuzzDataView::data() const
{
    return d_data_p;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
