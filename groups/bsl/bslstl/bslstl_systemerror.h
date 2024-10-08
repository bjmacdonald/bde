// bslstl_systemerror.h                                               -*-C++-*-
#ifndef INCLUDED_BSLSTL_SYSTEMERROR
#define INCLUDED_BSLSTL_SYSTEMERROR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

// BDE_VERIFY pragma: -TP25  // CLASSES are not defined in C++11

//@PURPOSE: Provide a standard compliant `system_error` class.
//
//@CLASSES:
//  bsl::system_error: a standard compliant `system_error` class
//
//@CANONICAL_HEADER: bsl_system_error.h
//
//@DESCRIPTION: This component defines class `bsl::system_error`, a class used
// for annotated exception objects about `errno`-style errors.  In C++11 mode,
// the vendor-supplied `<system_error>` implementation is used instead, and the
// corresponding names from `std` are imported into `bsl`.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Adding Annotation to an Error
/// - - - - - - - - - - - - - - - - - - - -
// Suppose we want to add an informative message when a system error occurs and
// include that as part of an exception that we throw when reporting the error.
// We can use `bsl::system_error` to do that.
//
// First, reset `errno` to avoid detecting old problems.
// ```
// errno = 0;
// ```
// Then, do something that will fail and set `errno`.
// ```
// strtod("1e2000", 0);
// ```
// Next, check that `errno` was actually set.
// ```
// assert(ERANGE == errno);
// ```
//  Finally, prepare an annotated exception and verify the annotation and the
//  error code stored within it.
// ```
// bsl::system_error annotated(errno, generic_category(), "1e2000");
// assert(strstr(annotated.what(), "1e2000"));
// assert(static_cast<int>(bsl::errc::result_out_of_range) ==
//        annotated.code().value());
// assert(&generic_category() == &annotated.code().category());
// ```

#include <bslscm_version.h>

#include <bsls_libraryfeatures.h>

#include <bslstl_errc.h>
#include <bslstl_error.h>

#include <errno.h>

#include <cstring>
#include <functional>
#include <stdexcept>
#include <string>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

#include <system_error>

namespace bsl {
using std::system_error;
}  // close namespace bsl

#else

namespace bsl {

                             // ==================
                             // class system_error
                             // ==================

/// This class represents exceptions that have an associated error code.
class system_error : public std::runtime_error {

  public:
    // CREATORS

    /// Create an object of this type holding the specified `code`.
    /// Optionally specify a string `what` to be added to the description of
    /// this object.
    system_error(error_code code, const std::string& what);
    system_error(error_code code, const char *what);
    system_error(error_code code);                                  // IMPLICIT

    /// Create an object of this type holding an error code holding the
    /// specified `value` and `category`.  Optionally specify a string
    /// `what` to be added to the description of this object.
    system_error(int                   value,
                 const error_category& category,
                 const std::string&    what);
    system_error(int value, const error_category& category, const char *what);
    system_error(int value, const error_category& category);

    // ACCESSORS

    /// Return a `const` reference to the error code held by this object.
    const error_code& code() const;

  private:
    // DATA
    error_code d_code;  // error code
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                             // ------------------
                             // class system_error
                             // ------------------

// CREATORS
inline
system_error::system_error(error_code code, const std::string& what)
: std::runtime_error(what + std::string(": ") + code.message())
, d_code(code)
{
}

inline
system_error::system_error(error_code code, const char *what)
: std::runtime_error(what + std::string(": ") + code.message())
, d_code(code)
{
}

inline
system_error::system_error(error_code code)
: std::runtime_error(code.message())
, d_code(code)
{
}

inline
system_error::system_error(int                   value,
                           const error_category& category,
                           const std::string&    what)
: std::runtime_error(what + std::string(": ") +
                     category.message(value))
, d_code(value, category)
{
}

inline
system_error::system_error(int                    value,
                           const error_category&  category,
                           const char            *what)
: std::runtime_error(what + std::string(": ") +
                            category.message(value))
, d_code(value, category)
{
}

inline
system_error::system_error(int value, const error_category& category)
: std::runtime_error(category.message(value))
, d_code(value, category)
{
}

// ACCESSORS
inline
const error_code& system_error::code() const
{
    return d_code;
}

}  // close namespace bsl

#endif
#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
