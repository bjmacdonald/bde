// bslfmt_formatterintegral.t.cpp                                     -*-C++-*-
#include <bslfmt_formatterintegral.h>

#include <bslfmt_formattertestutil.h> // Testing only

#include <bsls_bsltestutil.h>

#include <bslstl_string.h>

#include <stdio.h>

using namespace BloombergLP;
using namespace bsl;

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
// [ 2] USAGE EXAMPLE

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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bslfmt::FormatterTestUtil<char>    TestUtilChar;
typedef bslfmt::FormatterTestUtil<wchar_t> TestUtilWchar;

// ============================================================================
//                       HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

namespace {

/// Check whether the `bslfmt::formatter<t_TYPE, char>::parse` function works
/// as expected for the specified `format` string created at runtime.  The
/// specified `line` is used to identify the function call location.
template <class t_TYPE>
void testRuntimeCharParse(int line, const char *format)
{
    bsl::string message;

    bool rv = TestUtilChar::testParseVFormat<t_TYPE>(&message, false, format);
    ASSERTV(line, format, message.c_str(), rv);
}

/// Check whether the `bslfmt::formatter<t_TYPE, wchar_t>::parse` function
/// works as expected for the specified `format` string created at runtime.
/// The specified `line` is used to identify the function call location.
template <class t_TYPE>
void testRuntimeWcharParse(int line, const wchar_t *format)
{
    bsl::string message;

    bool rv = TestUtilWchar::testParseVFormat<t_TYPE>(&message, false, format);
    ASSERTV(line, format, message.c_str(), rv);
}

/// Check whether the `bslfmt::formatter<t_TYPE, t_CHAR>::format` function
/// produces the specified `expected` result for the specified `format` and
/// `value`.  The specified `line` is used to identify the function call
/// location.
template <class t_CHAR, class t_TYPE>
void testRuntimeFormat(int           line,
                       const t_CHAR *expected,
                       const t_CHAR *format,
                       t_TYPE        value)
{
    bsl::string message;

    bool rv = bslfmt::FormatterTestUtil<t_CHAR>::testEvaluateVFormat(
                                                                     &message,
                                                                     expected,
                                                                     true,
                                                                     format,
                                                                     value);
    ASSERTV(line, format, message.c_str(), rv);
}

/// Check whether the `bslfmt::formatter<t_TYPE, t_CHAR>::format` function
/// produces the specified `expected` result for the specified `format`,
/// `width` and `value`.  The specified `line` is used to identify the function
/// call location.
template <class t_CHAR, class t_TYPE>
void testWidthRuntimeFormat(int           line,
                            const t_CHAR *expected,
                            const t_CHAR *format,
                            int           width,
                            t_TYPE        value)
{
    bsl::string message;

    bool rv = bslfmt::FormatterTestUtil<t_CHAR>::testEvaluateVFormat(
                                                                     &message,
                                                                     expected,
                                                                     true,
                                                                     format,
                                                                     value,
                                                                     width);
    ASSERTV(line, format, message.c_str(), rv);
}

}  // close unnamed namespace

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    const int  test    = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose = argc > 2;

    printf("TEST %s CASE %d \n", __FILE__, test);

    switch (test) {  case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        //: 1 Demonstrate the functioning of this component.
        //
        // Plan:
        //: 1 Use test contexts to format a single integer.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

///Example: Formatting an integer
/// - - - - - - - - - - - - - - -
// We do not expect most users of `bsl::format` to interact with this type
// directly and instead use `bsl::format` or `bsl::vformat`, so this example is
// necessarily unrealistic.
//
// Suppose we want to test this formatter's ability to format an integer with
// defined alignment and padding.
//
// ```
    bslfmt::MockParseContext<char> mpc("*<5x", 1);

    bsl::formatter<int, char> f;
    mpc.advance_to(f.parse(mpc));

    int value = 42;

    bslfmt::MockFormatContext<char> mfc(value, 0, 0);

    mfc.advance_to(bsl::as_const(f).format(value, mfc));

    ASSERT("2a***" == mfc.finalString());
// ```
      } break;
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
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nBREATHING TEST"
                   "\n==============\n");

        static const struct {
            int         d_line;        // source line number
            const char *d_format_p;    // format spec
            const char *d_expected_p;  // format
            int         d_value;       // value to be formatted
        } POSITIVE_CHAR_DATA[] = {
            //LINE  FORMAT        EXPECTED      VALUE
            //----  -----------   ---------     -----
            { L_,   "{0:}",       "0",          0    },
            { L_,   "{0:+}",      "+0",         0    },
            { L_,   "{0:-}",      "0",          0    },
            { L_,   "{0: }",      " 0",         0    },
            { L_,   "{:}",        "5",          5    },
            { L_,   "{:+}",       "+5",         5    },
            { L_,   "{:-}",       "5",          5    },
            { L_,   "{: }",       " 5",         5    },
            { L_,   "{:#05x}",    "0x00c",      12   },
            { L_,   "{:#05X}",    "0X00C",      12   },
            { L_,   "{:*<6}",     "5*****",     5    },
            { L_,   "{:*>6}",     "*****5",     5    },
            { L_,   "{:*^6}",     "**5***",     5    },
            { L_,   "{:*^+6}",    "**+5**",     5    },
            { L_,   "{:#0x}",     "0x5",        5    },
            { L_,   "{:#01x}",    "0x5",        5    },
            { L_,   "{:#02x}",    "0x5",        5    },
            { L_,   "{:#03x}",    "0x5",        5    },
            { L_,   "{:#04x}",    "0x05",       5    },
            { L_,   "{:#04X}",    "0X05",       5    },
            { L_,   "{:#b}",      "0b101",      5    },
            { L_,   "{:#05b}",    "0b101",      5    },
            { L_,   "{:#06b}",    "0b0101",     5    },
            { L_,   "{:#06B}",    "0B1100",     12   },
            { L_,   "{:#d}",      "5",          5    },
            { L_,   "{:#d}",      "12",         12   },
            { L_,   "{:#o}",      "0",          0    },
            { L_,   "{:#o}",      "05",         5    },
            { L_,   "{:#o}",      "014",        12   },
            { L_,   "{:b}",       "1100",       12   },
            { L_,   "{:B}",       "1100",       12   },
            { L_,   "{:c}",       "0",          48   },
            { L_,   "{:c}",       "A",          65   },
            { L_,   "{:d}",       "12",         12   },
            { L_,   "{:o}",       "0",          0    },
            { L_,   "{:o}",       "14",         12   },
            { L_,   "{:x}",       "c",          12   },
            { L_,   "{:X}",       "C",          12   },
            { L_,   "{:6b}",      "  1100",     12   },
            { L_,   "{:6B}",      "  1100",     12   },
            { L_,   "{:6c}",      "a     ",     97   },
            { L_,   "{:6d}",      "    12",     12   },
            { L_,   "{:6o}",      "    14",     12   },
            { L_,   "{:6x}",      "     c",     12   },
            { L_,   "{:6X}",      "     C",     12   },
            { L_,   "{:#<5x}",    "5####",      5    },
        };

        size_t NUM_POSITIVE_CHAR_DATA = sizeof POSITIVE_CHAR_DATA /
                                        sizeof *POSITIVE_CHAR_DATA;

        static const struct {
            int         d_line;        // source line number
            const char *d_format_p;    // format spec
            const char *d_expected_p;  // format
            int         d_value;       // value to be formatted
        } NEGATIVE_CHAR_DATA[] = {
            //LINE  FORMAT         EXPECTED        VALUE
            //----  -----------    ------------    -----
            { L_,   "{0:}",        "-5",           -5    },
            { L_,   "{0:+}",       "-5",           -5    },
            { L_,   "{0:-}",       "-5",           -5    },
            { L_,   "{0: }",       "-5",           -5    },
            { L_,   "{:#05x}",     "-0x0c",        -12   },
            { L_,   "{:#05X}",     "-0X0C",        -12   },
            { L_,   "{:*<6}",      "-5****",       -5    },
            { L_,   "{:*>6}",      "****-5",       -5    },
            { L_,   "{:*^6}",      "**-5**",       -5    },
            { L_,   "{:*^+6}",     "**-5**",       -5    },
            { L_,   "{:#0x}",      "-0x5",         -5    },
            { L_,   "{:#01x}",     "-0x5",         -5    },
            { L_,   "{:#02x}",     "-0x5",         -5    },
            { L_,   "{:#03x}",     "-0x5",         -5    },
            { L_,   "{:#04x}",     "-0x5",         -5    },
            { L_,   "{:#05X}",     "-0X05",        -5    },
            { L_,   "{:#b}",       "-0b101",       -5    },
            { L_,   "{:#05b}",     "-0b101",       -5    },
            { L_,   "{:#07b}",     "-0b0101",      -5    },
            { L_,   "{:#08B}",     "-0B01100",     -12   },
            { L_,   "{:#d}",       "-5",           -5    },
            { L_,   "{:#d}",       "-12",          -12   },
            { L_,   "{:#o}",       "-05",          -5    },
            { L_,   "{:#o}",       "-014",         -12   },
            { L_,   "{:b}",        "-1100",        -12   },
            { L_,   "{:B}",        "-1100",        -12   },
            { L_,   "{:d}",        "-12",          -12   },
            { L_,   "{:o}",        "-14",          -12   },
            { L_,   "{:x}",        "-c",           -12   },
            { L_,   "{:X}",        "-C",           -12   },
        };

        size_t NUM_NEGATIVE_CHAR_DATA = sizeof NEGATIVE_CHAR_DATA /
                                        sizeof *NEGATIVE_CHAR_DATA;

        static const struct {
            int            d_line;        // source line number
            const wchar_t *d_format_p;    // format spec
            const wchar_t *d_expected_p;  // format
            int            d_value;       // value to be formatted
        } POSITIVE_WCHAR_DATA[] = {
            //LINE  FORMAT         EXPECTED       VALUE
            //----  -----------    -----------    -----
            { L_,   L"{0:}",       L"5",          5    },
            { L_,   L"{0:+}",      L"+5",         5    },
            { L_,   L"{0:-}",      L"5",          5    },
            { L_,   L"{0: }",      L" 5",         5    },
            { L_,   L"{:#05x}",    L"0x00c",      12   },
            { L_,   L"{:#05X}",    L"0X00C",      12   },
            { L_,   L"{:*<6}",     L"5*****",     5    },
            { L_,   L"{:*>6}",     L"*****5",     5    },
            { L_,   L"{:*^6}",     L"**5***",     5    },
            { L_,   L"{:*^+6}",    L"**+5**",     5    },
            { L_,   L"{:#0x}",     L"0x5",        5    },
            { L_,   L"{:#01x}",    L"0x5",        5    },
            { L_,   L"{:#02x}",    L"0x5",        5    },
            { L_,   L"{:#03x}",    L"0x5",        5    },
            { L_,   L"{:#04x}",    L"0x05",       5    },
            { L_,   L"{:#04X}",    L"0X05",       5    },
            { L_,   L"{:#b}",      L"0b101",      5    },
            { L_,   L"{:#05b}",    L"0b101",      5    },
            { L_,   L"{:#06b}",    L"0b0101",     5    },
            { L_,   L"{:#06B}",    L"0B1100",     12   },
            { L_,   L"{:#d}",      L"5",          5    },
            { L_,   L"{:#d}",      L"12",         12   },
            { L_,   L"{:#o}",      L"0",          0    },
            { L_,   L"{:#o}",      L"05",         5    },
            { L_,   L"{:#o}",      L"014",        12   },
            { L_,   L"{:b}",       L"1100",       12   },
            { L_,   L"{:B}",       L"1100",       12   },
            { L_,   L"{:c}",       L"0",          48   },
            { L_,   L"{:c}",       L"A",          65   },
            { L_,   L"{:d}",       L"12",         12   },
            { L_,   L"{:o}",       L"0",          0    },
            { L_,   L"{:o}",       L"14",         12   },
            { L_,   L"{:x}",       L"c",          12   },
            { L_,   L"{:X}",       L"C",          12   },
            { L_,   L"{:6b}",      L"  1100",     12   },
            { L_,   L"{:6B}",      L"  1100",     12   },
            { L_,   L"{:6c}",      L"a     ",     97   },
            { L_,   L"{:6d}",      L"    12",     12   },
            { L_,   L"{:6o}",      L"    14",     12   },
            { L_,   L"{:6x}",      L"     c",     12   },
            { L_,   L"{:6X}",      L"     C",     12   },
            { L_,   L"{:#<5x}",    L"5####",      5    },
        };

        size_t NUM_POSITIVE_WCHAR_DATA = sizeof POSITIVE_WCHAR_DATA /
                                         sizeof *POSITIVE_WCHAR_DATA;

        static const struct {
            int            d_line;        // source line number
            const wchar_t *d_format_p;    // format spec
            const wchar_t *d_expected_p;  // format
            int            d_value;       // value to be formatted
        } NEGATIVE_WCHAR_DATA[] = {
            //LINE  FORMAT          EXPECTED         VALUE
            //----  -----------     ------------     -----
            { L_,   L"{0:}",        L"-5",           -5    },
            { L_,   L"{0:+}",       L"-5",           -5    },
            { L_,   L"{0:-}",       L"-5",           -5    },
            { L_,   L"{0: }",       L"-5",           -5    },
            { L_,   L"{:#05x}",     L"-0x0c",        -12   },
            { L_,   L"{:#05X}",     L"-0X0C",        -12   },
            { L_,   L"{:*<6}",      L"-5****",       -5    },
            { L_,   L"{:*>6}",      L"****-5",       -5    },
            { L_,   L"{:*^6}",      L"**-5**",       -5    },
            { L_,   L"{:*^+6}",     L"**-5**",       -5    },
            { L_,   L"{:#0x}",      L"-0x5",         -5    },
            { L_,   L"{:#01x}",     L"-0x5",         -5    },
            { L_,   L"{:#02x}",     L"-0x5",         -5    },
            { L_,   L"{:#03x}",     L"-0x5",         -5    },
            { L_,   L"{:#04x}",     L"-0x5",         -5    },
            { L_,   L"{:#05X}",     L"-0X05",        -5    },
            { L_,   L"{:#b}",       L"-0b101",       -5    },
            { L_,   L"{:#05b}",     L"-0b101",       -5    },
            { L_,   L"{:#07b}",     L"-0b0101",      -5    },
            { L_,   L"{:#08B}",     L"-0B01100",     -12   },
            { L_,   L"{:#d}",       L"-5",           -5    },
            { L_,   L"{:#d}",       L"-12",          -12   },
            { L_,   L"{:#o}",       L"-05",          -5    },
            { L_,   L"{:#o}",       L"-014",         -12   },
            { L_,   L"{:b}",        L"-1100",        -12   },
            { L_,   L"{:B}",        L"-1100",        -12   },
            { L_,   L"{:d}",        L"-12",          -12   },
            { L_,   L"{:o}",        L"-14",          -12   },
            { L_,   L"{:x}",        L"-c",           -12   },
            { L_,   L"{:X}",        L"-C",           -12   },
        };

        size_t NUM_NEGATIVE_WCHAR_DATA = sizeof NEGATIVE_WCHAR_DATA /
                                         sizeof *NEGATIVE_WCHAR_DATA;

        for (size_t i = 0; i < NUM_POSITIVE_CHAR_DATA; ++i) {
            const int           LINE     = POSITIVE_CHAR_DATA[i].d_line;
            const char         *FORMAT   = POSITIVE_CHAR_DATA[i].d_format_p;
            const char         *EXPECTED = POSITIVE_CHAR_DATA[i].d_expected_p;
            const int           VALUE    = POSITIVE_CHAR_DATA[i].d_value;
            const long          L_VALUE  = static_cast<long>(VALUE);
            const long long     LL_VALUE = static_cast<long long>(VALUE);
            const long          U_VALUE  = static_cast<unsigned>(VALUE);
            const unsigned long UL_VALUE = static_cast<unsigned long>(VALUE);

            const unsigned long long ULL_VALUE =
                                        static_cast<unsigned long long>(VALUE);
            const signed char        SC_VALUE =
                                               static_cast<signed char>(VALUE);
            const unsigned char      UC_VALUE =
                                             static_cast<unsigned char>(VALUE);

            testRuntimeCharParse<                   int>(LINE, FORMAT);
            testRuntimeCharParse<          unsigned int>(LINE, FORMAT);
            testRuntimeCharParse<              long int>(LINE, FORMAT);
            testRuntimeCharParse<     unsigned long int>(LINE, FORMAT);
            testRuntimeCharParse<         long long int>(LINE, FORMAT);
            testRuntimeCharParse<unsigned long long int>(LINE, FORMAT);
            testRuntimeCharParse<  signed          char>(LINE, FORMAT);
            testRuntimeCharParse<unsigned          char>(LINE, FORMAT);

            testRuntimeFormat(LINE, EXPECTED, FORMAT,     VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT,   U_VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT,   L_VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT,  UL_VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT,  LL_VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT, ULL_VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT,  SC_VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT,  UC_VALUE);
        }

        for (size_t i = 0; i < NUM_NEGATIVE_CHAR_DATA; ++i) {
            const int        LINE     = NEGATIVE_CHAR_DATA[i].d_line;
            const char      *FORMAT   = NEGATIVE_CHAR_DATA[i].d_format_p;
            const char      *EXPECTED = NEGATIVE_CHAR_DATA[i].d_expected_p;
            const int        VALUE    = NEGATIVE_CHAR_DATA[i].d_value;
            const long       L_VALUE  = static_cast<long>(VALUE);
            const long long  LL_VALUE = static_cast<long long>(VALUE);

            testRuntimeFormat(LINE, EXPECTED, FORMAT,     VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT,   L_VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT,  LL_VALUE);
        }

        if (verbose) printf("\tTesting wstrings.\n");

        for (size_t i = 0; i < NUM_POSITIVE_WCHAR_DATA; ++i) {
            const int            LINE     = POSITIVE_WCHAR_DATA[i].d_line;
            const wchar_t       *FORMAT   = POSITIVE_WCHAR_DATA[i].d_format_p;
            const wchar_t       *EXPECTED =
                                           POSITIVE_WCHAR_DATA[i].d_expected_p;
            const int            VALUE    = POSITIVE_WCHAR_DATA[i].d_value;
            const long           L_VALUE  = static_cast<long>(VALUE);
            const long long      LL_VALUE = static_cast<long long>(VALUE);
            const unsigned long  UL_VALUE  = static_cast<unsigned long>(VALUE);

            const unsigned long long ULL_VALUE  =
                                        static_cast<unsigned long long>(VALUE);
            const signed char   SC_VALUE = static_cast<signed char>(VALUE);
            const unsigned char UC_VALUE = static_cast<unsigned char>(VALUE);

            testRuntimeWcharParse<                   int>(LINE, FORMAT);
            testRuntimeWcharParse<          unsigned int>(LINE, FORMAT);
            testRuntimeWcharParse<              long int>(LINE, FORMAT);
            testRuntimeWcharParse<     unsigned long int>(LINE, FORMAT);
            testRuntimeWcharParse<         long long int>(LINE, FORMAT);
            testRuntimeWcharParse<unsigned long long int>(LINE, FORMAT);
            testRuntimeWcharParse<  signed          char>(LINE, FORMAT);
            testRuntimeWcharParse<unsigned          char>(LINE, FORMAT);

            testRuntimeFormat(LINE, EXPECTED, FORMAT,     VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT,   L_VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT,  UL_VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT,  LL_VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT, ULL_VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT,  SC_VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT,  UC_VALUE);
        }

        for (size_t i = 0; i < NUM_NEGATIVE_WCHAR_DATA; ++i) {
            const int        LINE     = NEGATIVE_WCHAR_DATA[i].d_line;
            const wchar_t   *FORMAT   = NEGATIVE_WCHAR_DATA[i].d_format_p;
            const wchar_t   *EXPECTED = NEGATIVE_WCHAR_DATA[i].d_expected_p;
            const int        VALUE    = NEGATIVE_WCHAR_DATA[i].d_value;
            const long       L_VALUE  = static_cast<long>(VALUE);
            const long long  LL_VALUE = static_cast<long long>(VALUE);

            testRuntimeFormat(LINE, EXPECTED, FORMAT,     VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT,   L_VALUE);
            testRuntimeFormat(LINE, EXPECTED, FORMAT,  LL_VALUE);
        }

        if (verbose) printf("\tTesting `width` option.\n");
        {
            bsl::string  message;
            int          value    = 5;
            int          width    = 6;

            // `char`

            const char  *format   = "{:{}}";
            const char  *expected = "     5";

            testWidthRuntimeFormat(L_, expected, format, width, value);

            format   = "{:{}d}";
            expected = "     5";

            testWidthRuntimeFormat(L_, expected, format, width, value);

            format   = "{:0{}d}";
            expected = "000005";

            testWidthRuntimeFormat(L_, expected, format, width, value);

            format   = "{:#{}d}";
            expected = "     5";

            testWidthRuntimeFormat(L_, expected, format, width, value);

            format   = "{:#<{}d}";
            expected = "5#####";

            testWidthRuntimeFormat(L_, expected, format, width, value);

            // `wchar_t`

            value = 6;
            width = 5;

            const wchar_t  *wFormat   = L"{:{}}";
            const wchar_t  *wExpected = L"    6";

            testWidthRuntimeFormat(L_, wExpected, wFormat, width, value);

            wFormat   = L"{:{}d}";
            wExpected = L"    6";

            testWidthRuntimeFormat(L_, wExpected, wFormat, width, value);

            wFormat   = L"{:0{}d}";
            wExpected = L"00006";

            testWidthRuntimeFormat(L_, wExpected, wFormat, width, value);

            wFormat   = L"{:#{}d}";
            wExpected = L"    6";

            testWidthRuntimeFormat(L_, wExpected, wFormat, width, value);

            wFormat   = L"{:#<{}d}";
            wExpected = L"6####";

            testWidthRuntimeFormat(L_, wExpected, wFormat, width, value);
        }

        if (verbose) printf("\tTesting compile-time processing.\n");
        {
            bsl::string message;

            // `parse`

            bool rv = TestUtilChar::testParseFormat<int>(&message,
                                                         true,
                                                         "{0:}");
            ASSERTV(message.c_str(), rv);

            rv = TestUtilWchar::testParseFormat<int>(&message, true, L"{0:}");
            ASSERTV(message.c_str(), rv);

            rv = TestUtilChar::testParseFormat<unsigned long long int>(
                                                                     &message,
                                                                     true,
                                                                     "{:*<6}");
            ASSERTV(message.c_str(), rv);

            rv = TestUtilWchar::testParseFormat<unsigned long long int>(
                                                                    &message,
                                                                    true,
                                                                    L"{:*<6}");
            ASSERTV(message.c_str(), rv);

            // `format`

            const int VALUE     = 5;
            rv                  = TestUtilChar::testEvaluateFormat(&message,
                                                                   "5",
                                                                   true,
                                                                   "{:}",
                                                                   VALUE);
            ASSERTV(message.c_str(), rv);

            rv = TestUtilWchar::testEvaluateFormat(&message,
                                                   L"5",
                                                   true,
                                                   L"{:}",
                                                   VALUE);
            ASSERTV(message.c_str(), rv);

            const unsigned long long int ULL_VALUE = 12;
            rv = TestUtilChar::testEvaluateFormat(&message,
                                                  "12",
                                                  true,
                                                  "{:}",
                                                  ULL_VALUE);
            ASSERTV(message.c_str(), rv);

            rv = TestUtilWchar::testEvaluateFormat(&message,
                                                   L"12",
                                                   true,
                                                   L"{:}",
                                                   ULL_VALUE);
            ASSERTV(message.c_str(), rv);
        }

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\tTesting locale prohibition.\n");
        {
            try {
                bsl::string message;
                bool        rv =
                       TestUtilChar::testParseVFormat<int>(
                           &message,
                           false,
                           "{:}");

                ASSERTV(message.c_str(), rv);

                rv = TestUtilChar::testParseVFormat<int>(&message,
                                                         false,
                                                         "{:L}");
                ASSERTV(message.c_str(), !rv);
            }
            catch(const bsl::format_error& err) {
                ASSERTV(err.what(),
                        "Exception should have been caught by the "
                        "`FormatterTestUtil`",
                        false);
            }
        }

        if (verbose)
            printf("\tTesting prohibition of the `precision` option.\n");
        {
            try {
                bsl::string message;
                bool        rv = TestUtilChar::testParseVFormat<int>(&message,
                                                                     false,
                                                                     "{:6d}");

                ASSERTV(message.c_str(), rv);

                rv = TestUtilChar::testParseVFormat<int>(&message,
                                                         false,
                                                         "{:6.5d}");
                ASSERTV(message.c_str(), !rv);
            }
            catch(const bsl::format_error& err) {
                ASSERTV(err.what(),
                        "Exception should have been caught by the "
                        "`FormatterTestUtil`",
                        false);
            }
        }
#endif

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
// Copyright 2023 Bloomberg Finance L.P.
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
