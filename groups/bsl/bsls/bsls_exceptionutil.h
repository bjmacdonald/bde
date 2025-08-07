// bsls_exceptionutil.h                                               -*-C++-*-
#ifndef INCLUDED_BSLS_EXCEPTIONUTIL
#define INCLUDED_BSLS_EXCEPTIONUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide simplified exception constructs for non-exception builds.
//
//@CLASSES:
//
//@MACROS:
//  BSLS_TRY: begin a `try`-block
//  BSLS_CATCH(X): begin a `catch`-block for exception `X`
//  BSLS_THROW(X): `throw` exception `X`
//  BSLS_RETHROW: re-`throw` the current exception
//  BSLS_EXCEPTION_SPEC(SPEC): add `SPEC` to function exception specification
//  BSLS_NOTHROW_SPEC: declare that a function throws no exceptions
//  BSLS_EXCEPTION_VIRTUAL_NOTHROW: virtual `exception` method except. spec.
//  BSLS_EXCEPTION_WHAT_NOTHROW: `exception::what()` except. spec.
//
//@SEE_ALSO: bsls_compilerfeatures, bsls_cpp11
//
//@DESCRIPTION: Code that uses `try`, `throw` and `catch` constructs will
// often fail to compile when exceptions are disabled using a compiler switch,
// even if the `throw` statement is unlikely to be executed at run-time or if
// the `catch` clause can safely ignore an exception that will never occur.
// This component provides macros to replace `try`, `throw` and `catch`.
// These macros expand to normal exception constructs when exceptions are
// enabled and reasonable alternatives (usually no-ops) when exceptions are
// disabled.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using `bsls_exceptionutil` to Implement `vector`
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we wanted to define an implementation of a standard-defined `vector`
// template.  Unfortunately, the C++ standard requires that `vector` provide an
// `at` method that throws an `out_of_range` exception if the supplied index is
// not in the valid range of elements in the vector.  In this example we show
// using `BSLS_THROW` so that such an implementation will compile in both
// exception enabled and non-exception enabled builds.  Note that apart from
// memory allocation, and where required by the C++ standard, types defined in
// the BDE libraries do not throw exceptions, and are typically
// "exception-neutral" (see {`bsldoc_glossary`}), meaning they behave
// reasonably in the presence of injected exceptions, but do not themselves
// throw any exceptions.
//
// First we open a namespace `myStd` and define an `out_of_range` exception
// that the `at` method will throw (note that in practice, `out_of_range` would
// inherit from `logic_error`)':
// ```
// namespace myStd {
//
// class out_of_range  // ...
// {
//    // ...
// };
// ```
// Next, we declare the `vector` template and its template parameters (note
// that the majority of the implementation is elided, for clarity):
// ```
// template <class VALUE, class ALLOCATOR /* ... */>
// class vector {
//     // DATA
//     VALUE *d_begin_p;
//     VALUE *d_end_p;
//     // ...
//
//   public:
//
//     typedef typename ALLOCATOR::size_type size_type;
//
//     //...
// ```
// Then, we define the `at` method, which is required to throw an
// `out_of_range` exception.
// ```
//     const VALUE& at(size_type index) const
//     {
//         if (d_begin_p + index < d_end_p) {
//             return d_begin_p[index];                              // RETURN
//         }
// ```
// Now, we use `BSLS_THROW` to throw an `out_of_range` exception:
// ```
//         BSLS_THROW(out_of_range(/* ... */));
//     }
// ```
// Finally, we complete the (mostly elided) `vector` implementation:
// ```
//     // ...
//
// };
//
// }  // close namespace myStd
//
// struct DummyAllocator {
//     typedef int size_type;
// };
// ```
//
///Example 2: Using `bsls_exceptionutil` to Throw and Catch Exceptions
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates the macros defined in the
// `bsls_exceptionutil` component to both throw and catch exceptions in a way
// that will allow the code to compile in non-exception enabled builds.
//
// First, we define a couple of example exception classes (note that we cannot
// use `bsl::exception` in this example, as this component is defined below
//`bsl_exception.h`):
// ```
// class my_ExClass1
// {
// };
//
// class my_ExClass2
// {
// };
// ```
// Then, we define a function that never throws an exception, and use the
// `BSLS_NOTHROW_SPEC` to ensure the no-throw exception specification will be
// present in exception enabled builds, and elided in non-exception enabled
// builds:
// ```
// int noThrowFunc() BSLS_NOTHROW_SPEC
// {
//     return -1;
// }
// ```
// Next, we define a function that might throw `my_ExClass1` or `my_ExClass2`,
// and docuemnt which exception types might be thrown.  Note that dynamic
// exception specifications are deprecated in C++11 and removed from the
// language in C++17, so should not be used as a substitute for documentation
// in earlier language dialects:
// ```
// /// Return the specified integer `i`, unless `1 == 1` or `2 == i`.  If
// /// `1 == i` throw an exception of type `my_ExcClass1`.  If `2 == i`
// /// throw an exception of type `my_ExcClass2`.  Note that if exceptions
// /// are not enabled in the current build mode, then the program will
// /// `abort` rather than throw.
// int mightThrowFunc(int i)
// {
//     switch (i) {
//       case 0: break;
//       case 1: BSLS_THROW(my_ExClass1());
//       case 2: BSLS_THROW(my_ExClass2());
//     }
//     return i;
// }
// ```
// Then, we start the definition of a `testMain` function:
// ```
// int testMain()
// {
// ```
// Next, we use the `BDE_BUILD_TARGET_EXC` exception build flag to determine,
// at compile time, whether to initialize `ITERATIONS` to 3 (for exception
// enabled builds) or 1 (for non-exception enabled builds).  The different
// values of the `ITERATOR` ensure the subsequent for-loop calls
// `mightThrowFunc` in a way that generates exceptions for only exception
// enabled builds:
// ```
// #ifdef BDE_BUILD_TARGET_EXC
//     const int ITERATIONS = 3;
// #else
//     const int ITERATIONS = 1;
// #endif
//
//     for (int i = 0; i < ITERATIONS; ++i) {
// ```
// Then, we use a pair of nested `try` blocks constructed using
// `BSLS_TRY`, so that the code will compile whether or not exceptions are
// enabled (note that the curly brace placement is identical to normal
// `try` and `catch` constructs):
// ```
//         int caught = -1;
//         BSLS_TRY {
//
//             BSLS_TRY {
//                 noThrowFunc();
//                 mightThrowFunc(i);
// ```
// Notice that this example is careful to call `mightThrowFunc` in such a way
// that it will not throw in non-exception builds.  Although the use of
// `BSLS_TRY`, `BSLS_THROW`, and `BSLS_CATCH` ensures the code *compiles* in
// both exception, and non-exception enabled builds, attempting to `BSLS_THROW`
// an exception in a non-exception enabled build will invoke the assert handler
// and will typically abort the task.
// ```
//                 caught = 0; // Got here if no throw
//             }
// ```
// Next, we use `BSLS_CATCH` to define blocks for handling exceptions that may
// have been thrown from the preceding `BSLS_TRY`:
// ```
//             BSLS_CATCH(my_ExClass1) {
//                 caught = 1;
//             }
//             BSLS_CATCH(...) {
// ```
// Here, within the catch-all handler, we use the `BSLS_RETHROW` macro to
// re-throw the exception to the outer `try` block:
// ```
//                 BSLS_RETHROW;
//             } // end inner try-catch
//         }
//         BSLS_CATCH(my_ExClass2) {
//             caught = 2;
//         }
//         BSLS_CATCH(...) {
//             assert("Should not get here" && 0);
//         } // end outer try-catch
//
//         if (0 != caught) {
//             if (verbose)
//             printf("Caught exception my_ExClass: %d\n", caught);
//         }
//         else {
//             if (verbose)
//             printf("Caught no exceptions: %d\n", caught);
//         }
//         assert(i == caught);
//
//     } // end for (i)
//
//     return 0;
// }
// ```

