// bslmf_forwardingreftype.t.cpp                                      -*-C++-*-

#include <bsls_platform.h>

// the following suppresses warnings from `#include` inlined functions
#if defined(BSLS_PLATFORM_CMP_MSVC)
#pragma warning(disable:4180)
#endif

#include <bslmf_forwardingreftype.h>

#include <bslmf_addconst.h>
#include <bslmf_addcv.h>
#include <bslmf_addlvaluereference.h>
#include <bslmf_addvolatile.h>
#include <bslmf_assert.h>
#include <bslmf_isarray.h>
#include <bslmf_issame.h>          // for testing only

#include <bsla_maybeunused.h>

#include <bsls_bsltestutil.h>
#include <bsls_compilerfeatures.h>

#include <utility>     // `std::move` for C++11

#include <stdio.h>     // atoi()
#include <stdlib.h>    // atoi()

#include <typeinfo>

using namespace BloombergLP;

#if defined(BSLS_PLATFORM_CMP_MSVC)
#pragma warning(disable: 4521) // test classes offer multiple copy constructors
#endif

#if defined(BSLS_PLATFORM_CMP_SUN)
#pragma error_messages(off, refarray0)
#pragma error_messages(off, SEC_UNINITIALIZED_MEM_READ)
#endif

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#ifdef BSLS_PLATFORM_CMP_CLANG
#pragma GCC diagnostic ignored "-Wdeprecated-volatile"
#endif
#endif

// Suppress bde_verify messages about all-uppercase type names.  Test drivers
// are rife with short names like `F` or `PF` or `T` or `T1`.

// BDE_VERIFY pragma: -UC01

// ============================================================================
//                                TEST PLAN
// ----------------------------------------------------------------------------
//                                Overview
//                                --------
// This component provides a meta-function.  We simply verify that it returns
// the proper type for a list of suitably chosen arguments.  It also provides a
// utility function for which we verify that it returns the correct type and
// value.  Finally, we perform an end-to-end test that exercises the intended
// use of the component in order to verify that it is actually useful as
// specified.
// ----------------------------------------------------------------------------
// [ 2] ForwardingRefType<TYPE>::Type
// [ 3] ForwardingRefTypeUtil<TYPE>::TargetType
// [ 3] ForwardingRefTypeUtil<TYPE>::forwardToTarget(v)
// ----------------------------------------------------------------------------
// [ 4] END-TO-END OVERLOADING
// [ 5] USAGE EXAMPLES
// [ 1] CvRefMatch<T>::operator()()
// [ 1] CvArrayMatch<T>::operator()()
// [ 1] FuncMatch<F>::operator()()

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//               ADDITIONAL MACROS FOR THIS TEST DRIVER
// ----------------------------------------------------------------------------

#define ASSERT_SAME(X, Y) ASSERTV(L_, (bsl::is_same<X, Y>::value))

#if defined(BSLS_PLATFORM_CMP_IBM)                                            \
 ||(defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130)     \
 ||(defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VERSION < 40300)

# define BSLMF_FORWARDINGREFTYPE_NO_ARRAY_OF_UNKNOWN_BOUND 1
    // This macro signifies that this compiler rejects `Type[]` as incomplete,
    // even in contexts where it should be valid, such as where it will pass by
    // reference or pointer.
#endif

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION <= 1900
    // This was last tested with MSVC 2015, but the bug may persist in later
    // versions, not yet released.  Update the version test accordingly.

# define BSLMF_FORWARDINGREFTYPE_NO_ARRAY_DECAY_TO_RVALUE_REF 1
    // This compiler cannot bind an rvalue array, such as `char[]`, to a
    // rvalue-reference to a decayed array pointer, which would be `char *&&`
    // in this example.
#endif

#if defined(BSLS_PLATFORM_CMP_SUN )&& BSLS_PLATFORM_CMP_VERSION < 0x5130
# define BSLMF_FOWARDINGTYPE_WORK_AROUND_SUN_ARRAY_TESTS 1
    // The Sun compiler has problems with any test involving arrays, triggering
    // internal compiler errors with no hint of the line(s) triggering the
    // problem.  This appears to be an artifact of several function templates
    // that are called by explicitly specifying the first template parameter to
    // be an array type.  If the function argument is also an array type, the
    // internal compiler error is triggered, while transforming that function
    // parameter type to an array reference appears to solve the problem.  As
    // the fix is entirely about making the test driver well-formed, and does
    // not impact the component header, this is deemed an acceptable workaround
    // to pass the test.
    //
    // Note that it looks like the underlying cause has been resolved in the
    // latest 12.3 compiler, but retaining this workaround from an abundance
    // of caution.
#endif

#if defined(BSLS_PLATFORM_CMP_IBM)
# define BSLMF_FOWARDINGTYPE_NO_SUPPORT_FOR_POINTER_TO_CV_MEMBER_FUNCTION 1
    // This is an obscure bug encountered on AIX with xlC, last tested with
    // version 12.1.  The problem is the compiler fails to find a match when
    // passing a pointer-to-cv-qualified-member-function as an argument to a
    // function:
    // ```
    // struct Test {
    //     void function() const;
    // }
    //
    // typedef void (Test::*pmf)() const;  // use typedef so `const`ness clear
    //
    // void match(pmf) {}
    //
    // int main() {
    //     match(&Test::function); // xlC cannot find a match; fails to compile
    // }
    // ```
#endif

//=============================================================================
//                  GLOBAL TYPES/OBJECTS FOR TESTING
//-----------------------------------------------------------------------------

enum   Enum   { e_VAL1 = 1, e_VAL2 };

struct Struct {
    int d_data;

    /// Construct from the specified `v`.
    explicit Struct(int v) : d_data(v) { }

    /// Copy-construct from the specified `other`.
    Struct(Struct& other) : d_data(other.d_data) { }
    Struct(const Struct& other) : d_data(other.d_data) { }
    Struct(volatile Struct& other) : d_data(other.d_data) { }
    Struct(const volatile Struct& other) : d_data(other.d_data) { }
};

/// Return true if the specified `a` and `b` objects have same data member
/// value.
inline
bool operator==(Struct a, Struct b)
{
    return a.d_data == b.d_data;
}

union  Union  {
    int d_data;

    /// Construct from the specified `v`.
    explicit Union(int v) : d_data(v) { }

    /// Copy-construct from the specified `other`.
    Union(Union& other) : d_data(other.d_data) { }
    Union(const Union& other) : d_data(other.d_data) { }
    Union(volatile Union& other) : d_data(other.d_data) { }
    Union(const volatile Union& other) : d_data(other.d_data) { }
};

/// Return true if the specified `a` and `b` objects have the same data
/// member value.
inline
bool operator==(Union a, Union b)
{
    return a.d_data == b.d_data;
}

class  Class  {
    int d_data;
  public:
    /// Construct from the specified `v`.
    explicit Class(int v) : d_data(v) { }

    /// Copy-construct from the specified `other`.
    Class(Class& other) : d_data(other.d_data) { }
    Class(const Class& other) : d_data(other.d_data) { }
    Class(volatile Class& other) : d_data(other.d_data) { }
    Class(const volatile Class& other) : d_data(other.d_data) { }

    /// Return the value '2'.
    int two() { return 2; }

    /// Return the value of this object.
    int value() const { return d_data; }
};

/// Return true if the specified `a` and `b` have the same `value()`.
inline
bool operator==(Class a, Class b)
{
    return a.value() == b.value();
}

typedef void      F ();
typedef void (*  PF)();

typedef void    Fi  (int);
typedef void    FRi (int&);

typedef char    A [5];
typedef char   AU [];

typedef int Struct::*Pm;
typedef int (Class::*Pmf)();
typedef int (Class::*Pmq)() const;

/// Noop function that takes no arguments and returns nothing.
void func() { }

/// Noop function that takes one by-value argument and returns nothing.
void funcI(int) { }

/// Noop function that takes one by-reference argument and returns nothing.
void funcRi(int&) { }

enum {
    k_LVALUE,
    k_CONST_LVALUE,
    k_VOLATILE_LVALUE,
    k_CONST_VOLATILE_LVALUE,
    k_RVALUE,
    k_CONST_RVALUE,
    k_VOLATILE_RVALUE,
    k_CONST_VOLATILE_RVALUE,
    k_FUNC_POINTER,
    k_FUNC_REFERENCE
};

/// Return a copy of the specified `v` object.  If `TP` has a cv-qualifier,
/// then the compiler might discard it in the return type.  For testing
/// purposes, what is important is that we get exactly what we would get
/// when returning an object of type `TP`.
template <class TP>
TP toRvalue(TP v)
{
    return v;
}

