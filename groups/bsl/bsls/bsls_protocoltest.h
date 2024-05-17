// bsls_protocoltest.h                                                -*-C++-*-
#ifndef INCLUDED_BSLS_PROTOCOLTEST
#define INCLUDED_BSLS_PROTOCOLTEST

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide classes and macros for testing abstract protocols.
//
//@CLASSES:
//  bsls::ProtocolTestImp: provides a framework for testing protocol classes
//  bsls::ProtocolTest: provides tests for protocol class concerns
//
//@MACROS:
//  BSLS_PROTOCOLTEST_ASSERT: macro for testing protocol methods
//
//@DESCRIPTION: This component provides classes and macros that simplify the
// creation of test drivers for protocol (i.e., pure abstract interface)
// classes.
//
// The purpose of a test driver for a protocol class is to verify concerns for
// that protocol's definition.  Although each protocol is different and
// requires its own test driver, there is a common set of concerns that apply
// to all protocol classes.  This component allows us to verify those concerns
// in a generic manner.
//
// Each protocol class has to satisfy the following set of requirements
// (concerns):
//: o The protocol is abstract: no objects of it can be created.
//: o The protocol has no data members.
//: o The protocol has a virtual destructor.
//: o All methods of the protocol are pure virtual.
//: o All methods of the protocol are publicly accessible.
//
// There are two main exceptions to the above requirements:
//: o Adaptor classes that adapt one protocol to another will have non-pure
//:   virtual functions that invoke pure virtual functions.  Test drivers for
//:   such adaptors should test that the appropriate pure virtual function is
//:   called when the adapted function is invoked.
//: o Protocol classes that copy or extend those from the C++ Standard Library
//:   may have *pass-through* non-virtual functions that call private or
//:   protected virtual functions.  For example, non-virtual
//:   'bsl::memory_resource::allocate' calls private virtual 'do_allocate'.  In
//:   this case test drivers using this component should reference the
//:   non-virtual function as a proxy for the virtual function.
//
// This protocol test component is intended to verify conformance to these
// requirements; however, it is not possible to verify all protocol
// requirements fully within the framework of the C++ language.  The following
// aspects of the above requirements are not verified by this component:
//: o Non-creator methods of the protocol are *pure* virtual.
//: o There are no methods in the protocol other than the ones being tested.
//
// Additionally some coding guidelines related to protocols are also not
// verified:
//: o The destructor is not pure virtual.
//: o The destructor is not implemented inline.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Testing a Protocol Class
///- - - - - - - - - - - - - - - - - -
// This example demonstrates how to test a protocol class, 'ProtocolClass',
// using this protocol test component.  Our 'ProtocolClass' provides two of
// pure virtual methods ('foo' and 'bar'), along with a virtual destructor:
//..
//  struct ProtocolClass {
//      virtual ~ProtocolClass();
//      virtual const char *bar(char const *, char const *) = 0;
//      virtual int foo(int) const = 0;
//  };
//
//  ProtocolClass::~ProtocolClass()
//  {
//  }
//..
// First, we define a test class derived from this protocol, and implement its
// virtual methods.  Rather than deriving the test class from 'ProtocolClass'
// directly, the test class is derived from
// 'bsls::ProtocolTestImp<ProtocolClass>' (which, in turn, is derived
// automatically from 'ProtocolClass').  This special base class implements
// boilerplate code and provides useful functionality for testing of protocols.
//..
//  // ========================================================================
//  //                  GLOBAL CLASSES/TYPEDEFS FOR TESTING
//  // ------------------------------------------------------------------------
//
//  struct ProtocolClassTestImp : bsls::ProtocolTestImp<ProtocolClass> {
//      const char *bar(char const *, char const *) { return markDone(); }
//      int foo(int) const                          { return markDone(); }
//  };
//..
// Notice that in 'ProtocolClassTestImp' we must provide an implementation for
// every protocol method except for the destructor.  The implementation of each
// method calls the (protected) 'markDone' which is provided by the base class
// for the purpose of verifying that the method from which it's called is
// declared as virtual in the protocol class.
//
// Then, in our protocol test case we describe the concerns we have for the
// protocol class and the plan to test those concerns:
//..
//  // ------------------------------------------------------------------------
//  // PROTOCOL TEST:
//  //   Ensure this class is a properly defined protocol.
//  //
//  // Concerns:
//  //: 1 The protocol is abstract: no objects of it can be created.
//  //:
//  //: 2 The protocol has no data members.
//  //:
//  //: 3 The protocol has a virtual destructor.
//  //:
//  //: 4 All methods of the protocol are pure virtual.
//  //:
//  //: 5 All methods of the protocol are publicly accessible.
//  //
//  // Plan:
//  //: 1 Define a concrete derived implementation, 'ProtocolClassTestImp',
//  //:   of the protocol.
//  //:
//  //: 2 Create an object of the 'bsls::ProtocolTest' class template
//  //:   parameterized by 'ProtocolClassTestImp', and use it to verify
//  //:   that:
//  //:
//  //:   1 The protocol is abstract. (C-1)
//  //:
//  //:   2 The protocol has no data members. (C-2)
//  //:
//  //:   3 The protocol has a virtual destructor. (C-3)
//  //:
//  //: 3 Use the 'BSLS_PROTOCOLTEST_ASSERT' macro to verify that
//  //:   non-creator methods of the protocol are:
//  //:
//  //:   1 virtual, (C-4)
//  //:
//  //:   2 publicly accessible. (C-5)
//  //
//  // Testing:
//  //   virtual ~ProtocolClass();
//  //   virtual const char *bar(char const *, char const *) = 0;
//  //   virtual int foo(int) const = 0;
//  // ------------------------------------------------------------------------
//..
// Next we print the banner for this test case:
//..
//  if (verbose) puts("\nPROTOCOL TEST"
//                    "\n=============");
//..
// Then, we create an object of type
// 'bsls::ProtocolTest<ProtocolClassTestImp>', 'testObj':
//..
//  if (verbose) puts("\n\tCreate a test object.");
//
//  bsls::ProtocolTest<ProtocolClassTestImp> testObj(veryVerbose);
//..
// Now we use the 'testObj' to test some general concerns about the protocol
// class.
//..
//  if (verbose) puts("\tVerify that the protocol is abstract.");
//
//  ASSERT(testObj.testAbstract());
//
//  if (verbose) puts("\tVerify that there are no data members.");
//
//  ASSERT(testObj.testNoDataMembers());
//
//  if (verbose) puts("\tVerify that the destructor is virtual.");
//
//  ASSERT(testObj.testVirtualDestructor());
//..
// Finally we use the 'testObj' to test concerns for each individual method of
// the protocol class.  To test a protocol method we need to call it from
// inside the 'BSLS_PROTOCOLTEST_ASSERT' macro, and also pass the 'testObj':
//..
//  if (verbose) puts("\tVerify that methods are public and virtual.");
//
//  BSLS_PROTOCOLTEST_ASSERT(testObj, foo(77));
//  BSLS_PROTOCOLTEST_ASSERT(testObj, bar("", ""));
//..
// These steps conclude the protocol testing.  If there are any failures, they
// will be reported via standard test driver assertions (i.e., the standard
// 'ASSERT' macro).
//
///Example 2: Testing a Method Overloaded on 'const'ness
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a protocol that represent a sequence of integers.  Such a
// protocol will have an overloaded 'at()' method of both the 'const' and the
// "mutable" variation.  In verification of such methods we need to ensure that
// we verify *both* overloads of the 'virtual' function.
//
// First let's define the interesting parts of our imaginary sequence, the
// overloaded 'at()' methods, and a virtual destructor to avoid warnings:
//..
//  struct IntSeqExample {
//      // CREATORS
//      virtual ~IntSeqExample();
//
//      // MANIPULATORS
//      virtual int& at(bsl::size_t index) = 0;
//
//      // ACCESSORS
//      virtual int at(bsl::size_t index) const = 0;
//  };
//
//  IntSeqExample::~IntSeqExample()
//  {
//  }
//..
// Next, we define the test implementation as usual:
//..
//  struct IntSeqExampleTestImp : bsls::ProtocolTestImp<IntSeqExample> {
//      static int s_int;
//
//      int& at(size_t)       { s_int = 4; markDone(); return s_int; }
//      int  at(size_t) const { s_int = 2; return markDone();        }
//  };
//
//  int IntSeqExampleTestImp::s_int = 0;
//..
// Note the use of a dummy variable to return a reference.  We also use that
// variable, by giving it different values in the two overloads, to demonstrate
// that we have called the overload we have intended.
//
// Then, we test the non-'const' overload as usual:
//..
//  bsls::ProtocolTest<IntSeqExampleTestImp> testObj(veryVerbose);
//  BSLS_PROTOCOLTEST_ASSERT(testObj, at(0));
//..
// Now, we can verify that we have indeed tested the non-'const' overload:
//..
//  assert(4 == IntSeqExampleTestImp::s_int);
//..
// Finally, we test 'at(size_t) const' and also verify that we indeed called
// the intended overload.  Notice that we "force" the 'const' variant of the
// method to be picked by specifying a 'const Implementation' type argument to
// 'bsls::ProtocolTest':
//..
//  bsls::ProtocolTest<const IntSeqExampleTestImp> test_OBJ(veryVerbose);
//  BSLS_PROTOCOLTEST_ASSERT(test_OBJ, at(0));
//
//  assert(2 == IntSeqExampleTestImp::s_int);
//..
// Note that the assertion that verifies that the intended overload was called
// is not strictly necessary, it is included for demonstration purposes.
//
///Implementation Note
///- - - - - - - - - -
// This component has a number of private meta-functions on some platforms,
// e.g., 'ProtocolTest_EnableIf', 'ProtocolTest_IsClass', and
// 'ProtocolTest_IsAbstract'.  These mimic, to a limited extent, standard
// library meta-functions in the namespace 'std' that are not available on all
// platforms.  For general use, see the {'bslmf'} package and the {'bsl'}
// namespace for portable implementations of some of these meta-functions.

