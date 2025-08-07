// bsls_alignmentutil.t.cpp                                           -*-C++-*-

#include <bsls_alignmentutil.h>

#include <bsls_alignmentfromtype.h>
#include <bsls_asserttest.h>
#include <bsls_asserttestexception.h>
#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#include <bsls_types.h>  // for testing only

#include <iostream>
#include <limits>

#include <cstddef>  // offsetof() macro
#include <cstdlib>  // atoi()
#include <cstring>

#if defined(BSLS_PLATFORM_CMP_MSVC)
#pragma warning(disable:4312)
#endif

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// This component implements runtime computations based on compile-time values
// that differ among platforms.  The tests assume that alignment of `char` is
// 1, `short` is 2, `int` is 4, and `double` is at least 4.  In addition,
// certain invariants are tested, including:
//
// 1.  That all alignment calculations result in a power of 2.
//
// 2.  That `bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT` really is the largest
//     value that will be produced by the alignment calculations and that
//     `bsls::AlignmentUtil::MaxAlignedType` is aligned at
//     `bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT`.
//
// Each function is tested in isolation; there is no state carried between
// invocations.  For the each function we establish post-conditions and test
// that the postconditions hold over a reasonable range of inputs.
//-----------------------------------------------------------------------------
// TYPES
// [ 1] BSLS_MAX_ALIGNMENT
// [ 2] MaxAlignedType
//
// CLASS METHODS
// [ 3] static int calculateAlignmentFromSize(std::size_t size);
// [ 4] static int calculateAlignmentOffset(void *, int);
// [ 5] static bool is2ByteAligned(const void *);
// [ 5] static bool is4ByteAligned(const void *);
// [ 5] static bool is8ByteAligned(const void *);
// [ 5] static bool isAligned(const void *, std::size_t);
// [ 6] static std::size_t pointerAlignment(const void *address);
// [ 7] static int roundUpToMaximalAlignment(std::size_t);
// [ 8] static constexpr int defaultAlignmentOfAlignedStorage();
//-----------------------------------------------------------------------------
// [ 9] USAGE EXAMPLE -- Ensure the usage example compiles and works.
//=============================================================================

//-----------------------------------------------------------------------------
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

// TBD: This component use `cout` streaming instead of the BSL-standard
// `printf`, so wa can't delgate to `BSLS_BSLTESTUTIL_P`.
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define A(X) cout << #X " = " << ((void *) X) << endl;  // Print address
#define A_(X) cout << #X " = " << ((void *) X) << ", " << flush;
#define L_ __LINE__                           // current Line number
#define T_ cout << '\t';

// ============================================================================
//                  SEMI-STANDARD NEGATIVE-TESTING MACROS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL DEFINITIONS FOR TESTING
//-----------------------------------------------------------------------------

typedef bsls::AlignmentUtil Class;

#if defined(BSLS_PLATFORM_CPU_X86)                                            \
 && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG))
// On Linux x86, no natural type is aligned on a 64-bit boundary, but we need
// such a type to implement low-level constructs (e.g 64-bit atomic types).

struct Test8BytesAlignedType {
    long long d_dummy __attribute__((__aligned__(8)));
};
#endif

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// A sequential memory allocator is used to return a sequence of memory blocks
// of varying requested sizes from a larger chunk of contiguous memory.  Each
// block returned must also have an alignment that is sufficient for any
// conceivable object of that size.  To achieve a fully factored
// implementation, we might choose to provide a low-level helper function
// `naturallyAlign` that, given the `address` of the next available byte in
// the larger chunk along with the requested block `size` (in bytes), returns
// the first appropriately (or *naturally*) aligned address for the requested
// block at or after `address`:
// ```

    /// Return the closest memory address at or after the specified
    /// `*currentAddress` that is sufficiently aligned to accommodate any
    /// object of the specified `size`, and update `*currentAddress` to
    /// refer to the first available byte after the allocated object.  The
    /// behavior is undefined unless `1 <= size`.
    void *naturallyAlign(void **currentAddress, int size);
// ```
// We can implement the `naturallyAlign` helper function easily using the
// methods defined in this class:
// ```
    void *naturallyAlign(void **currentAddress, std::size_t size)
    {
        int   alignment = bsls::AlignmentUtil::calculateAlignmentFromSize(
                                                                size);
        int   offset    = bsls::AlignmentUtil::calculateAlignmentOffset(
                                                               *currentAddress,
                                                                alignment);
        char *result    = static_cast<char *>(*currentAddress) + offset;
        *currentAddress = result + size;
        return result;
    }
// ```
// We will then be able to use this `naturallyAlign` helper function to
// allocate, from a buffer of contiguous memory, efficiently (but not
// necessarily optimally) aligned memory for objects of varying sizes based
// solely on the size of each object (i.e., determined by its natural, not
// actual, alignment).
//
// To illustrate the functionality provided in this component, we begin by
// assuming that we have some user-defined type, `MyType`, comprising several
// data members:
// ```
    struct MyType {         // size 24; actual alignment 8; natural alignment 8
        int     d_int;
        double  d_double;   // Assume 8-byte alignment.
        char   *d_charPtr;  // Assume size <= 8 bytes.
    };
