// bslma_destructorguard.h                                            -*-C++-*-
#ifndef INCLUDED_BSLMA_DESTRUCTORGUARD
#define INCLUDED_BSLMA_DESTRUCTORGUARD

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a guard to unconditionally manage an object.
//
//@CLASSES:
//  bslma::DestructorGuard: guard to unconditionally manage an object
//
//@SEE_ALSO: bslma_destructorproctor, bslma_autodestructor
//
//@DESCRIPTION: This component provides a guard class template,
// `bslma::DestructorGuard`, to unconditionally manage an (otherwise-unmanaged)
// object of parameterized `TYPE` supplied at construction.  The managed object
// is destroyed automatically when the guard object goes out of scope by
// calling the (managed) object's destructor.
//
///Usage
///-----
// Suppose we have a situation where one of the two constructors will be called
// to create an object on the stack for performance reasons.  The construction
// thus occurs within either of the branches of an `if` statement, so the
// object itself, to survive the end of the "then" or "else" block, must be
// constructed in a `bsls::ObjectBuffer`.  Once constructed, the object would
// not be destroyed automatically, so to make sure it will be destroyed, we
// place it under the management of a `bslma::DestructorGuard`.  After that, we
// know that however the routine exits -- either by a return or as a result of
// an exception being thrown -- the object will be destroyed.
// ```
// double usageExample(double startValue)
// {
//     bsls::ObjectBuffer<std::vector<double> > buffer;
//     std::vector<double>& myVec = buffer.object();
//
//     if (startValue >= 0) {
//         new (&myVec) std::vector<double>(100, startValue);
//     }
//     else {
//         new (&myVec) std::vector<double>();
//     }
//
//     //***********************************************************
//     // Note the use of the destructor guard on 'myVec' (below). *
//     //***********************************************************
//
//     bslma::DestructorGuard<std::vector<double> > guard(&myVec);
// ```
// Note that regardless of how this routine terminates, `myVec` will be
// destroyed.
// ```
// // ...
//
// myVec.push_back(3.0);
// ```
// Note that `push_back` could allocate memory and therefore may throw.
// However, if it does, `myVec` will be destroyed automatically along with
// `guard`.
// ```
// if (myVec[0] >= 5.0) {
//     return 5.0;                                               // RETURN
// ```
// Note that `myVec` is automatically destroyed as the function returns.
// ```
// }
//
// return myVec[myVec.size() / 2];
// ```
// Note that `myVec` is destroyed after the temporary containing the return
// value is created.
// ```
// }
// ```

#include <bslscm_version.h>

#include <bsls_assert.h>

namespace BloombergLP {

namespace bslma {

                        // =====================
                        // class DestructorGuard
                        // =====================

/// This class implements a guard that unconditionally destroys a managed
/// object upon destruction by invoking the (managed) object's destructor.
template <class TYPE>
class DestructorGuard {

    // DATA
    TYPE *d_object_p;  // managed object

    // NOT IMPLEMENTED
    DestructorGuard(const DestructorGuard<TYPE>&);
    DestructorGuard<TYPE>& operator=(const DestructorGuard<TYPE>&);

  public:
    // CREATORS

    /// Create a destructor guard that unconditionally manages the specified
    /// `object`, and invokes the destructor of `object` upon the
    /// destruction of this guard.  The behavior is undefined unless
    /// `object` is non-zero.
    explicit DestructorGuard(TYPE *object);

    /// Destroy this destructor guard and the object it manages by invoking
    /// the destructor of the (managed) object.
    ~DestructorGuard();
};

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

                        // ---------------------
                        // class DestructorGuard
                        // ---------------------

// CREATORS
template <class TYPE>
inline
DestructorGuard<TYPE>::DestructorGuard(TYPE *object)
: d_object_p(object)
{
    BSLS_ASSERT_SAFE(object);
}

template <class TYPE>
inline
DestructorGuard<TYPE>::~DestructorGuard()
{
    BSLS_ASSERT_SAFE(d_object_p);

    d_object_p->~TYPE();
}

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

#ifdef bslma_DestructorGuard
#undef bslma_DestructorGuard
#endif
/// This alias is defined for backward compatibility.
#define bslma_DestructorGuard bslma::DestructorGuard
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