#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
#include <type_traits>
#endif

#include <cstddef>
#include <cstdio>

namespace BloombergLP {
namespace bsls {

                        // =============================
                        // class ProtocolTest_IsAbstract
                        // =============================

template <class T>
struct ProtocolTest_IsAbstract;
    // This class template is a compile-time meta-function, parameterized with
    // type 'T', the output of which is 'value', which will be 'true' if 'T' is
    // abstract and 'false' otherwise.  On some platforms, the 'IsAbstract'
    // test makes use of the fact that a type 'an array of objects of an
    // abstract type' (e.g., 'T[1]') cannot exist.  Note that it is only an
    // approximation, because this is also true for an incomplete type.  But,
    // this approximation is good enough for the purpose of testing protocol
    // classes.  On certain other platforms, the 'IsAbstract' test will make
    // use of the fact that abstract types cannot be returned.  This
    // approximation also has issues, noted below, but is also good enough for
    // the purpose of testing protocol classes.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER

template <class T>
struct ProtocolTest_IsAbstract {
    enum { value = std::is_abstract<T>::value };
};

#elif defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VERSION >= 110000

///Implementation Note
///-------------------
// GCC 11 and later adhere to the core language changes resulting from the
// paper P0929, which was approved as a defect report and applied to C++03 and
// later.  One effect of this paper is that it became well-formed to name the
// type 'T[1]' where 'T' is an abstract class type.  As a result, platforms
// that apply P0929 require a different implementation of an abstractness test
// in C++03 mode.

template <class T>
struct ProtocolTest_VoidType {
    // This component-private, meta-function 'struct' template provides a
    // single-parameter type trait that behaves like the 'bslmf::VoidType'
    // meta-function for use in the implementation of the
    // 'ProtocolTest_IsAbstract' meta-function when compiling in C++03 mode on
    // compilers that apply P0929 to that mode.  Note that this component is in
    // the 'bsls' package, which is levelized below 'bslmf', and so cannot
    // depend on 'bslmf::VoidType'.

