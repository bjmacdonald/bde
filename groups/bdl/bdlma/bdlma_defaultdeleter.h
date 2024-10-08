// bdlma_defaultdeleter.h                                             -*-C++-*-
#ifndef INCLUDED_BDLMA_DEFAULTDELETER
#define INCLUDED_BDLMA_DEFAULTDELETER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a concrete default deleter w/optionally-supplied allocator.
//
//@CLASSES:
//   bdlma::DefaultDeleter: concrete default `bdema`-based deleter
//
//@SEE_ALSO: bslma_allocator, bdlma_deleter
//
//@DESCRIPTION: This component provides a default concrete implementation of
// the `bdlma::Deleter` protocol:
// ```
//                     ( bdlma::DefaultDeleter )
//                                |       ctor
//                                v
//                        ( bdlma::Deleter )
//                                        dtor
//                                        deleteObject
// ```
// Upon construction, a `bdlma::DefaultDeleter` is optionally supplied with a
// `bdema`-style allocator.  If an allocator is not specified, the currently
// installed default allocator is used instead.  The memory footprint of
// objects that are subsequently deleted via calls to the `deleteObject` method
// of the deleter will be returned to the allocator that was established at
// construction.  Note that the allocator used to create the footprint of
// objects passed to `deleteObject` *must* be the same allocator that is used
// by the deleter.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// Suppose that we would like to transfer ownership of an object between
// threads using `bsl::shared_ptr`.  For the sake of discussion, the type of
// this object is `my_Obj` and we will suppose that it is created using a given
// `basicAllocator`.  Note that we assume that `my_Obj` does not require an
// allocator for any of its members:
// ```
// bslma::NewDeleteAllocator basicAllocator;
// my_Obj *object = new(basicAllocator) my_Obj;
// ```
// Next, create a concrete deleter for `object` using the same allocator as was
// used to allocate its footprint:
// ```
// bdlma::DefaultDeleter<my_Obj> deleter(&basicAllocator);
// ```
// Finally, create a shared pointer passing to it `object` and the address of
// `deleter`:
// ```
// bsl::shared_ptr<my_Obj> handle(object, &deleter, &basicAllocator);
// ```
// Now the `handle` can be passed to another thread or enqueued efficiently.
// When the reference count of `handle` goes to 0, `object` is automatically
// deleted via the `deleteObject` method of `deleter`, which in turn will
// invoke the destructor of `object`.  Note that since the type of the deleter
// used to instantiate `handle` is `bdlma::Deleter<my_Obj>`, any kind of
// deleter that implements this protocol can be passed.  Also note, on the
// downside, that the lifetime of `deleter` must be longer than the lifetime of
// all associated instances.

#include <bdlscm_version.h>

#include <bdlma_deleter.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_integralconstant.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslalg_typetraits.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace BloombergLP {
namespace bdlma {

                           // ====================
                           // class DefaultDeleter
                           // ====================

/// This `class` provides a default concrete implementation of the `Deleter`
/// protocol.  Instances of `DefaultDeleter<TYPE>` either use an allocator
/// optionally supplied at construction, or the currently installed default
/// allocator if an allocator is not provided.
template <class TYPE>
class DefaultDeleter : public Deleter<TYPE> {

    // DATA
    bslma::Allocator *d_allocator_p;  // memory allocator (held, *not* owned)

    // NOT IMPLEMENTED
    DefaultDeleter(const DefaultDeleter<TYPE>&);
    DefaultDeleter<TYPE>& operator=(const DefaultDeleter<TYPE>&);

  public:
    // CREATORS

    /// Create a default deleter.  Optionally specify a `basicAllocator`
    /// used to manage the memory footprint of objects passed to the
    /// `deleteObject` method.  If `basicAllocator` is 0, the currently
    /// installed default allocator is used.
    DefaultDeleter(bslma::Allocator *basicAllocator = 0);

    /// Destroy this default deleter.
    virtual ~DefaultDeleter();

    // MANIPULATORS

    /// Destroy the specified `instance` based on its static type and return
    /// its memory footprint to the appropriate memory manager.  The
    /// behavior is undefined unless the memory for the footprint of
    /// `instance` was supplied by the same allocator as is used by this
    /// default deleter.  Note that this method does not destroy the deleter
    /// itself.
    virtual void deleteObject(TYPE *instance);
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

// CREATORS
template <class TYPE>
inline
DefaultDeleter<TYPE>::DefaultDeleter(bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <class TYPE>
DefaultDeleter<TYPE>::~DefaultDeleter()
{
}

// MANIPULATORS
template <class TYPE>
inline
void DefaultDeleter<TYPE>::deleteObject(TYPE *instance)
{
    d_allocator_p->deleteObject(instance);
}

}  // close package namespace
}  // close enterprise namespace

// TRAITS

namespace BloombergLP {
namespace bslma {

template <class TYPE>
struct UsesBslmaAllocator<bdlma::DefaultDeleter<TYPE> > : bsl::true_type {};

}  // close namespace bslma
}  // close enterprise namespace

#endif

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