// ```
// We then define a function, `f`, which starts off by creating a maximally
// aligned `buffer` on the program stack:
// ```
//  void f()
//  {
//      // The remainder of the usage example is in the USAGE test case.
//  }
// ```

//=============================================================================
//                  CLASSES AND FUNCTIONS USED IN TESTS
//-----------------------------------------------------------------------------

#define IS_POW2(X) (((X) | ((X)-1)) + 1 == 2 * (X))

struct S1 { char d_buff[8]; S1(char); };
struct S2 { char d_buff[8]; int d_int; S2(); private: S2(const S2&); };
struct S3 { S1 d_s1; double d_double; short d_short; };
struct S4 { short d_shorts[5]; char d_c;  S4(int); private: S4(const S4&); };
#if defined(BSLS_PLATFORM_OS_LINUX) && defined(BSLS_PLATFORM_CPU_X86)
struct S5 { long long d_longLong __attribute__((__aligned__(8))); };
#endif
union  U1 { char d_c; int *d_pointer; };

template <class T>
inline
bool samePtrType(T *, void *)
{
    return false;
}

template <class T>
inline
bool samePtrType(T *, T *)
{
    return true;
}

template <class T1, class T2>
inline
bool sameType(T1 t1, T2 t2)
{
    return samePtrType(&t1, &t2);
}

typedef void (*FuncPtr)();

// Alignments for kinds of primitive types.

static const int CHAR_ALIGNMENT        = bsls::AlignmentFromType<char>::VALUE;
static const int SHORT_ALIGNMENT       = bsls::AlignmentFromType<short>::VALUE;
static const int INT_ALIGNMENT         = bsls::AlignmentFromType<int>::VALUE;
static const int LONG_ALIGNMENT        = bsls::AlignmentFromType<long>::VALUE;
static const int INT64_ALIGNMENT       =
                                     bsls::AlignmentFromType<long long>::VALUE;
static const int BOOL_ALIGNMENT        = bsls::AlignmentFromType<bool>::VALUE;
static const int WCHAR_T_ALIGNMENT     =
                                       bsls::AlignmentFromType<wchar_t>::VALUE;
static const int PTR_ALIGNMENT         = bsls::AlignmentFromType<void*>::VALUE;
static const int FUNC_PTR_ALIGNMENT    =
                                       bsls::AlignmentFromType<FuncPtr>::VALUE;
static const int FLOAT_ALIGNMENT       = bsls::AlignmentFromType<float>::VALUE;
static const int DOUBLE_ALIGNMENT      =
                                        bsls::AlignmentFromType<double>::VALUE;
static const int LONG_DOUBLE_ALIGNMENT =
                                   bsls::AlignmentFromType<long double>::VALUE;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
template <std::size_t t_START, std::size_t t_LEN>
struct Case8 {
    static void run()
    {
        Case8<t_START, t_LEN / 2>::run();
        Case8<t_START + t_LEN / 2, t_LEN - (t_LEN / 2)>::run();
    }
};