/// Function object type that can be invoked with a cv-qualified reference
/// to an object of the specified `TP` parameter type.
template <class TP>
struct CvRefMatch {

    /// Invoke with an l-value reference.
    int operator()(TP&) const { return k_LVALUE; }
    int operator()(const TP&) const { return k_CONST_LVALUE; }
    int operator()(volatile TP&) const { return k_VOLATILE_LVALUE; }
    int operator()(const volatile TP&) const { return k_CONST_VOLATILE_LVALUE;}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    /// Invoke with an r-value reference.
    int operator()(TP&&) const { return k_RVALUE; }
    int operator()(const TP&&) const { return k_CONST_RVALUE; }
    int operator()(volatile TP&&) const { return k_VOLATILE_RVALUE; }
    int operator()(const volatile TP&&) const
        { return k_CONST_VOLATILE_RVALUE; }
#endif
};

/// Function object type that can be invoked with an array of the specified
/// `TP` parameter type.
template <class TP>
class CvArrayMatch {

    /// Matches an array of known size.
    template <size_t k_SZ>
    int match(TP (&)[k_SZ], int) const { return int(k_SZ); }

    /// Matches an array of unknown size.
    int match(TP [], ...) const { return 0; }

  public:

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
    /// Matches both lvalue and rvalue arrays.
    template <class ANY>
    int operator()(ANY&& a) const { return match(a, 0); }
#else
    template <class ANY>
    int operator()(ANY& a) const { return match(a, 0); }
#endif
};

/// Function object type that can be invoked with a function, function
/// pointer, or function reference.
template <class FUNC>
struct FuncMatch {

    /// Class that is convertible from `FUNC *`
    struct MatchFuncPtr {

        /// Convert a `FUNC *` pointer to a `MatchFuncPtr` object.
        MatchFuncPtr(FUNC *) { }                                    // IMPLICIT
    };

    /// Match a function or a function reference.
    int operator()(FUNC&) const { return k_FUNC_REFERENCE; }

    /// Match function pointer.  This overload requires a user-defined
    /// conversion and this is less preferred than the previous one for
    /// arguments that match both.
    int operator()(MatchFuncPtr) const { return k_FUNC_POINTER; }
};

/// Forward the specified `arg` to the specified `target`.  This function is
/// called in the middle of a chain of function calls that forward to the
/// eventual `target` invocable object.
template <class TP, class INVOCABLE>
int endToEndIntermediary(typename bslmf::ForwardingRefType<TP>::Type arg,
                         const INVOCABLE&                         target)
{
    return target(bslmf::ForwardingRefTypeUtil<TP>::forwardToTarget(arg));
}

/// Forward the specified `arg` to the specified `target` via an
/// intermediate function.  This function is called at the start of a chain
/// of function calls that forward to the eventual `target` invocable
/// object.
template <class TP, class INVOCABLE>
int testEndToEnd(TP arg, const INVOCABLE& target)
{
    return endToEndIntermediary<TP>(arg, target);
}


#if defined(BSLMF_FOWARDINGTYPE_WORK_AROUND_SUN_ARRAY_TESTS)
template <class TP, class INVOCABLE>
int testEndToEndArray(typename bsl::add_lvalue_reference<TP>::type arg,
                      const INVOCABLE&                             target)
    // Forward the specified `arg` to the specified `target` via an
    // intermediate function.  This function is called at the start of a chain
    // of function calls that forward to the eventual `target` invocable
    // object.
{
    return endToEndIntermediary<TP>(arg, target);
}
#endif
//=============================================================================
//                           USAGE EXAMPLES
//-----------------------------------------------------------------------------

///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Direct look at metafunction results
///- - - - - - - - - - - - - - - - - - - - - - -
// In this example, we invoke `ForwardingRefType` on a variety of types and
// look at the resulting `Type` member:
// ```
    struct MyType {};
    typedef MyType& MyTypeRef;

    /// Usage example.
    void usageExample1()
    {
        BSLA_MAYBE_UNUSED typedef int                     T1;
        BSLA_MAYBE_UNUSED typedef int&                    T2;
        BSLA_MAYBE_UNUSED typedef const volatile double&  T3;
        BSLA_MAYBE_UNUSED typedef const double &          T4;
        BSLA_MAYBE_UNUSED typedef const float * &         T5;
        BSLA_MAYBE_UNUSED typedef const float * const &   T6;
        BSLA_MAYBE_UNUSED typedef MyType                  T7;
        BSLA_MAYBE_UNUSED typedef const MyType&           T8;
        BSLA_MAYBE_UNUSED typedef MyType&                 T9;
        BSLA_MAYBE_UNUSED typedef MyType                 *T10;
        BSLA_MAYBE_UNUSED typedef int                     T11[];
        BSLA_MAYBE_UNUSED typedef int                     T12[3];

        BSLA_MAYBE_UNUSED typedef const int&              EXP1;
        BSLA_MAYBE_UNUSED typedef int&                    EXP2;
        BSLA_MAYBE_UNUSED typedef const volatile double&  EXP3;
        BSLA_MAYBE_UNUSED typedef const double &          EXP4;
        BSLA_MAYBE_UNUSED typedef const float * &         EXP5;
        BSLA_MAYBE_UNUSED typedef const float * const &   EXP6;
        BSLA_MAYBE_UNUSED typedef const MyType&           EXP7;
        BSLA_MAYBE_UNUSED typedef const MyType&           EXP8;
        BSLA_MAYBE_UNUSED typedef MyType&                 EXP9;
        BSLA_MAYBE_UNUSED typedef MyType * const &        EXP10;
        BSLA_MAYBE_UNUSED typedef int * const &           EXP11;
        BSLA_MAYBE_UNUSED typedef int * const &           EXP12;


#if BSLS_COMPILERFEATURES_CPLUSPLUS>=201703L // `using` for readibility
        using bslmf::ForwardingRefType;
        ASSERT((bsl::is_same<ForwardingRefType<T1>::Type,  EXP1>::value));
        ASSERT((bsl::is_same<ForwardingRefType<T2>::Type,  EXP2>::value));
        ASSERT((bsl::is_same<ForwardingRefType<T3>::Type,  EXP3>::value));
        ASSERT((bsl::is_same<ForwardingRefType<T4>::Type,  EXP4>::value));
        ASSERT((bsl::is_same<ForwardingRefType<T5>::Type,  EXP5>::value));
        ASSERT((bsl::is_same<ForwardingRefType<T6>::Type,  EXP6>::value));
        ASSERT((bsl::is_same<ForwardingRefType<T7>::Type,  EXP7>::value));
        ASSERT((bsl::is_same<ForwardingRefType<T8>::Type,  EXP8>::value));
        ASSERT((bsl::is_same<ForwardingRefType<T9>::Type,  EXP9>::value));
        ASSERT((bsl::is_same<ForwardingRefType<T10>::Type, EXP10>::value));
        ASSERT((bsl::is_same<ForwardingRefType<T11>::Type, EXP11>::value));
        ASSERT((bsl::is_same<ForwardingRefType<T12>::Type, EXP12>::value));
#endif
    }
// ```


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

/// Return true if address of the specified `a` object matches the address
/// of the specified `b` object.  Unlike a direct test, this function can be
/// called even if `a` or `b` is an rvalue or rvalue reference.
template <class TYPE>
bool sameAddress(const TYPE& a, const TYPE& b)
{
    return &a == &b;
}

/// Return true if address of the specified `a` entity (of parameterized
/// `TYPE`)) matches the address of the specified `b` entity.  Note that
/// this second overload is necessary only for compilers like IBM xlC that
/// will not deduce a match for `const &` for function types and references.
template <class TYPE>
bool sameAddress(TYPE& a, TYPE& b)
{
    return &a == &b;
}

/// Test `forwardToTarget` when the specified `obj` is an rvalue, reporting
/// errors with the specified `lineNumber`.
template <class TYPE>
void testForwardToTargetVal(TYPE obj, int lineNumber)
{
    typedef typename bslmf::ForwardingRefType<TYPE>::Type       FwdType;
    typedef typename bslmf::ForwardingRefType<TYPE>::TargetType TargetType;

    ASSERTV(lineNumber, typeid(TYPE).name(),
           (bsl::is_same<TYPE,
                    typename bsl::remove_reference<TargetType>::type>::value ||
            bsl::is_same<const TYPE,
                    typename bsl::remove_reference<TargetType>::type>::value));

    FwdType fwdObj = obj;

    // For pass-by-value, compare original and final value.
    ASSERTV(lineNumber, typeid(TYPE).name(),
            obj ==
            bslmf::ForwardingRefTypeUtil<TYPE>::forwardToTarget(fwdObj));
}

