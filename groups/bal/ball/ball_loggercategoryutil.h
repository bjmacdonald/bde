// ball_loggercategoryutil.h                                          -*-C++-*-
#ifndef INCLUDED_BALL_LOGGERCATEGORYUTIL
#define INCLUDED_BALL_LOGGERCATEGORYUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a suite of utility functions for category management.
//
//@CLASSES:
//  ball::LoggerCategoryUtil: namespace for category management utilities
//
//@SEE_ALSO: ball_loggermanager, ball_categorymanager
//
//@DESCRIPTION: This component defines a `struct`, `ball::LoggerCategoryUtil`,
// that provides a set of utility functions for managing the categories
// contained in a `ball::LoggerManager` based on the notion of hierarchy.  In
// particular, the `setThresholdLevelsHierarchically` function modifies the
// threshold levels of each category in `ball::LoggerManager` whose name has
// the specified string as a prefix.  The `addCategoryHierarchically` function
// creates a new category that inherits threshold levels from the exiting
// category whose name is the longest prefix match, if such a category exists.
//
///Deprecation Notice
///------------------
// The `setThresholdLevels` function is deprecated in favor of
// `setThresholdLevelsHierarchically`.  The former is data-sensitive in the
// sense that the `*` located at the end of the specified category name will
// be treated as a special flag to turn on the prefix name matching, thus
// causing trouble for categories whose name ends with `*`.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Managing Categories
/// - - - - - - - - - - - - - - -
// The following code fragments illustrate basic usage of this component's
// `setThresholdLevelsHierarchically` and `addCategoryHierarchically` methods.
//
// First, we create two auxiliary functions that serve to print out the names
// and threshold level values of all the categories currently in the logger
// manager singleton:
// ```
// void printCategory(const ball::Category *category)
// {
//     bsl::cout << "\t[ " << category->categoryName()
//               << ", "   << category->recordLevel()
//               << ", "   << category->passLevel()
//               << ", "   << category->triggerLevel()
//               << ", "   << category->triggerAllLevel()
//               << " ]"   << bsl::endl;
// }
//
// void printAllCategories()
// {
//      ball::LoggerManager& lm = ball::LoggerManager::singleton();
//      using namespace bdlf::PlaceHolders;
//      lm.visitCategories(bdlf::BindUtil::bind(printCategory, _1));
// }
// ```
// Now, we set the default threshold levels of the logger manager object to
// [191, 95, 63, 31] (for brevity, the initialization of the logger manager
// singleton is elided):
// ```
//     ball::LoggerManager& lm = ball::LoggerManager::singleton();
//     lm.setDefaultThresholdLevels(191, 95, 63, 31);
// ```
// Then, we create two new categories, "EQ" and "EQ.MARKET", by calling the
// `addCategory` method of the logger manager class, with their threshold
// levels explicitly set to different values (which are also different from the
// default threshold levels):
// ```
//    lm.addCategory("EQ", 192, 96, 64, 32);
//    lm.addCategory("EQ.MARKET", 193, 97, 65, 33);
//    printAllCategories();
// ```
// The following is printed out by `printAllCategories`:
// ```
//    [ EQ, 192, 96, 64, 32 ]
//    [ EQ.MARKET, 193, 97, 65, 33 ]
// ```
// Next, we add a new category using `addCategoryHierarchically`:
// ```
//    ball::LoggerCategoryUtil::addCategoryHierarchically(&lm,
//                                                        "EQ.MARKET.NYSE");
//    printAllCategories();
// ```
// The new category with name "EQ.MARKET.NYSE" inherits its threshold levels
// from the category "EQ.MARKET" rather than having the default threshold
// levels or inheriting them from "EQ" because of the longest prefix matching
// rule:
// ```
//    [ EQ, 192, 96, 64, 32 ]
//    [ EQ.MARKET, 193, 97, 65, 33 ]
//    [ EQ.MARKET.NYSE, 193, 97, 65, 33 ]
// ```
// Then, we adjust the threshold levels for all categories whose name starts
// with "EQ.MARKET" using `setThresholdLevelsHierarchically`:
// ```
//    ball::LoggerCategoryUtil::setThresholdLevelsHierarchically(&lm,
//                                                               "EQ.MARKET",
//                                                               194,
//                                                               98,
//                                                               66,
//                                                               34);
//    printAllCategories();
// ```
// We will notice that the threshold levels of "EQ.MARKET" and
// "EQ.MARKET.NYSE" have been changed to the new values, while those of "EQ"
// remain unchanged:
// ```
//    [ EQ, 192, 96, 64, 32 ]
//    [ EQ.MARKET, 194, 98, 66, 34 ]
//    [ EQ.MARKET.NYSE, 194, 98, 66, 34 ]
// ```

