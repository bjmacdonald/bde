// bsla_unreachable.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLA_UNREACHABLE
#define INCLUDED_BSLA_UNREACHABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compiler-hint macro to indicate unreachable code.
//
//@MACROS:
//  BSLA_UNREACHABLE: indicate that a statement is intended to be not reached
//  BSLA_UNREACHABLE_IS_ACTIVE: defined if 'BSLA_UNREACHABLE' is active
//
//@SEE_ALSO: bsla_annotations
//
//@DESCRIPTION: This component provides a preprocessor macro that hints to the
// compile that a statement in the code is intended to be unreachable.  Note
// that an instance of 'BSLA_UNREACHABLE' must be followed by a ';' and is a
// statement in its own right.
//
///Macro Reference
///---------------
//: 'BSLA_UNREACHABLE':
//:     This macro will, when used and followed by a semicolon, create a
//:     statement that emits no code, but that is indicated to be unreachable,
//:     causing compilers, where supported, to issue warnings if there is
//:     actually a way that the statement can be reached.  Note that the
//:     behavior is undefined if control actually reaches a 'BSLA_UNREACHABLE'
//:     statement.
//:
//: 'BSLA_UNREACHABLE_IS_ACTIVE':
//:     The macro 'BSLA_UNREACHABLE_IS_ACTIVE' is defined if 'BSLA_UNREACHABLE'
//:     expands to something with the desired effect; otherwise
//:     'BSLA_UNREACHABLE_IS_ACTIVE' is not defined and 'BSLA_UNREACHABLE'
//:     expands to nothing.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Indicating That a Statement is Intended to be Unreachable
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// First, we define a function, 'directoriesInPath', that counts the number of
// directories in the 'PATH' environment variable.  If 'PATH' is not set, the
// program dumps core by calling 'BSLS_ASSERT_OPT':
//..
//  int directoriesInPath()
//  {
//      const char *path = ::getenv("PATH");
//      if (path) {
//          int ret = 1;
//          for (; *path; ++path) {
//              ret += ':' == *path;
//          }
//          return ret;                                               // RETURN
//      }
//
//      BSLS_ASSERT_OPT(false && "$PATH not set");
//  }
//..
// Then, we observe a compile error because the compiler expects the
// 'BSLA_ASSERT_OPT' to return and the function to run off the end and return
// 'void', while the function is declared to return 'int'.
//..
//  .../bsla_unreachable.t.cpp(141) : error C4715: 'directoriesInPath': not all
//  control paths return a value
//..
// Now, we put a 'BSLA_UNREACHABLE' statement after the 'BSLS_ASSERT_OPT',
// which tells the compiler that that point in the code is unreachable:
//..
//  int directoriesInPath()
//  {
//      const char *path = ::getenv("PATH");
//      if (path) {
//          int ret = 1;
//          for (; *path; ++path) {
//              ret += ':' == *path;
//          }
//          return ret;                                               // RETURN
//      }
//
//      BSLS_ASSERT_OPT(false && "$PATH not set");
//
//      BSLA_UNREACHABLE;
//  }
//..
// Finally, we observe that the compiler error is silenced and the build is
// successful.

#include <bsls_platform.h>

                       // =============================
                       // Checks for Pre-Defined macros
                       // =============================

#if defined(BSLA_UNREACHABLE)
#error BSLA_UNREACHABLE is already defined!
#endif

#if defined(BSLA_UNREACHABLE_IS_ACTIVE)
#error BSLA_UNREACHABLE_IS_ACTIVE is already defined!
#endif

                       // =========================
                       // Set macros as appropriate
                       // =========================

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
    #define BSLA_UNREACHABLE __builtin_unreachable()

    #define BSLA_UNREACHABLE_IS_ACTIVE 1
#elif defined(BSLS_PLATFORM_CMP_MSVC)
    #define BSLA_UNREACHABLE __assume(false)

    #define BSLA_UNREACHABLE_IS_ACTIVE 1
#else
    #define BSLA_UNREACHABLE
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
