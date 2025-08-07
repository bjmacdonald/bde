// bslfmt_unicodecodepoint.t.cpp                                      -*-C++-*-
#include <bslfmt_unicodecodepoint.h>

#include <bsls_bsltestutil.h>
#include <bsls_libraryfeatures.h>

#include <stdio.h>   // `printf`
#include <stdlib.h>  // `atoi`
#include <string.h>  // `strlen`
#include <wchar.h>   // `wcslen`

using namespace BloombergLP;
using namespace bslfmt;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// TBD
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------
namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);
        fflush(stdout);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q BSLS_BSLTESTUTIL_Q    // Quote identifier literally.
#define P BSLS_BSLTESTUTIL_P    // Print identifier and value.
#define P_ BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_ BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLS_BSLTESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                               TEST MACROS
// ----------------------------------------------------------------------------

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
#define UTF8_LITERAL(...)                                                     \
    static_cast<const char *>(static_cast<const void *>(u8##__VA_ARGS__))
#else
#define UTF8_LITERAL(EXPR) EXPR
#endif

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    const int  test    = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose = argc > 2;

    printf("TEST %s CASE %d \n", __FILE__, test);

    switch (test) {  case 0:
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create an object and extract some code point values using
        //:   `extract` method.  Verify the results using accessors.  Return
        //:   the object to uninitialized state using `reset` method.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        unsigned char bytes[4];
        bytes[0] = (unsigned char)0xff;
        bytes[1] = (unsigned char)0xfe;
        bytes[2] = (unsigned char)0x00;
        bytes[2] = (unsigned char)0x00;

        UnicodeCodePoint codePoint;
        codePoint.extract(UnicodeCodePoint::e_UTF8, bytes, 4);

        ASSERT(!codePoint.isValid());
        codePoint.reset();
        codePoint.extract(sizeof(wchar_t) == 2 ? UnicodeCodePoint::e_UTF16
                                               : UnicodeCodePoint::e_UTF32,
                         bytes,
                         4);

        ASSERT(!codePoint.isValid());

        const char *fmt1 = (const char *) UTF8_LITERAL("\U0001F600");
        const int   len1 = (int)strlen(fmt1);

        codePoint.reset();
        codePoint.extract(UnicodeCodePoint::e_UTF8, (const void *)fmt1, len1);

        ASSERT(0x1f600 == codePoint.codePointValue());
        ASSERT(2       == codePoint.codePointWidth());
        ASSERT(true    == codePoint.isValid()       );
        ASSERT(4       == codePoint.numSourceBytes());

        if (sizeof(wchar_t) == 2) {
            const wchar_t *fmt2 = (const wchar_t *) L"\U0001F600";
            const int      len2 = (int)wcslen(fmt2);

            codePoint.reset();
            codePoint.extract(UnicodeCodePoint::e_UTF16,
                              (const void *)fmt2,
                              len2 * sizeof(wchar_t));

            ASSERT(4 == codePoint.numSourceBytes());
        }
        else {
            const wchar_t *fmt2 = (const wchar_t *) L"\U0001F600";
            const int      len2 = (int)wcslen(fmt2);

            codePoint.reset();
            codePoint.extract(UnicodeCodePoint::e_UTF32,
                              (const void *)fmt2,
                              len2 * sizeof(wchar_t));

            ASSERT(4 == codePoint.numSourceBytes());
        }

        ASSERT(0x1f600 == codePoint.codePointValue());
        ASSERT(2       == codePoint.codePointWidth());
        ASSERT(true    == codePoint.isValid()       );
      } break;
      default: {
        printf("WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        printf("Error, non-zero test status = %d .\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