/// Test `forwardToTarget` when the specified `obj` is an array, reporting
/// errors with the specified `lineNumber`.
template <class TYPE>
#if defined(BSLMF_FOWARDINGTYPE_WORK_AROUND_SUN_ARRAY_TESTS)
void testForwardToTargetArray(
    typename bsl::add_lvalue_reference<TYPE>::type obj, int lineNumber)
#else
void testForwardToTargetArray(TYPE obj, int lineNumber)
#endif
{
    typedef typename bslmf::ForwardingRefType<TYPE>::Type           FwdType;
    typedef typename bslmf::ForwardingRefTypeUtil<TYPE>::TargetType TargetType;

    typedef typename bsl::conditional<
        bslmf::MovableRefUtil::IsReference<TYPE>::value, TYPE,
        typename bslmf::MovableRefUtil::AddLvalueReference<TYPE>::type
        >::type ExpTargetType;

    ASSERTV(lineNumber, typeid(TargetType).name(),
            (bsl::is_same<ExpTargetType, TargetType>::value));

    FwdType    fwdObj    = obj;
    TargetType targetObj =
        bslmf::ForwardingRefTypeUtil<TYPE>::forwardToTarget(fwdObj);

    // For arrays, compare address of first element of original and final
    // arrays.
    ASSERTV(lineNumber, typeid(TYPE).name(),
            &obj[0] ==
            &bslmf::MovableRefUtil::access(targetObj)[0]);
}

/// Test `forwardToTarget` when the specified `ref` is a reference type,
/// reporting errors with the specified `lineNumber`.
template <class TYPE>
void testForwardToTargetRef(TYPE ref, int lineNumber)
{
    typedef typename bslmf::ForwardingRefType<TYPE>::Type FwdType;
    typedef typename bslmf::ForwardingRefTypeUtil<TYPE>::TargetType TargetType;

    ASSERTV(lineNumber, typeid(TYPE).name(),
            (bsl::is_same<TYPE, TargetType>::value));

    FwdType fwdRef = ref;

    // For pass-by-reference, compare addresses of original and final
    // references.
    ASSERTV(
        lineNumber,
        typeid(TYPE).name(),
        sameAddress(
            ref, bslmf::ForwardingRefTypeUtil<TYPE>::forwardToTarget(fwdRef)));
}

int main(int argc, char *argv[])
{
    int  test        = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose     = argc > 2;
    bool veryVerbose = argc > 3;

    (void) verbose;      // eliminate unused variable warning
    (void) veryVerbose;  // eliminate unused variable warning

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.

      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLES
        //
        // Concerns: The usage examples in the component doc compile and run
        //   as promised.
        //
        // Plan: Copy the usage example from the component header, replace
        //   `assert` with `ASSERT` and `main` with `usageExampleN` and verify
        //   that it compiles and runs successfully.
        //
        // Testing:
        //   USAGE EXAMPLES
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLES"
                            "\n==============\n");

        usageExample1();

      } break;

      case 4: {
        // --------------------------------------------------------------------
        // TESTING END-TO-END OVERLOADING
        //
        // Concerns:
        // 1. An argument of type `T` that is passed to one function as `T`,
        //    forwarded through a second function as
        //    `ForwardingRefType<T>::Type` and passed to a third (target)
        //    function by calling `ForwardingRefTypeUtil<T>::forwardToTarget()`
        //    will be seen by the target function as if the original argument
        //    had been passed directly to it, including selecting the correct
        //    overload and instantiation of the target function.
        // 2. Rvalue types are forwarded to the target function without
        //    cv-qualification, regardless of whether an rvalue type is
        //    primitive or of class type.
        // 3. Lvalue reference types are forwarded to the target with the
        //    original cv-qualification.
        // 4. Rvalue reference types (C++11 and newer) are forwarded to the
        //    target with the original cv-qualification.
        // 5. Sized array types are forwarded to the target with the original
        //    array size.
        // 6. array-of-unknown-bound types are forwarded to the target as
        //    references to arrays of unknown bound.
        //
        // Plan:
        // 1. For concern 1, create a small set of functor classes with
        //    several overloads of `operator()`.  Each overload verifies
        //    correct invocation by returning a different enumerated
        //    value.  Call each functor via an intermediary function that
        //    takes a `TP` argument and passes it to a second intermediary
        //    function taking a `ForwardingRefType<TP>::Type` argument which,
        //    in turn, calls the functor.  Verify that the functor returns the
        //    same value as would be returned if it were called directly.
        // 2. For concern 2, perform step 1 with a functor class whose
        //    `operator()` takes `const`, `volatile`, and 'const
        //    volatile' lvalue reference types (C++03 and before) or
        //    unqualified, `const`, `volatile`, and `const volatile` rvalue
        //    reference types (C++11 and newer).  Instantiate with a variety
        //    of primitive and non-primitive rvalue types of different
        //    cv-qualifications.
        // 3. For concern 3, repeat step 2, except instantiate with lvalue
        //    reference types.
        // 4. For concern 4, repeat step 2, except instantiate with rvalue
        //    reference types.
        // 5. For concerns 5 and 6, perform step 1 with a functor class whose
        //    `operator()` returns the size of the array on which it is
        //    called, or zero for pointer types.  Instantiate with a variety
        //    of array types and reference-to-array types.
        //
        // Testing:
        //   END-TO-END OVERLOADING
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING END-TO-END OVERLOADING"
                            "\n==============================\n");

        int     i = 5;
        Enum    e = e_VAL2;
        Struct  s(99);
        Union   u(98);
        Class   c(97);
        double  d = 1.23;
        double *p = &d;
        F      *f_p = func;
        Pm      m_p  = &Struct::d_data;
        Pmf     mf_p = &Class::two;
        Pmq     mf_q = &Class::value;

        char a[5]    = { '5', '4', '3', '2', '1' };
#if !defined(BSLMF_FORWARDINGREFTYPE_NO_ARRAY_OF_UNKNOWN_BOUND)
        char (&au)[] = reinterpret_cast<AU&>(a);
#endif

// Volatile rvalue types are not useful and have strange rules.  Do not test
// them.  Attempting to test volatile rvalues will not only complicate the test
// driver unnecessarily, it may actually hide real errors.
#define TEST_ENDTOEND_RVALUE(TP, v) {                                         \
            typedef TP T;                                                     \
            typedef bsl::add_const<T>::type CT;                               \
            CvRefMatch<T> target;                                             \
            ASSERT(testEndToEnd<T>(v, target)   == target(toRvalue<T>(v)));   \
            ASSERT(testEndToEnd<CT>(v, target)  == target(toRvalue<CT>(v)));  \
        }

        if (veryVerbose) printf("rvalue types\n");
#if 0 // For support of `bdlf_bind`, basic types are forwarded as const refs
        TEST_ENDTOEND_RVALUE(int      , i);
        TEST_ENDTOEND_RVALUE(Enum     , e);
        TEST_ENDTOEND_RVALUE(double   , d);
        TEST_ENDTOEND_RVALUE(double * , p);
        TEST_ENDTOEND_RVALUE(F      * , f_p);
        TEST_ENDTOEND_RVALUE(Pm       , m_p);
        TEST_ENDTOEND_RVALUE(Pmf      , mf_p);
# if !defined(BSLMF_FOWARDINGTYPE_NO_SUPPORT_FOR_POINTER_TO_CV_MEMBER_FUNCTION)
        TEST_ENDTOEND_RVALUE(Pmq      , mf_q);
# endif // !BSLMF_FOWARDINGTYPE_NO_SUPPORT_FOR_POINTER_TO_CV_MEMBER_FUNCTION
#endif // 0
        TEST_ENDTOEND_RVALUE(Struct   , s);
        TEST_ENDTOEND_RVALUE(Union    , u);
        TEST_ENDTOEND_RVALUE(Class    , c);

#undef TEST_ENDTOEND_RVALUE

#define TEST_ENDTOEND_LVALUE_REF(TP, v) {                                     \
            typedef TP T;                                                     \
            typedef bsl::add_const<T>::type CT;                               \
            typedef bsl::add_volatile<T>::type VT;                            \
            typedef bsl::add_cv<T>::type CVT;                                 \
            CT& cv = v;                                                       \
            VT& vv = v;                                                       \
            CVT& cvv = v;                                                     \
            CvRefMatch<T> target;                                             \
            ASSERT(testEndToEnd<T&>(v, target)     == target(v));             \
            ASSERT(testEndToEnd<CT&>(cv, target)   == target(cv));            \
            ASSERT(testEndToEnd<VT&>(vv, target)   == target(vv));            \
            ASSERT(testEndToEnd<CVT&>(cvv, target) == target(cvv));           \
        }

        if (veryVerbose) printf("lvalue reference types\n");
        TEST_ENDTOEND_LVALUE_REF(int      , i);
        TEST_ENDTOEND_LVALUE_REF(Enum     , e);
        TEST_ENDTOEND_LVALUE_REF(Struct   , s);
        TEST_ENDTOEND_LVALUE_REF(Union    , u);
        TEST_ENDTOEND_LVALUE_REF(Class    , c);
        TEST_ENDTOEND_LVALUE_REF(double   , d);
        TEST_ENDTOEND_LVALUE_REF(double * , p);
        TEST_ENDTOEND_LVALUE_REF(F      * , f_p);
        TEST_ENDTOEND_LVALUE_REF(Pm       , m_p);
        TEST_ENDTOEND_LVALUE_REF(Pmf      , mf_p);
