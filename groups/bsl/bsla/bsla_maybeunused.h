// bsla_maybeunused.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLA_MAYBEUNUSED
#define INCLUDED_BSLA_MAYBEUNUSED

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a macro to suppress "unused" warnings.
//
//@MACROS:
//  BSLA_MAYBE_UNUSED:           suppress compiler warnings on unused entities
//  BSLA_MAYBE_UNUSED_IS_ACTIVE: defined if `BSLA_MAYBE_UNUSED` is active
//
//@SEE_ALSO: bsla_annotations
//
//@DESCRIPTION: This component provides a preprocessor macro that will suppress
// "unused" warnings on a locally defined function, type, or variable that is
// not used.
//
///Macro Reference
///---------------
//: `BSLA_MAYBE_UNUSED`:
//:    This annotation indicates that the so-annotated function, variable, or
//:    type is possibly unused and the compiler should not generate a warning
//:    for the unused identifier.
//:
//: `BSLA_MAYBE_UNUSED_IS_ACTIVE`:
//:     The macro `BSLA_MAYBE_UNUSED_IS_ACTIVE` is defined if
//:     `BSLA_MAYBE_UNUSED` expands to something with the desired effect;
//:      otherwise `BSLA_MAYBE_UNUSED_IS_ACTIVE` is not defined and
//:     `BSLA_MAYBE_UNUSED` expands to nothing.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: "Unused" Warnings
/// - - - - - - - - - - - - - -
// First, we define a namespace, `warn`, within the unnamed namespace with a
// type, a function, and a variable in it.  They are unused:
// ```
// namespace {
// namespace warn {
//
// double x;
//
// struct ResultRec {
//     double d_x;
//     double d_y;
// };
//
// int quadratic(double *zeroA,
//               double *zeroB,
//               double *zeroC,
//               double cubeFactor,
//               double a,
//               double b,
//               double c)
//     // Solve the quadratic function for the specified 'a', 'b', and 'c',
//     // where '0 = a * x^2 + b * x + c'.  If the quadratic has no solutions,
//     // return a non-zero value, and set the specified 'zeroA' and 'zeroB'
//     // to those solutions and return 0 otherwise.  The specified
//     // 'cubeFactor' and 'zeroC' are unused for now but will be used in
//     // future expansion of the function to handle cubic polynomials.
// {
//     typedef int UnusedTypedef;
//
//     const double discriminant = b * b - 4 * a * c;
//     if (discriminant < 0 || 0.0 == a) {
//         *zeroA = *zeroB = 0.0;
//         return -1;                                                // RETURN
//     }
//
//     const double root = ::sqrt(discriminant);
//     *zeroA = (-b + root) / (2 * a);
//     *zeroB = (-b - root) / (2 * a);
//
//     return 0;
// }
//
// }  // close namespace warn
// }  // close unnamed namespace
// ```
// Then, we observe the warnings:
// ```
// .../bsla_maybeunused.t.cpp:145:21: warning: typedef 'UnusedTypedef' locally
// defined but not used [-Wunused-local-typedefs]
//          typedef int UnusedTypedef;
//                      ^~~~~~~~~~~~~
// .../bsla_maybeunused.t.cpp:133:27: warning: unused parameter 'zeroC'
// [-Wunused-parameter]
//                    double *zeroC,
//                            ^~~~~
// .../bsla_maybeunused.t.cpp:134:26: warning: unused parameter 'cubeFactor'
// [-Wunused-parameter]
//                    double cubeFactor,
//                           ^~~~~~~~~~
// .../bsla_maybeunused.t.cpp:131:9: warning: 'int {anonymous}::warn::
// quadratic(double*, double*, double*, double, double, double, double)'
// defined but not used [-Wunused-function]
//      int quadratic(double *zeroA,
//          ^~~~~~~~~
// .../bsla_maybeunused.t.cpp:124:12: warning: '{anonymous}::warn::x' defined
// but not used [-Wunused-variable]
//      double x;
//             ^
// ```
// Note that none of the compilers currently in use by the development team
// issue a warning on the unused `warn::ResultRec`, but some in the future
// might.  In the meantime, `BSLA_MAYBE_UNUSED` is tolerated on type
// declarations without resulting in a syntax error.
//
// Next, we define a namespace, `nowarn`, within the unused namespace with
// exactly the same unused entities, using the `BSLA_MAYBE_UNUSED` annotation
// to silence the warnings:
// ```
// namespace {
// namespace nowarn {
//
// struct BSLA_MAYBE_UNUSED ResultRec {
//     double d_x;
//     double d_y;
// };
//
// BSLA_MAYBE_UNUSED double x;
//
// BSLA_MAYBE_UNUSED int quadratic(double                   *zeroA,
//                                 double                   *zeroB,
//                                 BSLA_MAYBE_UNUSED double *zeroC,
//                                 BSLA_MAYBE_UNUSED double  cubeFactor,
//                                 double                    a,
//                                 double                    b,
//                                 double                    c);
//     // Solve the quadratic function for the specified 'a', 'b', and 'c',
//     // where '0 = a * x^2 + b * x + c'.  If the quadratic has no solutions,
//     // return a non-zero value, and set the specified 'zeroA' and 'zeroB'
//     // to those solutions and return 0 otherwise.  The specified
//     // 'cubeFactor' and 'zeroC' are unused for now but will be used in
//     // future expansion of the function to handle cubic polynomials.
//
// int quadratic(double                   *zeroA,
//               double                   *zeroB,
//               BSLA_MAYBE_UNUSED double *zeroC,
//               BSLA_MAYBE_UNUSED double  cubeFactor,
//               double                    a,
//               double                    b,
//               double                    c)
// {
//     BSLA_MAYBE_UNUSED typedef int UnusedTypedef;
//
//     const double discriminant = b * b - 4 * a * c;
//     if (discriminant < 0 || 0.0 == a) {
//         *zeroA = *zeroB = 0.0;
//         return -1;                                                // RETURN
//     }
//
//     const double root = ::sqrt(discriminant);
//     *zeroA = (-b + root) / (2 * a);
//     *zeroB = (-b - root) / (2 * a);
//
//     return 0;
// }
//
// }  // close namespace nowarn
// }  // close unnamed namespace
// ```
// Finally, we observe that the warnings for the `nowarn` namespace are
// suppressed.

#include <bsls_compilerfeatures.h>

                       // =============================
                       // Checks for Pre-Defined macros
                       // =============================

#if defined(BSLA_MAYBE_USED)
#error BSLA_MAYBE_USED is already defined!
#endif

#if defined(BSLA_MAYBE_USED_IS_ACTIVE)
#error BSLA_MAYBE_USED_IS_ACTIVE is already defined!
#endif

                       // =========================
                       // Set macros as appropriate
                       // =========================

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_MAYBE_UNUSED)
    #define BSLA_MAYBE_UNUSED [[ maybe_unused ]]

    #define BSLA_MAYBE_UNUSED_IS_ACTIVE 1
#elif defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLA_MAYBE_UNUSED __attribute__((__unused__))

    #define BSLA_MAYBE_UNUSED_IS_ACTIVE 1
#else
    #define BSLA_MAYBE_UNUSED
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
