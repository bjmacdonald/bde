// bslstl_forwarditerator.h                                           -*-C++-*-
#ifndef INCLUDED_BSLSTL_FORWARDITERATOR
#define INCLUDED_BSLSTL_FORWARDITERATOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a template to create STL-compliant forward iterators.
//
//@CLASSES:
//  bslstl::ForwardIterator: forward iterator template
//
//@CANONICAL_HEADER: bsl_iterator.h
//
//@SEE_ALSO: bslstl_iterator, bslstl_bidirectionaliterator,
//           bslstl_randomaccessiterator
//
//@DESCRIPTION: This component provides an iterator adaptor that, given an
// implementation class defining a core set of iterator functionality, adapts
// it to provide an STL-compliant forward iterator interface.  The set of
// requirements for a forward iterator is found in "Table 106: Forward iterator
// requirements", under the tag "[forward.iterators]".  (Note that this
// reference is sourced in N3092, a C++0x working paper; the actual table
// number may vary in the actual standard.)  Include bsl_iterator.h to use this
// component.
//
///Usage
///-----
// Given the following "iterator-like" implementation class:
// ```
// template <class T>
// class my_IteratorImp {
//   public:
//     // CREATORS
//     my_IteratorImp();
//     my_IteratorImp(const my_IteratorImp&);
//     ~my_IteratorImp();
//
//     // An additional value-constructor should be supplied that can be
//     // called by the unspecified container type, providing access to the
//     // container's internal data structure that is to be iterated over.
//     // This would typically be called by 'begin' and 'end'.
//
//     // MANIPULATORS
//     my_IteratorImp& operator=(const my_IteratorImp&);
//
//     void operator++();
//
//     // ACCESSORS
//     T& operator*() const;
// };
//
// template <class T>
// bool operator==(const my_IteratorImp<T>& lhs,
//                 const my_IteratorImp<T>& rhs);
// ```
// simply add the following two `typedef`s to any container class that provides
// `my_IteratorImp<T>` access, and the container will have STL-compliant
// forward iterators:
// ```
// typedef bslstl::ForwardIterator<T, my_IteratorImp<T> > iterator;
// typedef bslstl::ForwardIterator<const T, my_IteratorImp<T> >
//                                                             const_iterator;
// ```
// Note that the implementation for `const_iterator` is `my_IteratorImp<T>` and
// *not* `my_IteratorImp<const T>`, rather the `const` is added to the return
// value of `operator*` by way of conversion to the first template argument.

// DOCUMENTATION IS INCOMPLETE: Note that we need to say something about 'T'
// and iterator stability.  Incrementing two copies of 'T' must produce two
// iterators that compare equal, and refer to exactly the same object.  Without
// this guarantee, we cannot build the multi-pass property of Forward Iterator,
// and merely have an Input Iterator.  This is not something we can detect at
// compile time, and is prohibitively expensive to validate at runtime.  It is
// the sort of thing that should be validated by a Forward Iterator test suite,
// it could be what we really need, in addition to the adapters, is a
// generalized test case to validate that a type conforms to the iterator
// requirements.

#include <bslscm_version.h>

#include <bslstl_iterator.h>

#include <bslmf_islvaluereference.h>
#include <bslmf_removecv.h>
#include <bslmf_util.h>

#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_util.h>

#include <cstddef>
#include <iterator>

namespace BloombergLP {

namespace bslstl {