#if !defined(BSLMF_FOWARDINGTYPE_NO_SUPPORT_FOR_POINTER_TO_CV_MEMBER_FUNCTION)
        TEST_ENDTOEND_LVALUE_REF(Pmq      , mf_q);
#endif

#undef TEST_ENDTOEND_LVALUE_REF

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

#define TEST_ENDTOEND_RVALUE_REF(TP, v) {                                     \
            typedef TP T;                                                     \
            typedef bsl::add_const<T>::type CT;                               \
            typedef bsl::add_volatile<T>::type VT;                            \
            typedef bsl::add_cv<T>::type CVT;                                 \
            CvRefMatch<T> target;                                             \
            ASSERT(testEndToEnd<T&&>(std::move(v), target) ==                 \
                   target(std::move(v)));                                     \
            ASSERT(testEndToEnd<CT&&>(static_cast<CT&&>(v), target) ==        \
                   target(static_cast<CT&&>(v)));                             \
            ASSERT(testEndToEnd<VT&&>(static_cast<VT&&>(v), target) ==        \
                   target(static_cast<VT&&>(v)));                             \
            ASSERT(testEndToEnd<CVT&&>(static_cast<CVT&&>(v), target) ==      \
                   target(static_cast<CVT&&>(v)));                            \
        }

        if (veryVerbose) printf("rvalue reference types\n");
        TEST_ENDTOEND_RVALUE_REF(int      , i);
        TEST_ENDTOEND_RVALUE_REF(Enum     , e);
        TEST_ENDTOEND_RVALUE_REF(Struct   , s);
        TEST_ENDTOEND_RVALUE_REF(Union    , u);
        TEST_ENDTOEND_RVALUE_REF(Class    , c);
        TEST_ENDTOEND_RVALUE_REF(double   , d);
        TEST_ENDTOEND_RVALUE_REF(double * , p);
        TEST_ENDTOEND_RVALUE_REF(F      * , f_p);
        TEST_ENDTOEND_RVALUE_REF(Pm       , m_p);
        TEST_ENDTOEND_RVALUE_REF(Pmf      , mf_p);
#if !defined(BSLMF_FOWARDINGTYPE_NO_SUPPORT_FOR_POINTER_TO_CV_MEMBER_FUNCTION)
        TEST_ENDTOEND_RVALUE_REF(Pmq      , mf_q);
#endif


#undef TEST_ENDTOEND_RVALUE_REF

#endif // defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

        if (veryVerbose) printf("array types\n");
#if defined(BSLMF_FOWARDINGTYPE_WORK_AROUND_SUN_ARRAY_TESTS)
#define TEST_ENDTOEND_ARRAY(TP, a, exp) {                                     \
            CvArrayMatch<char> am;                                            \
            ASSERT(testEndToEndArray<TP>(a, am)   == exp);                    \
            CvArrayMatch<const char> acm;                                     \
            ASSERT(testEndToEndArray<bsl::add_const<TP>::type>(a, acm)== exp);\
            CvArrayMatch<volatile char> avm;                                  \
            ASSERT(testEndToEndArray<bsl::add_volatile<TP>::type>(a, avm)     \
                                                                      == exp);\
            CvArrayMatch<const volatile char> acvm;                           \
            ASSERT(testEndToEndArray<bsl::add_cv<TP>::type>(a, acvm)  == exp);\
        }
#else
#define TEST_ENDTOEND_ARRAY(TP, a, exp) {                                     \
            CvArrayMatch<char> am;                                            \
            ASSERT(testEndToEnd<TP>(a, am)   == exp);                         \
            CvArrayMatch<const char> acm;                                     \
            ASSERT(testEndToEnd<bsl::add_const<TP>::type>(a, acm)  == exp);   \
            CvArrayMatch<volatile char> avm;                                  \
            ASSERT(testEndToEnd<bsl::add_volatile<TP>::type>(a, avm)  == exp);\
            CvArrayMatch<const volatile char> acvm;                           \
            ASSERT(testEndToEnd<bsl::add_cv<TP>::type>(a, acvm) == exp);      \
        }
#endif

        TEST_ENDTOEND_ARRAY(char[5], a,    5);
        TEST_ENDTOEND_ARRAY(char(&)[5], a, 5);
#if !defined(BSLMF_FORWARDINGREFTYPE_NO_ARRAY_OF_UNKNOWN_BOUND)
        TEST_ENDTOEND_ARRAY(char[], au,    0);
        TEST_ENDTOEND_ARRAY(char(&)[], au, 0);
# if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)                 \
  &&!defined(BSLMF_FORWARDINGREFTYPE_NO_ARRAY_DECAY_TO_RVALUE_REF)
        TEST_ENDTOEND_ARRAY(char *&&, au,  0);
# endif
#endif

        if (veryVerbose) printf("function types\n");
        FuncMatch<F> fm;
        ASSERT(testEndToEnd<F*>(f_p,  fm) == k_FUNC_POINTER);
        ASSERT(testEndToEnd<F&>(func, fm) == k_FUNC_REFERENCE);
      } break;

      case 3: {
        // --------------------------------------------------------------------
        // TESTING `bslmf::ForwardingRefTypeUtil`
        //
        // Concerns:
        // 1. For types that are not references, arrays, or functions,
        //    `ForwardingRefTypeUtil<TYPE>::TargetType` is similar to
        //    `TYPE` except that `TargetType` might be a const reference
        //    (C++03) or rvalue reference (C++11+).  An object of `TYPE`
        //    converted to `ForwardingRefType<TYPE>::Type` and then forwarded
        //    using 'ForwardingRefTypeUtil<TYPE>::forwardToTarget(), will yield
        //    a value equal to the original object.
        // 2. For array types of (known or unknown) size,
        //    `ForwardingRefTypeUtil<TYPE>::TargetType` yields a reference to
        //    `TYPE`.  An array object of `TYPE` converted to
        //    `ForwardingRefType<TYPE>::Type` then forwarded using
        //    'ForwardingRefTypeUtil<TYPE>::forwardToTarget() will yield a
        //    reference to the original array.
        // 3. For reference types, `ForwardingRefTypeUtil<TYPE>::TargetType`
        //    yields `TYPE`.  A reference of `TYPE` converted to
        //    `ForwardingRefType<TYPE>::Type` then forwarded using
        //    'ForwardingRefTypeUtil<TYPE>::forwardToTarget() will yield a
        //    a reference identical to the original.
        // 4. All of the above concerns apply when `TYPE` is
        //    cv-qualified.  Note that passing volatile-qualified objects by
        //    value or by rvalue-reference does not really happen in real code
        //    and is not supported by this component.
        // 5. For function types, `ForwardingRefTypeUtil<TYPE>::TargetType`
        //    yields `TYPE&`.  A function converted to
        //    `ForwardingRefType<TYPE>::Type` then forwarded using
        //    'ForwardingRefTypeUtil<TYPE>::forwardToTarget() will yield a a
        //    reference to the original function.
        //
        // Plan:
        // 1. For concern 1, implement a function template,
        //    `testForwardToTargetVal` that can be instantiated with a `TYPE`
        //    and that takes an argument `obj` of `TYPE`.  Instantiated on a
        //    variety of basic and non-basic types, `testForwardToTargetVal`
        //    performs the following operations:
        //    a Verify that `TargetType` is the expected transformation of
        //      `TYPE`.
        //    b Initialize a temporary variable of type
        //      `ForwardingRefType<TYPE>::Type` using 'obj.
        //    c Call `forwardToTarget` on the temporary variable and verify
        //      that the resulting object compares equal to `obj`.
        // 2. For concern 2, implement a function template,
        //    `testForwardToTargetArray`, that can be instantiated with an
        //    an array `TYPE` (or reference-to-array `TYPE`) and that takes
        //    an argument `obj` of `TYPE`.  Instantiated on a variety of array
        //    types of known and unknown size as well a lvalue and rvalues to
        //    such types, `testForwardToTargetArray` performs the following
        //    operations:
        //    a Verify that `TargetType` is the expected transformation of
        //      `TYPE`.
        //    b Initialize a temporary variable of type
        //      `ForwardingRefType<TYPE>::Type` using 'obj.
        //    c Call `forwardToTarget` on the temporary variable and verify
        //      that the resulting object has the same address as `obj`.
        // 3. For concern 3, implement a function template,
        //    `testForwardToTargetRef` that can be instantiated with a
        //    reference `TYPE` and that takes an argument `ref` of `TYPE`.
        //    Instantiated on a variety of lvalue and rvalue reference types,
        //    `testForwardToTargetRef`, performs the following operations:
        //    a Verify that `TargetType` is the expected transformation of
        //      `TYPE`.
        //    b Initialize a temporary variable of type
        //      `ForwardingRefType<TYPE>::Type` using `obj`.
        //    c Call `forwardToTarget` on the temporary variable and verify
        //      that the returned reference has the same address as `obj`.
        // 4. For concern 4, instantiate the templates defined in the previous
        //    steps using cv-qualified template parameters.
        // 5. For concern 5, instantiate the template with a function
        //    type, `F` and then:
        //    a Verify that `TargetType` is `F&`.
        //    b Initialize a temporary variable of type
        //      `ForwardingRefType<F>::Type` using `func`.
        //    c Call `forwardToTarget` on the temporary variable and verify
        //      that the returned reference has the same address as `func`.
        //
        // Testing:
        //   ForwardingRefTypeUtil<TYPE>::TargetType
        //   ForwardingRefTypeUtil<TYPE>::forwardToTarget(v)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `bslmf::ForwardingRefTypeUtil`"
                            "\n===================================\n");

        Enum    e = e_VAL2;
        Struct  s(99);
        Union   u(98);
        Class   c(97);
        double  d = 1.23;
        double *p = &d;
        char    a[5] = { '5', '4', '3', '2', '1' };
