// bsla_used.h                                                        -*-C++-*-
#ifndef INCLUDED_BSLA_USED
#define INCLUDED_BSLA_USED

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a macro to prevent elision of unused entities.
//
//@MACROS:
//  BSLA_USED:           emit annotated entity even if not referenced
//  BSLA_USED_IS_ACTIVE: defined if `BSLA_USED` is active
//
//@SEE_ALSO: bsla_annotations
//
//@DESCRIPTION: This component provides a preprocessor macro that will
// guarantee the emission of a local function, type, or variable whether it is
// used or not.
//
///Macro Reference
///---------------
//: `BSLA_USED`:
//:      This annotation indicates that the so-annotated function, variable, or
//:      type must be emitted even if it appears that it is not referenced.
//:
//: `BSLA_USED_IS_ACTIVE`:
//:     The macro `BSLA_USED_IS_ACTIVE` is defined if `BSLA_USED` expands to
//:     something with the desired effect; otherwise `BSLA_USED_IS_ACTIVE` is
//:     not defined and `BSLA_USED` expands to nothing.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Unused Variables
///- - - - - - - - - - - - - -
// First, we declare two unused static variables, one marked `BSLA_UNUSED` and
// the other marked `BSLA_USED`:
// ```
// static
// int usage_UNUSED_variable_no_warning BSLA_UNUSED;
//
// static
// int usage_USED_variable_no_warning BSLA_USED;
// ```
// Finally, if we compile with clang and go into the debugger and stop in
// `main`, which is in the same file and from which both variables are visible,
// we observe that the variable marked `BSLA_UNUSED` cannot be accessed, but
// the variable marked `BSLA_USED` can.
//
///Example 2: Unused Functions
///- - - - - - - - - - - - - -
// First, declare two unused static functions, one marked `BSLA_UNUSED` and one
// marked `BSLA_USED`:
// ```
// /// Print the specified 'woof'.
// static
// void usage_UNUSED_function_no_warning(int woof) BSLA_UNUSED;
//
// static
// void usage_UNUSED_function_no_warning(int woof)
// {
//     printf("%d\n", woof);
// }
//
// /// Print the specified 'woof'.
// static
// void usage_USED_function_no_warning(int woof) BSLA_USED;
//
// static
// void usage_USED_function_no_warning(int woof)
// {
//     printf("%d\n", woof);
// }
// ```
// Finally, if we compile with clang and go into the debugger, we find that we
// can put a breakpoint in the function marked `BSLA_USED`, but not in the
// function marked `BSLA_UNUSED`.

#include <bsls_platform.h>

// Note that we could conceivably migrate this to use '[[maybe_unused]]' when
// available, but that has more specific constraints over where it can be
// syntactically placed than the older vendor annotations.

                       // =============================
                       // Checks for Pre-Defined macros
                       // =============================

#if defined(BSLA_USED)
#error BSLA_USED is already defined!
#endif

#if defined(BSLA_USED_IS_ACTIVE)
#error BSLA_USED_IS_ACTIVE is already defined!
#endif

                       // =========================
                       // Set macros as appropriate
                       // =========================

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLA_USED       __attribute__((__used__))

    #define BSLA_USED_IS_ACTIVE 1
#else
    #define BSLA_USED
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
