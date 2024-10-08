// bsla_nonnullarg.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLA_NONNULLARG
#define INCLUDED_BSLA_NONNULLARG

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide macros to hint at null arguments to functions.
//
//@MACROS:
//  BSLA_NONNULLARGS: warn if any pointer arguments are null
//  BSLA_NONNULLARG(...): warn if indexed arguments are null
//  BSLA_NONNULLARGS_IS_ACTIVE: defined if `BSLA_NONNULLARGS` is active
//  BSLA_NONNULLARG_IS_ACTIVE:  defined if `BSLA_NONNULLARG`  is active
//
//@SEE_ALSO: bsla_annotations
//
//@DESCRIPTION: This component provides preprocessor macros that define
// compiler-specific compile-time annotations.  These macros instruct the
// compiler to warn if null is passed to certain arguments to a function, or,
// on platforms where the feature is not supported, expand to nothing.
//
// Note that the annotations cause warnings to be emitted if the covered
// argument is passed:
// * `NULL`
// * 0
// * `static_cast<TYPE *>(0)`
// * `nullptr` (with C++11)
// * a `const` variable known to be 0 (clang only, no warning with g++)
//
///Macro Reference
///---------------
//: `BSLA_NONNULLARGS`:
//:     This annotation indicates that a compiler warning is to be issued if
//:     any of the pointer arguments to this function are passed null.
//:
//: `BSLA_NONNULLARG(...)`:
//:     This annotation, passed a variable-length list of positive integers,
//:     indicates that a compiler warning is to be issued if null is passed to
//:     a pointer argument at any of the specified indices, where the first
//:     argument of the annotated function has an index of 1. Note that for
//:     non-static member functions, the implicit `this` argument has index 1.
//:
//: `BSLA_NONNULLARG_IS_ACTIVE`:
//:     The macro `BSLA_NONNULLARGS_IS_ACTIVE` is defined if `BSLA_NONNULLARGS`
//:     expands to something with the desired effect; otherwise
//:     `BSLA_NONNULLARGS_IS_ACTIVE` is not defined and `BSLA_NONNULLARGS`
//:     expands to nothing.
//:
//: `BSLS_NONNULLARGS_IS_ACTIVE`:
//:     The macro `BSLA_NONNULLARG_IS_ACTIVE` is defined if `BSLA_NONNULLARG`
//:     expands to something with the desired effect; otherwise
//:     `BSLA_NONNULLARG_IS_ACTIVE` is not defined and `BSLA_NONNULLARG`
//:     expands to nothing.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Passing Null to Arguments Annotated as Non-Null
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// First, we define a function, `usagePrint1`, annotated such that a compiler
// warning will occur if the first argument of the annotated function is passed
// 0, `NULL`, `nullptr`, or (on clang) a null pointer constant expression:
// ```
// // Print the specified 'string' the specified 'repetition' times.
// void usagePrint1(const char *string, int repetition) BSLA_NONNULLARG(1);
//
// void usagePrint1(const char *string, int repetition)
// {
//     for (int ii = 0; ii < repetition; ++ii) {
//         printf("%s\n", string);
//     }
// }
// ```
// Then, we define a nearly identical function annotated with
// `BSLA_NONNULLARGS` instead.  Note that only pointer arguments are affected
// by this annotation -- `repetition` is not affected and may be passed 0
// without a warning being emitted:
// ```
// // Print the specified 'string' the specified 'repetition' times.
// void usagePrint2(const char *string, int repetition) BSLA_NONNULLARGS;
//
// void usagePrint2(const char *string, int repetition)
// {
//     for (int ii = 0; ii < repetition; ++ii) {
//         printf("%s\n", string);
//     }
// }
// ```
// So the two different annotations on these functions have an identical
// effect -- affecting the `string` argument but not the `repetition` argument.
//
// Next, in `main`, we call both functions with a non-null first argument, and
// observe that no warning occurs.  Note that even though 0 is passed to the
// integer argument to `usagePrint2` and the `BSLA_NONNULLARGS` annotation was
// used, non-pointer arguments are not affected by that annotation:
// ```
//     usagePrint1("woof", 0);
//     usagePrint2("meow", 0);
// ```
// Then, we call both functions passing the first argument a variable whose
// value is known by the compiler to be null, but since `np1` is a non-`const`
// variable, no warning is issued:
// ```
//     char *np1 = NULL;
//     usagePrint1(np1,    0);
//     usagePrint2(np1,    0);
// ```
// Now, we call both functions passing various forms of constant null pointer
// expressions to the first argument:
// ```
//     usagePrint1(   0, -10);
//     usagePrint2(   0, -10);
//
//     usagePrint1(NULL, -20);
//     usagePrint2(NULL, -20);
//
//     usagePrint1(static_cast<char *>(0), -30);
//     usagePrint2(static_cast<char *>(0), -30);
//
//     #if __cplusplus >= 201103L
//         usagePrint1(nullptr, -40);
//         usagePrint2(nullptr, -40);
//     #endif
//
//     char * const np2 = 0;   // 'np2', unlike 'np1' above, is 'const'.
//     usagePrint1(np2, -50);    // Warning with clang, not g++
//     usagePrint2(np2, -50);    // Warning with clang, not g++
// ```
// Finally, we observe that the above calls result in the following warnings
// with clang w/C++11 support:
// ```
// .../bsla_nonnullarg.t.cpp:376:30: warning: null passed to a callee that
// requires a non-null argument [-Wnonnull]
//     usagePrint1(   0, -10);
//                    ~     ^
// .../bsla_nonnullarg.t.cpp:377:30: warning: null passed to a callee that
// requires a non-null argument [-Wnonnull]
//     usagePrint2(   0, -10);
//                    ~     ^
// .../bsla_nonnullarg.t.cpp:379:30: warning: null passed to a callee that
// requires a non-null argument [-Wnonnull]
//     usagePrint1(NULL, -20);
//                 ~~~~     ^
// .../bsla_nonnullarg.t.cpp:380:30: warning: null passed to a callee that
// requires a non-null argument [-Wnonnull]
//     usagePrint2(NULL, -20);
//                 ~~~~     ^
// .../bsla_nonnullarg.t.cpp:382:48: warning: null passed to a callee that
// requires a non-null argument [-Wnonnull]
//     usagePrint1(static_cast<char *>(0), -30);
//                 ~~~~~~~~~~~~~~~~~~~~~~     ^
// .../bsla_nonnullarg.t.cpp:383:48: warning: null passed to a callee that
// requires a non-null argument [-Wnonnull]
//     usagePrint2(static_cast<char *>(0), -30);
//                 ~~~~~~~~~~~~~~~~~~~~~~     ^
// .../bsla_nonnullarg.t.cpp:386:37: warning: null passed to a callee that
// requires a non-null argument [-Wnonnull]
//         usagePrint1(nullptr, -40);
//                     ~~~~~~~     ^
// .../bsla_nonnullarg.t.cpp:387:37: warning: null passed to a callee that
// requires a non-null argument [-Wnonnull]
//         usagePrint2(nullptr, -40);
//                     ~~~~~~~     ^
// .../bsla_nonnullarg.t.cpp:391:29: warning: null passed to a callee that
// requires a non-null argument [-Wnonnull]
//     usagePrint1(np2, -50);    // Warning with clang, not g++
//                 ~~~     ^
// .../bsla_nonnullarg.t.cpp:392:29: warning: null passed to a callee that
// requires a non-null argument [-Wnonnull]
//     usagePrint2(np2, -50);    // Warning with clang, not g++
//                 ~~~     ^
// ```

#include <bsls_platform.h>

                       // =============================
                       // Checks for Pre-Defined macros
                       // =============================

#if defined(BSLA_NONNULLARG)
#error BSLA_NONNULLARG is already defined!
#endif

#if defined(BSLA_NONNULLARG_IS_ACTIVE)
#error BSLA_NONNULLARG_IS_ACTIVE is already defined!
#endif

#if defined(BSLA_NONNULLARGS)
#error BSLA_NONNULLARGS is already defined!
#endif

#if defined(BSLA_NONNULLARGS_IS_ACTIVE)
#error BSLA_NONNULLARGS_IS_ACTIVE is already defined!
#endif
                       // =========================
                       // Set macros as appropriate
                       // =========================

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLA_NONNULLARG(...)   __attribute__((__nonnull__(__VA_ARGS__)))
    #define BSLA_NONNULLARGS       __attribute__((__nonnull__))

    #define BSLA_NONNULLARG_IS_ACTIVE  1
    #define BSLA_NONNULLARGS_IS_ACTIVE 1
#else
    #define BSLA_NONNULLARG(...)
    #define BSLA_NONNULLARGS
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