#if !defined(BSLMF_FORWARDINGREFTYPE_NO_ARRAY_OF_UNKNOWN_BOUND)
        char  (&au)[] = reinterpret_cast<AU&>(a);
#endif
        F      *f_p = func;
        Pm      m_p  = &Struct::d_data;
        Pmf     mf_p = &Class::two;
        Pmq     mf_q = &Class::value;

        testForwardToTargetVal<Enum    >(e, L_);
        testForwardToTargetVal<Struct  >(s, L_);
        testForwardToTargetVal<Union   >(u, L_);
        testForwardToTargetVal<Class   >(c, L_);
        testForwardToTargetVal<double  >(d, L_);
        testForwardToTargetVal<double *>(p, L_);
        testForwardToTargetVal<PF      >(f_p, L_);
        testForwardToTargetVal<Pm      >(m_p, L_);
        testForwardToTargetVal<Pmf     >(mf_p, L_);
#if !defined(BSLMF_FOWARDINGTYPE_NO_SUPPORT_FOR_POINTER_TO_CV_MEMBER_FUNCTION)
        testForwardToTargetVal<Pmq     >(mf_q, L_);
#endif

        testForwardToTargetVal<Enum    const>(e, L_);
        testForwardToTargetVal<Struct  const>(s, L_);
        testForwardToTargetVal<Union   const>(u, L_);
        testForwardToTargetVal<Class   const>(c, L_);
        testForwardToTargetVal<double  const>(d, L_);
        testForwardToTargetVal<double *const>(p, L_);
        testForwardToTargetVal<PF      const>(f_p, L_);
        testForwardToTargetVal<Pm      const>(m_p, L_);
        testForwardToTargetVal<Pmf     const>(mf_p, L_);
#if !defined(BSLMF_FOWARDINGTYPE_NO_SUPPORT_FOR_POINTER_TO_CV_MEMBER_FUNCTION)
        testForwardToTargetVal<Pmq     const>(mf_q, L_);
#endif

        // Do not test volatile rvalues of class types.  They have no real use
        // and require strange copy constructors and comparison operators to
        // test correctly.
        testForwardToTargetVal<Enum    volatile>(e, L_);
        testForwardToTargetVal<double  volatile>(d, L_);
        testForwardToTargetVal<double *volatile>(p, L_);
        testForwardToTargetVal<PF      volatile>(f_p, L_);
        testForwardToTargetVal<Pm      volatile>(m_p, L_);
        testForwardToTargetVal<Pmf     volatile>(mf_p, L_);  // fails on
                                                             // Oracle
                                                             // CC 12.4

#if !defined(BSLMF_FOWARDINGTYPE_NO_SUPPORT_FOR_POINTER_TO_CV_MEMBER_FUNCTION)
        testForwardToTargetVal<Pmq     volatile>(mf_q, L_);
#endif

#if !defined(BSLMF_FOWARDINGTYPE_WORK_AROUND_SUN_ARRAY_TESTS)
        testForwardToTargetVal<A       volatile>(a, L_);
#if !defined(BSLMF_FORWARDINGREFTYPE_NO_ARRAY_OF_UNKNOWN_BOUND)
        testForwardToTargetVal<AU      volatile>(au, L_);
#endif
#endif  // Disable testing arrays on broken compilers


        testForwardToTargetArray<A           >(a, L_);
        testForwardToTargetArray<A  const    >(a, L_);
        testForwardToTargetArray<A          &>(a, L_);
        testForwardToTargetArray<A  const   &>(a, L_);
        testForwardToTargetArray<A  volatile&>(a, L_);
#if !defined(BSLMF_FORWARDINGREFTYPE_NO_ARRAY_OF_UNKNOWN_BOUND)
        testForwardToTargetArray<AU          >(au, L_);
        testForwardToTargetArray<AU const    >(au, L_);
        testForwardToTargetArray<AU         &>(au, L_);
        testForwardToTargetArray<AU const   &>(au, L_);
        testForwardToTargetArray<AU volatile&>(au, L_);

# if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)   \
  &&!defined(BSLMF_FORWARDINGREFTYPE_NO_ARRAY_DECAY_TO_RVALUE_REF)
        testForwardToTargetArray<A          &&>(std::move(a), L_);
        testForwardToTargetArray<A  const   &&>(std::move(a), L_);
        testForwardToTargetArray<AU         &&>(std::move(au), L_);
        testForwardToTargetArray<AU const   &&>(std::move(au), L_);
# endif
#endif

        testForwardToTargetRef<Enum    &>(e, L_);
        testForwardToTargetRef<Struct  &>(s, L_);
        testForwardToTargetRef<Union   &>(u, L_);
        testForwardToTargetRef<Class   &>(c, L_);
        testForwardToTargetRef<double  &>(d, L_);
        testForwardToTargetRef<double *&>(p, L_);
        testForwardToTargetRef<F       &>(func, L_);
        testForwardToTargetRef<Fi      &>(funcI, L_);
        testForwardToTargetRef<FRi     &>(funcRi, L_);
        testForwardToTargetRef<PF      &>(f_p, L_);
        testForwardToTargetRef<Pm      &>(m_p, L_);
        testForwardToTargetRef<Pmf     &>(mf_p, L_);
#if !defined(BSLMF_FOWARDINGTYPE_NO_SUPPORT_FOR_POINTER_TO_CV_MEMBER_FUNCTION)
        testForwardToTargetRef<Pmq     &>(mf_q, L_);
#endif

        testForwardToTargetRef<Enum    const&>(e, L_);
        testForwardToTargetRef<Struct  const&>(s, L_);
        testForwardToTargetRef<Union   const&>(u, L_);
        testForwardToTargetRef<Class   const&>(c, L_);
        testForwardToTargetRef<double  const&>(d, L_);
        testForwardToTargetRef<double *const&>(p, L_);
        testForwardToTargetRef<PF      const&>(f_p, L_);
        testForwardToTargetRef<Pm      const&>(m_p, L_);
        testForwardToTargetRef<Pmf     const&>(mf_p, L_);
#if !defined(BSLMF_FOWARDINGTYPE_NO_SUPPORT_FOR_POINTER_TO_CV_MEMBER_FUNCTION)
        testForwardToTargetRef<Pmq     const&>(mf_q, L_);
#endif

        testForwardToTargetRef<Enum    volatile&>(e, L_);
        testForwardToTargetRef<Struct  volatile&>(s, L_);
        testForwardToTargetRef<Union   volatile&>(u, L_);
        testForwardToTargetRef<Class   volatile&>(c, L_);
        testForwardToTargetRef<double  volatile&>(d, L_);
        testForwardToTargetRef<double *volatile&>(p, L_);
        testForwardToTargetRef<PF      volatile&>(f_p, L_);
        testForwardToTargetRef<Pm      volatile&>(m_p, L_);
        testForwardToTargetRef<Pmf     volatile&>(mf_p, L_);