template <std::size_t t_START>
struct Case8<t_START, std::size_t(1)> {
    static void run()
    {
        constexpr std::size_t a =
                            Class::defaultAlignmentOfAlignedStorage<t_START>();

#if BSLS_COMPILERFEATURES_CPLUSPLUS > 202002L
#  if defined(BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#  endif
#endif
        ASSERT((std::is_same<std::aligned_storage<t_START>,
                             std::aligned_storage<t_START, a>>::value));
#if BSLS_COMPILERFEATURES_CPLUSPLUS > 202002L
#  if defined(BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC)
#    pragma GCC diagnostic pop
#  endif
#endif
    }
};
#endif

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 9: {
        // --------------------------------------------------------------------
        // USAGE TEST
        //   Make sure main usage examples compile and work as advertized.
        //   Thoroughly test the example function `allocateFromBuffer`.
        //
        // Test plan:
        //   Copy usage example verbatim into test driver then change `assert`
        //   to `ASSERT`.  Since usage example code declares template classes
        //   and functions, it must be placed outside of main().  Within this
        //   test case, therefore, we call the externally-declared functions
        //   defined above.
        //
        //   For completeness, test allocateFromBuffer by allocating
        //   various-sized and aligned objects.  Note that both scenarios of
        //   when `memory.d_buffer` is 4-byte aligned and 8-byte aligned are
        //   tested.  If `memory.d_buffer` is 8-byte aligned, `cursor` is
        //   offset by 4 to simulate a 4-byte aligned buffer.  If
        //   `memory.d_buffer` is 4-byte aligned, `cursor` is offset by 4 to
        //   simulate an 8-byte aligned buffer.
        //
        // Testing:
        //   USAGE EXAMPLE -- Ensure the usage example compiles and works.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUSAGE" << endl
                          << "\n=====" << endl;

        const int BUFFER_SIZE = 1024;

// TBD the following was determined by Oleg
// Re alignment of __int64 and double:
// 1) __alignof() returns 8 for both
// 2) struct members of these types get properly aligned
// 3) local variables of these types get properly aligned in the stack frame
// _HOWEVER_
// when variables of these types are on the stack, the compiler does not
// make any effort to align the stack frame to 8 bytes.
#if defined(BSLS_PLATFORM_CMP_MSVC)
static
#endif

// ```
        union {
            bsls::AlignmentUtil::MaxAlignedType d_dummy;  // force max. align.
            char                                d_buffer[BUFFER_SIZE];
        } buffer;
// ```
// Next we use the `bsls::AlignmentUtil` functions directly to confirm that
// `buffer` is sufficiently aligned to accommodate a `MaxAlignedType` object:
// ```
        int alignment = bsls::AlignmentFromType<
                                   bsls::AlignmentUtil::MaxAlignedType>::VALUE;
        int offset =
                  bsls::AlignmentUtil::calculateAlignmentOffset(
                                                               buffer.d_buffer,
                                                               alignment);
        ASSERT(0 == offset);  // sufficient alignment
// ```
// Below we perform various memory allocations using our `naturallyAlign`
// helper function:
// ```
        void *p         = static_cast<void *>(buffer.d_buffer);

        (void)            naturallyAlign(&p, sizeof(char));

        void *shortPtr5 = naturallyAlign(&p, 5 * sizeof(short));
// ```
// Note that the address held in `shortPtr` is numerically divisible by the
// alignment of a `short` on the current platform:
// ```
        ASSERT(0 == ((static_cast<char *>(shortPtr5) - buffer.d_buffer) %
                                       bsls::AlignmentFromType<short>::VALUE));

        ASSERT(bsls::AlignmentUtil::is2ByteAligned(shortPtr5));
// ```
// Next we use `naturallyAlign` to allocate a block of appropriate size and
// sufficient alignment to store a `MyType` object:
// ```
        void *objPtr = naturallyAlign(&p, sizeof(MyType));
// ```
// Note that the alignment of the address held in `objPtr` is numerically
// divisible by the actual alignment requirement:
// ```
        ASSERT(0 == bsls::AlignmentUtil::calculateAlignmentOffset(
                                      objPtr,
                                      bsls::AlignmentFromType<MyType>::VALUE));
// ```
// Assuming `buffer` has sufficient capacity, and the alignments for `char`,
// `short`, and `MyType` are, respectively, 1, 2, and 8, we would expect this
// layout within `buffer.d_buffer`:
// ```
//  charPtr shortPtr5                            objPtr
//  |       |                                                       |
//  V       V                                                       V
//  .---.---.---.---.---.---.---.---.---.---.---.---.---.---.---.---.---.---.-
//  |ccc|   |sssssss:sssssss:sssssss:sssssss:sssssss|   :   :   :   |oooooo...
//  ^---^---^---^---^---^---^---^---^---^---^---^---^---^---^---^---^---^---^-
//  0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18
// ```
// Note that on an atypical 32-bit platform where a `double` is 4-byte
// aligned, the actual alignment of `MyType` would be 4, but its natural
// alignment would still be 8 because its size would be 16; it is highly
// unlikely that `MyType` would have an actual (and therefore natural)
// alignment of 4 on a 64-bit platform when using default compiler settings.

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING `defaultAlignmentOfAlignedStorage`
        //
        // Concerns:
        // 1. For each object size `N` up to `alignof(std::max_align_t)`, the
        //    value of `defaultAlignmentOfAlignedStorage<N>()` is equal to the
        //    value of the second template argument for `std::aligned_storage`
        //    when the first template argument is specified as `N`.
        //
        // Plan:
        // 1. For each such object size, verify that `std::aligned_storage<N>`
        //    is the same type as `std::aligned_storage<N,
        //    defaultAlignmentOfAlignedStorage...<N>()>`.
        //
        // Testing:
        //   static constexpr int defaultAlignmentOfAlignedStorage();
        // --------------------------------------------------------------------

        if (verbose)
            cout << "\nTESTING `defaultAlignmentOfAlignedStorage`"
                 << "\n==========================================" << endl;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        Case8<1, alignof(std::max_align_t)>::run();
#endif
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING MAXIMUM ALIGNMENT:
        //   Memory management and allocation functions must be able to return
        //   the address of a block of memory into which any type of data can
        //   be stored.  Such a block of memory is called maximally aligned.
        //
        // Concerns:
        //   At compile time, a series of conditional compilation
        //   statements determine what the maximally-aligned type is
        //   based on macros set for the platform.  It is possible that
        //   the logic could be flawed or a new platform (combination
        //   of compiler, OS, and CPU) introduced.  While it is possible
        //   programmatically to determine the maximally-aligned type, the
        //   calculation is not efficient.  However, such functionality is
        //   well suited for a test driver.
        //
        // Plan:
        //   First, ensure that `MaxAlign` is the maximally aligned type.
        //
        //   Next, ensure that the size of the maximally aligned type is
        //   equal to its alignment offset.
        //
        //   Finally, ensure that `roundUpToMaximalAlignment` rounds its
        //   argument up to the nearest multiple of `sizeof(MaxAlign)`.
        //
        // Testing:
        //   static int roundUpToMaximalAlignment(std::size size);
        // --------------------------------------------------------------------

        ASSERT(0 == Class::roundUpToMaximalAlignment(0));

        struct MaxAlignAlign    { char c; Class::MaxAlignedType d_maxAlign; };

        const int maxAlignment = offsetof(MaxAlignAlign, d_maxAlign);
        if (veryVerbose) {
            T_; P_(maxAlignment);
            P_(sizeof(Class::MaxAlignedType));
        }

        int i;
        for (i = 1; i <= maxAlignment; ++i) {
            ASSERT(    maxAlignment ==
                                    (int) Class::roundUpToMaximalAlignment(i));
        }
        for (i = maxAlignment + 1; i <= 2 * maxAlignment; ++i) {
            ASSERT(2 * maxAlignment ==
                                    (int) Class::roundUpToMaximalAlignment(i));
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING `pointerAlignment`
        //
        // Concerns:
        // 1. The `pointerAlignment` function correctly returns the alignment
        //    of the address held in its pointer argument.
        // 2. If the address is 0, `pointerAlignment` returns 0.
        //
        // Plan:
        // 1. Using a set of addresses with alignments from 1 to `2 *
        //    k_MAX_ALIGNMENT`, verify that `pointerAlignment` returns the
        //    correct alignment value for each.  (C-1)
        // 2. Verify that, when given a null pointer, `pointerAlignment`
        //    returns 0.  (C-2)
        //
        // Testing:
        //   static std::size_t pointerAlignment(const void *address);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING `pointerAlignment`"
                          << "\n==========================" << endl;

        static const int k_MAX_ALIGNMENT = (int) Class::BSLS_MAX_ALIGNMENT;

        Class::MaxAlignedType alignedBuf[3] = { };

        char *p = reinterpret_cast<char *>(alignedBuf);
        if (Class::pointerAlignment(p) > k_MAX_ALIGNMENT) {
            p += k_MAX_ALIGNMENT;  // Aligned but not overaligned address
        }

        // Verify that `p` is *exactly* aligned to `k_MAX_ALIGNMENT`.
        ASSERTV((void*) p, Class::pointerAlignment(p), k_MAX_ALIGNMENT,
                Class::pointerAlignment(p) == k_MAX_ALIGNMENT);

        // Verify expected alignments in range [1, k_MAX_ALIGNMENT).
        for (int offset = 1; offset < k_MAX_ALIGNMENT; ++offset) {

            // Compute expected aligment using the (inefficient) left-shift
            // method so as not to test the `pointerAlignment` function using
            // the same algorithm.
            std::size_t expAlign;
            for (expAlign = 1; ! (offset & expAlign); expAlign <<= 1)
                ;

            ++p;
            ASSERTV(offset, expAlign, (void *) p,
                    Class::pointerAlignment(p) == expAlign);
        }

        // The final value of `p` is aligned to *at least* twice
        // `k_MAX_ALIGNMENT`, but possibly more.  Don't look for exact match.
        ++p;
        ASSERTV((void*) p, Class::pointerAlignment(p),
                Class::pointerAlignment(p) > k_MAX_ALIGNMENT);

        // Synthesize pointers with very large alignments.  These pointers do
        // not necessarily represent actual blocks of memory.
        std::size_t align = k_MAX_ALIGNMENT;
        do {
            ::size_t ptrBits = ~(align - 1);
            ASSERTV(ptrBits, align,
                    align == Class::pointerAlignment((void *) ptrBits));
            align <<= 1;
        } while (align < (std::numeric_limits<std::size_t>::max() >> 1));

        // Verify that null pointer alignment is returned as 0.
        ASSERT(0 == Class::pointerAlignment((void *) 0));

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING `is[[248]Byte]Aligned`
        //
        // Concerns:
        // 1. For a given address, `isXByteAligned` returns `true` if the
        //    address is a multiple of *X* and `false` otherwise.
        // 2. For a given address, `isAligned` returns `true` if the given
        //    address is a multiple of the specified alignment and false
        //    otherwise.
        // 3. If the specified address is 0, all of these functions return
        //    `true`, regardless of the passed-in alignment, if any.
        // 4. `isAligned` returns `true` when passed a 0 alignment, regardless
        //    of the input address.
        //
        // Plan:
        // 1. Using a table-driven approach, invoke each function with a
        //    variety of addresses.  Verify that each `isXbyteAligned` function
        //    returns the expected value for the address's actual alignment.
        //    (C-1)
        // 2. Using the same addresses as in step 1, verify that `isAligned`
        //    returns `true` when called with the address's actual alignment,
        //    `true` when called with half the address's actual alignment, and
        //    `false` when called with double the address's actual alignment.
        //    (C-2)
        // 3. Verify that all four functions return `true` for null addresses,
        //    regardless of the alignment argument, if any.
        // 4. For each address in step 2, verify that `isAligned` returns
        //    `true` when passed a 0 alignment.
        //
        // Testing:
        //   static bool is2ByteAligned(const void *);
        //   static bool is4ByteAligned(const void *);
        //   static bool is8ByteAligned(const void *);
        //   static bool isAligned(const void *, std::size_t);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl << "TESTING `is[[248]Byte]Aligned`" << endl
                         << "==============================" << endl;
        }

        static const struct {
            int          d_line;     // line number
            const void  *d_address;  // address
            std::size_t  d_align;    // address alignment
        } DATA[] = {
            //     Address                 Alignment
            //     ======================  =========
            { L_,  (const void *)0x1,          1 },
            { L_,  (const void *)0x4,          4 },
            { L_,  (const void *)0x5,          1 },
            { L_,  (const void *)0x6,          2 },
            { L_,  (const void *)0x8,          8 },
            { L_,  (const void *)0x2FE2,       2 },
            { L_,  (const void *)0x2FE3,       1 },
            { L_,  (const void *)0x2FE4,       4 },
            { L_,  (const void *)0x2FE8,       8 },
            { L_,  (const void *)0x2FE9,       1 },
            { L_,  (const void *)0xFFFFFFFF,   1 },
            { L_,  (const void *)0xFFFFFFFE,   2 },
            { L_,  (const void *)0xFFFFFFFC,   4 },
            { L_,  (const void *)0xFFFFFFF8,   8 },
            { L_,  (const void *)0xFFFFFFF0,  16 },
            { L_,  (const void *)0xFFFFFFE0,  32 },
            { L_,  (const void *)0xFFFFFFC0,  64 },
            { L_,  (const void *)0xFFFFFF80, 128 },
            { L_,  (const void *)0xFFFFFF00, 256 }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        // Test non-null pointers
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int          LINE   = DATA[ti].d_line;
            const void        *ADDR   = DATA[ti].d_address;
            const std::size_t  ALIGN  = DATA[ti].d_align;

            const bool         ALIGN2 = ALIGN >= 2;
            const bool         ALIGN4 = ALIGN >= 4;
            const bool         ALIGN8 = ALIGN >= 8;

            ASSERTV(LINE, ALIGN,    Class::isAligned(ADDR, 0));
            ASSERTV(LINE, ALIGN,    Class::isAligned(ADDR, ALIGN));
            ASSERTV(LINE, ALIGN,    Class::isAligned(ADDR, ALIGN >> 1));
            ASSERTV(LINE, ALIGN,  ! Class::isAligned(ADDR, ALIGN << 1));
            ASSERTV(LINE, ALIGN2 == Class::is2ByteAligned(ADDR));
            ASSERTV(LINE, ALIGN4 == Class::is4ByteAligned(ADDR));
            ASSERTV(LINE, ALIGN8 == Class::is8ByteAligned(ADDR));
        }

        // Test null pointer
        ASSERT(Class::isAligned(0, 0));
        ASSERT(Class::isAligned(0, 2));
        ASSERT(Class::isAligned(0, 4));
        ASSERT(Class::is2ByteAligned(0));
        ASSERT(Class::is4ByteAligned(0));
        ASSERT(Class::is8ByteAligned(0));

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING `calculateAlignmentOffset`
        //   Ensure correctness for each pair of (ma, a), where `ma` is a
        //   memory address from `baseAddr` to
        //   `baseAddr + 2 * BSLS_MAX_ALIGNMENT + 1`
        //   and `a` is an alignment in [1 2 4 .. BSLS_MAX_ALIGNMENT] such
        //   that:
        //     0 <= result
        //     result < a
        //     0 == (ma + result - baseAddr) % a;
        //     for each k in [ 0 .. result - 1 ]
        //         0 != (ma + k - baseAddr) % a;
        // Plan:
        //   Declare baseAddr as a pointer to a maximally-aligned memory
        //     buffer of size 2 * BSLS_MAX_ALIGNMENT + 1.
        //   Iterate `ma` over each address in buffer and iterate `a` each
        //     power of two up to BSLS_MAX_ALIGNMENT.
        //   Compute calculateAlignmentOffset(ma, a) and
        //     verify all concerns listed above.
        //
        // Tactics:
        //   Area test over meaningful range of inputs.
        //
        // Testing:
        //   static int calculateAlignmentOffset(void *, int);
        // --------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_GNU) && defined(BSLS_PLATFORM_OS_AIX)
        if (verbose) cout << "Test disabled on AIX GNU\n";
#else
        if (verbose) cout << endl << "Test calculateAlignmentOffset" << endl
                                  << "=============================" << endl;

        // MSC compiler lays out structs with 8-byte alignment, but then
        // allows those structs to appear on the stack with 4-byte alignment.
        // This is not necessarily a bug, but it is counter-intuitive.  To be
        // sure that this buffer has max alignment, we put it in static
        // storage.  This is for testing only -- in practice, it is rarely
        // necessary to force the alignment of a stack variable beyond what
        // the compiler requires.  Moreover, this trick does not even work for
        // gcc builds on AIX.

        const int BUF_SIZE = 3 * Class::BSLS_MAX_ALIGNMENT;

        static union {
            char                  d_buffer[BUF_SIZE];
            Class::MaxAlignedType d_align;
        } alignedBuf;

        char *baseAddr = alignedBuf.d_buffer;

        for (char *ma = baseAddr;
             ma < baseAddr + 2 * Class::BSLS_MAX_ALIGNMENT + 1;
             ++ma) {
            for (int a = 1;
                 a <= Class::BSLS_MAX_ALIGNMENT;
                 a <<= 1) {

                int offset = Class::calculateAlignmentOffset(ma, a);

                if (veryVerbose) {
                    cout << "\tMemory address = " << (void*) ma
                         << "\tAlignment = " << a
                         << "\tOffset = " << offset << endl;
                }

                ASSERTV(a, ma, 0 <= offset);
                ASSERTV(a, ma, offset < a);
                ASSERTV(a, ma, 0 == (ma - baseAddr + offset) % a);
                for (int k = 0; k < offset; ++k) {
                    ASSERTV(a, ma, k, 0 != (ma + k - baseAddr) % a);
                }
            }
        }

        if (verbose) cout << "\nNegative Testing" << endl;

        {
#ifdef BDE_BUILD_TARGET_EXC
            bsls::AssertTestHandlerGuard hG;

            // `0` in the expected `ALIGN` column indicates that the call is
            // out of contract, so any result will be accepted unless the
            // relevant `BSLS_ASSERT` macro is enabled.

            static struct {
                int         d_lineNumber;
                char        d_expectedResult;
                const void *d_address;
                int         d_align;
            } const DATA[] = {
                // LINE  RESULT  ADDRESS  ALIGN
                // ----  ------  ----     -----
                {  L_,   'F',     0,      -1 },
                {  L_,   'F',     0,       0 },
                {  L_,   'P',     0,       1 },
                {  L_,   'P',     0,       2 },
                {  L_,   'F',     0,       3 },
                {  L_,   'P',     0,       4 },
                {  L_,   'F',     0,       5 },
                {  L_,   'F',     0,       6 },
                {  L_,   'F',     0,       7 },
                {  L_,   'P',     0,       8 },
                {  L_,   'F',     0,       9 },
                {  L_,   'F',     0,       65535 },
                {  L_,   'P',     0,       65536 },
                {  L_,   'F',     0,       65537 }
            };
            const int DATA_SIZE = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < DATA_SIZE; ++i) {
                const int         LINE    = DATA[i].d_lineNumber;
                const char        RESULT  = DATA[i].d_expectedResult;
                const void *const ADDRESS = DATA[i].d_address;
                const int         ALIGN   = DATA[i].d_align;

                if(veryVerbose) {
                    T_ P_(RESULT) P_(ADDRESS) P(ALIGN)
                }

                if ('F' == RESULT)
                {
                    ASSERT_SAFE_FAIL(Class::calculateAlignmentOffset(ADDRESS,
                                                                     ALIGN));
                }
                else
                {
                    int a = 0;
                    ASSERT_SAFE_PASS(a = Class::calculateAlignmentOffset(
                                         ADDRESS,
                                         ALIGN));

                    ASSERTV(LINE, ADDRESS, ALIGN, a, a == 0);
                }
            }
#else
            if (verbose) cout <<
                "\nDISABLED in this (non-exception) build mode." << endl;
#endif
        }
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING `calculateAlignmentFromSize`
        //   Ensure correctness for all object sizes:
        //     Result must be greater than 0.
        //     Result must be less than or equal to `BSLS_MAX_ALIGNMENT`.
        //     Size must be evenly divisible by result.
        //     Size must not be evenly divisible by `2 * result` unless
        //     `result == BSLS_MAX_ALIGNMENT`.
        //
        // Plan:
        //   Assume `BSLS_MAX_ALIGNMENT <= 16`.
        //   Compare results against expected values for inputs from 1 to 16.
        //   Try all sizes up to `2 * BSLS_MAX_ALIGNMENT + 1`.
        //   Test all concerns listed above.
        //
        // Tactics:
        //   Area test over meaningful range of inputs.
        //
        // Testing:
        //   static int calculateAlignmentFromSize(std::size_t);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Test calculateAlignmentFromSize" << endl
            << "===============================" << endl;

        static struct {
            int d_line;
            int d_input;
            int d_expected;
        } const DATA[] = {
            // line input expected
            // ---- ----- --------
            {  L_,    1,    1 },
            {  L_,    2,    2 },
            {  L_,    3,    1 },
            {  L_,    4,    4 },
            {  L_,    5,    1 },
            {  L_,    6,    2 },
            {  L_,    7,    1 },
            {  L_,    8,    8 },
            {  L_,    9,    1 },
            {  L_,   10,    2 },
            {  L_,   11,    1 },
            {  L_,   12,    4 },
            {  L_,   13,    1 },
            {  L_,   14,    2 },
            {  L_,   15,    1 }
        };
        const int DATA_SIZE = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < DATA_SIZE; ++i) {
            const int LINE     = DATA[i].d_line;
            const int INPUT    = DATA[i].d_input;
            const int EXPECTED = DATA[i].d_expected;

            int a = Class::calculateAlignmentFromSize(INPUT);
            ASSERTV(LINE, EXPECTED == a);
        }

        for (int size = 1;
             size <= 2 * Class::BSLS_MAX_ALIGNMENT + 1;
             ++size) {
            int a = Class::calculateAlignmentFromSize(size);

            if (veryVerbose) {
                cout << "    Object size = " << size
                     << "\tGuessed alignment = " << a << endl;
            }

            ASSERTV(size,  a > 0);
            ASSERTV(size,  IS_POW2(a));
            ASSERTV(size,  a <= Class::BSLS_MAX_ALIGNMENT);
            ASSERTV(size,  0 == size % a);
            ASSERTV(size, (0 != size % (2 * a) ||
                           a == Class::BSLS_MAX_ALIGNMENT));

        }

        if (verbose) cout << "\nNegative Testing" << endl;

        {
#ifdef BDE_BUILD_TARGET_EXC
            bsls::AssertTestHandlerGuard hG;

            // `0` in the expected `ALIGN` column indicates that the call is
            // out of contract, so any result will be accepted unless the
            // relevant `BSLS_ASSERT` macro is enabled.

            static struct {
                int          d_lineNumber;
                char         d_expectedResult;
                std::size_t  d_size;
                int          d_expected;
            } const DATA[] = {
                // LINE  RESULT  SIZE  ALIGN
                // ----  ------  ----  -----
                {  L_,   'F',     0,      0 },
                {  L_,   'P',     1,      1 },
                {  L_,   'P',    std::numeric_limits<std::size_t>::max(),
                                                1 }
            };
            const int DATA_SIZE = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < DATA_SIZE; ++i) {
                const int         LINE   = DATA[i].d_lineNumber;
                const char        RESULT = DATA[i].d_expectedResult;
                const std::size_t SIZE   = DATA[i].d_size;
                const int         ALIGN  = DATA[i].d_expected;

                if(veryVerbose) {
                    T_ P_(RESULT) P_(SIZE) P(ALIGN)
                }

                if ('F' == RESULT) {
                    ASSERT_SAFE_FAIL(Class::calculateAlignmentFromSize(SIZE));
                } else {
                    int a = 0;
                    ASSERT_SAFE_PASS(a = Class::
                                     calculateAlignmentFromSize(SIZE));

                    ASSERTV(LINE, SIZE, ALIGN, a, ALIGN == a);
                }
            }
