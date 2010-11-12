// bdet_timeinterval.t.cpp -*-C++-*-

#include <bdet_timeinterval.h>

#include <bdeimp_fuzzy.h>                // for testing only

#include <bdex_instreamfunctions.h>      // for testing only
#include <bdex_outstreamfunctions.h>     // for testing only
#include <bdex_testinstream.h>           // for testing only
#include <bdex_testoutstream.h>          // for testing only
#include <bdex_testinstreamexception.h>  // for testing only
#include <bdex_byteoutstream.h>          // for testing only
#include <bdex_byteinstream.h>           // for testing only

#include <bsls_platformutil.h>           // for testing only

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strcmp()
#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

// TBD test overloaded operators taking 'double' arguments

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// The component under test is a value semantic scalar whose state is a
// identically its value.  There is no allocator involved.  As such, our
// testing concerns are (safely) limited to the mechanical functioning of the
// various methods and free operators and exception neutrality during 'bdex'
// streaming, together with the range constraints and normalization
// mechanisms.
//
// The component interface represents a time interval as real value, rounded to
// the nearest nanosecond, stored as the normalized sum of a number of seconds
// (64 bit integer) and a number of nanoseconds (limited in the range
// '[ -999999999, 999999999 ]').
//
// The basic manipulators for 'bdet_TimeInterval' are the default constructor
// and 'setInterval(seconds, nanoseconds)'.  The primary accessors are the
// direct accessors 'seconds()' and 'nanoseconds()'.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] bdet_TimeInterval();
// [12] bdet_TimeInterval(bsls_PlatformUtil::Int64 seconds, int nanosecs);
// [12] bdet_TimeInterval(double seconds);
// [ 8] bdet_TimeInterval(const bdet_TimeInterval& original);
// [ 2] ~bdet_TimeInterval();
// MANIPULATORS
// [10] bdet_TimeInterval& operator=(const bdet_TimeInterval& rhs);
// [10] bdet_TimeInterval& operator=(double rhs);
// [16] bdet_TimeInterval& operator+=(const bdet_TimeInterval& rhs);
// [16] bdet_TimeInterval& operator+=(double rhs);
// [16] bdet_TimeInterval& operator-=(const bdet_TimeInterval& rhs);
// [16] bdet_TimeInterval& operator-=(double rhs);
// [15] bdet_TimeInterval& addSeconds(bsls_PlatformUtil::Int64 seconds);
// [15] bdet_TimeInterval& addMilliseconds(Int64 milliseconds);
// [15] bdet_TimeInterval& addMicroseconds(Int64 microseconds);
// [15] bdet_TimeInterval& addNanoseconds(Int64 nanoseconds);
// [15] void addInterval(Int64 seconds, int nanoseconds);
// [ 2] void setInterval(bsls_PlatformUtil::Int64 seconds, int nanoseconds);
// ACCESSORS
// [ 4] bsls_PlatformUtil::Int64 seconds() const;
// [ 4] int nanoseconds() const;
// [ 5] bsls_PlatformUtil::Int64 totalMilliseconds() const;
// [ 5] bsls_PlatformUtil::Int64 totalMicroseconds() const;
// [ 5] double totalSecondsAsDouble() const;
// [11] int maxSupportedBdexVersion() const;
// FREE OPERATORS
// [17] bdet_TimeInterval operator+(const Obj& lhs, const Obj& rhs);
// [17] bdet_TimeInterval operator+(const Obj& lhs, double rhs);
// [17] bdet_TimeInterval operator+(double lhs, const Obj& rhs);
// [17] bdet_TimeInterval operator-(const Obj& lhs, const Obj& rhs);
// [17] bdet_TimeInterval operator-(const Obj& lhs, double rhs);
// [17] bdet_TimeInterval operator-(double lhs, const Obj& rhs);
// [17] bdet_TimeInterval operator-(const bdet_TimeInterval& rhs);
// [ 7] operator==(const bdet_TimeInterval&, const bdet_TimeInterval&);
// [ 7] operator==(const bdet_TimeInterval&, double);
// [ 7] operator==(double, const bdet_TimeInterval&);
// [ 7] operator!=(const bdet_TimeInterval&, const bdet_TimeInterval&);
// [ 7] operator!=(const bdet_TimeInterval&, double);
// [ 7] operator!=(double, const bdet_TimeInterval&);
// [14] operator< (const bdet_TimeInterval&, const bdet_TimeInterval&);
// [14] operator< (const bdet_TimeInterval&, double);
// [14] operator< (double, const bdet_TimeInterval&);
// [14] operator<=(const bdet_TimeInterval&, const bdet_TimeInterval&);
// [14] operator<=(const bdet_TimeInterval&, double);
// [14] operator<=(double, const bdet_TimeInterval&);
// [14] operator> (const bdet_TimeInterval&, const bdet_TimeInterval&);
// [14] operator> (const bdet_TimeInterval&, double);
// [14] operator> (double, const bdet_TimeInterval&);
// [14] operator<=(const bdet_TimeInterval&, const bdet_TimeInterval&);
// [14] operator<=(const bdet_TimeInterval&, double);
// [14] operator>=(double, const bdet_TimeInterval&);
// [ 6] ostream& operator<<(ostream &output, const Obj& timeInterval);
// ----------------------------------------------------------------------------
// [ 1] Breathing Test
// [17] USAGE Example
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACROS
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t"                             \
                    << #J << ": " << J << "\t" << #K << ": " << K << "\t"  \
                    << #L << ": " << L << "\t" << #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }


//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << '\t' << flush;          // Print tab w/o newline
#define NL()  cout << endl;                   // Print newline
#define P64(X) printf(#X " = %lld\n", (X));   // Print 64-bit integer id & val
#define P64_(X) printf(#X " = %lld,  ", (X)); // Print 64-bit integer w/o '\n'

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdet_TimeInterval        Obj;
typedef bdex_TestInStream        In;
typedef bdex_TestOutStream       Out;
typedef bsls_PlatformUtil::Int64 Int64;

const int NANOSECS_PER_SEC = 1000000000;