#if !defined(BSLMF_FOWARDINGTYPE_NO_SUPPORT_FOR_POINTER_TO_CV_MEMBER_FUNCTION)
        testForwardToTargetRef<Pmq     volatile&>(mf_q, L_);
#endif

        testForwardToTargetRef<Enum    const volatile&>(e, L_);
        testForwardToTargetRef<Struct  const volatile&>(s, L_);
        testForwardToTargetRef<Union   const volatile&>(u, L_);
        testForwardToTargetRef<Class   const volatile&>(c, L_);
        testForwardToTargetRef<double  const volatile&>(d, L_);
        testForwardToTargetRef<double *const volatile&>(p, L_);
        testForwardToTargetRef<PF      const volatile&>(f_p, L_);
        testForwardToTargetRef<Pm      const volatile&>(m_p, L_);
        testForwardToTargetRef<Pmf     const volatile&>(mf_p, L_);
#if !defined(BSLMF_FOWARDINGTYPE_NO_SUPPORT_FOR_POINTER_TO_CV_MEMBER_FUNCTION)
        testForwardToTargetRef<Pmq     const volatile&>(mf_q, L_);
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        testForwardToTargetRef<Struct  &&>(std::move(s), L_);
        testForwardToTargetRef<Union   &&>(std::move(u), L_);
        testForwardToTargetRef<Class   &&>(std::move(c), L_);
#if !(defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION <= 1800)
        // The following 6 tests fail for MS Visual C++ (tested up to VC 2013).
        // Suspect the optimizer is creating a temporary, rather than truly
        // passing by reference, when given a fundamental/primitive type.
        testForwardToTargetRef<Enum    &&>(std::move(e), L_);
        testForwardToTargetRef<double  &&>(std::move(d), L_);
        testForwardToTargetRef<double *&&>(std::move(p), L_);
        testForwardToTargetRef<PF      &&>(std::move(f_p), L_);
        testForwardToTargetRef<Pm      &&>(std::move(m_p), L_);
        testForwardToTargetRef<Pmf     &&>(std::move(mf_p), L_);
# if !defined(BSLMF_FOWARDINGTYPE_NO_SUPPORT_FOR_POINTER_TO_CV_MEMBER_FUNCTION)
        testForwardToTargetRef<Pmq     &&>(std::move(mf_q), L_);
# endif
#endif

        testForwardToTargetRef<Enum     const&&>(std::move(e), L_);
        testForwardToTargetRef<Struct   const&&>(std::move(s), L_);
        testForwardToTargetRef<Union    const&&>(std::move(u), L_);
        testForwardToTargetRef<Class    const&&>(std::move(c), L_);
        testForwardToTargetRef<double   const&&>(std::move(d), L_);
        testForwardToTargetRef<double * const&&>(std::move(p), L_);
        testForwardToTargetRef<PF       const&&>(std::move(f_p), L_);
        testForwardToTargetRef<Pm       const&&>(std::move(m_p), L_);
        testForwardToTargetRef<Pmf      const&&>(std::move(mf_p), L_);
#if !defined(BSLMF_FOWARDINGTYPE_NO_SUPPORT_FOR_POINTER_TO_CV_MEMBER_FUNCTION)
        testForwardToTargetRef<Pmq      const&&>(std::move(mf_q), L_);
#endif

        // Do not test volatile rvalue references.  They have no real uses and
        // would require distortions in the test that could result in missing
        // actual errors.
#endif

        // Test function type
        {
            typedef bslmf::ForwardingRefType<F>::Type FwdType;
            typedef bslmf::ForwardingRefTypeUtil<F>::TargetType TargetType;

            ASSERT_SAME(F&, TargetType);

            FwdType fwdRef = func;
            ASSERT(&func ==
                   bslmf::ForwardingRefTypeUtil<F>::forwardToTarget(fwdRef));
        }
      } break;

      case 2: {
        // --------------------------------------------------------------------
        // TESTING `bslmf::ForwardingRefType<TYPE>::Type`
        //
        // Concerns:
        // 1. The forwarding type for "*basic* type cvq `TP`" is
        //    `const vq&`, where `vq` is `volatile` if `TP` is
        //    volatile-qualified and is empty otherwise.
        // 2. The forwarding type for "*class* *or* *union* type cvq `TP`" is
        //    `const vq TP&`, where `vq` is `volatile` if `TP` is
        //    volatile-qualified and is empty otherwise.
        // 3. The forwarding type for "function of type `F`" or "reference to
        //    function of type `F`" is "reference to function of type
        //    `F`".  The forwarding type for "pointer to function of type `F`"
        //    is, `F *const&`.
        // 4. The forwarding type for "array of cvq `TP`" or "(lvalue or
        //    rvalue) reference to array of cvq `TP`" is "cvq `TP*`",
        //    regardless of whether the array size is known.
        // 5. The forwarding type for "lvalue reference to type cvq `TP`" is
        //    the same "lvalue reference to cvq `TP`, for non-function and
        //    non-array types.
        // 6. (C++11 and newer only) The forwarding type for "rvalue reference
        //    to cvq type `TP`" for non-function and non-array `TP` is 'const
        //    vq TP&`, where `vq' is `volatile` if `TP` is volatile-qualified
        //    and `vq` is empty otherwise.
        //
        // Test Plan:
        // 1. For concern 1, instantiate `ForwardingRefType` for fundamental,
        //    pointer, and enumeration types, both cv-qualified and
        //    unqualified, and verify  that the resulting `Type` member is
        //    the parameter type without cv qualification.
        // 2. For concern 2, instantiate `ForwardingRefType` for class and
        //    union type, both unqualified and cv-qualified, and verify that
        //    the resulting `Type` member is the expected const lvalue
        //    reference type.
        // 3. For concern 3, instantiate `ForwardingRefType` for a small number
        //    of function, reference-to-function, and pointer-to-function
        //    parameters and verify that the resulting `Type` member is the
        //    expected type.
        // 3. For concern 4, instantiate `ForwardingRefType` for a small number
        //    of array types, lvalue-reference-to-array types, and
        //    rvalue-reference-to-array types, both sized and unsized, and
        //    both cv-qualified and unqualified, and verify that the resulting
        //    `Type` member is the expected pointer type.
        // 5. For concern 5, instantiate `ForwardingRefType` for *lvalue*
        //    reference to fundamental, pointer, enumeration, class, and union
        //    types, both cv-qualified and unqualified, and verify that the
        //    resulting `Type` member is the same as the parameter type.
        // 6. For concern 6, instantiate `ForwardingRefType` for *rvalue*
        //    reference to fundamental, pointer, enumeration, class, and union
        //    types, both cv-qualified and unqualified, and verify that the
        //    resulting `Type` member is the expected const lvalue reference
        //    type.
        //
        // Testing:
        //   ForwardingRefType<TYPE>::Type
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING `bslmf::ForwardingRefType<TYPE>::Type`"
                            "\n===========================================\n");

#define TEST_FWD_TYPE(T, EXP) \
                   ASSERT_SAME(bslmf::ForwardingRefType<T>::Type, EXP)
