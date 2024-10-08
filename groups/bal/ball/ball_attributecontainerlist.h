// ball_attributecontainerlist.h                                      -*-C++-*-
#ifndef INCLUDED_BALL_ATTRIBUTECONTAINERLIST
#define INCLUDED_BALL_ATTRIBUTECONTAINERLIST

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a list of attribute container addresses.
//
//@CLASSES:
//          ball::AttributeContainerList: a list of container addresses
//  ball::AttributeContainerListIterator: an iterator over a container list
//
//@SEE_ALSO: ball_attribute, ball_attributecontainer
//
//@DESCRIPTION: This component defines a class `ball::AttributeContainerList`
// that provides a linked list of `ball::AttributeContainer` object
// *addresses*.  Addresses can be prepended (to the front of the list) using
// the `pushFront()` method.  The `pushFront()` method returns an iterator that
// can be used later to efficiently remove the added element.  The
// `ball::AttributeContainerList` also provides a `hasValue()` operation, that
// returns `true` if any of the attribute containers in the list contain the
// supplied attribute, and `false` otherwise.  The
// `ball::AttributeContainerList` maintains a store of free list-nodes to
// minimize the amount of memory allocation required if addresses are
// frequently added and removed from the container.  This component also
// defines a class `ball::AttributeContainerListIterator` (as well as the alias
// `ball::AttributeContainerList::iterator)` that provides an STL-style
// iterator over the addresses in a `ball::AttributeContainer`.
//
// This component participates in the implementation of "Rule-Based Logging".
// For more information on how to use that feature, please see the package
// level documentation and usage examples for "Rule-Based Logging".
//
///Thread Safety
///-------------
// `ball::AttributeContainerList` is *const* *thread-safe*, meaning that
// accessors may be invoked concurrently from different threads, but it is not
// safe to access or modify a `ball::AttributeContainerList` in one thread
// while another thread modifies the same object.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage of `ball::AttributeContainerList`
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we demonstrate how to create a
// `ball::AttributeContainerList` object, how to add and remove elements from
// the list, and how to walk the list of attribute container addresses.
//
// We start by creating three attribute sets that we will use to populate our
// attribute container list.  Note that this example uses the `AttributeSet`
// implementation of the `ball::AttributeContainer` protocol defined in the
// `ball_attributecontainer` component documentation.
// ```
// AttributeSet s1, s2, s3;
// s1.insert(ball::AttributeValue("Set1", 1));
// s2.insert(ball::AttributeValue("Set2", 2));
// s3.insert(ball::AttributeValue("Set3", 3));
// ```
// We now create a `ball::AttributeContainerList` and add the three attribute
// container addresses to the list:
// ```
// ball::AttributeContainerList exampleList;
// ball::AttributeContainerList::iterator s1Iter = exampleList.pushFront(&s1);
// ball::AttributeContainerList::iterator s2Iter = exampleList.pushFront(&s2);
// ball::AttributeContainerList::iterator s3Iter = exampleList.pushFront(&s3);
// ```
// We can use the `hasValue()` operation to test which attribute value are
// contained within the list of containers:
// ```
// assert(true == exampleList.hasValue("Set1", 1));
// assert(true == exampleList.hasValue("Set2", 2));
// assert(true == exampleList.hasValue("Set3", 3));
//
// assert(false == exampleList.hasValue("Set1", 2));
// assert(false == exampleList.hasValue("Set2", 1));
// assert(false == exampleList.hasValue("Set4", 1));
// ```
// We can use the iterators to efficiently remove elements from the list:
// ```
// exampleList.remove(s3Iter);
// ```
// Finally, we can use either the stream operator or the `print()` method to
// print the attributes within an attribute container list:
// ```
// bsl::cout << exampleList << bsl::endl;
// ```
// The resulting output will be the following:
// ```
// [  [  [ Set2 = 2 ]  ]  [  [ Set1 = 1 ]  ] ]
// ```
// Note that the output shows the values in `s2` (i.e., `("Set2", 2)`) and
// then the values in `s1` (i.e., `("Set1", 1)`).