    typedef void Type;
};

template <class VOID_TYPE, class T>
struct ProtocolTest_IsClassTypeImp {
    // This component-private, meta-function (primary) 'struct' template
    // provides part of the implementation of a type trait that behaves like
    // 'std::is_class' for use in the implementation of the
    // 'ProtocolTest_IsAbstract' meta-function when compiling in C++03 mode on
    // compilers that apply P0929 to that mode.  Note that in this mode,
    // 'std::is_class' is not available.

    enum { value = false };
};

template <class T>
struct ProtocolTest_IsClassTypeImp<
    typename ProtocolTest_VoidType<int T::*>::Type,
    T> {
    // This component-private, meta-function 'struct' template (partial
    // specialization) provides part of the implementation of a type trait that
    // behaves like 'std::is_class' for use in the implementation of the
    // 'ProtocolTest_IsAbstract' meta-function when compiling in C++03 mode on
    // compilers that apply P0929 to that mode.  Note that in this mode,
    // 'std::is_class' is not available.

    enum { value = true };
};

template <class T>
struct ProtocolTest_IsClassType {
    // This component-private, meta-function 'struct' template provides the
    // implementation of a type trait that behaves like 'std::is_class' for use
    // in the implementation of the 'ProtocolTest_IsAbstract' meta-function
    // when compiling in C++03 mode on compilers that apply P0929 to that mode.
    // Note that in this mode, 'std::is_class' is not available.