#define TEST_FWD_TYPE_UNCHANGED(T) TEST_FWD_TYPE(T, T)

        if (veryVerbose) printf("Basic types\n");

        TEST_FWD_TYPE(int                     , const int&);
        TEST_FWD_TYPE(int *                   , int * const &);
        TEST_FWD_TYPE(int Class::*            , int Class::* const &);
        TEST_FWD_TYPE(int (*)(float)          , int (* const &)(float));
        TEST_FWD_TYPE(int (Class::*)(char)    , int (Class::* const &)(char));
        TEST_FWD_TYPE(Enum                    , const Enum& );

        TEST_FWD_TYPE(const int               , const int &);
        TEST_FWD_TYPE(int *const              , int * const &);
        TEST_FWD_TYPE(const int *             , const int * const &);
        TEST_FWD_TYPE(int Class::* const      , int Class::* const &);
        TEST_FWD_TYPE(int (* const)(float)    , int (* const &)(float));
        TEST_FWD_TYPE(int (Class::* const)(char)
                                              , int (Class::* const &)(char));
        TEST_FWD_TYPE(const Enum              , const Enum &);

        TEST_FWD_TYPE(volatile int            , const volatile int &);
        TEST_FWD_TYPE(int *volatile           , int * volatile const &);
        TEST_FWD_TYPE(volatile int *          , volatile int * const &);
        TEST_FWD_TYPE(int Class::* volatile   , int Class::* const volatile &);
        TEST_FWD_TYPE(int (* volatile)(float) , int (*const volatile&)(float));
        TEST_FWD_TYPE(volatile Enum           , const volatile Enum &);

        TEST_FWD_TYPE(const volatile int      , const volatile int &);
        TEST_FWD_TYPE(int *const volatile     , int * const volatile &);
        TEST_FWD_TYPE(const volatile int *    , const volatile int * const &);
        TEST_FWD_TYPE(int Class::* const volatile
                                              , int Class::* const volatile&);
        TEST_FWD_TYPE(const volatile Enum     , const volatile Enum &);

        if (veryVerbose) printf("Class and union types\n");

        TEST_FWD_TYPE(Class                 , const Class&);
        TEST_FWD_TYPE(Struct                , const Struct&);
        TEST_FWD_TYPE(Union                 , const Union&);

        TEST_FWD_TYPE(const Class           , const Class&);
        TEST_FWD_TYPE(const Struct          , const Struct&);
        TEST_FWD_TYPE(const Union           , const Union&);

        TEST_FWD_TYPE(volatile Class        , const volatile Class&);
        TEST_FWD_TYPE(volatile Struct       , const volatile Struct&);
        TEST_FWD_TYPE(volatile Union        , const volatile Union&);

        TEST_FWD_TYPE(const volatile Class  , const volatile Class&);
        TEST_FWD_TYPE(const volatile Struct , const volatile Struct&);
        TEST_FWD_TYPE(const volatile Union  , const volatile Union&);

        if (veryVerbose) printf("Function types\n");

        TEST_FWD_TYPE(void()        , void(&)());
        TEST_FWD_TYPE(int(int)      , int(&)(int));
        TEST_FWD_TYPE(void(int&)    , void(&)(int&));

        TEST_FWD_TYPE(void(&)()     , void(&)());
        TEST_FWD_TYPE(int(&)(int)   , int(&)(int));
        TEST_FWD_TYPE(void(&)(int&) , void(&)(int&));

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        TEST_FWD_TYPE(void(&&)()    , void(&)());
        TEST_FWD_TYPE(int(&&)(int)  , int(&)(int));
        TEST_FWD_TYPE(void(&&)(int&), void(&)(int&));
#endif

        TEST_FWD_TYPE(void(*)()     , void(* const &)());
        TEST_FWD_TYPE(int(*)(int)   , int(* const &)(int));
        TEST_FWD_TYPE(void(*)(int&) , void(* const &)(int&));

        if (veryVerbose) printf("Array types\n");

        TEST_FWD_TYPE(int[5]                   , int* const &                 );
        TEST_FWD_TYPE(int*[5]                  , int**const &                 );
        TEST_FWD_TYPE(int[5][6]                , int(*const &)[6]             );
        TEST_FWD_TYPE(int(&)[5]                , int(&)[5]                    );
        TEST_FWD_TYPE(int *const(&)[5]         , int *const(&)[5]             );
        TEST_FWD_TYPE(int(&)[5][6]             , int(&)[5][6]                 );
        TEST_FWD_TYPE(int *const[5]            , int *const * const &         );
        TEST_FWD_TYPE(const int[5][6]          , const int(* const &)[6]      );
        TEST_FWD_TYPE(const int(&)[5]          , const int(&)[5]              );
        TEST_FWD_TYPE(volatile int[5]          , volatile int* const &        );
        TEST_FWD_TYPE(volatile int[5][6]       , volatile int(* const &)[6]   );
        TEST_FWD_TYPE(volatile int(&)[5]       , volatile int(&)[5]           );
        TEST_FWD_TYPE(const volatile int[5]    , const volatile int* const &  );
        TEST_FWD_TYPE(const volatile int[5][6] ,
                                              const volatile int(* const &)[6]);
        TEST_FWD_TYPE(const volatile int(&)[5] , const volatile int(&)[5]     );
#if !defined(BSLMF_FORWARDINGREFTYPE_NO_ARRAY_OF_UNKNOWN_BOUND)
        TEST_FWD_TYPE(Class[]                  , Class* const &               );
        TEST_FWD_TYPE(Struct[][6]              , Struct(* const &)[6]         );
        TEST_FWD_TYPE(Class(&)[]               , Class(&)[]                   );
        TEST_FWD_TYPE(Struct(&)[][6]           , Struct(&)[][6]               );
        TEST_FWD_TYPE(const int[]              , const int* const &           );
        TEST_FWD_TYPE(const int[][6]           , const int(* const &)[6]      );
        TEST_FWD_TYPE(volatile int[]           , volatile int* const &        );
        TEST_FWD_TYPE(volatile int[][6]        , volatile int(* const &)[6]   );
        TEST_FWD_TYPE(const volatile int[]     , const volatile int* const &  );
        TEST_FWD_TYPE(const volatile int[][6]  ,
                                              const volatile int(* const &)[6]);
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        TEST_FWD_TYPE(int *const(&&)[5]        , int *const(&)[5]        );
        TEST_FWD_TYPE(int(&&)[5]               , const int(&)[5]         );
        TEST_FWD_TYPE(int(&&)[5][6]            , const int(&)[5][6]      );
# if !defined(BSLMF_FORWARDINGREFTYPE_NO_ARRAY_OF_UNKNOWN_BOUND)
        TEST_FWD_TYPE(Class(&&)[]              , const Class(&)[]        );
        TEST_FWD_TYPE(Struct(&&)[][6]          , const Struct(&)[][6]    );
# endif
        TEST_FWD_TYPE(const int(&&)[5]         , const int(&)[5]         );
        TEST_FWD_TYPE(volatile int(&&)[5]      , const volatile int(&)[5]);
        TEST_FWD_TYPE(const volatile int(&&)[5], const volatile int(&)[5]);
#endif

        if (veryVerbose) printf("Lvalue references\n");

        TEST_FWD_TYPE_UNCHANGED(int&                        );
        TEST_FWD_TYPE_UNCHANGED(int *&                      );
        TEST_FWD_TYPE_UNCHANGED(int Class::*&               );
        TEST_FWD_TYPE_UNCHANGED(int (*&)(float)             );
        TEST_FWD_TYPE_UNCHANGED(int (Class::*&)(char)       );
        TEST_FWD_TYPE_UNCHANGED(Enum&                       );
        TEST_FWD_TYPE_UNCHANGED(Class&                      );
        TEST_FWD_TYPE_UNCHANGED(Struct&                     );
        TEST_FWD_TYPE_UNCHANGED(Union&                      );

        TEST_FWD_TYPE_UNCHANGED(const int&                  );
        TEST_FWD_TYPE_UNCHANGED(int *const&                 );
        TEST_FWD_TYPE_UNCHANGED(const int *&                );
        TEST_FWD_TYPE_UNCHANGED(int Class::* const&         );
        TEST_FWD_TYPE_UNCHANGED(int (* const&)(float)       );
        TEST_FWD_TYPE_UNCHANGED(int (Class::* const&)(char) );
        TEST_FWD_TYPE_UNCHANGED(const Enum&                 );
        TEST_FWD_TYPE_UNCHANGED(const Class&                );
        TEST_FWD_TYPE_UNCHANGED(const Struct&               );
        TEST_FWD_TYPE_UNCHANGED(const Union&                );

        TEST_FWD_TYPE_UNCHANGED(volatile int&               );
        TEST_FWD_TYPE_UNCHANGED(int *volatile&              );
        TEST_FWD_TYPE_UNCHANGED(volatile int *&             );
        TEST_FWD_TYPE_UNCHANGED(int Class::* volatile&      );
        TEST_FWD_TYPE_UNCHANGED(int (* volatile&)(float)    );
        TEST_FWD_TYPE_UNCHANGED(volatile Enum&              );
        TEST_FWD_TYPE_UNCHANGED(volatile Class&             );
        TEST_FWD_TYPE_UNCHANGED(volatile Struct&            );
        TEST_FWD_TYPE_UNCHANGED(volatile Union&             );

        TEST_FWD_TYPE_UNCHANGED(const volatile int&         );
        TEST_FWD_TYPE_UNCHANGED(int *const volatile&        );
        TEST_FWD_TYPE_UNCHANGED(const volatile int *&       );
        TEST_FWD_TYPE_UNCHANGED(int Class::* const volatile&);
        TEST_FWD_TYPE_UNCHANGED(const volatile Enum&        );
        TEST_FWD_TYPE_UNCHANGED(const volatile Class&       );
        TEST_FWD_TYPE_UNCHANGED(const volatile Struct&      );
        TEST_FWD_TYPE_UNCHANGED(const volatile Union&       );

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        if (veryVerbose) printf("Rvalue references\n");

        TEST_FWD_TYPE(int&&                  , const int&);
        TEST_FWD_TYPE(int *&&                , int *const &);
        TEST_FWD_TYPE(const int *&&          , const int *const &);
        TEST_FWD_TYPE(volatile int *&&       , volatile int *const &);
        TEST_FWD_TYPE(const volatile int *&& , const volatile int *const &);
        TEST_FWD_TYPE(int Class::*&&         , int Class::* const&);
        TEST_FWD_TYPE(int (*&&)(float)       , int (*const &)(float));
        TEST_FWD_TYPE(int (Class::*&&)(char) , int (Class::*const &)(char));
        TEST_FWD_TYPE(Enum&&                 , const Enum&);
        TEST_FWD_TYPE(Class&&                , const Class&);
        TEST_FWD_TYPE(Struct&&               , const Struct&);
        TEST_FWD_TYPE(Union&&                , const Union&);

        TEST_FWD_TYPE(const int&&            , const int&);
        TEST_FWD_TYPE(int *const&&           , int *const &);
        TEST_FWD_TYPE(const int *const &&    , const int *const &);
        TEST_FWD_TYPE(int Class::* const&&   , int Class::* const &);
        TEST_FWD_TYPE(int (* const&&)(float) , int (* const&)(float));
        TEST_FWD_TYPE(int (Class::* const&&)(char)
                                             , int (Class::* const&)(char));
        TEST_FWD_TYPE(const Enum&&           , const Enum&);
        TEST_FWD_TYPE(const Class&&          , const Class&);
        TEST_FWD_TYPE(const Struct&&         , const Struct&);
        TEST_FWD_TYPE(const Union&&          , const Union&);

        TEST_FWD_TYPE(volatile int&&         , const volatile int&);
        TEST_FWD_TYPE(int *volatile&&        , int *const volatile&);
        TEST_FWD_TYPE(int Class::* volatile&&, int Class::* const volatile&);
        TEST_FWD_TYPE(int (* volatile&&)(float)
                                             , int (* const volatile&)(float));
        TEST_FWD_TYPE(volatile Enum&&        , const volatile Enum&);
        TEST_FWD_TYPE(volatile Class&&       , const volatile Class&);
        TEST_FWD_TYPE(volatile Struct&&      , const volatile Struct&);
        TEST_FWD_TYPE(volatile Union&&       , const volatile Union&);

        TEST_FWD_TYPE(const volatile int&&   , const volatile int&);
        TEST_FWD_TYPE(int *const volatile&&  , int *const volatile&);
        TEST_FWD_TYPE(int Class::* const volatile&&
                                             , int Class::* const volatile&);
        TEST_FWD_TYPE(const volatile Enum&&  , const volatile Enum&);
        TEST_FWD_TYPE(const volatile Class&& , const volatile Class&);
        TEST_FWD_TYPE(const volatile Struct&&, const volatile Struct&);
        TEST_FWD_TYPE(const volatile Union&& , const volatile Union&);

