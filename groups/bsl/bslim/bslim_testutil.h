// bslim_testutil.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLIM_TESTUTIL
#define INCLUDED_BSLIM_TESTUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide test utilities for components above `bsl`.
//
//@CLASSES:
//
//@MACROS:
//  BSLIM_TESTUTIL_ASSERT(X): record and print error if `!X`
//  BSLIM_TESTUTIL_LOOP_ASSERT(I, X): print arguments if `!X`
//  BSLIM_TESTUTIL_LOOP2_ASSERT(I, J, X): print arguments if `!X`
//  BSLIM_TESTUTIL_LOOP3_ASSERT(I, J, K, X): print arguments if `!X`
//  BSLIM_TESTUTIL_LOOP4_ASSERT(I, J, K, L, X): print arguments if `!X`
//  BSLIM_TESTUTIL_LOOP5_ASSERT(I, J, K, L, M, X): print arguments if `!X`
//  BSLIM_TESTUTIL_LOOP6_ASSERT(I, J, K, L, M, N, X): print arguments if `!X`
//  BSLIM_TESTUTIL_LOOP7_ASSERT(I, J, K, L, M, N,O, X): print arguments if `!X`
//  BSLIM_TESTUTIL_LOOP8_ASSERT(I, J, K, L,M,N,O,V, X): print arguments if `!X`
//  BSLIM_TESTUTIL_ASSERTV(..., X): print generic arguments if `!X`
//  BSLIM_TESTUTIL_Q(X): quote identifier literally
//  BSLIM_TESTUTIL_P(X): print identifier and value
//  BSLIM_TESTUTIL_P_(X): print identifier and value without '\n'
//  BSLIM_TESTUTIL_L_: current line number
//  BSLIM_TESTUTIL_T_: print tab without '\n'
//
//@SEE_ALSO: bsls_bsltestutil
//
//@DESCRIPTION: This component provides the standard print macros used in
// BDE-style test drivers (`ASSERT`, `LOOP_ASSERT`, `ASSERTV`, `P`, `Q`, `L`,
// and `T`) for components above the `bsl` package group.
//
// This component also defines a set of overloads for the insertion operator
// (`<<`) to support the streaming of test types defined in the `bsltf`
// package.  These overloads are required for test drivers above the `bsl`
// package group in order to print objects of the `bsltf` types to `bsl::cout`.
//
// This component also defines a pair of methods, `setFunc` and `callFunc`,
// that allow a test driver to set and call a function by going through another
// compilation unit to preclude the optimizer from inlining the function call.

// Note that the 'bsltf' package resides below 'bsl+bslhdrs', in which
// 'bsl::cout' is defined; therefore, the components in 'bsltf' cannot directly
// define the overloads of the insertion operator to support printing the test
// types.  Instead, an alternate method supplied in 'bsls_bsltestutil' is used
// for test drivers in the 'bsl' package group.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Writing a Test Driver
/// - - - - - - - - - - - - - - - -
// First, we write an elided component to test, which provides a utility class:
//..
//  namespace bdlabc {
//
//  struct ExampleUtil {
//      // This utility class provides sample functionality to demonstrate how
//      // a test driver might be written validating its only method.
//
//      // CLASS METHODS
//      static int fortyTwo();
//          // Return the integer value 42.
//  };
//
//  // CLASS METHODS
//  inline
//  int ExampleUtil::fortyTwo()
//  {
//      return 42;
//  }
//
//  }  // close package namespace
//..
// Then, we can write an elided test driver for this component.  We start by
// providing the standard BDE assert test macro:
//..
//  //=========================================================================
//  //                       STANDARD BDE ASSERT TEST MACRO
//  //-------------------------------------------------------------------------
//  static int testStatus = 0;
//
//  static void aSsErT(bool b, const char *s, int i)
//  {
//      if (b) {
//          printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
//          fflush(stdout);
//          if (testStatus >= 0 && testStatus <= 100) ++testStatus;
//      }
//  }
//..
// Next, we define the standard print and 'LOOP_ASSERT' macros, as aliases to
// the macros defined by this component:
//..
//  //=========================================================================
//  //                       STANDARD BDE TEST DRIVER MACROS
//  //-------------------------------------------------------------------------
//
//  #define ASSERT       BSLIM_TESTUTIL_ASSERT
//  #define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
//  #define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
//  #define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
//  #define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
//  #define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
//  #define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
//  #define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
//  #define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
//  #define LOOP7_ASSERT BSLIM_TESTUTIL_LOOP7_ASSERT
//  #define LOOP8_ASSERT BSLIM_TESTUTIL_LOOP8_ASSERT
//  #define ASSERTV      BSLIM_TESTUTIL_ASSERTV
//
//  #define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
//  #define P            BSLIM_TESTUTIL_P   // Print identifier and value.
//  #define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
//  #define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
//  #define L_           BSLIM_TESTUTIL_L_  // current Line number
//..
// Now, using the (standard) abbreviated macro names we have just defined, we
// write a test function for the 'static' 'fortyTwo' method, to be called from
// a test case in the test driver:
//..
//  void testFortyTwo(bool verbose)
//      // Test 'bdlabc::ExampleUtil::fortyTwo' in the specified 'verbose'
//      // verbosity level.
//  {
//      const int value = bdlabc::ExampleUtil::fortyTwo();
//      if (verbose) P(value);
//      LOOP_ASSERT(value, 42 == value);
//  }
//..
// Finally, when 'testFortyTwo' is called from a test case in verbose mode we
// observe the console output:
//..
//  value = 42
//..