#else
            if (verbose) cout <<
                "\nDISABLED in this (non-exception) build mode." << endl;
#endif
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING `MaxAlignedType`
        //   Ensure that alignment of `MaxAlignedType` is `BSLS_MAX_ALIGNMENT`
        //     and `sizeof(MaxAlignedType) == BSLS_MAX_ALIGNMENT`
        //
        // Plan:
        //   Use bsls::AlignmentUtilFromType<T>::value to verify
        //   alignment of MaxAlignedType.
        //
        // Tactics:
        //   Exhaustive test over entire input range.
        //
        // Testing:
        //   MaxAlignedType
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING `MaxAlignedType`" << endl
                                  << "========================" << endl;

        typedef bsls::Types U;

        ASSERT((int) bsls::AlignmentFromType<Class::MaxAlignedType>::VALUE ==
               (int) Class::BSLS_MAX_ALIGNMENT);

        ASSERT(sizeof(Class::MaxAlignedType) == Class::BSLS_MAX_ALIGNMENT);

        if (verbose) cout << endl << "MAXIMUM ALIGNMENT TEST" << endl
                                  << "======================" << endl;

        struct ShortAlign       { char c; short  d_short;           };
        struct IntAlign         { char c; int    d_int;             };
        struct LongAlign        { char c; long   d_long;            };
        struct Int64Align       { char c; U::Int64 d_int64;         };
        struct FloatAlign       { char c; float  d_float;           };
        struct DoubleAlign      { char c; double d_double;          };
        struct LongDoubleAlign  { char c; long double d_longDouble; };
        struct VoidPtrAlign     { char c; void  *d_voidPtr;         };
