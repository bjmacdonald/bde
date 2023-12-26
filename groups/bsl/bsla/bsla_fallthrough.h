// bsla_fallthrough.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLA_FALLTHROUGH
#define INCLUDED_BSLA_FALLTHROUGH

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a macro to suppress warnings on 'switch' fall-throughs.
//
//@MACROS:
//  BSLA_FALLTHROUGH: do not warn if 'switch' 'case' falls through
//  BSLA_FALLTHROUGH_IS_ACTIVE: defined if 'BSLA_FALLTHROUGH' is active
//
//@SEE_ALSO: bsla_annotations
//
//@DESCRIPTION: This component provides a preprocessor macro that suppresses
// compiler warnings about flow of control fall-through from one 'case' or
// 'default' of a 'switch' statement to another.  On compilers where the
// appropriate attribute is not supported, the macro expands to nothing.
//
///Macro Reference
///---------------
//: 'BSLA_FALLTHROUGH':
//:     This annotation should be placed in a 'case' clause as the last
//:     statement within a flow of control that is expected to allow control to
//:     fall through instead of ending with a 'break', 'continue', or 'return'.
//:     This will prevent compilers from warning about fall-through.  The
//:     'BSLA_FALLTHROUGH' must be followed by a semicolon and may be nested
//:     within blocks, 'if's, or 'else's.
//
//: 'BSLA_FALLTHROUGH_IS_ACTIVE':
//:     The macro 'BSLA_FALLTHROUGH_IS_ACTIVE' is defined if 'BSLA_FALLTHROUGH'
//:     expands to something with the desired effect; otherwise
//:     'BSLA_FALLTHROUGH_IS_ACTIVE' is not defined and 'BSLA_FALLTHROUGH'
//:     expands to nothing.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Suppressing Fall-Through Warnings in a 'switch' Statement
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// First, we define a function:
//..
//  int usageFunction(int jj)
//      // Demonstrate the usage of 'BSLA_FALLTHROUGH', read the specified
//      // 'jj'.
//  {
//      for (int ii = 0; ii < 5; ++ii) {
//..
// Then, we have a 'switch' in the function:
//..
//          switch (ii) {
//            case 0: {
//              printf("%d\n", jj - 3);
//..
// Next, we see that 'BSLA_FALLTHROUGH;' as the last statement in a 'case'
// block before falling through silences the fall-through warning from the
// compiler:
//..
//               BSLA_FALLTHROUGH;
//            }
//            case 1:
//..
// Then, we see this also works on 'case's that don't have a '{}' block:
//..
//              jj -= 6;
//              printf("%d\n", jj);
//              BSLA_FALLTHROUGH;
//            case 2: {
//              if (jj > 4) {
//                  printf("%d\n", jj + 10);
//..
// Next, we see that a 'BSLA_FALLTHROUGH;' works within an 'if' block, provided
// that it's in the last statement in the flow of control before falling
// through:
//..
//                  BSLA_FALLTHROUGH;
//              }
//              else {
//                  return 0;                                         // RETURN
//              }
//            }
//            case 3: {
//              if (jj > 4) {
//                  continue;
//              }
//              else {
//                  printf("%d\n", ++jj);
//..
// Now, we see that a 'BSLA_FALLTHROUGH;' can also occur as the last statement
// in an 'else' block:
//..
//                  BSLA_FALLTHROUGH;
//              }
//            }
//            default: {
//              return 1;                                             // RETURN
//            } break;
//          }
//      }
//
//      return -7;
//  }
//..
// Finally, we see that if we compile when 'BSLA_FALLTHROUGH_IS_ACTIVE' is set,
// the above compiles with no warnings.

#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>

                       // =============================
                       // Checks for Pre-Defined macros
                       // =============================

#if defined(BSLA_FALLTHROUGH)
#error BSLA_FALLTHROUGH is already defined!
#endif

#if defined(BSLA_FALLTHROUGH_IS_ACTIVE)
#error BSLA_FALLTHROUGH_IS_ACTIVE is already defined!
#endif

                       // =========================
                       // Set macros as appropriate
                       // =========================

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ATTRIBUTE_FALLTHROUGH)
    #define BSLA_FALLTHROUGH [[ fallthrough ]]
#elif defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VERSION >= 70000
    #define BSLA_FALLTHROUGH __attribute__((fallthrough))
#elif defined(BSLS_PLATFORM_CMP_CLANG)
    #if __cplusplus >= 201103L && defined(__has_warning)
        #if  __has_feature(cxx_attributes) && \
             __has_warning("-Wimplicit-fallthrough")
            #define BSLA_FALLTHROUGH [[clang::fallthrough]]
        #endif
    #endif
#endif

#if defined(BSLA_FALLTHROUGH)
    #define BSLA_FALLTHROUGH_IS_ACTIVE                                        1
#else
    #define BSLA_FALLTHROUGH
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
