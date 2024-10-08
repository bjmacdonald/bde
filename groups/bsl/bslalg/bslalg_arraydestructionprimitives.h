// bslalg_arraydestructionprimitives.h                                -*-C++-*-
#ifndef INCLUDED_BSLALG_ARRAYDESTRUCTIONPRIMITIVES
#define INCLUDED_BSLALG_ARRAYDESTRUCTIONPRIMITIVES

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide primitive algorithms that destroy arrays.
//
//@CLASSES:
//  bslalg::ArrayDestructionPrimitives: namespace for array algorithms
//
//@SEE_ALSO: bslma_destructionutil, bslma_constructionutil
//
//@DESCRIPTION: This component provides utilities to destroy arrays with a
// uniform interface, but selecting a different implementation according to the
// traits possessed by the underlying type.
//
// The traits under consideration by this component are:
// ```
// Trait                             Note
// -----                             -------------------------------------
//  bslmf::IsBitwiseCopyable         Expressed in English as "TYPE has the
//                                   bit-wise copyable trait", or "TYPE is
//                                   bit-wise copyable", this trait also
//                                   implies that destructor calls can be
//                                   elided with no effect on observable
//                                   behavior.
//
// ```
//
///Usage
///-----
// TBD: maybe fix up usage example to show with allocator
// In this section we show intended use of this component.  Note that this
// component is for use by the `bslstl` package.  Other clients should use the
// STL algorithms (in header `<algorithm>` and `<memory>`).
//
///Example 1: Destroy Arrays of `int` and `Integer` Wrapper Objects
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we will use `bslalg::ArrayDestructionPrimitives` to destroy
// both an array of integer scalars and an array of `MyInteger` objects.
// Calling the `destroy` method on an array of integers is a no-op while
// calling the `destroy` method on an array of objects of `MyInteger` class
// invokes the destructor of each of the objects in the array.
//
// First, we define a `MyInteger` class that contains an integer value:
// ```
// /// This class represents an integer value.
// class MyInteger {
//
//     int d_intValue;  // integer value
//
//   public:
//     // CREATORS
//
//     /// Create a `MyInteger` object having integer value `0`.
//     MyInteger();
//
//     /// Create a `MyInteger` object having the specified `value`.
//     explicit MyInteger(int value);
//
//     /// Destroy this object.
//     ~MyInteger();
//
//     // ACCESSORS
//
//     /// Return the integer value contained in this object.
//     int getValue() const;
// };
// ```
// Then, we create an array of objects, `myIntegers`, of type `MyInteger` (note
// that we `bsls::ObjectBuffer` to allow us to safely invoke the destructor
// explicitly):
// ```
// bsls::ObjectBuffer<MyInteger> arrayBuffer[5];
// MyInteger *myIntegers = &arrayBuffer[0].object();
// for (int i = 0;i < 5; ++i) {
//     new (myIntegers + i) MyInteger(i);
// }
// ```
// Now, we define a primitive integer array:
// ```
// int scalarIntegers[] = { 0, 1, 2, 3, 4 };
// ```
// Finally, we use the uniform `bslalg::ArrayDestructionPrimitives:destroy`
// method to destroy both `myIntegers` and `scalarIntegers`:
// ```
// bslalg::ArrayDestructionPrimitives::destroy(myIntegers, myIntegers + 5);
// bslalg::ArrayDestructionPrimitives::destroy(scalarIntegers,
//                                             scalarIntegers + 5);
// ```

#include <bslscm_version.h>

#include <bsls_assert.h>
#include <bsls_types.h>

#include <bslma_allocatortraits.h>

#include <bslmf_isbitwisecopyable.h>

#include <stddef.h>  // 'size_t'
#include <cstring>  // 'memset', 'memcpy', and 'memmove'

namespace BloombergLP {

namespace bslalg {

                  // =================================
                  // struct ArrayDestructionPrimitives
                  // =================================

/// This `struct` provides a namespace for a suite of utility functions that
/// destroy arrays of elements of the parameterized type `TARGET_TYPE`.
/// Depending on the traits of `TARGET_TYPE`, the destructor may be invoked,
/// or not (optimized away by no-op).
struct ArrayDestructionPrimitives {

    // PRIVATE CLASS METHODS

    /// Destroy each instance of `TARGET_TYPE` in the array beginning at the
    /// specified `begin` address and ending immediately before the
    /// specified `end` address, using the specified `allocator`.  Elide the
    /// use of the destructor entirely if (template parameter) `TARGET_TYPE`
    /// is bitwise copyable, i.e., in the overload where the last argument
    /// (used only for overload resolution) is of type `bsl::true_type`.
    template <class TARGET_TYPE, class ALLOCATOR>
    static void destroy(TARGET_TYPE *begin,
                        TARGET_TYPE *end,
                        ALLOCATOR    allocator,
                        bsl::true_type);
    template <class TARGET_TYPE, class ALLOCATOR>
    static void destroy(TARGET_TYPE *begin,
                        TARGET_TYPE *end,
                        ALLOCATOR    allocator,
                        bsl::false_type);