#include <bsls_assert.h>
#include <bsls_buildtarget.h>
#include <bsls_compilerfeatures.h>
#include <bsls_deprecate.h>
#include <bsls_libraryfeatures.h>

                                 // ======
                                 // macros
                                 // ======

#ifdef BDE_BUILD_TARGET_EXC

                           // ------------------
                           // Exceptions enabled
                           // ------------------

#   define BSLS_TRY try
        // Start a try block.
        // Exceptions enabled: 'try'
        // Exceptions disabled: start of a normal (non-try) block

#   define BSLS_CATCH(X) catch (X)
        // Catch exception 'X'.  'X' must be a type with optional variable name
        // Exceptions enabled: 'catch (X)'
        // Exceptions disabled: ignore following block

#   define BSLS_THROW(X) throw X
        // Throw exception object 'X'.
        // Exceptions enabled: 'throw (X)'
        // Exceptions disabled: abort with a message

#   define BSLS_RETHROW throw
        // Within a 'BSLS_CATCH' clause, re-throw the last exception
        // Exceptions enabled: 'throw'
        // Exceptions disabled: abort with a message

# if !BSLS_DEPRECATE_IS_ACTIVE(BDE, 3, 17)
#   if defined(BSLS_COMPILERFEATURES_SUPPORT_THROW_SPECIFICATIONS)
      /// @DEPRECATED: This macro is deprecated, as the language feature itself
      /// is deprecated in C++11, and is entirely removed in C++17.  It is
      /// recommended to simply document the potential set of exceptions that
      /// may be thrown by a function instead.
      ///
      /// Declare a dynamic exception specification for a function.
      /// Usage:
      /// ```
      /// void f() BSLS_EXCEPTION_SPEC((std::logic_error));
      /// ```
      /// `SPEC` must be a comma-separated list of one or more exception
      /// types enclosed in parenthesis.  (Double parenthesis are mandatory)
      /// Use `BSLS_NOTHROW_SPEC` (below) to declare that a function does not
      /// throw.
      /// Exceptions enabled: `throw SPEC`
      /// Exceptions disabled: empty