#include <balscm_version.h>

#include <bslma_allocator.h>
#include <bslma_bslallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ball {

class  Attribute;
class  AttributeContainer;
struct AttributeContainerList_Node;

              // ====================================
              // class AttributeContainerListIterator
              // ====================================

/// This class provides an STL-style iterator over a sequence of
/// `AttributeContainer` object addresses.  The behavior of the `operator*`
/// method is undefined unless the iterator is at a valid position in the
/// sequence of `AttributeContainer` object addresses (i.e., not the "end")
/// and the referenced element has not been removed since the iterator was
/// constructed.
class AttributeContainerListIterator {

    // PRIVATE TYPES
    typedef AttributeContainerList_Node Node;

    // DATA
    Node *d_node_p;  // current iterator location

    // FRIENDS
    friend bool operator==(const AttributeContainerListIterator&,
                           const AttributeContainerListIterator&);
    friend class AttributeContainerList;

  public:
    // CREATORS

    /// Create an uninitialized iterator.
    AttributeContainerListIterator();

    /// Create an iterator having the same value as the specified
    /// `original` one.
    AttributeContainerListIterator(
                               const AttributeContainerListIterator& original);

    /// Create an iterator at the specified `position`.
    AttributeContainerListIterator(AttributeContainerList_Node *position);

    /// Destroy this object.
    //! ~AttributeContainerListIterator() = default;

    // MANIPULATORS

    /// Assign this iterator the value of the specified `rhs` and return a
    /// modifiable reference to this iterator.
    AttributeContainerListIterator& operator=(
                                    const AttributeContainerListIterator& rhs);

    /// Advance this iterator to the next attribute container in the list
    /// and return the value of this iterator.  The behavior is undefined
    /// unless the iterator is at a valid position in the list.
    AttributeContainerListIterator operator++(int);

    /// Advance this iterator to the next attribute container in the list
    /// and return the value of the iterator prior to this method call.
    /// The behavior is undefined unless the iterator is at a valid
    /// position in the list.
    AttributeContainerListIterator operator++();

    // ACCESSORS

    /// Return the address of the non-modifiable attribute container at
    /// which this iterator is positioned.  The behavior is undefined unless
    /// this iterator is at a valid position in the list.
    const AttributeContainer *operator*() const;

    /// Return `true` if this iterator is at a valid position in the
    /// sequence of `AttributeContainer` addresses and `false` otherwise.
    bool valid() const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and the specified `rhs` iterators
/// have the same value and `false` otherwise.  Two iterators have the same
/// value if they refer to the same position in the same list, or if both
/// iterators are at an invalid position in the list (i.e., the "end" of the
/// list, or the default constructed value).
bool operator==(const AttributeContainerListIterator& lhs,
                const AttributeContainerListIterator& rhs);

/// Return `true` if the specified `lhs` and the specified `rhs` iterators
/// do not have the same value and `false` otherwise.  Two iterators do not
/// have the same value if they differ in either the list to which they
/// refer or the position in the list object.
bool operator!=(const AttributeContainerListIterator& lhs,
                const AttributeContainerListIterator& rhs);

                    // ============================
                    // class AttributeContainerList
                    // ============================

/// This class provides an in-core value-semantic list of
/// `AttributeContainer` object addresses.  Attribute container addresses
/// are added to this list using `pushFront()`, which returns an iterator
/// located at the new element.  A `AttributeContainerList::iterator` object
/// remains valid until the element referred to by the iterator is removed.
/// Attribute container addresses can be removed using either `remove()`,
/// `removeAll()`, or `removeAllAndRelease()`.  This list object attempts to
/// minimize the number of memory allocations it requires by placing the
/// memory for elements that have been released in a free memory store, and
/// re-using the memory when new elements are added.  The `removeAll()`
/// removes all the elements from the list, but does not release any
/// allocated memory (placing it in the free store).  The
/// `removeAllAndRelease()` operation removes all elements and releases all
/// allocated memory.  Note that maintaining a free store is important for
/// this component because the expectation is that elements will be both
/// added and removed frequently.
class AttributeContainerList {
  public:
    // TYPES
    typedef bsl::allocator<char> allocator_type;

