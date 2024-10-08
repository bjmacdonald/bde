// bdlt_intervalconversionutil.h                                      -*-C++-*-
#ifndef INCLUDED_BDLT_INTERVALCONVERSIONUTIL
#define INCLUDED_BDLT_INTERVALCONVERSIONUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functions to convert between time-interval representations.
//
//@CLASSES:
//  bdlt::IntervalConversionUtil: functions to convert time intervals
//
//@SEE_ALSO: bdlt_datetimeinterval, bsls_timeinterval
//
//@DESCRIPTION: This component provides a utility `struct`,
// `bdlt::IntervalConversionUtil`, that defines functions to convert between
// C++ value types providing different representations of time intervals,
// (e.g., `bsls::TimeInterval` and `bdlt::DatetimeInterval`).
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Interfacing With an API That Uses `bsls::TimeInterval`
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Some APIs, such as `bsls::SystemTime`, use `bsls::TimeInterval` in their
// interface.  In order to use those APIs in components implemented in terms of
// `bdlt::DatetimeInterval`, it is necessary to convert between the
// `bsls::TimeInterval` and `bdlt::DatetimeInterval` representations for a time
// interval.  This conversion can be accomplished conveniently using
// `bdlt::IntervalConversionUtil`.
//
// Suppose we wish to pass the system time -- as returned by
// `bsls::SystemTime::nowRealtimeClock` -- to a function that displays a time
// that is represented as a `bdlt::DatetimeInterval` since the UNIX epoch.
//
// First, we include the declaration of the function that displays a
// `bdlt::DatetimeInterval`:
// ```
// void displayTime(const bdlt::DatetimeInterval& timeSinceEpoch);
// ```
// Then, we obtain the current system time from `bsls::SystemTime`, and store
// it in a `bsls::TimeInterval`:
// ```
// bsls::TimeInterval systemTime = bsls::SystemTime::nowRealtimeClock();
// ```
// Now, we convert the `bsls::TimeInterval` into a `bdlt::DatetimeInterval`
// using `convertToDatetimeInterval`:
// ```
// bdlt::DatetimeInterval timeSinceEpoch =
//        bdlt::IntervalConversionUtil::convertToDatetimeInterval(systemTime);
//
// assert(timeSinceEpoch.totalMilliseconds() ==
//                                            systemTime.totalMilliseconds());
// ```
// Finally, we display the time by passing the converted value to
// `displayTime`:
// ```
// displayTime(timeSinceEpoch);
// ```

#include <bdlscm_version.h>

#include <bdlt_datetimeinterval.h>
#include <bdlt_timeunitratio.h>

#include <bsls_assert.h>
#include <bsls_buildtarget.h>
#include <bsls_review.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>

#include <bsl_climits.h>

namespace BloombergLP {
namespace bdlt {

                        // =============================
                        // struct IntervalConversionUtil
                        // =============================

/// This utility `struct`, `IntervalConversionUtil`, defines functions to
/// convert between `bsls::TimeInterval` and `bdlt::DatetimeInterval`
/// representations of time intervals.
struct IntervalConversionUtil {

  private:
    // PRIVATE TYPES
    typedef TimeUnitRatio      Ratio;
    typedef bsls::Types::Int64 Int64;

    // PRIVATE CLASS DATA
    static const Int64 k_INT_MAX = INT_MAX;
    static const Int64 k_INT_MIN = INT_MIN;
    static const Int64 k_DATETIME_INTERVAL_SECONDS_MAX =
                                (k_INT_MAX + 1) * Ratio::k_SECONDS_PER_DAY - 1;
    static const Int64 k_DATETIME_INTERVAL_SECONDS_MIN =
                                (k_INT_MIN - 1) * Ratio::k_SECONDS_PER_DAY + 1;
    static const Int64 k_DATETIME_INTERVAL_NSEC_REMAINDER_MAX =
                                (Ratio::k_MICROSECONDS_PER_SECOND - 1)
                                        * Ratio::k_NANOSECONDS_PER_MICROSECOND;
    static const Int64 k_DATETIME_INTERVAL_NSEC_REMAINDER_MIN =
                                       -k_DATETIME_INTERVAL_NSEC_REMAINDER_MAX;

  public:
    // CLASS METHODS

    /// Return as a `bdlt::DatetimeInterval` the (approximate) value of the
    /// specified `interval` truncated toward zero, to microsecond
    /// resolution.  The behavior is undefined unless the value of
    /// `interval`, expressed with nanosecond precision, is within the range
    /// of time intervals supported by a `DatetimeInterval` -- i.e.,
    /// `DT_MIN * 10^6 <= TI_NSECS <= DT_MAX * 10^6`, where `TI_NSECS` is
    /// the total number of nanoseconds in `interval`, `DT_MIN` is the
    /// lowest (negative) value expressible by `DatetimeInterval`, and
    /// `DT_MAX` is the highest (positive) value expressible by
    /// `DatetimeInterval`.  Note that, while `bsls::TimeInterval` has
    /// nanosecond resolution, `bdlt::DatetimeInterval` has only microsecond
    /// resolution.
    static DatetimeInterval convertToDatetimeInterval(
                                          const bsls::TimeInterval&  interval);

    /// Return as a `bsls::TimeInterval` the value of the specified
    /// `interval`.
    static bsls::TimeInterval convertToTimeInterval(
                                            const DatetimeInterval&  interval);
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                       // -----------------------------
                       // struct IntervalConversionUtil
                       // -----------------------------

// CLASS METHODS
inline
DatetimeInterval IntervalConversionUtil::convertToDatetimeInterval(
                                            const bsls::TimeInterval& interval)
{
    // Check that the value of 'interval' is within the valid range supported
    // by 'Dateinterval'.

    BSLS_ASSERT(   k_DATETIME_INTERVAL_SECONDS_MIN  < interval.seconds()
                    || (k_DATETIME_INTERVAL_SECONDS_MIN == interval.seconds()
                       && k_DATETIME_INTERVAL_NSEC_REMAINDER_MIN <=
                                                      interval.nanoseconds()));

    BSLS_ASSERT(   interval.seconds() <  k_DATETIME_INTERVAL_SECONDS_MAX
                    || (interval.seconds() == k_DATETIME_INTERVAL_SECONDS_MAX
                       && interval.nanoseconds() <=
                                      k_DATETIME_INTERVAL_NSEC_REMAINDER_MAX));

    // Note that we need to access the time via a comination of the
    // 'totalSeconds' and 'nanoseconds' accessors, because the time range
    // expressable in a 'DatetimeInterval' is greater than can be expressed in
    // an 'Int64' by 'totalMicroseconds'.

    return DatetimeInterval(0,  // days
                            0,  // hours
                            0,  // minutes
                            interval.totalSeconds(),
                            0,  // milliseconds
                            interval.nanoseconds() /
                                      Ratio::k_NANOSECONDS_PER_MICROSECOND_32);
}

inline
bsls::TimeInterval IntervalConversionUtil::convertToTimeInterval(
                                              const DatetimeInterval& interval)
{
    return bsls::TimeInterval(interval.totalSeconds(),
                              Ratio::k_NANOSECONDS_PER_MILLISECOND_32 *
                                                      interval.milliseconds() +
                              Ratio::k_NANOSECONDS_PER_MICROSECOND_32 *
                                                      interval.microseconds());
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