    enum {
        value = ProtocolTest_IsClassTypeImp<void, T>::value
    };
};

template <bool CONDITION, class T = void>
struct ProtocolTest_EnableIf {
    // This component-private, meta-function (primary) 'struct' template
    // provides part of the implementation of a type trait that behaves like
    // 'std::enable_if' for use in the implementation of the
    // 'ProtocolTest_IsAbstract' meta-function when compiling in C++03 mode on
    // compilers that apply P0929 to that mode.  Note that in this mode,
    // 'std::enable_if' is not available.
};

template <class T>
struct ProtocolTest_EnableIf<true, T> {
    // This component-private, meta-function 'struct' template (partial
    // specialization) provides part of the implementation of a type trait that
    // behaves like 'std::enable_if' for use in the implementation of the
    // 'ProtocolTest_IsAbstract' meta-function when compiling in C++03 mode on
    // compilers that apply P0929 to that mode.  Note that in this mode,
    // 'std::enable_if' is not available.

    typedef T Type;
};

struct ProtocolTest_NoType {
    // this component-private 'struct' provides a type having a size that is
    // guaranteed to be different than the size of 'ProtocolTest_YesType', and
    // is used in the implementation of the 'ProtocolTest_IsAbstract'
    // meta-function when compiling in C++03 mode on compilers that apply P0929
    // to that mode.

    char d_padding;
};

struct ProtocolTest_YesType {
    // this component-private 'struct' provides a type having a size that is
    // guaranteed to be different than the size of 'ProtocolTest_NoType', and
    // is used in the implementation of the 'ProtocolTest_IsAbstract'
    // meta-function when compiling in C++03 mode on compilers that apply P0929
    // to that mode.

    char d_padding[17];
};

struct ProtocolTest_IsReturnableImpUtil {
    // This component-private 'struct' provides a namespace for a 'test'
    // overload set used to determine if a specified type can be returned from
    // a function-call expression or not.  This 'struct' is used in the
    // implementation of the 'ProtocolTest_IsAbstract' meta-function when
    // compiling in C++03 mode on compilers that apply P0929 to that mode.

  private:
    // PRIVATE CLASS METHODS
    template <class T>
    static T returnThe();
        // Return a prvalue of the specified 'T' type.  Note that this function
        // is declared but not defined.  It is similar in nature to
        // 'std::declval', with the important distinction that return type of
        // 'std::declval' is a reference type, and the return type of this
        // function is not (necessarily) a reference type.

  public:
    // CLASS METHODS
    template <class T>
    static ProtocolTest_NoType test(...);
    template <class T>
    static ProtocolTest_YesType
    test(typename ProtocolTest_EnableIf<static_cast<bool>(
             sizeof(static_cast<void>(returnThe<T>()), 0))>::Type *);
        // Return a 'ProtocolTest_YesType' prvalue if the specified 'T' type
        // can be returned from a function-call expression, and return a
        // 'ProtocolTest_NoType' prvalue otherwise.  The behavior is undefined
        // unless this function is invoked with a single argument that is
        // convertible to a 'void *'.  Note that this function is declared but
        // not defined.
};

template <class T>
struct ProtocolTest_IsReturnable {
    // This component-private, meta-function 'struct' template provides a
    // compile-time constant 'value' class member with the value 'true' if the
    // supplied 'T' type can be returned from a function-call expression, and
    // provides a 'value' class member with the value 'false' otherwise.  This
    // meta-function is used in the implementation of the
    // 'ProtocolTest_IsAbstract' meta-function when compiling in C++03 mode on
    // compilers that apply P0929 to that mode.

    enum {
        value = sizeof(ProtocolTest_YesType) ==
                sizeof(ProtocolTest_IsReturnableImpUtil::test<T>(0))
    };
};


template <class T>
struct ProtocolTest_IsAbstract {
    // This component-private, meta-function 'struct' template provides a
    // compile-time constant 'value' class member with the value 'true' if the
    // supplied 'T' type is an abstract class type (or, and this is a defect,
    // if 'T' is a class type with a private destructor), and provides a
    // 'value' class member with the value 'false' otherwise.  This
    // meta-function matches the behavior 'std::is_abstract' would have if it
    // were available except for non-abstract types with a private destructor,
    // and is for use when compiling in C++03 mode on compilers that apply
    // P0929 to that mode.

    enum {
        value = ProtocolTest_IsClassType<T>::value &&
                !ProtocolTest_IsReturnable<T>::value
    };
};

#else

template <class T>
struct ProtocolTest_IsAbstract {
    // This component-private, meta-function 'struct' template provides a
    // compile-time constant 'value' class member with the value 'true' if the
    // supplied 'T' type is an abstract class type, and provides a 'value'
    // class member with the value 'false' otherwise.  This meta-function
    // matches the behavior 'std::is_abstract' would have if it were
    // available on C++03 platforms.