    /// Destroy each instance of `TARGET_TYPE` in the array beginning at the
    /// specified `begin` address and ending immediately before the
    /// specified `end` address.  Elide the use of the destructor entirely
    /// if (template parameter) `TARGET_TYPE` is bitwise copyable, i.e. in
    /// the overload where the last argument (used only for overload
    /// resolution) if of type `bsl::true_type`.
    template <class TARGET_TYPE>
    static void destroy(TARGET_TYPE *begin, TARGET_TYPE *end, bsl::true_type);
    template <class TARGET_TYPE>
    static void destroy(TARGET_TYPE *begin, TARGET_TYPE *end, bsl::false_type);

    /// Overwrite the specified `numBytes` of memory starting at the
    /// specified `ptr`.
    static void scribbleOverMemory(void *ptr, size_t numBytes);

  public:
    // CLASS METHODS

    /// Destroy the elements in the segment of an array of parameterized
    /// `TARGET_TYPE` beginning at the specified `begin` address and ending
    /// immediately before the specified `end` address, using the specified
    /// `allocator`.  If `begin == 0` and `end == 0` this function has no
    /// effect.  The behavior is undefined unless either (1) `begin <= end`,
    /// `begin != 0`, and `end != 0`, or (2) `begin == 0 && end == 0`.  Note
    /// that this method does not deallocate any memory (except memory
    /// deallocated by the element destructor calls).
    template <class TARGET_TYPE, class ALLOCATOR>
    static void
    destroy(TARGET_TYPE *begin, TARGET_TYPE *end, ALLOCATOR allocator);

    /// Destroy of the elements in the segment of an array of parameterized
    /// `TARGET_TYPE` beginning at the specified `begin` address and ending
    /// immediately before the specified `end` address.  If `begin == 0` and
    /// `end == 0` this function has no effect.  The behavior is undefined
    /// unless either (1) `begin <= end`, `begin != 0`, and `end != 0`, or
    /// (2) `begin == 0 && end == 0`.  Note that this method does not
    /// deallocate any memory (except memory deallocated by the element
    /// destructor calls).
    template <class TARGET_TYPE>
    static void destroy(TARGET_TYPE *begin, TARGET_TYPE *end);
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                  // ---------------------------------
                  // struct ArrayDestructionPrimitives
                  // ---------------------------------

// PRIVATE CLASS METHODS
template <class TARGET_TYPE, class ALLOCATOR>
inline
void ArrayDestructionPrimitives::destroy(TARGET_TYPE       *begin,
                                         TARGET_TYPE       *end,
                                         ALLOCATOR,
                                         bsl::true_type)
{
    // 'bslmf::IsBitwiseCopyable' is a valid surrogate for having a trivial
    // destructor.

#ifdef BSLS_ASSERT_SAFE_IS_ACTIVE
    if (begin) {
        scribbleOverMemory(begin, (end - begin) * sizeof(TARGET_TYPE));
    }
#else
    (void) begin;
    (void) end;
#endif
}

template <class TARGET_TYPE, class ALLOCATOR>
void ArrayDestructionPrimitives::destroy(TARGET_TYPE *begin,
                                         TARGET_TYPE *end,
                                         ALLOCATOR    allocator,
                                         bsl::false_type)
{
    for (; begin != end; ++begin) {
        bsl::allocator_traits<ALLOCATOR>::destroy(allocator, begin);
    }
}

template <class TARGET_TYPE>
inline
void ArrayDestructionPrimitives::destroy(TARGET_TYPE *begin,
                                         TARGET_TYPE *end,
                                         bsl::true_type)
{
    // 'bslmf::IsBitwiseCopyable' is a valid surrogate for having a trivial
    // destructor.

#ifdef BSLS_ASSERT_SAFE_IS_ACTIVE
    scribbleOverMemory(begin, (end - begin) * sizeof(TARGET_TYPE));
#else
    (void) begin;
    (void) end;
#endif
}

template <class TARGET_TYPE>
void ArrayDestructionPrimitives::destroy(TARGET_TYPE *begin,
                                         TARGET_TYPE *end,
                                         bsl::false_type)
{
    for (; begin != end; ++begin) {
        begin->~TARGET_TYPE();
    }
}

// CLASS METHODS
template <class TARGET_TYPE, class ALLOCATOR>
inline
void ArrayDestructionPrimitives::destroy(TARGET_TYPE *begin,
                                         TARGET_TYPE *end,
                                         ALLOCATOR    allocator)
{
    BSLS_ASSERT_SAFE(begin || !end);
    BSLS_ASSERT_SAFE(end   || !begin);
    BSLS_ASSERT_SAFE(begin <= end);

    destroy(begin,
            end,
            allocator,
            typename bslmf::IsBitwiseCopyable<TARGET_TYPE>::type());
}

template <class TARGET_TYPE>
inline
void ArrayDestructionPrimitives::destroy(TARGET_TYPE *begin,
                                         TARGET_TYPE *end)
{
    BSLS_ASSERT_SAFE(begin || !end);
    BSLS_ASSERT_SAFE(end   || !begin);
    BSLS_ASSERT_SAFE(begin <= end);

    destroy(begin,
            end,
            typename bslmf::IsBitwiseCopyable<TARGET_TYPE>::type());
}

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

/// This alias is defined for backward compatibility.
typedef bslalg::ArrayDestructionPrimitives bslalg_ArrayDestructionPrimitives;

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