#if defined(BSLS_PLATFORM_CPU_X86)                                            \
 && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG))
        struct Test8bytesAlign  { char c; Test8BytesAlignedType
                                               d_8BytesAlignedType; };
#endif
        struct MaxAlignAlign    { char c; Class::MaxAlignedType d_maxAlign; };

        const int EXP =
            1 + ( (1                                       - 1) // char
                | (offsetof(ShortAlign, d_short)           - 1)
                | (offsetof(IntAlign, d_int)               - 1)
                | (offsetof(LongAlign, d_long)             - 1)
                | (offsetof(Int64Align, d_int64)           - 1)
                | (offsetof(FloatAlign, d_float)           - 1)
                | (offsetof(DoubleAlign, d_double)         - 1)
#if ! (defined(BSLS_PLATFORM_CPU_POWERPC) && defined(BSLS_PLATFORM_OS_LINUX))
                | (offsetof(LongDoubleAlign, d_longDouble) - 1)
#endif
                | (offsetof(VoidPtrAlign, d_voidPtr)       - 1)
#if defined(BSLS_PLATFORM_CPU_X86)                                            \
 && (defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG))
                | (offsetof(Test8bytesAlign,
                            d_8BytesAlignedType)           - 1)
#endif
                );

        const int maxAlignment = offsetof(MaxAlignAlign, d_maxAlign);
        if (veryVerbose) {
            T_; P_(maxAlignment);
            P_(sizeof(Class::MaxAlignedType)); P(EXP);
        }

        ASSERT(EXP == maxAlignment);
        ASSERT(EXP == sizeof(Class::MaxAlignedType));

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING COMPILE-TIME CONSTANTS
        //   BSLS_MAX_ALIGNMENT is divisible by the alignment constant,
        //   The alignment is a non-negative integral power of 2.
        //   Ensure that BSLS_MAX_ALIGNMENT is the same as one or more of the
        //   primitive alignments.
        //   Ensure that BSLS_MAX_ALIGNMENT is the largest primitive alignment.
        //
        // Plan:
        //   Verify each concern for each primitive data type.
        //   Note: Verifying that BSLS_MAX_ALIGNMENT is divisible by each
        //   alignment constant guarantees that BSLS_MAX_ALIGNMENT is at least
        //   as big as each alignment constant.
        //   Display alignments in very verbose mode (only).
        //
        //   In veryVerbose mode, print the alignment of each type being
        //     tested.
        //
        // Tactics:
        //   Exhaustive test over entire input range.
        //
        // Testing:
        //   BSLS_MAX_ALIGNMENT
        //
        //   PRINT ALIGNMENTS IN VERY VERBOSE MODE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING COMPILE-TIME CONSTANTS"
                          << "\n==============================" << endl;

        if (veryVerbose) {
            P(Class::BSLS_MAX_ALIGNMENT);
            P(sizeof(Class::MaxAlignedType));
        }

        if (veryVerbose) cout << "\nVerify 0 == BSLS_MAX_ALIGNMENT % alignment"
                              << endl;

        ASSERT(0 == Class::BSLS_MAX_ALIGNMENT % CHAR_ALIGNMENT);
        ASSERT(0 == Class::BSLS_MAX_ALIGNMENT % SHORT_ALIGNMENT);
        ASSERT(0 == Class::BSLS_MAX_ALIGNMENT % INT_ALIGNMENT);
        ASSERT(0 == Class::BSLS_MAX_ALIGNMENT % LONG_ALIGNMENT);
        ASSERT(0 == Class::BSLS_MAX_ALIGNMENT % INT64_ALIGNMENT);
        ASSERT(0 == Class::BSLS_MAX_ALIGNMENT % BOOL_ALIGNMENT);
        ASSERT(0 == Class::BSLS_MAX_ALIGNMENT % WCHAR_T_ALIGNMENT);
        ASSERT(0 == Class::BSLS_MAX_ALIGNMENT % PTR_ALIGNMENT);
        ASSERT(0 == Class::BSLS_MAX_ALIGNMENT % FUNC_PTR_ALIGNMENT);
        ASSERT(0 == Class::BSLS_MAX_ALIGNMENT % FLOAT_ALIGNMENT);
        ASSERT(0 == Class::BSLS_MAX_ALIGNMENT % DOUBLE_ALIGNMENT);
        ASSERT(0 == Class::BSLS_MAX_ALIGNMENT % LONG_DOUBLE_ALIGNMENT);

        if (veryVerbose) cout <<
            "\nVerify alignment is non-negative integral power of 2." << endl;

        ASSERT(IS_POW2(Class::BSLS_MAX_ALIGNMENT));

      } break;
      default: {
        cerr << "WARNING: CASE `"<< test << "' NOT FOUND." <<endl;
        testStatus = -1;
      } break;
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
