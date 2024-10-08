// bslmt_saturatedtimeconversionimputil.h                             -*-C++-*-
#ifndef INCLUDED_BSLMT_SATURATEDTIMECONVERSIONIMPUTIL
#define INCLUDED_BSLMT_SATURATEDTIMECONVERSIONIMPUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide special narrowing conversions for time types.
//
//@CLASSES:
//   bslmt::SaturatedTimeConversionImpUtil: saturating time conversions.
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a namespace containing static functions
// that perform narrowing conversions on time-related types,
// `bslmt::SaturatedTimeConversionImpUtil`.  The provided conversions
// "saturate", meaning that for input values outside the representable range of
// the destination variable, the destination variable will be set to its
// maximum or minimum value (whichever is closer to the input value).  Such
// saturating conversions are typically used to implement functions with a
// time-out (e.g., `bslmt::Condition::timedWait`) where a user-supplied
// time-out must be converted to a different time type before calling a
// system-library function.  In such situations it is often simpler, safer, and
// within contract to perform a saturating conversion, rather than either
// return an error status or define complex undefined behavior for the
// function.
//
///Conversion to Milliseconds
///--------------------------
// This component defines an overloaded function named `toMillisec`.  In
// addition to the saturating behavior defined above, `toMillisec` may also
// involve a loss of precision but without a concomitant loss of value.  In
// particular, the converted value will be the smallest representable value
// greater than or equal to the original value.
//
// For example, when `bsls::TimeInterval(0, 1234567)` is converted to an
// `unsigned int` using `toMillisec`, the result is 2 since 1234567 nanoseconds
// includes a fractional millisecond.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// Suppose we need to assign a value held in a `bsls::TimeInterval` to an
// `unsigned int`, where the `unsigned int` is to contain an equivalent time
// interval expressed in milliseconds.  A `bsls::TimeInterval` is able to
// represent intervals that are outside the range of intervals that can be
// represented by an `unsigned int` number of milliseconds (e.g., any negative
// time interval).  `bslmt::SaturatedTimeConversionImpUtil` handles values
// outside the representable range of the destination type by "saturating",
// that is values outside the representable range of the destination type will
// be assigned the maximum or minimum representable value of the destination
// type (whichever is closer to the source value).
//
// First, we define variables of our source (`bsls::TimeInterval`) and
// destination (`unsigned int`) types:
// ```
// unsigned int destinationInterval;
// bsls::TimeInterval sourceInterval;
// ```
// Then, we try a value that does not require saturation and observe that
// `toMillisec` converts it without modification (beyond loss of precision):
// ```
// sourceInterval.setInterval(4, 321000000);
// bslmt::SaturatedTimeConversionImpUtil::toMillisec(
//                                      &destinationInterval, sourceInterval);
// assert(4321 == destinationInterval);
// ```
// Next, we calculate the maximum value that can be represented in an
// `unsigned int` number of milliseconds, and verify that converting an
// equivalent `bsls::TimeInterval` does not modify the value:
// ```
// const unsigned int maxDestinationInterval =
//                                   bsl::numeric_limits<unsigned int>::max();
// bsls::TimeInterval maximumTimeInterval(
//                             maxDestinationInterval / 1000,
//                             (maxDestinationInterval % 1000) * 1000 * 1000);
// bslmt::SaturatedTimeConversionImpUtil::toMillisec(
//                                &destinationInterval, maximumTimeInterval);
// assert(maxDestinationInterval == destinationInterval);
// ```
// Now, we attempt to convert a value greater than the maximum representable in
// an `unsigned int` milliseconds and verify that the resulting value is the
// maximum representable `unsigned int` value:
// ```
// bsls::TimeInterval aboveMaxInterval = maximumTimeInterval +
//                                         bsls::TimeInterval(0, 1000 * 1000);
// bslmt::SaturatedTimeConversionImpUtil::toMillisec(
//                                    &destinationInterval, aboveMaxInterval);
// assert(maxDestinationInterval == destinationInterval);
// ```
// Next, we try a value less than 0 and observe the result of the saturated
// conversion is 0 (the minimum representable value):
// ```
// bsls::TimeInterval belowMinimumInterval(-1, 0);
// bslmt::SaturatedTimeConversionImpUtil::toMillisec(
//                                &destinationInterval, belowMinimumInterval);
// assert(0 == destinationInterval);
// ```
// Finally, when we convert a `bsls::TimeInterval` containing a fractional
// millisecond using `toMillisec`, the converted value is greater than the
// input value:
// ```
// bsls::TimeInterval piMSec(0, 3141593);  // 'pi' millseconds
// unsigned int       mSec;
// bslmt::SaturatedTimeConversionImpUtil::toMillisec(&mSec, piMSec);
// assert(4 == mSec);
// ```

#include <bslscm_version.h>

#include <bslmt_platform.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <bsls_platform.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>

#include <bsl_limits.h>
#include <bsl_ctime.h>

#include <time.h>   // POSIX timespec

#ifdef BSLS_PLATFORM_OS_DARWIN
#include <mach/clock_types.h>    // for 'mach_timespec_t'
#endif

namespace BloombergLP {
namespace bslmt {

                   // ====================================
                   // class SaturatedTimeConversionImpUtil
                   // ====================================

/// This `struct` provides a namespace for utility functions that convert
/// time values between different time representations, and "saturate" when
/// values are outside the range of values that may be represented in the
/// destination type (meaning that values above the maximum representable
/// value of the result type are set to the maximum value of the result
/// type, and values below the minimum representable value of the result
/// type are set to the minimum value for the result type).
struct SaturatedTimeConversionImpUtil {

    // TYPES

    // Here we define type 'TimeSpec' -- an alias to 'timespec' on Unix, but
    // defined as a struct on Windows, to guarantee that 'TimeSpec' exists on
    // all platforms.

    /// Provide type for Windows platform
#ifdef BSLMT_PLATFORM_POSIX_THREADS
    typedef timespec TimeSpec;
#else
    struct TimeSpec {

        bsls::Types::Int64 tv_sec;
        int                tv_nsec;
    };
#endif

    // CLASS METHODS

#if BSLS_PLATFORM_OS_DARWIN
    static void toTimeSpec(mach_timespec_t           *dst,
                           const bsls::TimeInterval&  src);
#endif

    /// Assign to the specified `dst` the value of the specified `src`, and
    /// if `src` is less than the lowest representable value of `*dst`, set
    /// `*dst` to the minimum value it can represent, and if `src` is
    /// greater than the highest representable value of `*dst`, set `*dst`
    /// to the maximum value that it can represent.
    static void toTimeSpec(TimeSpec *dst, const bsls::TimeInterval& src);

    /// Assign to the specified `dst` the value of the specified `src`, and
    /// if `src` is less than the lowest representable `time_t` value, set
    /// `dst` to the minimum `time_t` value, and if `src` is greater than
    /// the highest representable `time_t` value, set `dst` to the maximum
    /// `time_t` value.
    static void toTimeT(bsl::time_t *dst, const bsls::Types::Int64 src);

    /// Assign to the specified `dst` the value of the specified `src`
    /// converted to milliseconds, and if that value is a negative time
    /// interval, set `dst` to 0, and if that value is greater than the
    /// maximum representable value of `dst` set `dst` to its maximum
    /// representable value.  See {Conversion to Milliseconds}.
    static void toMillisec(unsigned int              *dst,
                           const bsls::TimeInterval&  src);
    static void toMillisec(unsigned long             *dst,
                           const bsls::TimeInterval&  src);
    static void toMillisec(bsls::Types::Uint64       *dst,
                           const bsls::TimeInterval&  src);
};

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