    typedef char                    YesType;
    typedef struct { char a[2]; }   NoType;

    template <class U>
    static NoType test(U (*)[1]);

    template <class U>
    static YesType test(...);

    enum { value = sizeof(test<T>(0)) == sizeof(YesType) };
};

#endif

                     // ===================================
                     // class ProtocolTest_MethodReturnType
                     // ===================================

struct ProtocolTest_MethodReturnType {
    // This class is a proxy for a return type designed to simplify testing
    // implementations of protocol methods.
    // 'ProtocolTest_MethodReturnType' can be converted to any
    // non-reference type (i.e., the type can be either a value or pointer
    // type, but not a reference type).  When an object of this class is
    // returned from a test implementation of a protocol method, it is
    // implicitly converted to the return type of the protocol method.

    // ACCESSORS
    template <class T>
    operator T() const;
        // Return a temporary value of type 'T'.  The returned object is valid
        // but it does not have any meaningful value so it should not be used.
        // Type 'T' is required to be default-constructible.
};

                   // ======================================
                   // class ProtocolTest_MethodReturnRefType
                   // ======================================

struct ProtocolTest_MethodReturnRefType {
    // This class is a proxy for a return type designed to simplify testing
    // implementations of protocol methods.
    // 'ProtocolTest_MethodReturnRefType' can be converted to any
    // reference type.  When an object of this class is returned from a test
    // implementation of a protocol method, it is implicitly converted to
    // the return type of the protocol method.

    // ACCESSORS
    template <class T>
    operator T&() const;
        // Return a 'T&' reference to an invalid object.  The returned value
        // should not be used and should be immediately discarded.
};

                           // =======================
                           // class ProtocolTest_Dtor
                           // =======================

template <class BSLS_TESTIMP>
struct ProtocolTest_Dtor : BSLS_TESTIMP {
    // This class template is a helper protocol-test implementation class that
    // tests that a protocol destructor is declared 'virtual', which it does by
    // calling the 'markDone' function from its destructor.  The destructor
    // will be executed if the protocol's destructor is declared 'virtual' and
    // not executed otherwise.  Note that the 'BSLS_TESTIMP' template parameter
    // is required to be a type derived from 'ProtocolTestImp' class.

    // CREATORS
    ~ProtocolTest_Dtor();
        // Destroy this object and call the 'markDone' method, indicating that
        // the base class's destructor was declared 'virtual'.
};

                          // =========================
                          // class ProtocolTest_Status
                          // =========================

class ProtocolTest_Status {
    // This class keeps track of the test status, which includes the status of
    // the last test and the number of failures across all tests.

  private:
    // DATA
    int  d_failures;   // number of test failures encountered so far
    bool d_last;       // result of the last test ('true' indicates success)

  public:
    // CREATORS
    ProtocolTest_Status();
        // Create an object of the 'ProtocolTest_Status' class with the
        // default state in which 'failures() == 0' and 'last() == true'.

    // MANIPULATORS
    void resetLast();
        // Reset the status of the last test to 'true'.

    void fail();
        // Record a test failure by increasing the number of 'failures' and
        // setting the status of the last test to 'false'.

    // ACCESSORS
    int failures() const;
        // Return the number of failures encountered during testing of a
        // protocol class, which is 0 if all tests succeeded or if no tests
        // ran.

    bool last() const;
        // Return 'true' if the last test completed successfully (or no test
        // has yet completed), and 'false' if it failed.
};

                         // ===========================
                         // class ProtocolTest_AsBigAsT
                         // ===========================

template <class T>
class ProtocolTest_AsBigAsT {
    // This auxiliary structure has a size no less than the size of (template
    // parameter) 'T'.

#if defined (BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES)
    // DATA
    std::max_align_t d_dummy[sizeof(T) / sizeof(std::max_align_t) + 1];
#else
    // PRIVATE TYPES
    union MaxAlignType {
        void               *d_v_p;
        unsigned long long  d_ull;
        long double         d_ul;
    };

    // DATA
    MaxAlignType d_dummy[sizeof(T) / sizeof(MaxAlignType) + 1];
#endif
};

                            // =====================
                            // class ProtocolTestImp
                            // =====================

template <class BSLS_PROTOCOL>
class ProtocolTestImp : public BSLS_PROTOCOL {
    // This mechanism class template is a base class for a test implementation
    // of a protocol class defined by the 'BSLS_PROTOCOL' template parameter.
    // Its purpose is to reduce the boilerplate test code required to verify
    // that derived virtual methods are called.  It provides 'markDone' member
    // functions one of which should be called from each method of the protocol
    // class test implementation to indicate that the virtual method is
    // correctly overridden.  It also overloads 'operator->' to serve as a
    // proxy to 'BSLS_PROTOCOL' and detect when 'BSLS_PROTOCOL' methods are
    // called.