#include <bslscm_version.h>

#include <bslmf_assert.h>

#include <bsl_iostream.h>
#include <bsl_string.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsltf_templatetestfacility.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

                       // =================
                       // Macro Definitions
                       // =================

#define BSLIM_TESTUTIL_ASSERT(X)                                              \
    aSsErT(!(X), #X, __LINE__);

#define BSLIM_TESTUTIL_DEBUG_REP(X) BloombergLP::bslim::TestUtil::debugRep(X)

#define BSLIM_TESTUTIL_LOOP0_ASSERT                                           \
    BSLIM_TESTUTIL_ASSERT

#define BSLIM_TESTUTIL_LOOP_ASSERT(I,X)                                       \
    if (!(X)) { bsl::cout << #I ": " << BSLIM_TESTUTIL_DEBUG_REP(I) <<        \
                                                         "    (context)\n";   \
                aSsErT(1, #X, __LINE__); }

#define BSLIM_TESTUTIL_LOOP1_ASSERT                                           \
    BSLIM_TESTUTIL_LOOP_ASSERT

#define BSLIM_TESTUTIL_LOOP2_ASSERT(I,J,X)                                    \
    if (!(X)) { bsl::cout << #I ": " << BSLIM_TESTUTIL_DEBUG_REP(I) << "\t"   \
                          << #J ": " << BSLIM_TESTUTIL_DEBUG_REP(J) <<        \
                                                         "    (context)\n";   \
                aSsErT(1, #X, __LINE__); }

#define BSLIM_TESTUTIL_LOOP3_ASSERT(I,J,K,X)                                  \
    if (!(X)) { bsl::cout << #I ": " << BSLIM_TESTUTIL_DEBUG_REP(I) << "\t"   \
                          << #J ": " << BSLIM_TESTUTIL_DEBUG_REP(J) << "\t"   \
                          << #K ": " << BSLIM_TESTUTIL_DEBUG_REP(K) <<        \
                                                         "    (context)\n";   \
                aSsErT(1, #X, __LINE__); }

#define BSLIM_TESTUTIL_LOOP4_ASSERT(I,J,K,L,X)                                \
    if (!(X)) { bsl::cout << #I ": " << BSLIM_TESTUTIL_DEBUG_REP(I) << "\t"   \
                          << #J ": " << BSLIM_TESTUTIL_DEBUG_REP(J) << "\t"   \
                          << #K ": " << BSLIM_TESTUTIL_DEBUG_REP(K) << "\t"   \
                          << #L ": " << BSLIM_TESTUTIL_DEBUG_REP(L) <<        \
                                                         "    (context)\n";   \
                aSsErT(1, #X, __LINE__); }

#define BSLIM_TESTUTIL_LOOP5_ASSERT(I,J,K,L,M,X)                              \
    if (!(X)) { bsl::cout << #I ": " << BSLIM_TESTUTIL_DEBUG_REP(I) << "\t"   \
                          << #J ": " << BSLIM_TESTUTIL_DEBUG_REP(J) << "\t"   \
                          << #K ": " << BSLIM_TESTUTIL_DEBUG_REP(K) << "\t"   \
                          << #L ": " << BSLIM_TESTUTIL_DEBUG_REP(L) << "\t"   \
                          << #M ": " << BSLIM_TESTUTIL_DEBUG_REP(M) <<        \
                                                         "    (context)\n";   \
               aSsErT(1, #X, __LINE__); }

#define BSLIM_TESTUTIL_LOOP6_ASSERT(I,J,K,L,M,N,X)                            \
    if (!(X)) { bsl::cout << #I ": " << BSLIM_TESTUTIL_DEBUG_REP(I) << "\t"   \
                          << #J ": " << BSLIM_TESTUTIL_DEBUG_REP(J) << "\t"   \
                          << #K ": " << BSLIM_TESTUTIL_DEBUG_REP(K) << "\t"   \
                          << #L ": " << BSLIM_TESTUTIL_DEBUG_REP(L) << "\t"   \
                          << #M ": " << BSLIM_TESTUTIL_DEBUG_REP(M) << "\t"   \
                          << #N ": " << BSLIM_TESTUTIL_DEBUG_REP(N) <<        \
                                                         "    (context)\n";   \
               aSsErT(1, #X, __LINE__); }

#define BSLIM_TESTUTIL_LOOP7_ASSERT(I,J,K,L,M,N,O,X)                          \
    if (!(X)) { bsl::cout << #I ": " << BSLIM_TESTUTIL_DEBUG_REP(I) << "\t"   \
                          << #J ": " << BSLIM_TESTUTIL_DEBUG_REP(J) << "\t"   \
                          << #K ": " << BSLIM_TESTUTIL_DEBUG_REP(K) << "\t"   \
                          << #L ": " << BSLIM_TESTUTIL_DEBUG_REP(L) << "\t"   \
                          << #M ": " << BSLIM_TESTUTIL_DEBUG_REP(M) << "\t"   \
                          << #N ": " << BSLIM_TESTUTIL_DEBUG_REP(N) << "\t"   \
                          << #O ": " << BSLIM_TESTUTIL_DEBUG_REP(O) <<        \
                                                         "    (context)\n";   \
               aSsErT(1, #X, __LINE__); }

#define BSLIM_TESTUTIL_LOOP8_ASSERT(I,J,K,L,M,N,O,V,X)                        \
    if (!(X)) { bsl::cout << #I ": " << BSLIM_TESTUTIL_DEBUG_REP(I) << "\t"   \
                          << #J ": " << BSLIM_TESTUTIL_DEBUG_REP(J) << "\t"   \
                          << #K ": " << BSLIM_TESTUTIL_DEBUG_REP(K) << "\t"   \
                          << #L ": " << BSLIM_TESTUTIL_DEBUG_REP(L) << "\t"   \
                          << #M ": " << BSLIM_TESTUTIL_DEBUG_REP(M) << "\t"   \
                          << #N ": " << BSLIM_TESTUTIL_DEBUG_REP(N) << "\t"   \
                          << #O ": " << BSLIM_TESTUTIL_DEBUG_REP(O) << "\t"   \
                          << #V ": " << BSLIM_TESTUTIL_DEBUG_REP(V) <<        \
                                                         "    (context)\n";   \
               aSsErT(1, #X, __LINE__); }

// The 'BSLIM_TESTUTIL_EXPAND' macro is required to work around a preprocessor
// issue on Windows that prevents '__VA_ARGS__' from being expanded in the
// definition of 'BSLIM_TESTUTIL_NUM_ARGS'.

#define BSLIM_TESTUTIL_EXPAND(X)                                              \
    X

#define BSLIM_TESTUTIL_NUM_ARGS_IMPL(X8, X7, X6, X5, X4, X3, X2, X1, X0,      \
                                     N, ...)                                  \
    N

#define BSLIM_TESTUTIL_NUM_ARGS(...)                                          \
    BSLIM_TESTUTIL_EXPAND(BSLIM_TESTUTIL_NUM_ARGS_IMPL(                       \
                                   __VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1, 0, ""))

#define BSLIM_TESTUTIL_LOOPN_ASSERT_IMPL(N, ...)                              \
    BSLIM_TESTUTIL_EXPAND(BSLIM_TESTUTIL_LOOP ## N ## _ASSERT(__VA_ARGS__))

#define BSLIM_TESTUTIL_LOOPN_ASSERT(N, ...)                                   \
    BSLIM_TESTUTIL_LOOPN_ASSERT_IMPL(N, __VA_ARGS__)

#define BSLIM_TESTUTIL_ASSERTV(...)                                           \
    BSLIM_TESTUTIL_LOOPN_ASSERT(                                              \
                             BSLIM_TESTUTIL_NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

/// Quote identifier literally.
#define BSLIM_TESTUTIL_Q(X)                                                   \
    bsl::cout << "<| " #X " |>" << bsl::endl;

/// Print identifier and its value.
#define BSLIM_TESTUTIL_P(X)                                                   \
    bsl::cout << #X " = " << BSLIM_TESTUTIL_DEBUG_REP(X) << bsl::endl;

/// `P(X)` without '\n'
#define BSLIM_TESTUTIL_P_(X)                                                  \
    bsl::cout << #X " = " << BSLIM_TESTUTIL_DEBUG_REP(X) << ", " << bsl::flush;

/// current Line number
#define BSLIM_TESTUTIL_L_                                                     \
    __LINE__

/// Print tab (w/o newline).
#define BSLIM_TESTUTIL_T_                                                     \
    bsl::cout << "\t" << bsl::flush;

namespace BloombergLP {
namespace bslim {

                        // ==============
                        // class TestUtil
                        // ==============

/// This `struct` provides a namespace for a suite of utility functions that
/// facilitate the creation of BDE-style test drivers.
struct TestUtil {

  private:
    // PRIVATE CLASS METHODS

    /// Return `ptr` without modification.  Note that this is NOT an inline
    /// function, so that if the caller is not in the same module, the
    /// compiler has no way of knowing that it's an identity transform.
    static void *identityPtr(void *ptr);

  public:
    // CLASS METHODS

    /// Return `true` if the specified `lhs` has the same value as the
    /// specified `rhs`, and `false` otherwise.  Optionally specify an
    /// `errorStream` on which, if `lhs` and `rhs` are not the same', a
    /// description of how the two strings differ will be written.  If
    /// `errorStream` is not supplied, `stdout` is used.
    static bool compareText(bslstl::StringRef lhs,
                            bslstl::StringRef rhs,
                            bsl::ostream&     errorStream = bsl::cout);

    /// Return the specified `functionPtr` (expected to be a static function
    /// pointer) without modification.  The value of `functionPtr` is
    /// transformed through `identityPtr` so that if the caller is in a
    /// different module, the compiler will have no way of knowing that this
    /// is an identity transform and thus no way of inlining the call.
    ///
    /// Note: the Windows optimizer is still able to inline the call, it may
    /// be comparing the result of this function with the argument and
    /// branching to inline on equality and call on inequality, so the
    /// Windows optimizer has to be turned off with
    /// `# pragma optimize("", off)`.
    ///
    /// Also note that even with an optimizer that can't figure out that
    /// this is an identity transform, there is still the possibility of
    /// chaining the call.
    template <class FUNCTION_PTR>
    static FUNCTION_PTR makeFunctionCallNonInline(FUNCTION_PTR functionPtr);

    template <class T>
    static const T& debugRep(const T& arg) { return arg; }
    static int      debugRep(wchar_t  arg) { return arg; }
};

// ============================================================================
//                           INLINE FUNCTION DEFINITIONS
// ============================================================================

                                // --------
                                // TestUtil
                                // --------

template <class FUNCTION_PTR>
inline
FUNCTION_PTR TestUtil::makeFunctionCallNonInline(FUNCTION_PTR function)
{
    BSLMF_ASSERT(sizeof(FUNCTION_PTR) == sizeof(void *));

    return reinterpret_cast<FUNCTION_PTR>(identityPtr(reinterpret_cast<void *>(
                                                                   function)));
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2012 Bloomberg Finance L.P.
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