#     define BSLS_EXCEPTION_SPEC(SPEC) throw SPEC
#   else
      /// @DEPRECATED: This macro is deprecated, and cannot expand to a
      /// supported syntax on the current platform.
#     define BSLS_EXCEPTION_SPEC(SPEC)
#   endif  // BSLS_COMPILERFEATURES_SUPPORT_THROW_SPECIFICATIONS
# endif // BSLS_DEPRECATE_IS_ACTIVE

        /// Declare that a function does not throw any exceptions:
        /// Usage:
        /// ```
        /// void f() BSLS_NOTHROW_SPEC;
        /// ```
        /// Exceptions enabled: `throw ()` or `noexcept`
        /// Exceptions disabled: empty
#   if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
#       define BSLS_NOTHROW_SPEC noexcept
#   else
#       define BSLS_NOTHROW_SPEC throw ()
#   endif

    /// The exception specification that overrides of the virtual destructor
    /// and the `exception::what()` virtual method should use.  It is a
    /// separate macro from `BSLS_NOTHROW_SPEC` because the GNU library
    /// unconditionally declares the function `throw()`, regardless if
    /// exceptions are enabled or not - and overrides must do the same.
#   define BSLS_EXCEPTION_VIRTUAL_NOTHROW BSLS_NOTHROW_SPEC

    /// More specialized name for the no-throw declaration of the
    /// `exception::what()` method.  Use `BSLS_EXCEPTION_VIRTUAL_NOTHROW`
    /// instead.
#   define BSLS_EXCEPTION_WHAT_NOTHROW BSLS_EXCEPTION_VIRTUAL_NOTHROW

#else // If exceptions are disabled

                           // -------------------
                           // Exceptions disabled
                           // -------------------

#   define BSLS_TRY if (1)

#   define BSLS_CATCH(X) else if (0)

#   define BSLS_THROW(X) BSLS_ASSERT_INVOKE_NORETURN(                        \
                                                "Tried to throw " #X         \
                                                " with exceptions disabled");

#   define BSLS_RETHROW BSLS_ASSERT_INVOKE_NORETURN(                         \
                                           "Tried to re-throw exception "    \
                                           "with exceptions disabled");

# if !BSLS_DEPRECATE_IS_ACTIVE(BDE, 3, 17)
#   define BSLS_EXCEPTION_SPEC(SPEC)
# endif // BSLS_DEPRECATE_IS_ACTIVE

#   define BSLS_NOTHROW_SPEC

    /// The exception specification that overrides of the
    /// `exception::what()` virtual method should use.  It is a separate
    /// macro from `BSLS_NOTHROW_SPEC` because the GNU library
    /// unconditionally declares the function `throw()`, regardless if
    /// exceptions are enabled or not - and overrides must do the same.
#   if defined(BSLS_LIBRARYFEATURES_STDCPP_GNU) || \
       defined(BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD) || \
       defined(BSLS_LIBRARYFEATURES_STDCPP_IBM)
#       if defined(BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT)
#           define BSLS_EXCEPTION_VIRTUAL_NOTHROW noexcept
#       else
#           define BSLS_EXCEPTION_VIRTUAL_NOTHROW throw ()
#       endif
#   else
#       define BSLS_EXCEPTION_VIRTUAL_NOTHROW
#   endif


#endif // BDE_BUILD_TARGET_EXC

namespace BloombergLP {
}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLS_EXCEPTIONUTIL)

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
