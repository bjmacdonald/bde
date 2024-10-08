// bdlt_datetimeutil.h                                                -*-C++-*-
#ifndef INCLUDED_BDLT_DATETIMEUTIL
#define INCLUDED_BDLT_DATETIMEUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide common non-primitive operations on `bdlt::Datetime`.
//
//@CLASSES:
//   bdlt::DatetimeUtil: non-primitive functions on `bdlt::Datetime`
//
//@SEE_ALSO: bdlt_datetime, bdlt_datetimeinterval, bdlt_epochutil
//
//@DESCRIPTION: This component provides non-primitive operations on
// `bdlt::Datetime` objects.  In particular, `bdlt::DatetimeUtil` supplies
// conversions of universal time to and from the C-standard `struct` `tm`
// (which we alias as `bsl::tm`) representations.
//
// This utility component provides the following (static) methods:
// ```
//  int convertFromTm(bdlt::Datetime *result, const tm& timeStruct);
//  bsl::tm convertToTm(const bdlt::Datetime& datetime);
// ```
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Converting Between `bsl::tm` and `bdlt::Datetime`
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// When interfacing with legacy systems, we may encounter calls that represent
// date/time information using the standard `bsl::tm`. In such cases, we have
// to be able to convert that information to/from a `bdlt::Datetime` object in
// order to interface with the rest of our systems.
//
// Suppose we have a legacy system that tracks last-access times in terms of
// `bsl::tm`. We can use the `convertToTm` and `convertFromTm` routines from
// this component to convert that information.
//
// First, we define a class, `MyAccessTracker`, that the legacy system uses to
// manage last-access times (eliding the implementation for brevity):
// ```
// /// This class provides a facility for tracking last access times
// /// associated with usernames.
// class MyAccessTracker {
//
//     // LOCAL TYPE
//     typedef bsl::map<bsl::string, bsl::tm>  TStringTmMap;
//
//     // DATA
//     TStringTmMap                            m_accesses; // map names to
//                                                         // accesses
//
//   public:
//     // TRAITS
//     BSLMF_NESTED_TRAIT_DECLARATION(MyAccessTracker,
//                                    bslma::UsesBslmaAllocator);
//
//     // CREATORS
//
//     /// Create an object which will track the last access time ...
//     explicit MyAccessTracker(bslma::Allocator *basicAllocator = 0);
//
//     // MANIPULATORS
//
//     /// Update the last access time for the specified `username` with
//     /// the specified `accessTime`.
//     void updateLastAccess(const bsl::string&  username,
//                           const bsl::tm&      accessTime);
//
//     // ACCESSORS
//
//     /// Load into the specified `result` the last access time associated
//     /// with the specified `username`, if any.  Return 0 on success, and
//     /// non-0 (with no effect on `result`) if there's no access time
//     /// associated with `username`.
//     int getLastAccess(bsl::tm *result, const bsl::string& username) const;
// };
// ```
// Next, we define a utility to allow us to use `bdlt::Datetime` with our
// legacy access tracker:
// ```
// class MyAccessTrackerUtil {
//   public:
//
//     /// Load into the specified `result` the last access time associated
//     /// with the specified `username` in the specified `tracker`, if
//     /// any.  Returns 0 on success, and non-0 (with no effect on
//     /// `result`) if there's no access time associated with `username`
//     /// or the associated access time cannot be converted to
//     /// `bdlt::Datetime`.
//     static int getLastAccess(bdlt::Datetime         *result,
//                              const MyAccessTracker&  tracker,
//                              const bsl::string&      username);
//
//     /// Update the instance pointed to by the specified `tracker` by
//     /// adding the specified `username` with its associated specified
//     /// `accessTime`.
//     static void updateLastAccess(MyAccessTracker       *tracker,
//                                  const bsl::string&     username,
//                                  const bdlt::Datetime&  accessTime);
// };
// ```
// Then, we implement `getLastAccess`:
// ```
//                         // -------------------------
//                         // class MyAccessTrackerUtil
//                         // -------------------------
//
// int MyAccessTrackerUtil::getLastAccess(bdlt::Datetime         *result,
//                                        const MyAccessTracker&  tracker,
//                                        const bsl::string&      username)
// {
//     BSLS_ASSERT(result);
//
//     bsl::tm legacyAccessTime;
//
//     int rc = tracker.getLastAccess(&legacyAccessTime, username);
//
//     if (rc) {
//         return rc;                                                // RETURN
//     }
//
//     return bdlt::DatetimeUtil::convertFromTm(result, legacyAccessTime);
// }
// ```
// Next, we implement `updateLastAccess`:
// ```
// void MyAccessTrackerUtil::updateLastAccess(
//                                          MyAccessTracker       *tracker,
//                                          const bsl::string&     username,
//                                          const bdlt::Datetime&  accessTime)
// {
//     BSLS_ASSERT(tracker);
//
//     bsl::tm legacyAccessTime;
//
//     legacyAccessTime = bdlt::DatetimeUtil::convertToTm(accessTime);
//
//     tracker->updateLastAccess(username, legacyAccessTime);
// }
// ```
// Finally, we create an access tracker then interact with it using
// `bdlt::Datetime` times.
// ```
// /// Exercise `MyAccessTracker` for pedagogical purposes.
// void exerciseTracker()
// {
//     MyAccessTracker accessTracker; // Datetime each user last accessed a
//                                    // resource.
//
//     bsl::string    richtofenName = "Baron von Richtofen";
//     bdlt::Datetime richtofenDate(1918, 4, 21, 11, 0, 0);
//     MyAccessTrackerUtil::updateLastAccess(&accessTracker,
//                                            richtofenName,
//                                            richtofenDate);
//
//     // ... some time later ....
//
//     bdlt::Datetime lastAccessTime;
//     int rc = MyAccessTrackerUtil::getLastAccess(&lastAccessTime,
//                                                  accessTracker,
//                                                  richtofenName);
//     assert(0 == rc);
//     assert(lastAccessTime == richtofenDate);
//
//     // Do something with the retrieved date...
// }
// ```

