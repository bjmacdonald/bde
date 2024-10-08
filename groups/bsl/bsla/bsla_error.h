// bsla_error.h                                                       -*-C++-*-
#ifndef INCLUDED_BSLA_ERROR
#define INCLUDED_BSLA_ERROR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a macro to emit an error message when a function is called.
//
//@MACROS:
//  BSLA_ERROR(QUOTED_MESSAGE): emit error message and fail compilation
//  BSLA_ERROR_IS_ACTIVE:       defined if `BSLA_ERROR` is active
//
//@SEE_ALSO: bsla_annotations
//
//@DESCRIPTION: This component provides a preprocessor macro that flags a
// function such that the compile will fail with an error message when the
// function is called.  On platforms where the appropriate attribute is not
// supported, the macro expands to nothing.
//
///Macro Reference
///---------------
//: `BSLA_ERROR(QUOTED_MESSAGE)`:
//:     This annotation, when used, will cause compilation to fail with an
//:     error message when a call to the so-annotated function is not removed
//:     through dead-code elimination or other optimizations.  While it is
//:     possible to leave the function undefined, thus incurring a link-time
//:     failure, with the use of this macro the invalid call will be diagnosed
//:     earlier (i.e., at compile time), and the diagnostic will include the
//:     location of the function call.  The message `QUOTED_MESSAGE`, which
//:     should be a double-quoted string, will appear in the error message.
//
//: `BSLA_ERROR_IS_ACTIVE`:
//:     The macro `BSLA_ERROR_IS_ACTIVE` is defined if `BSLA_ERROR` expands to
//:     something with the desired effect; otherwise `BSLA_ERROR_IS_ACTIVE` is
//:     not defined and `BSLA_ERROR` expands to nothing.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Flagging a Function for a Compile Failure and Message if Used
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// First, we declare and define a function annotated with `BSLA_ERROR`.  Note
// that the argument to `BSLA_ERROR` must be a quoted string:
// ```
// void usageFunc() BSLA_ERROR("Don't call 'usageFunc'");
//     // Do nothing.
//
// void usageFunc()
// {
// }
// ```
// Now, we call `usageFunc`:
// ```
// usageFunc();
// ```
// Finally, observe that the compile fails with the following error message:
// ```
// .../bsla_error.t.cpp:226:16: error: call to 'usageFunc' declared with
// attribute error: Don't call 'usageFunc'
//      usageFunc();
//                 ^
// ```

#include <bsls_platform.h>

                       // =============================
                       // Checks for Pre-Defined macros
                       // =============================

#if defined(BSLA_ERROR)
#error BSLA_ERROR is already defined!
#endif

#if defined(BSLA_ERROR_IS_ACTIVE)
#error BSLA_ERROR_IS_ACTIVE is already defined!
#endif

                       // =========================
                       // Set macros as appropriate
                       // =========================

#if defined(BSLS_PLATFORM_CMP_GNU)
    // The '__error__' attribute is not supported by clang as of version 7.0.

    #define BSLA_ERROR(QUOTED_MESSAGE)                                        \
                                     __attribute__((__error__(QUOTED_MESSAGE)))

    #define BSLA_ERROR_IS_ACTIVE 1
#else
    #define BSLA_ERROR(QUOTED_MESSAGE)
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