  private:
    // DATA
    mutable ProtocolTest_Status *d_status;   // test status object for test
                                             // failure reporting; mutable, so
                                             // it can be set from 'const'
                                             // methods in order to be able to
                                             // verify 'const' methods.

    mutable bool                 d_entered;  // 'true' if this object entered a
                                             // protocol method call; mutable,
                                             // so it can be set from 'const'
                                             // methods in order to be able to
                                             // verify 'const' methods.

    mutable bool                 d_exited;   // 'true' if this object exited a
                                             // protocol method in the derived
                                             // class; mutable, so it can be
                                             // set from 'const' methods in
                                             // order to be able to verify
                                             // 'const' methods.
  public:
    // TYPES
    typedef BSLS_PROTOCOL ProtocolType;

    // CREATORS
    ProtocolTestImp();
        // Create an object of the 'ProtocolTestImp' class.

    ~ProtocolTestImp();
        // Destroy this object and check the status of the test execution
        // (success or failure).  On test failure, report it to
        // 'ProtocolTest_Status'.

    // MANIPULATORS
    BSLS_PROTOCOL *operator->();
        // Dereference this object as if it were a pointer to 'BSLS_PROTOCOL'
        // in order to call a method on 'BSLS_PROTOCOL'.  Also mark this
        // object as 'entered' for the purpose of calling a protocol method.

    // ACCESSORS
    const BSLS_PROTOCOL *operator->() const;
        // Dereference this object as if it were a 'const BSLS_PROTOCOL *s' in
        // order to call a 'const' method on 'BSLS_PROTOCOL'.  Also mark this
        // object as 'entered' for the purpose of calling a protocol method.

    ProtocolTest_MethodReturnType markDone() const;
        // Return a proxy object convertible to any value or pointer type.
        // Derived classed should call this method from their implementations
        // of protocol virtual methods to indicate that virtual methods were
        // overridden correctly.

    ProtocolTest_MethodReturnRefType markDoneRef() const;
        // Return a proxy object convertible to any reference type.  Derived
        // classed should call this method from their implementations of
        // protocol virtual methods to indicate that virtual methods were
        // overridden correctly.

    template <class T>
    T markDoneVal(const T& value) const;
        // Return the specified 'value'.  Derived classes should call this
        // method from their implementations of protocol virtual methods to
        // indicate that virtual methods were overridden correctly.

    void markEnter() const;
        // Mark this object as entered for the purpose of calling a protocol
        // method.  The 'entered' property is tested in the destructor to
        // check for test failures (i.e., if 'entered == false' then the test
        // cannot fail since it never ran).  Note that 'markEnter' and
        // 'markDone' calls have to be paired for a protocol-method-call test
        // to succeed.

    void setTestStatus(ProtocolTest_Status *testStatus) const;
        // Connect this protocol test object with the specified 'testStatus'
        // object, which will be used for test failure reporting.
};

                             // ==================
                             // class ProtocolTest
                             // ==================

template <class BSLS_TESTIMP>
class ProtocolTest {
    // This mechanism class template provides the implementation of protocol
    // testing concerns via 'test*' methods (for non-method concerns), and via
    // 'operator->' (for method concerns).  The 'BSLS_TESTIMP' template
    // parameter is required to be a class derived from 'ProtocolTestImp'
    // that provides test implementations of all protocol methods.

  private:
    // TYPES
    typedef typename BSLS_TESTIMP::ProtocolType ProtocolType;

    // DATA
    ProtocolTest_Status d_status;
    bool                d_verbose;  // print trace messages if 'true'

  private:
    // PRIVATE MANIPULATORS
    void startTest();
        // Start a new test by resetting this object to the state before the
        // test.

    void trace(char const *message) const;
        // Print a trace 'message' if 'd_verbose' is 'true'.

  public:
    // CREATORS
    explicit
    ProtocolTest(bool verbose = false);
        // Construct a 'ProtocolTest' object.

    // MANIPULATORS
    BSLS_TESTIMP method(const char *methodDesc = "");
        // Return a 'BSLS_TESTIMP' object to perform testing of a specific
        // method which gets called via 'operator->()'.  Note that
        // 'BSLS_TESTIMP' is a proxy to the actual protocol class.