#endif // defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

      } break;

      case 1: {
        // --------------------------------------------------------------------
        // TESTING TEST INFRASTRUCTURE
        //
        // Concerns:
        // 1. For `crm` of type `CvRefMatch<T>`, `crm(v)` returns the correct
        //    enumerator indicating whether `v` is an lvalue, const lvalue,
        //    volatile lvalue, const volatile lvalue, rvalue, const rvalue,
        //    volatile rvalue, or const volatile rvalue.
        // 2. For `cam` of type `CvArrayMatch<T>` and `a` of type array-of-T,
        //    `cam(a)` returns the number of elements of `a` or `0` if `a` has
        //    unknown bounds.
        // 3. For `cam` of type `CvArrayMatch<T>` and `p` of type
        //    pointer-to-T, `cam(p)` returns 0.
        // 4. Concerns 2 and 3 apply regardless of the cv qualification of `a`
        //    or `p`.
        // 5. For `cam` of type `CvArrayMatch<T>` and `a` of type
        //    rvalue-reference-to-array-of-T, `CvArrayMatch<T>` returns the
        //    same value as if `a` were not an rvalue reference.
        // 6. For `fm` of type `FuncMatch<F>`, `fm(f)` returns
        //    `k_FUNC_REFERENCE` if `f` is of type `F` or `F&` and
        //    `k_FUNC_POINTER` if `f` is of type `F*`.
        //
        // Plan:
        // 1. For concern 1, create a variable, `crm` of type
        //    `CvRefMatch<int>`.  Call `crm(v)` for variables `v` of type cvq
        //    `int` correct lvalue enumeration value is returned.  In C++11 or
        //    later mode, test with `std::move(v)` and verify that the correct
        //    lvalue enumeration is returned.  Also test with a literal integer
        //    and verify that the return value is `k_RVALUE_REF` in C++11 mode
        //    or `k_CONST_LVALUE_REF` in C++03 mode.
        // 2. For concerns 2 and 3, create a variable, `cam` of type
        //    `CvArrayMatch<int>`.  Define variables of type array-of-int,
        //    with both known and unknown sizes.  Verify that `cam(v)` returns
        //    the correct size (or 0) for each array variable, `v`.
        // 3. For concern 4, repeat step 2 for variables of each cvq
        //    combination.
        // 4. For concern 5 (C++11 or later), repeat steps 2 and 3 using
        //    `cam(std::move(v))`.
        // 6. For concern 6, create a variable, `fm`, of type
        //    `FuncMatch<F>`.  Call `fm(f)` for arguments `f` of type `void()`,
        //    `void(&)()`, and `void(*)()`, and verify that the correct
        //    enumeration value is returned.
        //
        // Testing:
        //   CvRefMatch<T>::operator()()
        //   CvArrayMatch<T>::operator()()
        //   FuncMatch<F>::operator()()
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING TEST INFRASTRUCTURE"
                            "\n===========================\n");

        if (veryVerbose) printf("CvRefMatch\n");

        int i                  = 0;
        const int ci           = 1;
        volatile int vi        = 2;
        const volatile int cvi = 3;
        volatile int &rvi      = vi;

        CvRefMatch<int> crm;
        ASSERT(k_LVALUE                  == crm(  i));
        ASSERT(k_CONST_LVALUE            == crm( ci));
        ASSERT(k_VOLATILE_LVALUE         == crm( vi));
        ASSERT(k_CONST_VOLATILE_LVALUE   == crm(cvi));
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        ASSERT(k_RVALUE                  == crm(std::move(  i)));
        ASSERT(k_CONST_RVALUE            == crm(std::move( ci)));
        ASSERT(k_VOLATILE_RVALUE         == crm(std::move( vi)));
        ASSERT(k_CONST_VOLATILE_RVALUE   == crm(std::move(cvi)));
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
        ASSERT(k_RVALUE                  == crm(5));
        ASSERT(k_RVALUE                  == crm(toRvalue(rvi)));
#else
        ASSERT(k_CONST_LVALUE            == crm(5));
        ASSERT(k_CONST_LVALUE            == crm(toRvalue(rvi)));
#endif

        if (veryVerbose) printf("CvArrayMatch\n");

        int                  a[1]   = { 1 };
        const int            ca[2]  = { 1, 2 };
        volatile int         va[3]  = { 1, 2, 3 };
        const volatile int   cva[4] = { 1, 2, 3, 4};
        int                (*pa)[]   = reinterpret_cast<int (*)[]>(a);
        const int          (*cpa)[]  = pa;
        volatile int       (*vpa)[]  = pa;
        const volatile int (*cvpa)[] = pa;
        int                 *p;
        const int           *cp;
        volatile int        *vp;
        const volatile int  *cvp;

        CvArrayMatch<int>                cam;
        CvArrayMatch<const int>          camc;
        CvArrayMatch<volatile int>       camv;
        CvArrayMatch<const volatile int> camcv;
        ASSERT(1 == cam  (  a));
        ASSERT(2 == camc ( ca));
        ASSERT(3 == camv ( va));
        ASSERT(4 == camcv(cva));
#if !defined(BSLMF_FORWARDINGREFTYPE_NO_ARRAY_OF_UNKNOWN_BOUND)
        ASSERT(0 == cam  (*  pa));
        ASSERT(0 == camc (* cpa));
        ASSERT(0 == camv (* vpa));
        ASSERT(0 == camcv(*cvpa));
#endif

        ASSERT(0 == cam  (  p));
        ASSERT(0 == camc ( cp));
        ASSERT(0 == camv ( vp));
        ASSERT(0 == camcv(cvp));

        if (veryVerbose) printf("FuncMatch\n");
        FuncMatch<F> fm;
        F& func_r = func;
        F* func_p = func;
        ASSERT(k_FUNC_REFERENCE == fm(func));
        ASSERT(k_FUNC_REFERENCE == fm(func_r));
        ASSERT(k_FUNC_POINTER   == fm(&func));
        ASSERT(k_FUNC_POINTER   == fm(func_p));
      } break;

      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
