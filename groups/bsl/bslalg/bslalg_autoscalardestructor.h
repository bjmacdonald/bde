// bslalg_autoscalardestructor.h                                      -*-C++-*-
#ifndef INCLUDED_BSLALG_AUTOSCALARDESTRUCTOR
#define INCLUDED_BSLALG_AUTOSCALARDESTRUCTOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a proctor for destroying an object.
//
//@CLASSES:
//  bslalg::AutoScalarDestructor: exception-neutrality proctor for an object
//
//@SEE_ALSO: bslma_autodestructor, bslma_destructionutil
//
//@DESCRIPTION: This component provides a proctor object to manage an
// otherwise-unmanaged instance of a user-defined type.  If not explicitly
// released, the instance managed by the proctor object is automatically
// destroyed by the proctor's destructor, using the `bslma::DestructionUtil`
// utility.
//
///Usage
///-----

#include <bslscm_version.h>

#include <bslma_destructionutil.h>

#include <bsls_assert.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslalg_scalardestructionprimitives.h>
#endif

namespace BloombergLP {

namespace bslalg {

                     // ==========================
                     // class AutoScalarDestructor
                     // ==========================

/// This `class` provides a specialized proctor object that, upon
/// destruction and unless the `release` method is called, destroys the
/// guarded object of the parameterized `OBJECT_TYPE`.
template <class OBJECT_TYPE>
class AutoScalarDestructor {

    // DATA
    OBJECT_TYPE *d_object_p;  // address of guarded object

  private:
    // NOT IMPLEMENTED
    AutoScalarDestructor(const AutoScalarDestructor&);
    AutoScalarDestructor& operator=(const AutoScalarDestructor&);

  public:
    // CREATORS

    /// Create a guard object, proctoring the specified `object` of the
    /// parameterized `OBJECT_TYPE` (if non-zero), that will invoke the
    /// destructor of `object` upon destruction.
    AutoScalarDestructor(OBJECT_TYPE *object);

    /// Call the destructor on the object of the parameterized `OBJECT_TYPE`
    /// that is proctored by this guard object, if any, and destroy this
    /// object.
    ~AutoScalarDestructor();

    // MANIPULATORS

    /// Set the address of the object proctored by this guard object to 0,
    /// thereby releasing from management the object currently managed by
    /// this proctor, if any.
    void release();

    /// Set this guard object to manage the specified `object`, thereby
    /// releasing from management any object currently managed by this
    /// proctor, if any.
    void reset(OBJECT_TYPE *object);
};

// ============================================================================
//                      TEMPLATE FUNCTION DEFINITIONS
// ============================================================================

                     // --------------------------
                     // class AutoScalarDestructor
                     // --------------------------

// CREATORS
template <class OBJECT_TYPE>
inline
AutoScalarDestructor<OBJECT_TYPE>::AutoScalarDestructor(OBJECT_TYPE *object)
: d_object_p(object)
{
}

template <class OBJECT_TYPE>
inline
AutoScalarDestructor<OBJECT_TYPE>::~AutoScalarDestructor()
{
    if (d_object_p) {
        bslma::DestructionUtil::destroy(d_object_p);
    }
}

// MANIPULATORS
template <class OBJECT_TYPE>
inline
void AutoScalarDestructor<OBJECT_TYPE>::release()
{
    d_object_p = 0;
}

template <class OBJECT_TYPE>
inline
void AutoScalarDestructor<OBJECT_TYPE>::reset(OBJECT_TYPE *object)
{
    d_object_p = object;
}

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

#ifdef bslalg_AutoScalarDestructor
#undef bslalg_AutoScalarDestructor
#endif
/// This alias is defined for backward compatibility.
#define bslalg_AutoScalarDestructor bslalg::AutoScalarDestructor
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