    bool testAbstract();
        // Return 'true' (i.e., the test passed) if the protocol class being
        // tested is abstract and return 'false' (i.e., the test failed)
        // otherwise.  Increase the count of 'failures' and set 'lastStatus' to
        // 'false' on failure.

    bool testNoDataMembers();
        // Return 'true' (i.e., the test passed) if the protocol class being
        // tested contains no data fields and return 'false' (i.e., the test
        // failed) otherwise.  Increase the count of 'failures' and set
        // 'lastStatus' to 'false' on failure.

    bool testVirtualDestructor();
        // Return 'true' (i.e., the test passed) if the protocol class being
        // tested has a virtual destructor and return 'false' (i.e., the test
        // failed) otherwise.  Increase the 'failures' count and set
        // 'lastStatus' to 'false' on failure.

    // ACCESSORS
    int failures() const;
        // Return the number of failures encountered during testing of a
        // protocol class.  The returned value is 0 if all tests succeeded, or
        // no tests ran.

    bool lastStatus() const;
        // Return 'true' if the last test completed successfully (or no test
        // has yes completed) and 'false' otherwise.
};

}  // close package namespace

                          // ========================
                          // BSLS_PROTOCOLTEST_ASSERT
                          // ========================

// This macro provides a test for method-related concerns of a protocol class.
// It ensures that a method is publicly accessible and declared 'virtual'.  It
// requires that a standard test driver 'ASSERT' macro is defined, which is
// used to assert the test completion status.

#define BSLS_PROTOCOLTEST_ASSERT(test, methodCall)                            \
    do {                                                                      \
        (void) test.method(                                                   \
                "inside BSLS_PROTOCOLTEST_ASSERT("#methodCall")")->methodCall;\
        if (!test.lastStatus()) {                                             \
            ASSERT(0 && "Not a virtual method: "#methodCall);                 \
        }                                                                     \
    } while (0)

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

namespace bsls {

                     // -----------------------------------
                     // class ProtocolTest_MethodReturnType
                     // -----------------------------------

// ACCESSORS
template <class T>
inline
ProtocolTest_MethodReturnType::operator T() const
{
    return T();
}

                   // --------------------------------------
                   // class ProtocolTest_MethodReturnRefType
                   // --------------------------------------

// ACCESSORS
template <class T>
inline
ProtocolTest_MethodReturnRefType::operator T&() const
{
    static ProtocolTest_AsBigAsT<T> obj;
    T *pObj = reinterpret_cast<T *>(&obj);
    return *pObj;
}

                       // -----------------------
                       // class ProtocolTest_Dtor
                       // -----------------------

// CREATORS
template <class BSLS_TESTIMP>
inline
ProtocolTest_Dtor<BSLS_TESTIMP>::~ProtocolTest_Dtor()
{
    this->markDone();
}

                      // -------------------------
                      // class ProtocolTest_Status
                      // -------------------------

// CREATORS
inline
ProtocolTest_Status::ProtocolTest_Status()
: d_failures(0)
, d_last(true)
{
}

// MANIPULATORS
inline
void ProtocolTest_Status::resetLast()
{
    d_last = true;
}

inline
void ProtocolTest_Status::fail()
{
    ++d_failures;
    d_last = false;
}

// ACCESSORS
inline
int ProtocolTest_Status::failures() const
{
    return d_failures;
}

inline
bool ProtocolTest_Status::last() const
{
    return d_last;
}

                          // ---------------------
                          // class ProtocolTestImp
                          // ---------------------

// CREATORS
template <class BSLS_PROTOCOL>
inline
ProtocolTestImp<BSLS_PROTOCOL>::ProtocolTestImp()
: d_status(0)
, d_entered(false)
, d_exited(false)
{
}

template <class BSLS_PROTOCOL>
inline
ProtocolTestImp<BSLS_PROTOCOL>::~ProtocolTestImp()
{
    if (d_entered && !d_exited) {
        d_status->fail();
    }
}

// MANIPULATORS
template <class BSLS_PROTOCOL>
inline
typename ProtocolTestImp<BSLS_PROTOCOL>::ProtocolType *
ProtocolTestImp<BSLS_PROTOCOL>::operator->()
{
    markEnter();
    return static_cast<BSLS_PROTOCOL *>(this);
}

// ACCESSORS
template <class BSLS_PROTOCOL>
inline
const typename ProtocolTestImp<BSLS_PROTOCOL>::ProtocolType *
ProtocolTestImp<BSLS_PROTOCOL>::operator->() const
{
    markEnter();
    return static_cast<const BSLS_PROTOCOL *>(this);
}

template <class BSLS_PROTOCOL>
inline
ProtocolTest_MethodReturnType
ProtocolTestImp<BSLS_PROTOCOL>::markDone() const
{
    d_exited = true;
    return ProtocolTest_MethodReturnType();
}

template <class BSLS_PROTOCOL>
inline
ProtocolTest_MethodReturnRefType
ProtocolTestImp<BSLS_PROTOCOL>::markDoneRef() const
{
    d_exited = true;
    return ProtocolTest_MethodReturnRefType();
}

template <class BSLS_PROTOCOL>
template <class T>
inline
T ProtocolTestImp<BSLS_PROTOCOL>::markDoneVal(const T& value) const
{
    d_exited = true;
    return value;
}

template <class BSLS_PROTOCOL>
inline
void ProtocolTestImp<BSLS_PROTOCOL>::markEnter() const
{
    d_entered = true;
}

template <class BSLS_PROTOCOL>
inline
void ProtocolTestImp<BSLS_PROTOCOL>::setTestStatus(
                                         ProtocolTest_Status *testStatus) const
{
    d_status = testStatus;
}

                       // ------------------
                       // class ProtocolTest
                       // ------------------

// PRIVATE MANIPULATORS
template <class BSLS_TESTIMP>
inline
void ProtocolTest<BSLS_TESTIMP>::startTest()
{
    d_status.resetLast();
}

template <class BSLS_TESTIMP>
inline
void ProtocolTest<BSLS_TESTIMP>::trace(char const *message) const
{
    if (d_verbose) {
        std::printf("\t%s\n", message);
    }
}

// CREATORS
template <class BSLS_TESTIMP>
inline
ProtocolTest<BSLS_TESTIMP>::ProtocolTest(bool verbose)
: d_verbose(verbose)
{
}

// MANIPULATORS
template <class BSLS_TESTIMP>
inline
BSLS_TESTIMP ProtocolTest<BSLS_TESTIMP>::method(const char *methodDesc)
{
    trace(methodDesc);
    startTest();

    BSLS_TESTIMP impl;
    impl.setTestStatus(&d_status);
    return impl;
}

template <class BSLS_TESTIMP>
inline
bool ProtocolTest<BSLS_TESTIMP>::testAbstract()
{
    trace("inside ProtocolTest::testAbstract()");
    startTest();

    if (!ProtocolTest_IsAbstract<ProtocolType>::value) {
        d_status.fail();
    }

    return lastStatus();
}

template <class BSLS_TESTIMP>
inline
bool ProtocolTest<BSLS_TESTIMP>::testNoDataMembers()
{
    trace("inside ProtocolTest::testNoDataMembers()");
    struct EmptyProtocol
    {
        virtual ~EmptyProtocol() {}
    };

    startTest();

    if (sizeof(EmptyProtocol) != sizeof(ProtocolType)) {
        d_status.fail();
    }

    return lastStatus();
}

template <class BSLS_TESTIMP>
bool ProtocolTest<BSLS_TESTIMP>::testVirtualDestructor()
{
    trace("inside ProtocolTest::testVirtualDestructor");
    startTest();

    // Can't use an automatic buffer and the placement new for an object of
    // type ProtocolTest_Dtor<BSLS_TESTIMP> here, because bslma::Allocator
    // defines its own placement new, making it impossible to test
    // bslma::Allocator protocol this way.

    // Prepare a test
    ProtocolTest_Dtor<BSLS_TESTIMP> * obj =
                                    new ProtocolTest_Dtor<BSLS_TESTIMP>();
    BSLS_TESTIMP * base = obj;
    obj->setTestStatus(&d_status);

    // Run the test.
    obj->markEnter();
    delete base;

    // 'ProtocolTest_Dtor::~ProtocolTest_Dtor' will be called only if
    // the destructor was declared 'virtual' in the interface, but
    // 'BSLS_TESTIMP::~BSLS_TESTIMP' is always executed to check if the
    // derived destructor was called.

    return lastStatus();
}

// ACCESSORS
template <class BSLS_TESTIMP>
inline
int ProtocolTest<BSLS_TESTIMP>::failures() const
{
    return d_status.failures();
}

template <class BSLS_TESTIMP>
inline
bool ProtocolTest<BSLS_TESTIMP>::lastStatus() const
{
    return d_status.last();
}

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

#ifdef bsls_ProtocolTest
#undef bsls_ProtocolTest
#endif
#define bsls_ProtocolTest bsls::ProtocolTest
    // This alias is defined for backward compatibility.

#ifdef bsls_ProtocolTestImp
#undef bsls_ProtocolTestImp
#endif
#define bsls_ProtocolTestImp bsls::ProtocolTestImp
    // This alias is defined for backward compatibility.
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
