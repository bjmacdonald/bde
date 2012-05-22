// bslalg_typetraitsgroupstlsequence.h                                -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITSGROUPSTLSEQUENCE
#define INCLUDED_BSLALG_TYPETRAITSGROUPSTLSEQUENCE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide facilities for grouping types with compile-time traits.
//
//@CLASSES:
//  bslalg::TypeTraitsGroupStlSequence: for STL sequence containers
//
//@SEE_ALSO: bslmf_typetraits
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides a facility for computing the trait
// class of STL sequence containers based on the traits of the underlying value
// type and allocator.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_HASTRAIT
#include <bslalg_hastrait.h>
#endif

#ifndef INCLUDED_BSLALG_PASSTHROUGHTRAIT
#include <bslalg_passthroughtrait.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISEMOVEABLE
#include <bslalg_typetraitbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITHASSTLITERATORS
#include <bslalg_typetraithasstliterators.h>
#endif

namespace BloombergLP {

namespace bslalg {

                  // ================================
                  // class TypeTraitsGroupStlSequence
                  // ================================

template <typename T, typename ALLOCATOR>
struct TypeTraitsGroupStlSequence :
           TypeTraitHasStlIterators,
           PassthroughTrait<ALLOCATOR, TypeTraitBitwiseMoveable>,
           PassthroughTraitBslmaAllocator<ALLOCATOR> {
    // Type traits for STL *sequence* containers of the parameterized type 'T'.
    // A sequence container is bitwise moveable if the allocator is bitwise
    // moveable.  It uses 'bslma' allocators if the parameterized 'ALLOCATOR'
    // is convertible from 'bslma::Allocator*'.
};

}  // close package namespace

// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslalg_TypeTraitsGroupStlSequence
#undef bslalg_TypeTraitsGroupStlSequence
#endif
#define bslalg_TypeTraitsGroupStlSequence bslalg::TypeTraitsGroupStlSequence
    // This alias is defined for backward compatibility.

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