#include <balscm_version.h>

namespace BloombergLP {
namespace ball {

class LoggerManager;
class Category;

                        // =========================
                        // struct LoggerCategoryUtil
                        // =========================

/// This struct provides a suite of utility functions that facilitate the
/// management of the categories in `LoggerManager`.
struct LoggerCategoryUtil {

    // CLASS METHODS

    /// Add, to the specified `loggerManager`, a new category having the
    /// specified `categoryName`; return the address of the modifiable new
    /// category on success, and 0, with no effect, if a category by that
    /// name already exists or if the number of existing categories in
    /// `loggerManager` has reached the maximum capacity.  The newly created
    /// category will have the same threshold levels as the category in
    /// `loggerManager` whose name is the longest non-empty prefix of
    /// `categoryName` if such a category exists, and the default threshold
    /// levels of `loggermanager` (which might be overridden by a default
    /// threshold levels callback) otherwise.
    static Category *addCategoryHierarchically(LoggerManager *loggerManager,
                                               const char    *categoryName);

    /// Set, in the specified `loggerManager`, the threshold levels of every
    /// category whose name has, as a prefix, the specified
    /// `categoryNamePrefix` to the specified threshold values,
    /// `recordLevel`, `passLevel`, `triggerLevel`, and `triggerAllLevel`.
    /// Return the number of categories whose threshold levels were set, and
    /// a negative value, with no effect, if any of the specified threshold
    /// values is outside the range `[0 .. 255]`.  The behavior is undefined
    /// unless `loggerManager` is not null and `categoryNamePrefix` is
    /// null-terminated.
    static int setThresholdLevelsHierarchically(
                                             LoggerManager *loggerManager,
                                             const char    *categoryNamePrefix,
                                             int            recordLevel,
                                             int            passLevel,
                                             int            triggerLevel,
                                             int            triggerAllLevel);

    /// Set the threshold levels of each category currently in the registry
    /// of the specified `loggerManager` whose name matches the specified
    /// `pattern` to the specified `recordLevel`, `passLevel`,
    /// `triggerLevel`, and `triggerAllLevel` values, respectively, if each
    /// of the threshold values is in the range `[0 .. 255]`.  Return the
    /// number of categories whose threshold levels were set, and a negative
    /// value if any of the threshold values were invalid.  `pattern` is
    /// assumed to be of the form "X" or "X*" where X is a sequence of 0 or
    /// more characters and `*` matches any string (including the empty
    /// string).  The behavior is undefined unless `loggerManager` is not in
    /// the process of being destroyed.  Note that only a `*` located at the
    /// end of `pattern` is recognized as a special character.  Also note
    /// that this function has no effect on the threshold levels of
    /// categories added to the registry after it is called.
    ///
    /// **DEPRECATED**: Use `setThresholdLevelsHierarchically` instead.
    static int setThresholdLevels(LoggerManager *loggerManager,
                                  const char    *pattern,
                                  int            recordLevel,
                                  int            passLevel,
                                  int            triggerLevel,
                                  int            triggerAllLevel);
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
