// bslstp_util.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLSTP_UTIL
#define INCLUDED_BSLSTP_UTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace for utility functions for STL functionality.
//
//@CLASSES:
//  bslstp::Util: namespace for utility functions
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component defines a class, 'bslstp::Util', that provides a
// namespace for utility functions used to implement STL functionality in the
// 'bslstp' package.
//
///Usage
///-----
// This component is for internal use only.  A usage example is not provided.

// Prevent 'bslstp' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "<bslstp_util.h> header can't be included directly in \
BSL_OVERRIDES_STD mode"
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_BSLMF_METAINT
#include <bslmf_metaint.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

namespace BloombergLP {

namespace bslstp {

                        // ==========
                        // class Util
                        // ==========

class Util {
    // Namespace for utility functions used to implement STL functionality.

    // PRIVATE TYPES
    template <class ALLOCATOR, int IS_BSLMA_ALLOC>
    struct AllocatorUtil {
        // Simplify template specializations and overloading.

        static ALLOCATOR copyContainerAllocator(const ALLOCATOR& rhsAlloc);
            // Return the appropriate allocator for use when copy-constructing
            // a container.  'rhsAlloc' is intended to be the allocator from
            // the container being copied.  If 'isBslmaAlloc' is of type
            // 'bslmf::MetaInt<1>' then ignore 'rhsAlloc' and return the
            // default allocator.  Otherwise, return 'rhsAlloc' unchanged.
    };

    template <class ALLOCATOR>
    struct AllocatorUtil<ALLOCATOR, 0> {
        // Specialization for non-'bslma' allocators.

        static ALLOCATOR copyContainerAllocator(const ALLOCATOR& rhsAlloc);
            // Return the appropriate allocator for use when copy-constructing
            // a container.  'rhsAlloc' is intended to be the allocator from
            // the container being copied.  If 'isBslmaAlloc' is of type
            // 'bslmf::MetaInt<1>' then ignore 'rhsAlloc' and return the
            // default allocator.  Otherwise, return 'rhsAlloc' unchanged.
    };

  public:
    // CLASS METHODS
    template <class ALLOCATOR>
    static ALLOCATOR copyContainerAllocator(const ALLOCATOR& rhsAlloc);
        // Return the appropriate allocator for use when copy-constructing a
        // container.  'rhsAlloc' is intended to be the allocator from the
        // container being copied.  If the 'ALLOCATOR' type uses 'bslma'
        // allocator semantics, then ignore 'rhsAlloc' and return the default
        // allocator.  Otherwise, return 'rhsAlloc' unchanged.

    template <class CONTAINER, class QUICKSWAP_FUNC>
    static void swapContainers(CONTAINER&            c1,
                               CONTAINER&            c2,
                               const QUICKSWAP_FUNC& quickswap);
        // Swap the contents of containers 'c1' and 'c2', correctly handling
        // the case where the two containers have different allocator values.
        // The 'quickswap' argument is a binary function object that is called
        // to quickly swap two 'CONTAINER' objects with matching allocators.
        // (The 'quickswap' object does not need to check that the allocator
        // matched -- it is guaranteed by the caller.)  Strong exception
        // guarantee: if the allocator or copy constructor of either container
        // throws an exception, then the values of 'c1' and 'c2' are left
        // unchanged.  The 'quickswap' function must not throw an exception.
};

}  // close package namespace

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

               //------------------------------------------------
               // struct AllocatorUtil<ALLOCATOR, IS_BSLMA_ALLOC>
               //------------------------------------------------

// PRIVATE CLASS METHODS
template <class ALLOCATOR, int IS_BSLMA_ALLOC>
inline
ALLOCATOR bslstp::Util::AllocatorUtil<ALLOCATOR, IS_BSLMA_ALLOC>::
copyContainerAllocator(const ALLOCATOR&)
{
    return ALLOCATOR();
}

                     //-----------------------------------
                     // struct AllocatorUtil<ALLOCATOR, 0>
                     //-----------------------------------

// PRIVATE CLASS METHODS
template <class ALLOCATOR>
inline
ALLOCATOR bslstp::Util::AllocatorUtil<ALLOCATOR, 0>::
copyContainerAllocator(const ALLOCATOR& rhsAlloc)
{
    return rhsAlloc;
}

namespace bslstp {

                              //-----------
                              // class Util
                              //-----------

// CLASS METHODS
template <class ALLOCATOR>
inline
ALLOCATOR Util::copyContainerAllocator(const ALLOCATOR& rhsAlloc)
{
    typedef typename
        bslmf::IsConvertible<bslma::Allocator*,ALLOCATOR>::Type IsBslma;

    return AllocatorUtil<ALLOCATOR, IsBslma::VALUE>::copyContainerAllocator(
                                                                     rhsAlloc);
}

template <class CONTAINER, class QUICKSWAP_FUNC>
void Util::swapContainers(CONTAINER&            c1,
                          CONTAINER&            c2,
                          const QUICKSWAP_FUNC& quickswap)
{
    typedef typename CONTAINER::allocator_type allocator_type;
    allocator_type alloc1 = c1.get_allocator();
    allocator_type alloc2 = c2.get_allocator();

    if (alloc1 == alloc2) {
        quickswap(c1, c2);
    }
    else {
        // Create copies of c1 and c2 using each-other's allocators Exception
        // leaves originals untouched.

        CONTAINER c1copy(c1, alloc2);
        CONTAINER c2copy(c2, alloc1);

        // Now use bit-wise swap (no exceptions thrown).

        quickswap(c1, c2copy);
        quickswap(c2, c1copy);
    }
}

}  // close package namespace

#ifndef BDE_OMIT_TRANSITIONAL  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

typedef bslstp::Util bslstp_Util;
    // This alias is defined for backward compatibility.
#endif  // BDE_OMIT_TRANSITIONAL -- BACKWARD_COMPATIBILITY

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