                    //======================
                    // class ForwardIterator
                    //======================

/// Given an `ITER_IMP` type that implements a minimal subset of an iterator
/// interface, this template generates a complete iterator that meets all of
/// the requirements of a "forward iterator" in the C++ standard.  If `T` is
/// const-qualified, then the resulting type is a const iterator.  `T` shall
/// not be a function, reference type or void.  `ITER_IMP` must provide
/// public operations so that, for objects `i` and `j` of type `ITER_IMP`,
/// the following operations are supported:
/// ```
///    ITER_IMP i;                             Default construction
///    ITER_IMP j(i);                          Copy construction
///    i = j                                   Assignment
///    ++i                                     Increment to next element
///    i == j // convertible to bool           Equality comparison
///    *i     // reference convertible to T&   Element access (dereference)
/// ```
template <class T, class ITER_IMP, class TAG_TYPE = std::forward_iterator_tag>
class ForwardIterator
#if defined(BSLS_LIBRARYFEATURES_STDCPP_LIBCSTD)
// Sun CC workaround: iterators must be derived from 'std::iterator' to work
// with the native std library algorithms.  However, 'std::iterator' is
// deprecated in C++17, so do not rely on derivation unless required, to avoid
// deprecation warnings on modern compilers.
    : public std::iterator<TAG_TYPE,
                           typename bsl::remove_cv<T>::type,
                           std::ptrdiff_t,
                           T *,
                           T&>
#endif
{

#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE) && \
    defined(BSLS_COMPILERFEATURES_SUPPORT_STATIC_ASSERT)
    static_assert(
        bsl::is_lvalue_reference<
                    decltype(*bslmf::Util::declval<const ITER_IMP&>())>::value,
        "Forward iterators must return a true reference to their element when "
                                                              "dereferenced.");
#endif

    // PRIVATE TYPES
    typedef typename bsl::remove_cv<T>::type UnCvqT;  // value type without
                                                      // 'const' and 'volatile'
                                                      // qualifications

    typedef ForwardIterator<UnCvqT, ITER_IMP, TAG_TYPE>
                                             ForwardNonConstIterator;

  private:
    // DATA
    ITER_IMP d_imp;  // externally-supplied implementation of iterator
                     // functionality

  public:
    // TYPES
    typedef UnCvqT                     value_type;
    typedef std::ptrdiff_t             difference_type;
    typedef T                         *pointer;
    typedef T&                         reference;
    typedef std::forward_iterator_tag  iterator_category;

    // CREATORS

    /// Construct the default value for this iterator type.  All default-
    /// constructed `ForwardIterator` objects represent non-dereferenceable
    /// iterators into the same empty range.  They do not have a singular
    /// value unless an object of the type specified by the template
    /// parameter `ITER_IMP` has a singular value after
    /// value-initialization.
    ForwardIterator();

    /// Construct a forward iterator having the specified `implementation`
    /// of the parameterized `ITER_IMP` type.
    ForwardIterator(const ITER_IMP& implementation);

    /// Create a `ForwardIterator` having the same value as the specified
    /// `original` iterator.  Note that this method's definition is compiler
    /// generated.
    //! ForwardIterator(const ForwardIterator& original);

    /// Construct a forward iterator from the specified `other` iterator of
    /// another (compatible) `ForwardIterator` type, e.g., a mutable
    /// iterator of the same type.  Note that this constructor may be the
    /// copy constructor (inhibiting the implicit declaration of a copy
    /// constructor above), or may be an additional overload.
    ForwardIterator(const ForwardNonConstIterator& other);

    /// Destroy this iterator.  Note that this method's definition is
    /// compiler generated.
    //! ~ForwardIterator();

    // MANIPULATORS

    /// Copy the value of the specified `rhs` to this iterator.  Return a
    /// reference to this modifiable object.  Note that this method's
    /// definition is compiler generated.
    //! ForwardIterator& operator=(const ForwardIterator& rhs);

    /// Copy the value of the specified `rhs` of another (compatible)
    /// `ForwardIterator` type, (e.g., a mutable iterator of the same type)
    /// to this iterator.  Return a reference to this modifiable object.
    /// Note that this method may be the copy-assignment operator
    /// (inhibiting the implicit declaration of a copy-assignment operator
    /// above), or may be an additional overload.
    ForwardIterator& operator=(const ForwardNonConstIterator& rhs);

    /// Increment to the next element.  Return a reference to this
    /// modifiable iterator.  The behavior is undefined if, on entry, this
    /// iterator has the past-the-end value for an iterator over the
    /// underlying sequence.
    ForwardIterator& operator++();

    /// Return a modifiable reference to the implementation object.
    ITER_IMP& imp();

    // ACCESSORS

    /// Return a reference to the current, modifiable element.  The behavior
    /// is undefined if this iterator has the past-the-end value for an
    /// iterator over the underlying sequence.
    T& operator*() const;

    /// Return a pointer to the current, modifiable element.  The behavior
    /// is undefined if this iterator has the past-the-end value for an
    /// iterator over the underlying sequence.
    T *operator->() const;

    /// Return a non-modifiable reference to the implementation object.
    const ITER_IMP& imp() const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` iterator has the same value as the
/// specified `rhs` iterator, and `false` otherwise.  Two iterators have the
/// same value if they refer to the same element, or both have the past-the-
/// end value for the underlying sequence.  The behavior is undefined unless
/// both iterators refer to the same underlying sequence.
template <class T1, class T2, class ITER_IMP, class TAG_TYPE>
bool operator==(const ForwardIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                const ForwardIterator<T2,ITER_IMP,TAG_TYPE>& rhs);

/// Return `true` if the specified `lhs` iterator does not have the same
/// value as the specified `rhs` iterator, and `false` otherwise.  Two
/// iterators do not have the same value if (1) they do not refer to the
/// same element and (2) both do not have the past-the-end iterator value
/// for the underlying sequence.  The behavior is undefined unless both
/// iterators refer to the same underlying sequence.
template <class T1, class T2, class ITER_IMP, class TAG_TYPE>
bool operator!=(const ForwardIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                const ForwardIterator<T2,ITER_IMP,TAG_TYPE>& rhs);

/// Increment the specified `iter` to the next element.  Return the previous
/// value of `iter`.  The behavior is undefined if, on entry, `iter` has the
/// past-the-end value for an iterator of the underlying sequence.
template <class T, class ITER_IMP, class TAG_TYPE>
ForwardIterator<T,ITER_IMP,TAG_TYPE>
operator++(ForwardIterator<T,ITER_IMP,TAG_TYPE>& iter, int);

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        //----------------------
                        // class ForwardIterator
                        //----------------------

// CREATORS
template <class T, class ITER_IMP, class TAG_TYPE>
inline
ForwardIterator<T,ITER_IMP,TAG_TYPE>::ForwardIterator()
: d_imp()
{
}

template <class T, class ITER_IMP, class TAG_TYPE>
inline
ForwardIterator<T,ITER_IMP,TAG_TYPE>::
ForwardIterator(const ITER_IMP& implementation)
: d_imp(implementation)
{
}

template <class T, class ITER_IMP, class TAG_TYPE>
inline
ForwardIterator<T,ITER_IMP,TAG_TYPE>::
ForwardIterator(const ForwardNonConstIterator& other)
: d_imp(other.imp())
{
}

// MANIPULATORS
template <class T, class ITER_IMP, class TAG_TYPE>
inline
ForwardIterator<T,ITER_IMP,TAG_TYPE> &
ForwardIterator<T,ITER_IMP,TAG_TYPE>::operator=(
                                            const ForwardNonConstIterator& rhs)
{
    d_imp = rhs.imp();
    return *this;
}

template <class T, class ITER_IMP, class TAG_TYPE>
inline
ForwardIterator<T,ITER_IMP,TAG_TYPE>&
ForwardIterator<T,ITER_IMP,TAG_TYPE>::operator++()
{
    ++this->d_imp;
    return *this;
}

template <class T, class ITER_IMP, class TAG_TYPE>
inline
ITER_IMP& ForwardIterator<T,ITER_IMP,TAG_TYPE>::imp()
{
    return d_imp;
}

// ACCESSORS
template <class T, class ITER_IMP, class TAG_TYPE>
inline
T& ForwardIterator<T,ITER_IMP,TAG_TYPE>::operator*() const
{
    return *d_imp;
}

template <class T, class ITER_IMP, class TAG_TYPE>
inline
T *ForwardIterator<T,ITER_IMP,TAG_TYPE>::operator->() const
{
    return BSLS_UTIL_ADDRESSOF(*d_imp);
}

template <class T, class ITER_IMP, class TAG_TYPE>
inline
const ITER_IMP&
ForwardIterator<T,ITER_IMP,TAG_TYPE>::imp() const
{
    return d_imp;
}

}  // close package namespace

// FREE OPERATORS
template <class T, class ITER_IMP, class TAG_TYPE>
inline
bslstl::ForwardIterator<T,ITER_IMP,TAG_TYPE>
bslstl::operator++(ForwardIterator<T,ITER_IMP,TAG_TYPE>& iter, int)
{
    ForwardIterator<T,ITER_IMP,TAG_TYPE> tmp(iter);
    ++iter;
    return tmp;
}

template <class T1, class T2, class ITER_IMP, class TAG_TYPE>
inline
bool bslstl::operator==(const ForwardIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                        const ForwardIterator<T2,ITER_IMP,TAG_TYPE>& rhs)
{
    return lhs.imp() == rhs.imp();
}

template <class T1,  class T2, class ITER_IMP, class TAG_TYPE>
inline
bool bslstl::operator!=(const ForwardIterator<T1,ITER_IMP,TAG_TYPE>& lhs,
                        const ForwardIterator<T2,ITER_IMP,TAG_TYPE>& rhs)
{
    return !(lhs == rhs);
}

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

#ifdef bslstl_ForwardIterator
#undef bslstl_ForwardIterator
#endif
/// This alias is defined for backward compatibility.
#define bslstl_ForwardIterator bslstl::ForwardIterator
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