  private:
    // PRIVATE TYPES
    typedef AttributeContainerList_Node Node;

    // DATA
    Node             *d_head_p;       // head of the linked list of elements
    Node             *d_free_p;       // head of the free store
    int               d_length;       // length of the list
    allocator_type    d_allocator;    // allocator

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(AttributeContainerList,
                                   bslma::UsesBslmaAllocator);

    // PUBLIC TYPES

    /// An iterator over this list.
    typedef AttributeContainerListIterator iterator;

    // CREATORS

    /// Create an empty container list.  Optionally specify an `allocator`
    /// (e.g., the address of a `bslma::Allocator` object) to supply memory;
    /// otherwise, the default allocator is used.
    AttributeContainerList();
    explicit AttributeContainerList(const allocator_type& allocator);

    /// Create a container list having the same value as the specified
    /// `original`.  Optionally specify an `allocator` (e.g., the address of
    /// a `bslma::Allocator` object) to supply memory; otherwise, the
    /// default allocator is used.
    AttributeContainerList(
                   const AttributeContainerList& original,
                   const allocator_type&         allocator = allocator_type());

    /// Destroy this object.
    ~AttributeContainerList();

    // MANIPULATORS

    /// Assign this container list the value of the specified `rhs` one, and
    /// return a reference to this list.
    AttributeContainerList& operator=(const AttributeContainerList& rhs);

    /// Prepend the address of the specified `container` to this list of
    /// attribute container addresses, and return an iterator located at
    /// the newly added list element.
    iterator pushFront(const AttributeContainer *container);

    /// Remove the specified `element` from this list.  The behavior is
    /// undefined unless `element` is a valid iterator on this list and the
    /// referenced address has not previously been removed (by either
    /// `remove()`, `removeAll()`, or `removeAllAndRelease()`.
    void remove(const iterator& element);

    /// Remove all the elements from this list.  After this operation
    /// returns, `numContainers()` will be 0.  This operation adds all the
    /// allocated memory to an internal free store, and does not deallocate
    /// any memory.
    void removeAll();

    /// Remove all the elements from this list and deallocate any allocated
    /// memory.
    void removeAllAndRelease();

    // ACCESSORS

    /// Return an iterator positioned at the beginning of the list of
    /// `AttributeContainer` object addresses represented by this object.
    iterator begin() const;

    /// Return an iterator positioned one past the final
    /// `AttributeContainer` object address in the list of addresses
    /// represented by this object.
    iterator end() const;

    /// Return the number of attribute container addresses currently in
    /// this list.
    int numContainers() const;

    /// Return `true` if the attribute having specified `value` exists in
    /// any of the attribute containers referred to by this object, and
    /// `false` otherwise.
    bool hasValue(const Attribute& value) const;

    /// Format this object to the specified output `stream` at the
    /// (absolute value of) the optionally specified indentation `level`
    /// and return a reference to `stream`.  If `level` is specified,
    /// optionally specify `spacesPerLevel`, the number of spaces per
    /// indentation level for this and all of its nested objects.  If
    /// `level` is negative, suppress indentation of the first line.  If
    /// `spacesPerLevel` is negative, format the entire output on one line,
    /// suppressing all but the initial indentation (as governed by
    /// `level`).  If `stream` is not valid on entry, this operation has no
    /// effect.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;

                                  // Aspects

