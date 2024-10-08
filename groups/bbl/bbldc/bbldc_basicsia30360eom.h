// bbldc_basicsia30360eom.h                                           -*-C++-*-
#ifndef INCLUDED_BBLDC_BASICSIA30360EOM
#define INCLUDED_BBLDC_BASICSIA30360EOM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide stateless functions for SIA 30/360 end-of-month convention.
//
//@CLASSES:
//  bbldc::BasicSia30360Eom: SIA 30/360 eom convention stateless functions
//
//@DESCRIPTION: This component provides a `struct`, `bbldc::BasicSia30360Eom`,
// that serves as a namespace for defining a suite of date-related functions
// used to compute the day count and year fraction between two dates as
// prescribed by the Standard Industry Association (SIA) 30/360 day-count
// convention with end-of-month (eom) adjustments.  In this day-count
// convention (also known as "US 30/360" or just "30/360"), each year is
// assumed to have 12 months and 360 days, with each month consisting of
// exactly 30 days.  Special end-of-month rule adjustments *are* made to
// account for the last day of February.  Note that in this day-count
// convention, the second date may or may not be adjusted depending on the
// first date.
//
///SIA-30/360-eom Day Count Algorithm
///----------------------------------
// Given `beginDate` and `endDate`, let:
// ```
// Ye =  year of earlier date           Yl =  year of later date
// Me = month of earlier date           Ml = month of later date
// De =   day of earlier date           Dl =   day of later date
//
//   o If Dl is the last day of February (29 in a leap year, else 28)
//     and De is the last day of February, change Dl to 30.
//
//   o If De is the last day of February, change De to 30.
//
//   o If Dl is 31 and De is 30 or 31, change Dl to 30.
//
//   o If De is 31, change De to 30.
//
// daysDiff ::= sign(endDate - beginDate) *
//                                   (Yl - Ye) * 360 + (Ml - Me) * 30 + Dl - De
// ```
// Reference: Standard Securities Calculation Methods (1996)
//            ISBN 1-882936-01-9
//
// The year fraction is simply the day count divided by 360.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Computing Day Count and Year Fraction
/// - - - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to use
// `bbldc::BasicSia30360Eom` methods.  First, create two `bdlt::Date`
// variables, `d1` and `d2`:
// ```
// const bdlt::Date d1(2004,  9, 30);
// const bdlt::Date d2(2004, 12, 31);
// ```
// Then, compute the day count between the two dates:
// ```
// const int daysDiff = bbldc::BasicSia30360Eom::daysDiff(d1, d2);
// assert(90 == daysDiff);
// ```
// Finally, compute the year fraction between the two dates:
// ```
// const double yearsDiff = bbldc::BasicSia30360Eom::yearsDiff(d1, d2);
// assert(0.25 == yearsDiff);
// ```

#include <bblscm_version.h>

namespace BloombergLP {
namespace bdlt {

class Date;

}  // close namespace bdlt

namespace bbldc {

                         // =======================
                         // struct BasicSia30360Eom
                         // =======================

/// This `struct` provides a namespace for a suite of pure functions that
/// compute values based on dates according to the SIA 30/360 end-of-month
/// day-count convention.
struct BasicSia30360Eom {

    // CLASS METHODS

    /// Return the (signed) number of days between the specified `beginDate`
    /// and `endDate` according to the SIA 30/360 end-of-month day-count
    /// convention.  If `beginDate <= endDate`, then the result is
    /// non-negative.  Note that reversing the order of `beginDate` and
    /// `endDate` negates the result.
    static int daysDiff(const bdlt::Date& beginDate,
                        const bdlt::Date& endDate);

    /// Return the (signed fractional) number of years between the specified
    /// `beginDate` and `endDate` according to the SIA 30/360 end-of-month
    /// day-count convention.  If `beginDate <= endDate`, then the result is
    /// non-negative.  Note that reversing the order of `beginDate` and
    /// `endDate` negates the result; specifically,
    /// `|yearsDiff(b, e) + yearsDiff(e, b)| <= 1.0e-15` for all dates `b`
    /// and `e`.
    static double yearsDiff(const bdlt::Date& beginDate,
                            const bdlt::Date& endDate);
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