#include <bdlscm_version.h>

#include <bdlt_datetime.h>

#include <bsls_assert.h>
#include <bsls_review.h>

#include <bsl_ctime.h>            // 'bsl::tm'

namespace BloombergLP {
namespace bdlt {

                            // ===================
                            // struct DatetimeUtil
                            // ===================

/// This utility `struct` provides a namespace for a suite of functions
/// operating on objects of type `Datetime`.
struct DatetimeUtil {

  public:
    // CLASS METHODS

    /// Load into the specified `result` the value of the specified
    /// `timeStruct`.  Return 0 on success, and a non-zero value with no
    /// effect on `result` if `timeStruct` is invalid or otherwise cannot be
    /// represented as a `Datetime`.  Values in fields `tm_wday`, `tm_yday`,
    /// and `tm_isdst` are ignored.  The time 24:00:00 will be recognized,
    /// and leap seconds (i.e., values in `tm_sec` of 60 or 61) which can
    /// otherwise be represented as a `Datetime` will cause the conversion
    /// to succeed with the `result` "rolling over" into the zeroth second
    /// of next minute.  Note that time zones are irrelevant for this
    /// conversion.
    static int convertFromTm(Datetime *result, const bsl::tm& timeStruct);

    /// Return or load into the specified `result` the value of the
    /// specified `datetime` expressed as a `bsl::tm`.  Each field in the
    /// result is set to its proper value except `tm_isdst`, which is set to
    /// `-1` to indicate that no information on daylight saving time is
    /// available.  A time value of 24:00:00:00 will be converted to
    /// 0:00:00.  Note that time zones are irrelevant for this conversion.
    static bsl::tm convertToTm(                 const Datetime& datetime);
    static void    convertToTm(bsl::tm *result, const Datetime& datetime);
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                            // -------------------
                            // struct DatetimeUtil
                            // -------------------

inline
int DatetimeUtil::convertFromTm(Datetime       *result,
                                const bsl::tm&  timeStruct)
{
    BSLS_ASSERT(result);

    bool isLeapSecond = false;
    int  seconds      = timeStruct.tm_sec;

    if (seconds > 59) {
        // Start handling leap seconds by shifting to the previous non-leap
        // second time.
        isLeapSecond   = true;
        seconds        = 59;
    }

    int rc = result->setDatetimeIfValid(timeStruct.tm_year + 1900,
                                        timeStruct.tm_mon  + 1,
                                        timeStruct.tm_mday,
                                        timeStruct.tm_hour,
                                        timeStruct.tm_min,
                                        seconds);                   // msec = 0

    if (isLeapSecond && !rc) {
        // Finish leap second handling by rolling over into second '0' in the
        // next minute.
        result->addSeconds(1);
    }

    return rc;
}

inline
bsl::tm DatetimeUtil::convertToTm(const Datetime& datetime)
{

    // 'struct tm' may contain non POSIX standard fields (e.g., on Linux/OSX),
    // which we want to 0 initialize.

    bsl::tm result = bsl::tm();

    result.tm_sec   = datetime.second();
    result.tm_min   = datetime.minute();
    const int hour   = datetime.hour();
    if (24 == hour) {
        result.tm_hour = 0;
    }
    else {
        result.tm_hour = hour;
    }
    result.tm_mday  = datetime.day();
    result.tm_mon   = datetime.month() - 1;
    result.tm_year  = datetime.year() - 1900;
    result.tm_wday  = datetime.date().dayOfWeek() - 1;
    result.tm_yday  = datetime.date().dayOfYear() - 1;
    result.tm_isdst = -1;  // This information is unavailable.

    return result;
}

inline
void DatetimeUtil::convertToTm(bsl::tm *result, const Datetime& datetime)
{
    BSLS_ASSERT(result);

    *result = convertToTm(datetime);
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
