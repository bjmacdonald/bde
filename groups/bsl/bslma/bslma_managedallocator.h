// bslma_managedallocator.h                                           -*-C++-*-
#ifndef INCLUDED_BSLMA_MANAGEDALLOCATOR
#define INCLUDED_BSLMA_MANAGEDALLOCATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a protocol for memory allocators that support `release`.
//
//@INTERNAL_DEPRECATED: use `bdlma_managedallocator` instead.
//
//@CLASSES:
//  bslma::ManagedAllocator: protocol for allocators with `release` capability
//
//@SEE_ALSO: bslma_allocator
//
//@DESCRIPTION: This component provides a protocol (pure abstract interface)
//  class, `bslma::ManagedAllocator`, which extends the base-level protocol
//  class, `bslma::Allocator`, providing the ability to `release` all memory
//  currently allocated through the protocol back to the memory supplier of the
//  derived concrete allocator object.
// ```
//  ,-----------------------.
// ( bslma::ManagedAllocator )
//  `-----------------------'
//              |       release
//              V
//      ,----------------.
//     ( bslma::Allocator )
//      `----------------'
//                      allocate
//                      deallocate
// ```
//
///Usage
///-----
// The `bslma::ManagedAllocator` protocol class serves as a useful internal
// interface for documentation purpose and could be used as a parameter to
// low-level helper functions for some implementations.  We have yet to find a
// suitable real-world example and when one becomes available, it will be
// added.

#ifdef BDE_OPENSOURCE_PUBLICATION // DEPRECATED
#error "bslma_managedallocator is deprecated"
#endif
#include <bslscm_version.h>

#include <bslma_allocator.h>

namespace BloombergLP {

namespace bslma {

                        // ======================
                        // class ManagedAllocator
                        // ======================

/// Provide a protocol for allocators with the ability to `release` all
/// memory currently allocated through the protocol back to the memory
/// supplier of the derived concrete allocator object.
class ManagedAllocator : public Allocator {

  private:
    // PRIVATE ACCESSORS

    /// Do nothing.  Note that this function is added to avoid including a
    /// (redundant) vtable into every translation unit that includes this
    /// type.  Although we expect that these redundant vtables, identified
    /// by the `-Wweak-vtables` warning of the clang compiler (version 4.0
    /// and higher), will be consolidated by the linker, this workaround
    /// avoids the space being used in the generated object files, which may
    /// be important for very heavily used types like this one.
    /// Implementing a virtual function out-of-line enables the compiler to
    /// use this component translation unit as a "home" for the single
    /// shared copy of the vtable.
    virtual void vtableDummy() const;

  public:
    // MANIPULATORS

    /// Release all memory currently allocated through this allocator.
    virtual void release() = 0;
};

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

/// This alias is defined for backward compatibility.
typedef bslma::ManagedAllocator bslma_ManagedAllocator;
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