//=============================================================================
//                      HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 18: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example 1"
                          << "\n=======================" << endl;
        {
            bdet_TimeInterval i1;
            ASSERT(  0 == i1.seconds() );
            ASSERT(  0 == i1.nanoseconds() );

            i1.setInterval(-5, -10);
            i1.addInterval(4, 10);
            ASSERT( -1 == i1.seconds() );
            ASSERT(  0 == i1.nanoseconds() );

            bdet_TimeInterval i2(i1);
            ASSERT( -1 == i2.seconds() );
            ASSERT(  0 == i2.nanoseconds() );

            ASSERT( i2 == i1 );

            i2.addSeconds(1).addMilliseconds(2);
            i2.addMicroseconds(3).addNanoseconds(4);

            ASSERT( 0 == i2.seconds() );
            ASSERT( 2003004 == i2.nanoseconds() );

            i2 = 3.5;
            i2 += 2.73;
            ASSERT( 6 == i2.seconds() );
            ASSERT( 230000000 == i2.nanoseconds() );

            if (verbose) cout << i2 << endl;
        }

        if (verbose) cout << "\nTesting Usage Example 2"
                          << "\n=======================" << endl;
        {
            struct my_SystemTime {
                static bdet_TimeInterval now() {
                    return bdet_TimeInterval(0, 0);
                }
            };

            {
                static const double TIME_LIMIT = 1.5;

                bdet_TimeInterval timeout;
                timeout = my_SystemTime::now() + TIME_LIMIT;
                if (verbose) cout << timeout << endl;
            }

            {
                static const double TIME_LIMIT = 3;

                bdet_TimeInterval timeout;
                timeout = my_SystemTime::now() + TIME_LIMIT;
                if (verbose) cout << timeout << endl;
            }
        }

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING ARITHMETIC FREE OPERATORS:
        //   The arithmetic free operators are very similar in implementation
        //   and concerns to the member arithmetic assignment operators.  The
        //   test will thus be very similar as well, differing only in the
        //   details of object manipulation, but not at all in concept.
        //
        // Plan:
        //   In a double loop, generate several lhs values x and several
        //   rhs values y.  Calculate the expected result z of x + y
        //   from the loop variables and confirm that z == x + y.  Similarly
        //   verify the result of x - y.
        //
        // Testing
        //   bdet_TimeInterval operator+(const Obj& lhs, const Obj& rhs);
        //   bdet_TimeInterval operator+(const Obj& lhs, double rhs);
        //   bdet_TimeInterval operator+(double lhs, const Obj& rhs);
        //   bdet_TimeInterval operator-(const Obj& lhs, const Obj& rhs);
        //   bdet_TimeInterval operator-(const Obj& lhs, double rhs);
        //   bdet_TimeInterval operator-(double lhs, const Obj& rhs);
        //   bdet_TimeInterval operator-(const bdet_TimeInterval& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Arithmetic Free Operators"
                          << "\n=================================" << endl;

        Obj x, y, z, exp;  const Obj &X = x, &Y = y, &Z = z, &EXP = exp;

        {
            if (verbose) cout << "\nTesting: '+' or '-'" << endl;

            struct {
                int d_lineNum;
                int d_secs;
                int d_nsecs;
            } VALUES[] = {
                //line   secs   nanosecs
                //----   ----   --------
                { L_,      0,          0 },
                { L_,      5,          1 },
                { L_,      2,  999999999 },
                { L_,     -3,         -1 },
                { L_,     -1, -999999999 }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < 1; i++) {
                const int LINE    = VALUES[i].d_lineNum;
                const int I_SECS  = VALUES[i].d_secs;
                const int I_NSECS = VALUES[i].d_nsecs;
                x.setInterval(I_SECS, I_NSECS);
                for (int j = 0; j < NUM_VALUES; j++) {
                    const int J_SECS  = VALUES[j].d_secs;
                    const int J_NSECS = VALUES[j].d_nsecs;
                    y.setInterval(J_SECS, J_NSECS);

                    if (veryVerbose) { T_();  P_(X);  P_(Y); }
                    z = x + y;
                    exp.setInterval(I_SECS + J_SECS, I_NSECS + J_NSECS);
                    if (veryVerbose) { P_(Z);  P_(EXP); }
                    LOOP2_ASSERT(i, j, Z == EXP);

                    z = x - y;
                    exp.setInterval(I_SECS - J_SECS, I_NSECS - J_NSECS);
                    if (veryVerbose) { P_(Z);  P(EXP); }
                    LOOP2_ASSERT(i, j, Z == EXP);
                }

                z = -x;
                exp.setInterval(-I_SECS, -I_NSECS);
                if (veryVerbose) { P_(Z);  P(EXP); }
                LOOP_ASSERT(LINE, Z == EXP);
            }

            // 'double' as left operand
            for (int i = 0; i < 1; i++) {
                const int LINE  = VALUES[i].d_lineNum;
                const int I_SECS  = VALUES[i].d_secs;
                const int I_NSECS = VALUES[i].d_nsecs;
                double x = I_SECS + (double)I_NSECS / NANOSECS_PER_SEC;
                for (int j = 0; j < NUM_VALUES; j++) {
                    const int J_SECS  = VALUES[j].d_secs;
                    const int J_NSECS = VALUES[j].d_nsecs;
                    y.setInterval(J_SECS, J_NSECS);

                    if (veryVerbose) { T_();  P_(x);  P_(Y); }
                    z = x + y;
                    exp.setInterval(I_SECS + J_SECS, I_NSECS + J_NSECS);
                    if (veryVerbose) { P_(Z);  P_(EXP); }
                    LOOP2_ASSERT(i, j, Z == EXP);

                    z = x - y;
                    exp.setInterval(I_SECS - J_SECS, I_NSECS - J_NSECS);
                    if (veryVerbose) { P_(Z);  P(EXP); }
                    LOOP2_ASSERT(i, j, Z == EXP);
                }

                z = -x;
                exp.setInterval(-I_SECS, -I_NSECS);
                if (veryVerbose) { P_(Z);  P(EXP); }
                LOOP_ASSERT(LINE, Z == EXP);
            }

            // 'double' as right operand
            for (int i = 0; i < 1; i++) {
                const int LINE  = VALUES[i].d_lineNum;
                const int I_SECS  = VALUES[i].d_secs;
                const int I_NSECS = VALUES[i].d_nsecs;
                x.setInterval(I_SECS, I_NSECS);
                for (int j = 0; j < NUM_VALUES; j++) {
                    const int J_SECS  = VALUES[j].d_secs;
                    const int J_NSECS = VALUES[j].d_nsecs;
                    double y = J_SECS + (double)J_NSECS / NANOSECS_PER_SEC;

                    if (veryVerbose) { T_();  P_(X);  P_(y); }
                    z = x + y;
                    exp.setInterval(I_SECS + J_SECS, I_NSECS + J_NSECS);
                    if (veryVerbose) { P_(Z);  P_(EXP); }
                    LOOP2_ASSERT(i, j, Z == EXP);

                    z = x - y;
                    exp.setInterval(I_SECS - J_SECS, I_NSECS - J_NSECS);
                    if (veryVerbose) { P_(Z);  P(EXP); }
                    LOOP2_ASSERT(i, j, Z == EXP);
                }

                z = -x;
                exp.setInterval(-I_SECS, -I_NSECS);
                if (veryVerbose) { P_(Z);  P(EXP); }
                LOOP_ASSERT(LINE, Z == EXP);
            }
        }

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING ARITHMETIC ASSIGNMENT METHODS:
        //   The arithmetic assignment operators modify the data members for
        //   seconds and nanoseconds.  We have to verify that the resulted
        //   values are expected and the internal representation is
        //   canonical.
        //
        // Plan:
        //   In a double loop, generate several initial values x and several
        //   rhs operand values y.  Calculate the expected result z of x += y
        //   from the loop variables and confirm that z == x.  Then verify that
        //   the result of x -= y is the original value of x.
        //
        // Testing
        //   bdet_TimeInterval& operator+=(const bdet_TimeInterval& rhs);
        //   bdet_TimeInterval& operator+=(double rhs);
        //   bdet_TimeInterval& operator-=(const bdet_TimeInterval& rhs);
        //   bdet_TimeInterval& operator-=(double rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Arithmetic Assignment Operators"
                          << "\n======================================="
                          << endl;

        Obj x, y, z;  const Obj &X = x, &Y = y, &Z = z;

        {
            if (verbose)
                cout << "\nTesting: 'operator+=' and 'operator-='" << endl;

            struct {
                int d_lineNum;
                int d_secs;
                int d_nsecs;
            } VALUES[] = {
                //line   secs   nanosecs
                //----   ----   --------
                { L_,      0,          0 },
                { L_,      5,          1 },
                { L_,      2,  999999999 },
                { L_,     -3,         -1 },
                { L_,     -1, -999999999 }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; i++) {
                const int I_SECS  = VALUES[i].d_secs;
                const int I_NSECS = VALUES[i].d_nsecs;
                x.setInterval(I_SECS, I_NSECS);
                for (int j = 0; j < NUM_VALUES; j++) {
                    const int J_SECS  = VALUES[j].d_secs;
                    const int J_NSECS = VALUES[j].d_nsecs;
                    y.setInterval(J_SECS, J_NSECS);

                    if (veryVerbose) { T_();  P_(X);  P_(Y); }
                    x += y;
                    z.setInterval(I_SECS + J_SECS, I_NSECS + J_NSECS);
                    if (veryVerbose) { P_(X);  P_(Z); }
                    LOOP2_ASSERT(i, j, Z == X);

                    x -= y;
                    z.setInterval(I_SECS, I_NSECS);
                    if (veryVerbose) { P_(X);  P(Z); }
                    LOOP2_ASSERT(i, j, Z == X);
                }
            }

            for (int i = 0; i < NUM_VALUES; i++) {
                const int I_SECS  = VALUES[i].d_secs;
                const int I_NSECS = VALUES[i].d_nsecs;
                x.setInterval(I_SECS, I_NSECS);
                for (int j = 0; j < NUM_VALUES; j++) {
                    const int J_SECS  = VALUES[j].d_secs;
                    const int J_NSECS = VALUES[j].d_nsecs;
                    double y = J_SECS + (double)J_NSECS / NANOSECS_PER_SEC;

                    if (veryVerbose) { T_();  P_(X);  P_(y); }
                    x += y;
                    z.setInterval(I_SECS + J_SECS, I_NSECS + J_NSECS);
                    if (veryVerbose) { P_(X);  P_(Z); }
                    LOOP2_ASSERT(i, j, Z == X);

                    x -= y;
                    z.setInterval(I_SECS, I_NSECS);
                    if (veryVerbose) { P_(X);  P(Z); }
                    LOOP2_ASSERT(i, j, Z == X);
                }
            }
        }

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'add' METHODS:
        //   Each 'add' method adds a time interval represented in various
        //   formats to the object's value.  We have to verify that the
        //   resulted values are expected and that the internal representation
        //   of the object value is canonical.
        //
        // Plan:
        //   First test 'addInterval' explicitly and then use the tested
        //   'addInterval' as an oracle to test the other four methods in
        //   a loop-based test.
        //
        //   Select an arbitrary non-zero initial time-interval value X, and
        //   tabulate a set of input (operand) values and the corresponding
        //   expected object values.  Verify that 'addInterval' produces the
        //   expected result.  Then, in a twice nested loop, test 'addInterval'
        //   over a wide dynamic range, but on an initial object value of 0
        //   (so that the tested 'setInterval' may be used to confirm success).
        //
        //   In a loop, generate arguments for each of the one-argument 'add'
        //   methods and use the (tested) 'addInterval' method as an oracle.
        //
        // Testing:
        //   bdet_TimeInterval& addSeconds(bsls_PlatformUtil::Int64 seconds);
        //   bdet_TimeInterval& addMilliseconds(Int64 milliseconds);
        //   bdet_TimeInterval& addMicroseconds(Int64 microseconds);
        //   bdet_TimeInterval& addNanoseconds(Int64 nanoseconds);
        //   bdet_TimeInterval& addInterval(Int64 seconds, int nanoseconds);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'add' Methods"
                          << "\n=====================" << endl;

        const int REF_SEC  = 1;
        const int REF_NSEC = 1;

        const Obj REF(REF_SEC, REF_NSEC);
        const Obj REF2(REF_SEC, 1000000);

        {
            static const struct {
                int d_lineNum;       // source line number
                int d_seconds;       // seconds to add
                int d_nsecs;         // milliseconds to add
                int d_expSeconds;    // expected seconds value
                int d_expNsecs;      // expected milliseconds value
            } DATA[] = {
                //         - time added -     - expected values -
                //line #    s          ns        s          ns
                //------   --          --       --          --
                { L_,       0,          0,       1,          1 },
                { L_,       0,  999999999,       2,          0 },
                { L_,       0,-1000000001,       0,          0 },
                { L_,       1,         -2,       1,  999999999 },
                { L_,      -2,          0,       0, -999999999 }
            };

            if (verbose) cout << "\nTesting: 'addInterval'" << endl;
            if (verbose) cout << "\n\twith non-zero initial value" << endl;
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE  = DATA[i].d_lineNum;
                const int SECS  = DATA[i].d_seconds;
                const int NSECS = DATA[i].d_nsecs;

                const int EXP_SECS  = DATA[i].d_expSeconds;
                const int EXP_NSECS = DATA[i].d_expNsecs;

                Obj x(REF);  const Obj& X = x;
                if (veryVerbose) { T_();  P_(X); }
                x.addInterval(SECS, NSECS);
                const Obj Y(EXP_SECS, EXP_NSECS);
                if (veryVerbose) { P_(X);  P(Y); }
                LOOP_ASSERT(LINE, Y == X);
            }
        }

        {
            const int STOP  = 1000 * 1000 * 1000;
            const int START = -STOP;
            const int STEP  = STOP;

            if (verbose) cout << "\n\twith 0 initial value, in a loop" << endl;
            for (int j = START; j <= STOP ; j += STEP)
            for (int i = START; i <= STOP ; i += STEP) {
                Obj x;  const Obj &X = x;
                if (veryVerbose) { T_();  P_(X); }

                x.addInterval(i, j);
                const Obj Y(i, j);
                if (veryVerbose) { P_(X);  P_(Y);  P(i); }

                LOOP2_ASSERT(i, j, Y == X);
            }
        }

        {
            int STOP  = 4;
            int START = -STOP;
            int STEP  = STOP / 4;
            int arg;

            if (verbose) cout << "\nTesting: 'addSeconds'" << endl;
            for (arg = START; arg <= STOP ; arg += STEP) {
                Obj x(REF), y(REF);  const Obj &X = x, &Y = y;
                if (veryVerbose) { T_();  P_(X); }

                x.addSeconds(arg);
                y.addInterval(arg, 0);
                if (veryVerbose) { P_(X);  P_(Y);  P(arg); }

                LOOP_ASSERT(arg, Y == X);
            }
            for (arg = START; arg <= STOP ; arg += STEP) {
                Obj x(-REF), y(-REF);  const Obj &X = x, &Y = y;
                if (veryVerbose) { T_();  P_(X); }

                x.addSeconds(arg);
                y.addInterval(arg, 0);
                if (veryVerbose) { P_(X);  P_(Y);  P(arg); }

                LOOP_ASSERT(arg, Y == X);
            }

            if (verbose) cout << "\nTesting: 'addMilliseconds'" << endl;
            for (arg = START; arg <= STOP ; arg += STEP) {
                Obj x(REF), y(REF);  const Obj &X = x, &Y = y;
                if (veryVerbose) { T_();  P_(X); }

                x.addMilliseconds(arg * 1000);
                y.addInterval(arg, 0);
                if (veryVerbose) { P_(X);  P_(Y);  P(arg); }

                LOOP_ASSERT(arg, Y == X);
            }
            {
                Obj x(REF), y(REF);  const Obj &X = x, &Y = y;
                if (veryVerbose) { P_(X); }

                x.addMilliseconds(-1);
                y.addInterval(0, -1000000);
                if (veryVerbose) { P_(X);  P_(Y); }

                LOOP_ASSERT(arg, Y == X);
            }
            {
                Obj x(REF2), y(REF2);  const Obj &X = x, &Y = y;
                if (veryVerbose) { P_(X); }

                x.addMilliseconds(999);
                y.addInterval(0, 999000000);
                if (veryVerbose) { P_(X);  P_(Y); }

                LOOP_ASSERT(arg, Y == X);
            }

            if (verbose) cout << "\nTesting: 'addMicroseconds'" << endl;
            for (arg = START; arg <= STOP ; arg += STEP) {
                Obj x(REF), y(REF);  const Obj &X = x, &Y = y;
                if (veryVerbose) { T_();  P_(X); }

                x.addMicroseconds(arg * 1000000);
                y.addInterval(arg, 0);
                if (veryVerbose) { P_(X);  P_(Y);  P(arg); }

                LOOP_ASSERT(arg, Y == X);
            }
            {
                Obj x(REF), y(REF);  const Obj &X = x, &Y = y;
                if (veryVerbose) { P_(X); }

                x.addMicroseconds(-1000);
                y.addInterval(0, -1000000);
                if (veryVerbose) { P_(X);  P_(Y); }

                LOOP_ASSERT(arg, Y == X);
            }
            {
                Obj x(REF2), y(REF2);  const Obj &X = x, &Y = y;
                if (veryVerbose) { P_(X); }

                x.addMicroseconds(999000);
                y.addInterval(0, 999000000);
                if (veryVerbose) { P_(X);  P_(Y); }

                LOOP_ASSERT(arg, Y == X);
            }

            if (verbose) cout << "\nTesting: 'addNanoseconds'" << endl;
            for (arg = START; arg <= STOP ; arg += STEP) {
                Obj x(REF), y(REF);  const Obj &X = x, &Y = y;
                if (veryVerbose) { T_();  P_(X); }

                x.addNanoseconds(bsls_PlatformUtil::Int64(arg)
                                                           * NANOSECS_PER_SEC);
                y.addInterval(arg, 0);
                if (veryVerbose) { P_(X);  P_(Y);  P(arg) }

                LOOP_ASSERT(arg, Y == X);
            }
            {
                Obj x(REF), y(REF);  const Obj &X = x, &Y = y;
                if (veryVerbose) { P_(X); }

                x.addNanoseconds(-1000000);
                y.addInterval(0, -1000000);
                if (veryVerbose) { P_(X);  P_(Y); }

                LOOP_ASSERT(arg, Y == X);
            }
            {
                Obj x(REF2), y(REF2);  const Obj &X = x, &Y = y;
                if (veryVerbose) { P_(X); }

                x.addNanoseconds(999000000);
                y.addInterval(0, 999000000);
                if (veryVerbose) { P_(X);  P_(Y); }

                LOOP_ASSERT(arg, Y == X);
            }
        }

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING RELATIONAL OPERATORS (<, <=, >=, >):
        //   Each operator implements a logical expression on the contained
        //   'bdet_TimeInterval' values, which must be verified.
        //
        // Plan:
        //   Specify an ordered set S of unique object values.  For each (u, v)
        //   in the set S x S, verify the result of u OP v for each OP in
        //   {<, <=, >=, >}.
        //
        // Testing
        //   operator< (const bdet_TimeInterval&, const bdet_TimeInterval&);
        //   operator< (const bdet_TimeInterval&, double);
        //   operator< (double, const bdet_TimeInterval&);
        //   operator<=(const bdet_TimeInterval&, const bdet_TimeInterval&);
        //   operator<=(const bdet_TimeInterval&, double);
        //   operator<=(double, const bdet_TimeInterval&);
        //   operator> (const bdet_TimeInterval&, const bdet_TimeInterval&);
        //   operator> (const bdet_TimeInterval&, double);
        //   operator> (double, const bdet_TimeInterval&);
        //   operator<=(const bdet_TimeInterval&, const bdet_TimeInterval&);
        //   operator<=(const bdet_TimeInterval&, double);
        //   operator>=(double, const bdet_TimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Relational Operators"
                          << "\n============================" << endl;

        if (verbose) cout <<
            "\nTesting: 'operator<', 'operator<=', 'operator>=', 'operator>'"
                          << endl;
        {
            static const struct {
                int d_lineNum;
                int d_seconds;
                int d_nsecs;
            } VALUES[] = {
                //line secs nanosecs
                //---- ---- --------
                { L_,   -3,   -1 },
                { L_,   -2,   -1 },
                { L_,   -1,   -3 },
                { L_,   -1,   -2 },
                { L_,    0,    0 },
                { L_,    1,    2 },
                { L_,    1,    3 },
                { L_,    2,    1 },
                { L_,    3,    1 }
             };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int I_SECS  = VALUES[i].d_seconds;
                const int I_NSECS = VALUES[i].d_nsecs;
                Obj v;  const Obj& V = v;
                v.setInterval(I_SECS, I_NSECS);
                double dv = I_SECS + (double)I_NSECS / NANOSECS_PER_SEC;

                for (int j = 0; j < NUM_VALUES; ++j) {
                    const int J_SECS  = VALUES[j].d_seconds;
                    const int J_NSECS = VALUES[j].d_nsecs;
                    Obj u;  const Obj& U = u;
                    u.setInterval(J_SECS, J_NSECS);
                    double du = J_SECS + (double)J_NSECS / NANOSECS_PER_SEC;
                    if (veryVerbose) { T_();  P_(i);  P_(j);  P_(V);  P(U); }
                    LOOP2_ASSERT(i, j, j <  i == U  <  V);
                    LOOP2_ASSERT(i, j, j <  i == du <  V);
                    LOOP2_ASSERT(i, j, j <  i == U  <  dv);

                    LOOP2_ASSERT(i, j, j <= i == U  <= V);
                    LOOP2_ASSERT(i, j, j <= i == du <= V);
                    LOOP2_ASSERT(i, j, j <= i == U  <= dv);

                    LOOP2_ASSERT(i, j, j >= i == U  >= V);
                    LOOP2_ASSERT(i, j, j >= i == du >= V);
                    LOOP2_ASSERT(i, j, j >= i == U  >= dv);

                    LOOP2_ASSERT(i, j, j >  i == U  >  V);
                    LOOP2_ASSERT(i, j, j >  i == du >  V);
                    LOOP2_ASSERT(i, j, j >  i == U  >  dv);
                }
            }
        }

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING ADDITIONAL SETTING MANIPULATORS:
        // --------------------------------------------------------------------
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING INITIALIZATION CONSTRUCTOR:
        //   The constructors must normalize inputs to obtain the internal
        //   canonical representation of a time interval.  That is, the
        //   seconds field range is [-2^63, 2^63 - 1] and the nanoseconds
        //   field range is [-999999999, 999999999].  The seconds and
        //   nanoseconds fields may of course differ in sign.
        //
        //   For the constructor that takes a double as input, the input
        //   double (representing a time interval in seconds) will first be
        //   rounded to the closest whole number of nanoseconds before been
        //   normalized.
        //
        // Plan:
        //   Specify a set S of time intervals as (s, ns) pairs having "widely"
        //   varying values.  For each (s, ns) in S, construct a time interval
        //   object X and verify that X has the expected value.  The values
        //   should exhibit normal behavior, boundary behavior, out-of-bound
        //   behavior, etc.
        //
        //   Specify another set T of time intervals as floating point numbers
        //   in seconds.  Construct a time interval object X for each number
        //   and compare against the expected values.
        //
        // Testing
        //   bdet_TimeInterval(bsls_PlatformUtil::Int64 secs, int nanosecs);
        //   bdet_TimeInterval(double seconds);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Initialization Constructors"
                          << "\n===================================" << endl;

        if (verbose)
            cout << "\nTesting: 'bdet_TimeInterval(secs, nanosecs)'" << endl;
        {
            static const struct {
                int d_lineNum;         // Source line number
                int d_secs;            // Input seconds
                int d_nsecs;           // Input nanoseconds
                int d_expSecs;         // Expected seconds
                int d_expNsecs;        // Expected nanoseconds
            } VALUES[] = {
                //line secs    nanosecs   expected secs   expected nanosecs
                //---- ----    --------   -------------   -----------------
                { L_,   0,         0,           0,                0 },
                { L_,   0,         0,           0,                0 },
                { L_,   1,    1000000000,       2,                0 },
                { L_,  -1,   -1000000000,      -2,                0 },
                { L_,   2,   -1000000001,       0,        999999999 },
                { L_,  -2,    1000000001,       0,       -999999999 }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int LINE  = VALUES[i].d_lineNum;
                const int SECS   = VALUES[i].d_secs;
                const int NSECS  = VALUES[i].d_nsecs;
                const int ESECS  = VALUES[i].d_expSecs;
                const int ENSECS = VALUES[i].d_expNsecs;

                {
                    Obj x(SECS, NSECS);  const Obj& X = x;
                    if (veryVerbose) {
                        T_(); P_(X); P_(SECS); P(NSECS);
                    }
                    LOOP_ASSERT(LINE, ESECS  == X.seconds());
                    LOOP_ASSERT(LINE, ENSECS == X.nanoseconds());
                }

            }
        }

        if (verbose)
            cout << "\nTesting: 'bdet_TimeInterval(double secs)'" << endl;
        {
            static const struct {
                int d_lineNum;
                double d_secs; int e_secs; int e_nsecs;
            } VALUES[] = {
                //line     double      secs     nanosecs
                //----     ------      ----     --------
                { L_,            0.0,   0,            0 },
                { L_,            1.0,   1,            0 },
                { L_,   1.9999999994,   1,    999999999 },
                { L_,  -1.9999999994,  -1,   -999999999 },
                { L_,   2.9999999995,   3,            0 },
                { L_,  -2.9999999995,  -3,            0 },
                { L_,   3.0000000004,   3,            0 },
                { L_,  -3.0000000004,  -3,            0 },
                { L_,   4.0000000005,   4,            1 },
                { L_,  -4.0000000005,  -4,           -1 }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int LINE  = VALUES[i].d_lineNum;
                const double DSECS = VALUES[i].d_secs;
                const int SECS  = VALUES[i].e_secs;
                const int NSECS = VALUES[i].e_nsecs;

                {
                    Obj x(DSECS);  const Obj& X = x;
                    if (veryVerbose) {
                        T_(); P_(X); P_(SECS); P(NSECS);
                    }
                    LOOP_ASSERT(LINE, SECS  == X.seconds());
                    LOOP_ASSERT(LINE, NSECS == X.nanoseconds());
                }
            }
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'bdex' STREAMING FUNCTIONALITY:
        //   The 'bdex' streaming concerns for this component are absolutely
        //   standard.  We first probe the member functions 'outStream' and
        //   'inStream' in the manner of a "breathing test" to verify basic
        //   functionality, then we thoroughly test that functionality using
        //   the available bdex stream functions, which forward appropriate
        //   calls to the member functions.  We next step through the sequence
        //   of possible stream states (valid, empty, invalid, incomplete, and
        //   corrupted), appropriately selecting data sets as described below.
        //   In all cases, exception neutrality is confirmed using the
        //   specially instrumented 'bdex_TestInStream' and a pair of standard
        //   macros, 'BEGIN_BDEX_EXCEPTION_TEST' and
        //   'END_BDEX_EXCEPTION_TEST', which configure the
        //   'bdex_TestInStream' object appropriately in a loop.
        //
        // Plan:
        //   PRELIMINARY MEMBER FUNCTION TEST
        //     First perform a trivial direct test of the 'outStream' and
        //     'inStream' methods (the rest of the testing will use the stream
        //     operators).
        //
        //   VALID STREAMS
        //     For the set S of globally-defined test values, use all
        //     combinations (u, v) in the cross product S X S, stream the
        //     value of v into (a temporary copy of) u and assert u == v.
        //
        //   EMPTY AND INVALID STREAMS
        //     For each u in S, create a copy and attempt to stream into it
        //     from an empty and then invalid stream.  Verify after each try
        //     that the object is unchanged and that the stream is invalid.
        //
        //   INCOMPLETE (BUT OTHERWISE VALID) DATA
        //     Write 3 distinct objects to an output stream buffer of total
        //     length N.  For each partial stream length from 0 to N - 1,
        //     construct an input stream and attempt to read into objects
        //     initialized with distinct values.  Verify values of objects
        //     that are either successfully modified or left entirely
        //     unmodified, and that the stream became invalid immediately after
        //     the first incomplete read.  Finally ensure that each object
        //     streamed into is in some valid state by assigning it a distinct
        //     new value and testing for equality.
        //
        //   CORRUPTED DATA
        //     Use the underlying stream package to simulate a typical valid
        //     (control) stream and verify that it can be streamed in
        //     successfully.  Then for each data field in the stream (beginning
        //     with the version number), provide one or more similar tests with
        //     that data field corrupted.  After each test, verify that the
        //     object is in some valid state after streaming, and that the
        //     input stream has gone invalid.
        //
        //   Finally, tests of the explicit wire format will be performed.
        //
        // Testing:
        //   int maxSupportedBdexVersion() const;
        //   BDEX STREAMING
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Streaming Functionality"
                          << "\n===============================" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // scalar and array object values for various stream tests

        const int SA = 1,  NSA = 2;   // VA
        const int SB = 3,  NSB = 4;   // VB
        const int SC = 5,  NSC = 6;   // VC
        const int SD = 7,  NSD = 8;   // VD
        const int SE = 9,  NSE = 10;  // VE
        const int SF = 11, NSF = 12;  // VF

        const Obj VA(SA, NSA);
        const Obj VB(SB, NSB);
        const Obj VC(SC, NSC);
        const Obj VD(SD, NSD);
        const Obj VE(SE, NSE);
        const Obj VF(SF, NSF);

        const int NUM_VALUES = 6;
        const Obj VALUES[NUM_VALUES] = { VA, VB, VC, VD, VE, VF };
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout <<
            "\nTesting 'streamOut' and (valid) 'streamIn' functionality."
                          << endl;

        const int VERSION = Obj::maxSupportedBdexVersion();
        if (verbose) cout << "\nDirect initial trial of 'streamOut' and"
                             " (valid) 'streamIn' functionality." << endl;
        {
            const Obj X(VC);
            Out out;
            bdex_OutStreamFunctions::streamOut(out, X, VERSION);

            const char *const OD  = out.data();
            const int         LOD = out.length();
            In in(OD, LOD);
            in.setSuppressVersionCheck(1);
            ASSERT(in);                                 ASSERT(!in.isEmpty());

            Obj t(VA);  const Obj& T = t;               ASSERT(X != T);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(X == T);
            ASSERT(in);                                 ASSERT(in.isEmpty());
        }

        if (verbose) cout <<
            "\nThorough test of stream operators ('<<' and '>>')." << endl;
        {
            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj X(VALUES[i]);
                Out out;
                bdex_OutStreamFunctions::streamOut(out, X, VERSION);
                const char *const OD  = out.data();
                const int         LOD = out.length();

                // Verify that each new value overwrites every old value
                // and that the input stream is emptied, but remains valid.

                for (int j = 0; j < NUM_VALUES; ++j) {
                    In in(OD, LOD);  In &testInStream = in;
                    in.setSuppressVersionCheck(1);
                    LOOP2_ASSERT(i, j, in);  LOOP2_ASSERT(i, j, !in.isEmpty());

                    Obj t(VALUES[j]);
                    BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                      LOOP2_ASSERT(i, j, X == t == (i == j));
                      bdex_InStreamFunctions::streamIn(in, t, VERSION);
                    } END_BDEX_EXCEPTION_TEST
                    LOOP2_ASSERT(i, j, X == t);
                    LOOP2_ASSERT(i, j, in);  LOOP2_ASSERT(i, j, in.isEmpty());
                }
            }
        }

        if (verbose) cout <<
            "\nTesting streamIn functionality via operator ('>>')." << endl;

        if (verbose) cout << "\tOn empty and invalid streams." << endl;
        {
            Out out;
            const char *const  OD = out.data();
            const int         LOD = out.length();
            ASSERT(0 == LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
                In in(OD, LOD);  In &testInStream = in;
                in.setSuppressVersionCheck(1);
                LOOP_ASSERT(i, in);           LOOP_ASSERT(i, in.isEmpty());

                // Ensure that reading from an empty or invalid input stream
                // leaves the stream invalid and the target object unchanged.

                const Obj X(VALUES[i]);  Obj t(X);  LOOP_ASSERT(i, X == t);
                BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                  bdex_InStreamFunctions::streamIn(in, t, VERSION);
                  LOOP_ASSERT(i, !in);     LOOP_ASSERT(i, X == t);
                  bdex_InStreamFunctions::streamIn(in, t, VERSION);
                  LOOP_ASSERT(i, !in);     LOOP_ASSERT(i, X == t);
                } END_BDEX_EXCEPTION_TEST
            }
        }

        if (verbose) cout <<
            "\tOn incomplete (but otherwise valid) data." << endl;
        {
            const Obj W1 = VA, X1 = VB, Y1 = VC;
            const Obj W2 = VB, X2 = VC, Y2 = VD;
            const Obj W3 = VC, X3 = VD, Y3 = VE;

            Out out;
            bdex_OutStreamFunctions::streamOut(out, X1, VERSION);
            const int LOD1 = out.length();
            bdex_OutStreamFunctions::streamOut(out, X2, VERSION);
            const int LOD2 = out.length();
            bdex_OutStreamFunctions::streamOut(out, X3, VERSION);
            const int LOD  = out.length();
            const char *const OD = out.data();

            for (int i = 0; i < LOD; ++i) {
              In in(OD, i);  In &testInStream = in;
              in.setSuppressVersionCheck(1);
              BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                LOOP_ASSERT(i, in); LOOP_ASSERT(i, !i == in.isEmpty());
                Obj t1(W1), t2(W2), t3(W3);

                if (i < LOD1) {
                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i, !in);
                                         if (0 == i) LOOP_ASSERT(i, W1 == t1);
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W2 == t2);
                    bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W3 == t3);
                }
                else if (i < LOD2) {
                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X1 == t1);
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    LOOP_ASSERT(i, !in);
                                      if (LOD1 == i) LOOP_ASSERT(i, W2 == t2);
                    bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W3 == t3);
                }
                else {
                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X1 == t1);
                    bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                    LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X2 == t2);
                    bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                    LOOP_ASSERT(i, !in);
                                      if (LOD2 == i) LOOP_ASSERT(i, W3 == t3);
                }

                                LOOP_ASSERT(i, Y1 != t1);
                t1 = Y1;        LOOP_ASSERT(i, Y1 == t1);

                                LOOP_ASSERT(i, Y2 != t2);
                t2 = Y2;        LOOP_ASSERT(i, Y2 == t2);

                                LOOP_ASSERT(i, Y3 != t3);
                t3 = Y3;        LOOP_ASSERT(i, Y3 == t3);
              } END_BDEX_EXCEPTION_TEST
            }
        }

        if (verbose) cout << "\tOn corrupted data." << endl;

        const Obj W;               // default value (0)
        const Obj X(VA);           // control value (VA)
        const Obj Y(VB);           // new value (VB)

        if (verbose) cout << "\t\tGood stream (for control)." << endl;
        {
            Out out;
            out.putInt64((bsls_PlatformUtil::Int64)SB);
            out.putInt32(NSB);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);  ASSERT(in);
                             ASSERT(W != t);  ASSERT(X != t);  ASSERT(Y == t);
        }

        if (verbose) cout << "\t\tBad version." << endl;
        if (verbose) cout << "\t\t\tVersion too small." <<endl;
        {
            const char version = 0; // too small ('version' must be >= 1)

            Out out;
            out.putInt64((bsls_PlatformUtil::Int64)SB);
            out.putInt32(NSB);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, version);  ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }

        if (verbose) cout << "\t\t\tVersion too big." << endl;
        {
            const char version = 5; // too large (current versions all = 1)

            Out out;
            out.putInt64((bsls_PlatformUtil::Int64)SB);
            out.putInt32(NSB);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, version);  ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }

        if (verbose) cout << "\t\tBad value." << endl;
        if (verbose) cout << "\t\t\tseconds pos., nanoseconds neg." <<endl;
        {
            Out out;
            out.putInt64((bsls_PlatformUtil::Int64)1);
            out.putInt32(-1);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);  ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        if (verbose) cout << "\t\t\tseconds neg., nanoseconds pos." <<endl;
        {
            Out out;
            out.putInt64((bsls_PlatformUtil::Int64)-1);
            out.putInt32(1);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);  ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }

        if (verbose) cout << "\nTesting 'maxSupportedBdexVersion()'." << endl;
        {
            const Obj X;
            ASSERT(1 == X.maxSupportedBdexVersion());
        }

        if (verbose) cout << "\nWire format direct tests." << endl;
        {
            static const struct {
                int         d_lineNum;     // source line number
                int         d_second;      // specification second
                int         d_nanosecond;  // specification nanosecond
                int         d_version;     // version to stream with
                int         d_length;      // expect output length
                const char *d_fmt_p;       // expected output format
            } DATA[] = {
                //line  sec  nano  ver  len  format
                //----  ---  ----  ---  ---  -------------------------------
                { L_,   14,   10,   0,   0,  ""                              },
                { L_,   20,    8,   0,   0,  ""                              },

                { L_,   14,   10,   1,  12,
                          "\x00\x00\x00\x00\x00\x00\x00\x0e\x00\x00\x00\x0a" },
                { L_,   20,    8,   1,  12,
                          "\x00\x00\x00\x00\x00\x00\x00\x14\x00\x00\x00\x08" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE        = DATA[i].d_lineNum;
                const int SECOND      = DATA[i].d_second;
                const int NANOSECOND  = DATA[i].d_nanosecond;
                const int VERSION     = DATA[i].d_version;
                const int LEN         = DATA[i].d_length;
                const char *const FMT = DATA[i].d_fmt_p;

                Obj mX(SECOND, NANOSECOND);  const Obj& X = mX;
                bdex_ByteOutStream out;  X.bdexStreamOut(out, VERSION);

                LOOP_ASSERT(LINE, LEN == out.length());
                LOOP_ASSERT(LINE, 0 == memcmp(out.data(),
                                              FMT,
                                              LEN));

                if (verbose && memcmp(out.data(), FMT, LEN)) {
                    const char *hex = "0123456789abcdef";
                    P_(LINE);
                    for (int j = 0; j < out.length(); ++j) {
                        cout << "\\x"
                             << hex[(unsigned char)*
                                          (out.data() + j) >> 4]
                             << hex[(unsigned char)*
                                       (out.data() + j) & 0x0f];
                    }
                    cout << endl;
                }

                Obj mY;  const Obj& Y = mY;
                if (LEN) { // version is supported
                    bdex_ByteInStream in(out.data(),
                                         out.length());
                    mY.bdexStreamIn(in, VERSION);
                }
                else { // version is not supported
                    mY = X;
                    bdex_ByteInStream in;
                    mY.bdexStreamIn(in, VERSION);
                    LOOP_ASSERT(LINE, !in);
                }
                LOOP_ASSERT(LINE, X == Y);
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
        //   Any value must be assignable to an object having any initial value
        //   without affecting the rhs operand value.  Also, any object must be
        //   assignable to itself.
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  Construct and initialize all combinations
        //   (u, v) in the cross product S x S, copy construct a control w from
        //   v, assign v to u, and assert that w == u and w == v.  Then test
        //   aliasing by copy constructing a control w from each u in S,
        //   assigning u to itself, and verifying that w == u.
        //
        // Testing:
        //   bdet_TimeInterval& operator=(const bdet_TimeInterval& rhs)
        //   bdet_TimeInterval& operator=(double rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Assignment Operator"
                          << "\n===========================" << endl;

        if (verbose) cout << "\nTesting Assignment u = V." << endl;
        {
            static const struct {
                int                      d_lineNum;
                bsls_PlatformUtil::Int64 d_secs;
                int                      d_nsecs;
            } VALUES[] = {
                //line     secs       nanosecs
                //----     ----       --------
                { L_,             0,          0 },
                { L_,             1,          1 },
                { L_,             1,  999999999 },
                { L_,            -1,         -1 },
                { L_,            -1, -999999999 },
                { L_,  3000000000LL,  999999999 },
                { L_, -3000000000LL, -999999999 }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
            int i;

            for (i = 0; i < NUM_VALUES; ++i) {
                Obj v;  const Obj& V = v;
                v.setInterval(VALUES[i].d_secs, VALUES[i].d_nsecs);
                for (int j = 0; j < NUM_VALUES; ++j) {
                    Obj u;  const Obj& U = u;
                    u.setInterval(VALUES[j].d_secs, VALUES[j].d_nsecs);
                    if (veryVerbose) { T_();  P_(V);  P_(U); }
                    Obj w(V);  const Obj &W = w;          // control
                    u = V;
                    if (veryVerbose) P(U);
                    LOOP2_ASSERT(i, j, W == U);
                    LOOP2_ASSERT(i, j, W == V);
                }
            }

            for (i = 0; i < NUM_VALUES; ++i) {
                double V = (double)VALUES[i].d_secs +
                                  (double)VALUES[i].d_nsecs / NANOSECS_PER_SEC;
                for (int j = 0; j < NUM_VALUES; ++j) {
                    Obj u;  const Obj& U = u;
                    u.setInterval(VALUES[j].d_secs, VALUES[j].d_nsecs);
                    if (veryVerbose) { T_();  P_(V);  P_(U); }
                    Obj w(V);  const Obj &W = w;          // control
                    u = V;
                    if (veryVerbose) P(U);
                    LOOP2_ASSERT(i, j, W == U);
                    LOOP2_ASSERT(i, j, W == V);
                }
            }

            if (verbose) cout << "\nTesting assignment u = u (Aliasing)."
                              << endl;

            for (i = 0; i < NUM_VALUES; ++i) {
                const int LINE  = VALUES[i].d_lineNum;
                Obj u;  const Obj& U = u;
                u.setInterval(VALUES[i].d_secs, VALUES[i].d_nsecs);
                Obj w(U);  const Obj &W = w;              // control
                u = u;
                if (veryVerbose) { T_();  P_(U);  P(W); }
                LOOP_ASSERT(LINE, W == U);
            }
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'g':
        //   Void for 'bdet_datetimeinterval'.
        // --------------------------------------------------------------------

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //   Any value must be able to be copy constructed without affecting
        //   its argument.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identically valued objects w and x using tested
        //   methods.  Then copy construct an object y from x, and use the
        //   equality operator to assert that both x and y have the same value
        //   as w.
        //
        // Testing:
        //   bdet_TimeInterval(const bdet_TimeInterval& original);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Copy Constructor"
                          << "\n========================" << endl;

        if (verbose) cout << "\nTesting copy constructor." << endl;
        {
            static const struct {
                int                      d_lineNum;
                bsls_PlatformUtil::Int64 d_secs;
                int                      d_nsecs;
            } VALUES[] = {
                //line     secs       nanosecs
                //----     ----       --------
                { L_,             0,          0 },
                { L_,             1,          1 },
                { L_,             1,  999999999 },
                { L_,            -1,         -1 },
                { L_,            -1, -999999999 },
                { L_,  3000000000LL,  999999999 },
                { L_, -3000000000LL, -999999999 }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int LINE  = VALUES[i].d_lineNum;

                Obj w;  const Obj& W = w;           // control
                w.setInterval(VALUES[i].d_secs, VALUES[i].d_nsecs);

                Obj x;  const Obj& X = x;
                x.setInterval(VALUES[i].d_secs, VALUES[i].d_nsecs);

                Obj y(X);  const Obj &Y = y;
                if (veryVerbose) { T_();  P_(W);  P_(X);  P(Y); }
                LOOP_ASSERT(LINE, X == W);  LOOP_ASSERT(LINE, Y == W);
            }
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS:
        //   Any subtle variation in value must be detected by the equality
        //   operators.  The test data have variations in parameters for both
        //   'seconds' and 'nanoseconds'.
        //
        // Plan:
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  Verify the correctness of 'operator==' and
        //   'operator!=' using all elements (u, v) of the cross product
        //    S X S.
        //
        // Testing:
        //   operator==(const bdet_TimeInterval&, const bdet_TimeInterval&);
        //   operator==(const bdet_TimeInterval&, double);
        //   operator==(double, const bdet_TimeInterval&);
        //   operator!=(const bdet_TimeInterval&, const bdet_TimeInterval&);
        //   operator!=(const bdet_TimeInterval&, double);
        //   operator!=(double, const bdet_TimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Equality Operators"
                          << "\n==========================" << endl;

        if (verbose) cout <<
            "\nCompare each pair of values (u, v) in S X S." << endl;

        static const struct {
            int                      d_lineNum;
            bsls_PlatformUtil::Int64 d_secs;
            int                      d_nsecs;
        } VALUES[] = {
            //line     secs       nanosecs
            //----     ----       --------
            { L_,             0,          0 },
            { L_,             1,          1 },
            { L_,             1,  999999999 },
            { L_,            -1,         -1 },
            { L_,            -1, -999999999 },
            { L_,  3000000000LL,  999999999 },
            { L_, -3000000000LL, -999999999 }
        };

        const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

        {
            for (int i = 0; i < NUM_VALUES; ++i) {
                const int I_SECS  = VALUES[i].d_secs;
                const int I_NSECS = VALUES[i].d_nsecs;
                Obj u;  const Obj& U = u;
                u.setInterval(I_SECS, I_NSECS);
                for (int j = 0; j < NUM_VALUES; ++j) {
                    const int J_SECS  = VALUES[j].d_secs;
                    const int J_NSECS = VALUES[j].d_nsecs;
                    Obj v;  const Obj& V = v;
                    v.setInterval(J_SECS, J_NSECS);
                    bool isSame = i == j;
                    if (veryVerbose) { P_(i); P_(j); P_(U); P(V); }
                    LOOP2_ASSERT(i, j,  isSame == (U  == V));
                    LOOP2_ASSERT(i, j, !isSame == (U  != V));
                    LOOP2_ASSERT(i, j,  isSame == (V  == U));
                    LOOP2_ASSERT(i, j, !isSame == (V  != U));
                }
            }
        }
        {
            for (int i = 0; i < NUM_VALUES; ++i) {
                const int I_SECS  = VALUES[i].d_secs;
                const int I_NSECS = VALUES[i].d_nsecs;
                double du = I_SECS + (double)I_NSECS / NANOSECS_PER_SEC;
                Obj u(du);  const Obj& U = u;
                for (int j = 0; j < NUM_VALUES; ++j) {
                    const int J_SECS  = VALUES[j].d_secs;
                    const int J_NSECS = VALUES[j].d_nsecs;
                    double dv = J_SECS + (double)J_NSECS / NANOSECS_PER_SEC;
                    Obj v(dv);  const Obj& V = v;
                    bool isSame = i == j;
                    if (veryVerbose) { P_(i); P_(j); P_(U); P_(du);
                                                     P_(V);  P(dv); }
                    LOOP2_ASSERT(i, j,  isSame == (du == V));
                    LOOP2_ASSERT(i, j,  isSame == (U  == dv));

                    LOOP2_ASSERT(i, j, !isSame == (du != V));
                    LOOP2_ASSERT(i, j, !isSame == (U  != dv));

                    LOOP2_ASSERT(i, j,  isSame == (dv == U));
                    LOOP2_ASSERT(i, j,  isSame == (V  == du));

                    LOOP2_ASSERT(i, j, !isSame == (dv != U));
                    LOOP2_ASSERT(i, j, !isSame == (V  != du));
                }
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR:
        //   The output operator is implemented using the now-fully-tested
        //   direct accessors.  A few test vectors can thoroughly test the
        //   actual output-streaming functionality.
        //
        // Plan:
        //   For each of a small representative set of object values, use
        //   'ostrstream' to write that object's value to a character buffer
        //   and then compare the contents of that buffer with the expected
        //   output format.
        //
        // Testing:
        //   ostream& operator<<(ostream &output, const Obj& timeInterval);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Output (<<) Operator"
                          << "\n============================" << endl;

        if (verbose) cout << "\nTesting 'operator<<' (ostream)." << endl;
        {
            static const struct {
                int                      d_lineNum;   // source line number
                bsls_PlatformUtil::Int64 d_seconds;  // second field value
                int                      d_nsecs;    // nanosecond field value
                const char               *d_fmt_p;   // expected output format
            } DATA[] = {
                //line     secs        nsecs        output format
                //---- ------------   ---------   ---------------------------
                { L_,             0,          0, "(0, 0)"                    },
                { L_,             0,        100, "(0, 100)"                  },
                { L_,             0,       -100, "(0, -100)"                 },
                { L_,  3000000000LL,  999999999, "(3000000000, 999999999)"   },
                { L_, -3000000000LL, -999999999, "(-3000000000, -999999999)" }
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            const int SIZE = 1000;        // big enough to hold output string
            const char XX = (char) 0xFF;  // represents unset 'char'
            char mCtrlBuf[SIZE];  memset(mCtrlBuf, XX, SIZE);
            const char *CTRL_BUF = mCtrlBuf; // Used for extra character check.

            for (int di = 0; di < NUM_DATA;  ++di) {
                const int                      LINE    = DATA[di].d_lineNum;
                const bsls_PlatformUtil::Int64 SECONDS = DATA[di].d_seconds;
                const int                      NSECS   = DATA[di].d_nsecs;
                const char *const              FMT     = DATA[di].d_fmt_p;

                char buf[SIZE];
                memcpy(buf, CTRL_BUF, SIZE); // Preset buf to 'unset' values.

                Obj x;  const Obj& X = x;
                x.setInterval(SECONDS, NSECS);

                if (veryVerbose) cout << "\tEXPECTED FORMAT: " << FMT << endl;
                ostrstream out(buf, SIZE);  out << X << ends;
                if (veryVerbose) cout << "\tACTUAL FORMAT:   " << buf << endl;

                const int SZ = strlen(FMT) + 1;
                LOOP_ASSERT(LINE, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(LINE, XX == buf[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE,  0 == memcmp(buf, FMT, SZ));
                LOOP_ASSERT(LINE,  0 ==
                                     memcmp(buf + SZ, CTRL_BUF + SZ, SIZE-SZ));
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING COMPOUND ACCESSORS:
        //   Each compound accessor returns the time interval interpreted in
        //   the method's specified units.
        //
        // Concerns:
        //   That the correct value is returned.
        //   That overflow does not occur for reasonable input values.
        //   That the loss of precision is as expected.
        //
        // Plan:
        //   For each of a sequence of unique object values, verify that each
        //   of the accessors returns the expected value.
        //
        // Testing:
        //   bsls_PlatformUtil::Int64 totalMilliseconds() const;
        //   bsls_PlatformUtil::Int64 totalMicroseconds() const;
        //   double totalSecondsAsDouble() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING COMPOUND ACCESSORS"
                             "\n==========================" << endl;

        if (verbose) cout << "\nTesting: totalMilliseconds()"
                             "\n============================" << endl;
        {
            static const struct {
                int                      d_lineNum;
                bsls_PlatformUtil::Int64 d_secs;
                int                      d_nsecs;
                bsls_PlatformUtil::Int64 d_expect;
            } VALUES[] = {
              //line                secs    nanosecs         result expected
              //----   -----------------   ---------   ---------------------
                {L_,                 0LL,          0,                    0LL},
                {L_,                 1LL,          1,                 1000LL},
                {L_,                -1LL,         -1,                -1000LL},
                {L_,                42LL,         42,                42000LL},
                {L_,             -1024LL,     -66660,             -1024000LL},
                {L_,           5678901LL,   55555555,           5678901055LL},
                {L_,       12345678901LL,    2727272,       12345678901002LL},
                {L_,  9223372036854775LL,  807999999,  9223372036854775807LL},
                                                        // == LLONG_MAX
                {L_, -9223372036854775LL, -808999999, -9223372036854775808LL},
                                                        // == LLONG_MIN
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls_PlatformUtil::Int64 SECS = VALUES[i].d_secs;
                const int NSECS = VALUES[i].d_nsecs;
                const int LINE  = VALUES[i].d_lineNum;

                Obj x;  const Obj& X = x;
                x.setInterval(SECS, NSECS);

                if (veryVerbose) {
                    T_(); P_(i);    P(X);
                    T_(); P_(SECS); P(NSECS);
                    T_(); P(X.totalMilliseconds());
                }

                LOOP_ASSERT(LINE, x.totalMilliseconds() == VALUES[i].d_expect);
            }
        }

        if (verbose) cout << "\nTesting: totalMicroseconds()"
                             "\n============================" << endl;
        {
            static const struct {
                int                      d_lineNum;
                bsls_PlatformUtil::Int64 d_secs;
                int                      d_nsecs;
                bsls_PlatformUtil::Int64 d_expect;
            } VALUES[] = {
              //line              secs    nanosecs        result expected
              //----   ---------------   ---------  ---------------------
                {L_,              0LL,          0,                    0LL},
                {L_,              1LL,          1,              1000000LL},
                {L_,             -1LL,         -1,             -1000000LL},
                {L_,             42LL,       4942,             42000004LL},
                {L_,          -1024LL,     -66660,          -1024000066LL},
                {L_,        5678901LL,   54321098,        5678901054321LL},
                {L_,    12345678901LL,    2727272,    12345678901002727LL},
                {L_,  9223372036854LL,  775807999,  9223372036854775807LL},
                                                        // == LLONG_MAX
                {L_, -9223372036854LL, -775808999, -9223372036854775808LL},
                                                        // == LLONG_MIN
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls_PlatformUtil::Int64 SECS = VALUES[i].d_secs;
                const int LINE  = VALUES[i].d_lineNum;
                const int NSECS = VALUES[i].d_nsecs;

                Obj x;  const Obj& X = x;
                x.setInterval(SECS, NSECS);

                if (veryVerbose) {
                    T_(); P_(i);    P(X);
                    T_(); P_(SECS); P(NSECS);
                    T_(); P(X.totalMicroseconds());
                }

                LOOP_ASSERT(LINE, x.totalMicroseconds() == VALUES[i].d_expect);
            }
        }

        if (verbose) cout << "\nTesting: totalSecondsAsDouble()"
                             "\n===============================" << endl;
        {
            static const struct {
                int                      d_lineNum;
                bsls_PlatformUtil::Int64 d_secs;
                int                      d_nsecs;
                double                   d_expect;
            } VALUES[] = {
              //line              secs    nanosecs            result expected
              //----   ---------------   ---------   ------------------------
                {L_,               0LL,          0,                       0.0},
                {L_,               1LL,          1,               1.000000001},
                {L_,              -1LL,         -1,              -1.000000001},
                {L_,              42LL,         42,              42.000000042},
                {L_,           -1024LL,     -66600,             -1024.0000666},
                {L_,         5678901LL,   55555555,         5678901.055555555},
                {L_,     12345678901LL,    2727272,     12345678901.002727272},
                {L_,  92372036854775LL,  999999999,  92372036854775.999999999},
                {L_, -92237203684775LL, -808999999, -92237203684775.808999999},
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls_PlatformUtil::Int64 SECS = VALUES[i].d_secs;
                const int LINE  = VALUES[i].d_lineNum;
                const int NSECS = VALUES[i].d_nsecs;

                Obj x;  const Obj& X = x;
                x.setInterval(SECS, NSECS);

                if (veryVerbose) {
                    T_(); P_(i);    P(X);
                    T_(); P_(SECS); P(NSECS);
                    T_(); P(X.totalSecondsAsDouble());
                }

                LOOP_ASSERT(LINE, bdeimp_Fuzzy::eq(x.totalSecondsAsDouble(),
                                                   VALUES[i].d_expect));
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING DIRECT ACCESSORS:
        //   Each accessor directly returns the internal representations of
        //   seconds and nanoseconds.
        //
        // Plan:
        //   For each of a sequence of unique object values, verify that each
        //   of the direct accessors returns the expected value.
        //
        // Testing:
        //   bsls_PlatformUtil::Int64 seconds() const;
        //   int nanoseconds() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING BASIC ACCESSORS"
                          << "\n=======================" << endl;
        {
            static const struct {
                int                      d_lineNum;
                bsls_PlatformUtil::Int64 d_secs;
                int                      d_nsecs;
            } VALUES[] = {
                //line          secs                 nanosecs
                //----  -------------------------    ---------
                { L_,                           0,           0 },
                { L_,                           0,           1 },
                { L_,                           1,           1 },
                { L_,                          -1,          -1 },
                { L_,       9223372036854775807LL,   999999999 },
                { L_,      -9223372036854775808LL,  -999999999 }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const bsls_PlatformUtil::Int64 SECS = VALUES[i].d_secs;
                const int LINE  = VALUES[i].d_lineNum;
                const int NSECS = VALUES[i].d_nsecs;

                Obj x;  const Obj& X = x;
                x.setInterval(SECS, NSECS);

                if (veryVerbose) {
                    T_(); P_(i);    P(X);
                    T_(); P_(SECS); P(NSECS);
                }

                LOOP_ASSERT(LINE, SECS  == X.seconds());
                LOOP_ASSERT(LINE, NSECS == X.nanoseconds());
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'gg':
        //   Void for 'bdet_TimeInterval'.
        // --------------------------------------------------------------------
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
        //   The input seconds and nanoseconds must be normalized to obtain the
        //   canonical representation of a time interval.  That is, the
        //   internal seconds field range is [-2^63, 2^63 -1] and the
        //   nanoseconds field range is [-999999999, 999999999].  The seconds
        //   and nanoseconds fields do not diff in sign.
        //
        // Plan:
        //   First, verify the default constructor by testing the value of the
        //   resulting object.
        //
        //   Next, for a sequence of independent test values, use the default
        //   constructor to create an object and use the primary manipulator
        //   to set its value.  Verify that value using the direct accessors.
        //   Note that the destructor is exercised on each configuration as the
        //   object being tested leaves scope.
        //
        // Testing:
        //   bdet_TimeInterval();
        //   ~bdet_TimeInterval();
        //   void setInterval(bsls_PlatformUtil::Int64 secs, int nanoseconds);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Primary Manipulator"
                          << "\n===========================" << endl;

        if (verbose) cout << "\nTesting default constructor." << endl;

        Obj x;  const Obj& X = x;        if (veryVerbose) P(X);

        ASSERT(0 == X.seconds());
        ASSERT(0 == X.nanoseconds());

        if (verbose) cout << "\nTesting 'setInterval'." << endl;
        {
            static const struct {
                int d_lineNum;         // Source line number
                int d_secs;            // Input seconds
                int d_nsecs;           // Input nanoseconds
                int d_expSecs;         // Expected seconds
                int d_expNsecs;        // Expected nanoseconds
            } VALUES[] = {
                //line secs    nanosecs   expected secs   expected nanosecs
                //---- ----    --------   -------------   -----------------
                { L_,   0,         0,           0,                0 },
                { L_,   1,    1000000000,       2,                0 },
                { L_,  -1,   -1000000000,      -2,                0 },
                { L_,   2,   -1000000001,       0,        999999999 },
                { L_,  -2,    1000000001,       0,       -999999999 }
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int LINE   = VALUES[i].d_lineNum;
                const int SECS   = VALUES[i].d_secs;
                const int NSECS  = VALUES[i].d_nsecs;
                const int ESECS  = VALUES[i].d_expSecs;
                const int ENSECS = VALUES[i].d_expNsecs;

                Obj x;  const Obj& X = x;
                x.setInterval(SECS, NSECS);
                if (veryVerbose) {
                    T_(); P_(X); P_(SECS); P(NSECS);
                }
                LOOP_ASSERT(LINE, ESECS  == X.seconds());
                LOOP_ASSERT(LINE, ENSECS == X.nanoseconds());
            }
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Exercise a broad cross-section of value-semantic functionality
        //   before beginning testing in earnest.  Probe that functionality
        //   systematically and incrementally to discover basic errors in
        //   isolation.
        //
        // Plan:
        //   Create four test objects by using the default, initializing, and
        //   copy constructors.  Exercise the basic value-semantic methods and
        //   the equality operators using these test objects.  Invoke the
        //   primary manipulator [3, 6], copy constructor [2, 8], and
        //   assignment operator without [9, 10] and with [11] aliasing.  Use
        //   the direct accessors to verify the expected results.  Display
        //   object values frequently in verbose mode.  Note that 'VA', 'VB',
        //   and 'VC' denote unique, but otherwise arbitrary, object values,
        //   while '0' denotes the default object value.
        //
        // 1.  Create a default object x1.          { x1:0 }
        // 2.  Create an object x2 (copy from x1).  { x1:0  x2:0 }
        // 3.  Set x1 to VA.                        { x1:VA x2:0 }
        // 4.  Set x2 to VA.                        { x1:VA x2:VA }
        // 5.  Set x2 to VB.                        { x1:VA x2:VB }
        // 6.  Set x1 to 0.                         { x1:0  x2:VB }
        // 7.  Create an object x3 (with value VC). { x1:0  x2:VB x3:VC }
        // 8.  Create an object x4 (copy from x1).  { x1:0  x2:VB x3:VC x4:0 }
        // 9.  Assign x2 = x1.                      { x1:0  x2:0  x3:VC x4:0 }
        // 10. Assign x2 = x3.                      { x1:0  x2:VC x3:VC x4:0 }
        // 11. Assign x1 = x1 (aliasing).           { x1:0  x2:VC x3:VC x4:0 }
        //
        // Testing:
        //   This Test Case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------
        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Values for testing

        const int SA = 1, NA = 2;  // VA fields
        const int SB = 3, NB = 4;  // VB fields
        const int SC = 5, NC = 6;  // VC fields

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create a default object x1."
                             "\t\t\t{ x1:0 }" << endl;
        Obj mX1;  const Obj& X1 = mX1;
        if (verbose) { T_();  P(X1); }

        if (verbose) cout << "\ta. Check initial state of x1." << endl;
        ASSERT(0 == X1.seconds());
        ASSERT(0 == X1.nanoseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy from x1)."
                             "\t\t{ x1:0  x2:0 }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (verbose) { T_();  P(X2); }

        if (verbose) cout << "\ta. Check the initial state of x2." << endl;
        ASSERT(0 == X2.seconds());
        ASSERT(0 == X2.nanoseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Set x1 to a new value VA."
                             "\t\t\t{ x1:VA x2:0 }" << endl;
        mX1.setInterval(SA, NA);
        if (verbose) { T_();  P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(SA == X1.seconds());
        ASSERT(NA == X1.nanoseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Set x2 to a new value VA."
                             "\t\t\t{ x1:VA x2:VA }" << endl;
        mX2.addSeconds(SA);     // Use different manipulators to get to the
        mX2.addNanoseconds(NA); // same place...
        if (verbose) { T_();  P(X2); }

        if (verbose) cout << "\ta. Check initial state of x2." << endl;
        ASSERT(SA == X2.seconds());
        ASSERT(NA == X2.nanoseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Set x2 to VB."
                             "\t\t\t\t{ x1:VA x2:VB }" << endl;
        mX2.setInterval(SB, NB);
        if (verbose) { T_();  P(X2); }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(SB == X2.seconds());
        ASSERT(NB == X2.nanoseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Set x1 to 0."
                             "\t\t\t\t{ x1:0  x2:VB }" << endl;
        mX1.setInterval(0, 0);
        if (verbose) { T_();  P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(0 == X1.seconds());
        ASSERT(0 == X1.nanoseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Create an object x3 with value VC."
                             "\t\t{ x1:0  x2:VB x3:VC }" << endl;

        Obj mX3(SC, NC);  const Obj& X3 = mX3;
        if (verbose) { T_();  P(X3); }

        if (verbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(SC == X3.seconds());
        ASSERT(NC == X3.nanoseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Create an object x4 (copy from x1)."
                             "\t\t{ x1:0  x2:VB x3:VC x4:0 }" << endl;

        Obj mX4(X1);  const Obj& X4 = mX4;
        if (verbose) { T_();  P(X4); }

        if (verbose) cout << "\ta. Check initial state of x4." << endl;
        ASSERT(0 == X4.seconds());
        ASSERT(0 == X4.nanoseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X4 == X1));        ASSERT(0 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(0 == (X4 == X3));        ASSERT(1 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9. Assign x2 = x1."
                             "\t\t\t\t{ x1:0  x2:0  x3:VC x4:0 }" << endl;

        mX2 = X1;
        if (verbose) { T_();  P(X2); }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(0 == X2.seconds());
        ASSERT(0 == X2.nanoseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(1 == (X2 == X4));        ASSERT(0 == (X2 != X4));


        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 10. Assign x2 = x3."
                             "\t\t\t\t{ x1:0  x2:VC x3:VC x4:0 }" << endl;

        mX2 = X3;
        if (verbose) { T_();  P(X2); }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(SC == X2.seconds());
        ASSERT(NC == X2.nanoseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 11. Assign x1 = x1 (aliasing)."
                             "\t\t\t{ x1:0  x2:VC x3:VC x4:0 }" << endl;

        mX1 = X1;
        if (verbose) { T_();  P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(0 == X1.seconds());
        ASSERT(0 == X1.nanoseconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(1 == (X1 == X4));        ASSERT(0 == (X1 != X4));

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