    /// Return the allocator used by this object to supply memory.  Note
    /// that if no allocator was supplied at construction the default
    /// allocator in effect at construction is used.
    allocator_type get_allocator() const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` lists have the same
/// value, and `false` otherwise.  Two lists have the same value if they
/// have the same number of attribute container addresses, and the address
/// at each index position have the same value.
bool operator==(const AttributeContainerList& lhs,
                const AttributeContainerList& rhs);

/// Return `true` if the specified `lhs` and `rhs` lists do not have the
/// same value, and `false` otherwise.  Two lists do not have the same
/// value if have differing numbers of attribute container addresses or any
/// of the addresses at corresponding indices have different values.
inline
bool operator!=(const AttributeContainerList& lhs,
                const AttributeContainerList& rhs);

/// Write a formatted description of the specified `rhs` to the specified
/// `stream` and return a reference to the modifiable `stream`.
inline
bsl::ostream& operator<<(bsl::ostream&                 output,
                         const AttributeContainerList& rhs);

                 // =================================
                 // class AttributeContainerList_Node
                 // =================================

/// This is an implementation type of `AttributeContainerList` and should
/// not be used by clients of this package.  A `AttributeContainerList_Node`
/// represents a node in a `AttributeContainerList` object.
struct AttributeContainerList_Node {

    const AttributeContainer     *d_value_p;      // address value of this
                                                  // element

    AttributeContainerList_Node  *d_next_p;       // next element

    AttributeContainerList_Node **d_prevNextAddr_p;
                                                  // address of previous
                                                  // element's next pointer
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

              // ------------------------------------
              // class AttributeContainerListIterator
              // ------------------------------------

// CREATORS
inline
AttributeContainerListIterator::AttributeContainerListIterator()
: d_node_p(0)
{
}

inline
AttributeContainerListIterator::AttributeContainerListIterator(
                                         AttributeContainerList_Node *position)
: d_node_p(position)
{
}

inline
AttributeContainerListIterator::AttributeContainerListIterator(
                                const AttributeContainerListIterator& original)
: d_node_p(original.d_node_p)
{
}

// MANIPULATORS
inline
AttributeContainerListIterator&
AttributeContainerListIterator::operator=(
                                     const AttributeContainerListIterator& rhs)
{
    d_node_p = rhs.d_node_p;
    return *this;
}

inline
AttributeContainerListIterator
AttributeContainerListIterator::operator++(int)
{
    Node *current = d_node_p;
    d_node_p = d_node_p->d_next_p;
    return AttributeContainerListIterator(current);
}

inline
AttributeContainerListIterator
AttributeContainerListIterator::operator++()
{
    d_node_p = d_node_p->d_next_p;
    return AttributeContainerListIterator(d_node_p);
}

// ACCESSORS
inline
const AttributeContainer *
AttributeContainerListIterator::operator*() const
{
    return d_node_p->d_value_p;
}

inline
bool AttributeContainerListIterator::valid() const
{
    return 0 != d_node_p;
}

                    // ============================
                    // class AttributeContainerList
                    // ============================

// CREATORS
inline
AttributeContainerList::AttributeContainerList()
: d_head_p(0)
, d_free_p(0)
, d_length(0)
, d_allocator()
{
}

inline
AttributeContainerList::AttributeContainerList(const allocator_type& allocator)
: d_head_p(0)
, d_free_p(0)
, d_length(0)
, d_allocator(allocator)
{
}

inline
AttributeContainerList::~AttributeContainerList()
{
    removeAllAndRelease();
}

// ACCESSORS
inline
AttributeContainerList::iterator
AttributeContainerList::begin() const
{
    return iterator(d_head_p);
}

inline
AttributeContainerList::iterator AttributeContainerList::end() const
{
    return iterator(0);
}

inline
int AttributeContainerList::numContainers() const
{
    return d_length;
}

inline
AttributeContainerList::allocator_type
AttributeContainerList::get_allocator() const
{
    return d_allocator;
}

}  // close package namespace

// FREE OPERATORS
inline
bool ball::operator==(const AttributeContainerListIterator& lhs,
                      const AttributeContainerListIterator& rhs)
{
    return lhs.d_node_p == rhs.d_node_p;
}

inline
bool ball::operator!=(const AttributeContainerListIterator& lhs,
                      const AttributeContainerListIterator& rhs)
{
    return !(lhs == rhs);
}

inline
bool ball::operator!=(const AttributeContainerList& lhs,
                      const AttributeContainerList& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& ball::operator<<(bsl::ostream&                 output,
                               const AttributeContainerList& rhs)
{
    return rhs.print(output, 0, -1);
}

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
