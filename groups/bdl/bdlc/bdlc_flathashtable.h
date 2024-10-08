// bdlc_flathashtable.h                                               -*-C++-*-
#ifndef INCLUDED_BDLC_FLATHASHTABLE
#define INCLUDED_BDLC_FLATHASHTABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an open-addressed hash table like Abseil `flat_hash_map`.
//
//@CLASSES:
//  bdlc::FlatHashTable: open-addressed hash table like Abseil `flat_hash_map`
//
//@SEE_ALSO: bdlc_flathashmap, bdlc_flathashset
//
//@DESCRIPTION: This component provides the class template
// `bdlc::FlatHashTable`, which forms the underlying implementation of
// `bdlc::FlatHashMap` and `bdlc::FlatHashSet`.  It is based on the Abseil
// implementation of `flat_hash_map`.  The data structure is an open-addressed
// hash table.  (In "open addressing", entries are kept in an array, the hash
// value of an entry points to its possible initial location, and searches for
// an entry proceed by stepping to other positions in the table.  This differs
// from "separate chaining", in which the hash value is used to locate a
// "bucket" of entries that all have the same hash value.)
//
// This version differs from typical open-addressing schemes in that it uses an
// array of bytes parallel to the array of entries to hold hashlets (for this
// implementation, the seven lowest-order bits of the hash) of used entries or
// indicators of unused (empty or erased) entries.  This hashlet array permits
// use of platform-specific instructions to optimize various methods (e.g.,
// through use of SSE instructions).
//
// The implemented data structure is inspired by Google's `flat_hash_map`
// CppCon presentations (available on YouTube).  The implementation draws from
// Google's open source `raw_hash_set.h` file at:
// https://github.com/abseil/abseil-cpp/blob/master/absl/container/internal.
//
///Load Factor and Resizing
///------------------------
// An invariant of `bdlc::FlatHashTable` is that
// `0 <= load_factor() <= max_load_factor() <= 1.0`.  Any operation that would
// result in `load_factor() > max_load_factor()` for a `bdlc::FlatHashTable`
// instance causes the capacity to increase.  This resizing allocates new
// memory, copies or moves all elements to the new memory, and reclaims the
// original memory.  The transfer of elements involves rehashing the element to
// determine its new location.  As such, all iterators, pointers, and
// references to elements of the `bdlc::FlatHashTable` are invalidated.
//
// Note that the value returned by `max_load_factor` is implementation
// dependent and cannot be changed by the user.
//
///Requirements on `KEY`, `ENTRY`, `ENTRY_UTIL`, `HASH`, and `EQUAL`
///-----------------------------------------------------------------
// The template parameter type `ENTRY` must be copy or move constructible.  The
// template parameter types `HASH` and `EQUAL` must be default and copy
// constructible function objects.
//
// `ENTRY_UTIL` must support static methods `constructFromKey`, `construct`,
// and `key` compatible with the following statements for objects `entry` of
// type `ENTRY`, `key` of type `KEY`, and `allocator` of type
// `bslma::Allocator`:
// ```
// ENTRY_UTIL::constructFromKey(&entry, &allocator, key);
// ENTRY_UTIL::construct(&entry, &allocator, args...);
// const KEY& keyOfEntry = ENTRY_UTIL::key(entry);
// ```
//
// `HASH` must support a function call operator compatible with the following
// statements for an object `key` of type `KEY`:
// ```
// HASH        hash;
// bsl::size_t result = hash(key);
// ```
//
// `EQUAL` must support a function call operator compatible with the
//  following statements for objects `key1` and `key2` of type `KEY`:
// ```
// EQUAL equal;
// bool  result = equal(key1, key2);
// ```
// where the definition of the called function defines an equivalence
// relationship on keys that is both reflexive and transitive.
//
// `HASH` and `EQUAL` function-objects are further constrained; if the
// comparator claims that two values are equal, the hasher must produce the
// same hash value for each.
//
// If support for `operator==` is required, the type `ENTRY` must be
// equality-comparable.
//
///Iterator, Pointer, and Reference Invalidation
///---------------------------------------------
// Any change in capacity of a `bdlc::FlatHashTable` invalidates all pointers,
// references, and iterators.  A `bdlc::FlatHashTable` manipulator that erases
// an element invalidates all pointers, references, and iterators to the erased
// elements.
//
///Exception Safety
///----------------
// A `bdlc::FlatHashTable` is exception neutral, and all of the methods of
// `bdlc::FlatHashTable` provide the basic exception safety guarantee (see
// {`bsldoc_glossary`|Basic Guarantee}).
//
///Move Semantics in C++03
///-----------------------
// Move-only types are supported by `bdlc::FlatHashTable` on C++11, and later,
// platforms only (where `BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES` is defined),
// and are not supported on C++03 platforms.  Unfortunately, in C++03, there
// are user types where a `bslmf::MovableRef` will not safely degrade to a
// lvalue reference when a move constructor is not available (types providing a
// constructor template taking any type), so `bslmf::MovableRefUtil::move`
// cannot be used directly on a user supplied template type.  See internal bug
// report 99039150 for more information.
//
///Usage
///-----
// There is no usage example for this component since it is not meant for
// direct client use.

#include <bdlscm_version.h>

#include <bdlc_flathashtable_groupcontrol.h>

#include <bdlb_bitutil.h>

#include <bslalg_arrayprimitives.h>
#include <bslalg_swaputil.h>

#include <bslma_allocator.h>
#include <bslma_constructionutil.h>
#include <bslma_default.h>
#include <bslma_deallocatorproctor.h>
#include <bslma_destructorguard.h>
#include <bslma_destructorproctor.h>

#include <bslmf_assert.h>
#include <bslmf_enableif.h>
#include <bslmf_isbitwisecopyable.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_isconvertible.h>
#include <bslmf_movableref.h>
#include <bslmf_util.h>    // 'forward(V)'

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_objectbuffer.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>
#include <bsls_types.h>
#include <bsls_util.h>     // 'forward<T>(V)'

#include <bsl_cstddef.h>
#include <bsl_cstdint.h>
#include <bsl_cstring.h>
#include <bsl_iterator.h>
#include <bsl_limits.h>
#include <bsl_type_traits.h>
#include <bsl_utility.h>

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Tue Feb 13 09:14:21 2024
// Command line: sim_cpp11_features.pl bdlc_flathashtable.h
# define COMPILING_BDLC_FLATHASHTABLE_H
# include <bdlc_flathashtable_cpp03.h>
# undef COMPILING_BDLC_FLATHASHTABLE_H
#else

namespace BloombergLP {
namespace bdlc {

// FORWARD DECLARATIONS
struct FlatHashTable_ImplUtil;

template <class ENTRY>
class FlatHashTable_IteratorImp;

template <class ENTRY>
bool operator==(const class FlatHashTable_IteratorImp<ENTRY>&,
                const class FlatHashTable_IteratorImp<ENTRY>&);

                     // ===============================
                     // class FlatHashTable_IteratorImp
                     // ===============================

/// This class implements the methods required by `bsl::ForwardIterator` to
/// provide forward iterators.  As such, an instance of this class
/// represents a position within a flat hash table.  This class uses no
/// features of the `ENTRY` type except for addresses of `ENTRY` objects.
template <class ENTRY>
class FlatHashTable_IteratorImp
{
    // PRIVATE TYPES
    typedef FlatHashTable_GroupControl GroupControl;

    // DATA
    ENTRY               *d_entries_p;
    const bsl::uint8_t  *d_controls_p;
    bsl::size_t          d_additionalLength;

    // FRIENDS
    friend bool operator==<>(const FlatHashTable_IteratorImp&,
                             const FlatHashTable_IteratorImp&);

  public:
    // CREATORS

    /// Create a `FlatHashTable_IteratorImp` having the default,
    /// non-dereferencable value.
    FlatHashTable_IteratorImp();

    /// Create a `FlatHashTable_IteratorImp` referencing the first element
    /// of the specified `entries` and `controls`, which have the specified
    /// `additionalLength` values.  The behavior is undefined unless
    /// `entries` points to at least `1 + additionalLength` entry values and
    /// `controls` points to at least
    /// `1 + additionalLength + ControlGroup::k_SIZE` control values.
    FlatHashTable_IteratorImp(ENTRY              *entries,
                              const bsl::uint8_t *controls,
                              bsl::size_t         additionalLength);

    /// Create a `FlatHashTable_IteratorImp` having the same value as the
    /// specified `original`.
    FlatHashTable_IteratorImp(const FlatHashTable_IteratorImp& original);

    //! ~FlatHashTable_IteratorImp() = default;
        // Destroy this object.

    // MANIPULATORS

    /// Assign to this `FlatHashTable_IteratorImp` the value of the
    /// specified `rhs`.
    FlatHashTable_IteratorImp& operator=(const FlatHashTable_IteratorImp& rhs);

    /// Advance the `FlatHashTable_IteratorImp` to the next present element
    /// in the underlying flat hash table.  If there is no such element,
    /// assign this object to `FlatHashTable_InteratorImp()`.  The behavior
    /// is undefined unless this `FlatHashTable_IteratorImp` refers to a
    /// valid element of the underlying sequence.
    void operator++();

    // ACCESSORS

    /// Return a reference to the element referred to by this
    /// `FlatHashTable_IteratorImp`.  The behavior is undefined unless this
    /// `FlatHashTable_IteratorImp() != *this`.
    ENTRY& operator*() const;
};

// FREE OPERATORS

/// Return true if the specified `a` and `b` are equal.  Two
/// `FlatHashTable_IteratorImp` objects are equal if they both refer to the
/// same element of the underlying flat hash table, or are both not
/// dereferenceable.  The behavior is undefined unless `a` and `b` refer to
/// the same `FlatHashTable`.
template <class ENTRY>
bool operator==(const FlatHashTable_IteratorImp<ENTRY>& a,
                const FlatHashTable_IteratorImp<ENTRY>& b);

                           // ===================
                           // class FlatHashTable
                           // ===================

/// This class template provides a flat hash table implementation useful for
/// implementing a flat hash set and flat hash map.
template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
class FlatHashTable
{
    // PRIVATE TYPES
    typedef FlatHashTable_GroupControl       GroupControl;
    typedef FlatHashTable_ImplUtil           ImplUtil;
    typedef FlatHashTable_IteratorImp<ENTRY> IteratorImp;

  public:
    // TYPES
    typedef KEY        key_type;
    typedef ENTRY      entry_type;
    typedef ENTRY_UTIL entry_util_type;
    typedef HASH       hash_type;
    typedef EQUAL      key_equal_type;

    typedef typename bslstl::ForwardIterator<ENTRY,
                                             IteratorImp> iterator;
    typedef typename bslstl::ForwardIterator<const ENTRY,
                                             IteratorImp> const_iterator;

  private:
    // DATA
    ENTRY            *d_entries_p;          // entries of this table
    bsl::uint8_t     *d_controls_p;         // control values of this table
    bsl::size_t       d_size;               // number of active values
    bsl::size_t       d_capacity;           // size of values array
    int               d_groupControlShift;  // number of bits to shift hash
    HASH              d_hasher;             // hashing functor
    EQUAL             d_equal;              // equality functor
    bslma::Allocator *d_allocator_p;        // allocator

    // PRIVATE CLASS METHODS

    /// Return the index of the first available entry indicated by the
    /// specified `controls` at or after the specified `index`, assuming
    /// `controls` has the specified `capacity`.  The behavior is undefined
    /// unless `index < capacity` and `controls` has at least `capacity`
    /// entries.
    static bsl::size_t findAvailable(bsl::uint8_t *controls,
                                     bsl::size_t   index,
                                     bsl::size_t   capacity);

    // PRIVATE MANIPULATORS

    /// Load `true` into the specified `notFound` if there is no entry in
    /// this table having the specified `key` with the specified
    /// `hashValue`, and `false` otherwise.  Return the index of the entry
    /// within `d_entries_p` which contains the `key` if such an entry
    /// exists, otherwise insert an entry with value obtained from
    /// `ENTRY_UTIL::construct` and return the index of this entry.  This
    /// method rehashes the table if the `key` was not present and the
    /// addition of an entry would cause the load factor to exceed
    /// `max_load_factor()`.  The behavior is undefined unless
    /// `hashValue == d_hasher(key)`.
    bsl::size_t indexOfKey(bool        *notFound,
                           const KEY&   key,
                           bsl::size_t  hashValue);

    /// Change the capacity of this table to the specified `newCapacity`,
    /// and redistribute all the contained elements into the new sequence of
    /// entries, according to their hash values.  The behavior is undefined
    /// unless `0 < newCapacity` and `newCapacity` satisfies all class
    /// invariants.
    void rehashRaw(bsl::size_t newCapacity);

    // PRIVATE ACCESSORS

    /// Return the index of the entry within `d_entries_p` containing the
    /// specified `key`, which has the specified `hashValue`, or
    /// `d_capacity` if the `key` is not present.  The behavior is undefined
    /// unless `hashValue == d_hasher(key)`.
    bsl::size_t findKey(const KEY& key, bsl::size_t hashValue) const;

    /// Return the minimum capacity that satisfies all class invariants, and
    /// is at least the specified `minimumCapacity`.
    bsl::size_t minimumCompliantCapacity(bsl::size_t minimumCapacity) const;

    // NOT IMPLEMENTED
    FlatHashTable();

  public:
    // PUBLIC CLASS DATA
    static const bsl::size_t  k_MIN_CAPACITY = 2 * GroupControl::k_SIZE;
                                                      // min. non-zero capacity

    static const bsl::int8_t  k_HASHLET_MASK = 0x7f;  // hashlet = hash & MASK

    static const bsl::size_t  k_MAX_LOAD_FACTOR_NUMERATOR = 7;
                                                      // numerator of fraction
                                                      // that specifies the
                                                      // maximum load factor

    static const bsl::size_t  k_MAX_LOAD_FACTOR_DENOMINATOR = 8;
                                                      // denominator of
                                                      // fraction that
                                                      // specifies the maximum
                                                      // load factor

    // CREATORS

    /// Create an empty table having at least the specified `capacity`, that
    /// will use the specified `hash` to generate hash values for the keys
    /// of the entries contained in this table, and the specified `equal` to
    /// verify that the keys of the two entries are the same.  Optionally
    /// specify a `basicAllocator` used to supply memory.  If
    /// `basicAllocator` is 0, the currently installed default allocator is
    /// used.  If `0 == capacity`, no memory is allocated and the object is
    /// defined to be in the "zero-capacity" state.
    FlatHashTable(bsl::size_t       capacity,
                  const HASH&       hash,
                  const EQUAL&      equal,
                  bslma::Allocator *basicAllocator = 0);

    /// Create a table having the same value, hasher, and key-equality
    /// comparator as the specified `original`.  Optionally specify a
    /// `basicAllocator` used to supply memory.  If `basicAllocator` is 0,
    /// the currently installed default allocator is used.
    FlatHashTable(const FlatHashTable&  original,
                  bslma::Allocator     *basicAllocator = 0);

    /// Create an table having the same value as the specified `original`
    /// object by moving (in constant time) the contents of `original` to
    /// the new table.  Use a copy of `original.hash_function()` to generate
    /// hash values for the keys of the entries contained in this table.
    /// Use a copy of `original.key_eq()` to verify that two keys are equal.
    /// The allocator associated with `original` is propagated for use in
    /// the newly-created table.  `original` is left in a (valid)
    /// unspecified state.
    explicit FlatHashTable(bslmf::MovableRef<FlatHashTable> original);

    /// Create a table having the same value, hasher, and key-equality
    /// comparator as the specified `original` object by moving the contents
    /// of `original` to the new table, and using the specified
    /// `basicAllocator` to supply memory.  Use a copy of
    /// `original.hash_function()` to generate hash values for the entries
    /// contained in this table.  Use a copy of `original.key_eq()` to
    /// verify that the keys of two entries are equal.  This method requires
    /// that the (template parameter) type `ENTRY` be `move-insertable` into
    /// this `FlatHashTable`.  If `basicAllocator` is 0, the currently
    /// installed default allocator is used.  If `original` and the newly
    /// created object have the same allocator then the value of `original`
    /// becomes unspecified but valid, and no exceptions will be thrown;
    /// otherwise `original` is unchanged (and an exception may be thrown).
    FlatHashTable(bslmf::MovableRef<FlatHashTable>  original,
                  bslma::Allocator                 *basicAllocator);

    /// Destroy this object and each of its entries.
    ~FlatHashTable();

    // MANIPULATORS

    /// Assign to this object the value, hasher, and key-equality functor of
    /// the specified `rhs` object and return a reference offering
    /// modifiable access to this object.
    FlatHashTable& operator=(const FlatHashTable& rhs);

    /// Assign to this object the value, hash function, and key-equality
    /// comparator of the specified `rhs` object and return a reference
    /// offering modifiable access to this object.  The entries of `rhs` are
    /// moved (in constant time) to this object if the two have the same
    /// allocator, otherwise entries from `rhs` are moved into this table.
    /// In either case, `rhs` is left in a valid but unspecified state.  If
    /// an exception is thrown, this object is left in a valid but
    /// unspecified state.
    FlatHashTable& operator=(bslmf::MovableRef<FlatHashTable> rhs);

    /// If an entry with the specified `key` is not already present in this
    /// table, insert an entry having the value defined by
    /// `ENTRY_UTIL::construct`; otherwise, this method has no effect.
    /// Return an iterator referring to the (possibly newly inserted) object
    /// in this table with the `key`.
    template <class KEY_TYPE>
    ENTRY& operator[](BSLS_COMPILERFEATURES_FORWARD_REF(KEY_TYPE) key);

    /// Remove all entries from this table.  Note that this table will be
    /// empty after calling this method, but allocated memory may be
    /// retained for future use.  See the `capacity` method.
    void clear();

    /// Return a pair of iterators providing modifiable access to the
    /// sequence of objects in this flat hash table having the specified
    /// `key`, where the first iterator is positioned at the start of the
    /// sequence, and the second is positioned one past the end of the
    /// sequence.  If this table contains no object having `key`, then the
    /// two returned iterators will have the same value, `end()`.  Note that
    /// since each key in a flat hash table is unique, the returned range
    /// contains at most one element.
    bsl::pair<iterator, iterator> equal_range(const KEY& key);

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    /// Create an `ENTRY` object from the specified `args`, and attempt to
    /// add it to this flat hash table.  Return a `bsl::pair` containing an
    /// iterator to the newly inserted object and `true` if the element was
    /// added.  If an entry with the same key already exists in this flat
    /// hash table, return an iterator to that entry and `false`.  This
    /// method requires that the `ENTRY` be `copy-constructible`.
    template< class... ARGS>
    bsl::pair<iterator, bool> emplace(ARGS&&... args);
#endif

    /// Remove from this table the object having the specified `key`, if it
    /// exists, and return 1; otherwise (there is no object with a key equal
    /// to `key` in this table) return 0 with no other effect.  This method
    /// invalidates all iterators, and references to the removed element.
    bsl::size_t erase(const KEY& key);

    /// Remove from this table the object at the specified `position`, and
    /// return an iterator referring to the element immediately following
    /// the removed element, or to the past-the-end position if the removed
    /// element was the last element in the sequence of elements maintained
    /// by this table.  This method invalidates all iterators, and
    /// references to the removed element.  The behavior is undefined unless
    /// `position` refers to an object in this table.
    iterator erase(const_iterator position);
    iterator erase(iterator position);

    /// Remove from this table the objects starting at the specified `first`
    /// position up to, but not including, the specified `last` position,
    /// and return `last`.  This method invalidates all iterators, and
    /// references to the removed element.  The behavior is undefined unless
    /// `first` and `last` either refer to elements in this table or are the
    /// `end` iterator, and the `first` position is at or before the `last`
    /// position in the iteration sequence provided by this container.
    iterator erase(const_iterator first, const_iterator last);

    /// Return an iterator providing modifiable access to the object in this
    /// flat hash table with a key equal to the specified `key`, if such an
    /// entry exists, and `end()` otherwise.
    iterator find(const KEY& key);

#if defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130
    template <class ENTRY_TYPE>
    bsl::pair<iterator, bool> insert(
                           BSLS_COMPILERFEATURES_FORWARD_REF(ENTRY_TYPE) entry)
#else
    /// Insert the specified `entry` into this table if the key of the
    /// `entry` does not already exist in this table; otherwise, this method
    /// has no effect.  Return a `pair` whose `first` member is an iterator
    /// referring to the (possibly newly inserted) object in this table
    /// whose key is the equal to that of the object to be inserted, and
    /// whose `second` member is `true` if a new entry was inserted, and
    /// `false` if a entry having an equal key was already present.  Bitwise
    /// movable types that are not bitwise copyable will be copied (to avoid
    /// confusion with regard to calling the `entry` destructor after this
    /// call).
    template <class ENTRY_TYPE>
    typename bsl::enable_if<bsl::is_convertible<ENTRY_TYPE, ENTRY>::value,
                            bsl::pair<iterator, bool> >::type
                    insert(BSLS_COMPILERFEATURES_FORWARD_REF(ENTRY_TYPE) entry)
#endif
    {
        // Note that some compilers require functions declared with 'enable_if'
        // to be defined inline.

        bool        notFound;
        bsl::size_t hashValue = d_hasher(ENTRY_UTIL::key(entry));
        bsl::size_t index     = indexOfKey(&notFound,
                                           ENTRY_UTIL::key(entry),
                                           hashValue);

        if (notFound) {
            ENTRY_UTIL::construct(
                             d_entries_p + index,
                             d_allocator_p,
                             BSLS_COMPILERFEATURES_FORWARD(ENTRY_TYPE, entry));

            d_controls_p[index] = static_cast<bsl::uint8_t>(
                                                   hashValue & k_HASHLET_MASK);

            ++d_size;
        }

        return bsl::pair<iterator, bool>(IteratorImp(d_entries_p  + index,
                                                     d_controls_p + index,
                                                     d_capacity   - index - 1),
                                         notFound);
    }

    /// Create an object for each iterator in the range starting at the
    /// specified `first` iterator and ending immediately before the
    /// specified `last` iterator, by converting from the object referred to
    /// by each iterator.  Insert into this table each such object whose key
    /// is not already contained.  The (template parameter) type
    /// `INPUT_ITERATOR` shall meet the requirements of an input iterator
    /// defined in the C++11 standard [24.2.3] providing access to values of
    /// a type convertible to `ENTRY`.  The behavior is undefined unless
    /// `first` and `last` refer to a sequence of valid values where `first`
    /// is at a position at or before `last`.
    template <class INPUT_ITERATOR>
    void insert(INPUT_ITERATOR first, INPUT_ITERATOR last);

    /// Change the capacity of this table to at least the specified
    /// `minimumCapacity`, and redistribute all the contained elements into
    /// a new sequence of entries, according to their hash values.  If
    /// `0 == minimumCapacity` and `0 == size()`, the table is returned to
    /// the zero-capacity state.  On return, `load_factor()` is less than or
    /// equal to `max_load_factor()` and all iterators, pointers, and
    /// references to elements of this `FlatHashTable` are invalidated.
    void rehash(bsl::size_t minimumCapacity);

    /// Change the capacity of this table to at least a capacity that can
    /// accommodate the specified `numEntries` (accounting for the load
    /// factor invariant), and redistribute all the contained elements into
    /// a new sequence of entries, according to their hash values.  If
    /// `0 == numEntries` and `0 == size()`, the table is returned to the
    /// zero-capacity state.  After this call, `load_factor()` will be less
    /// than or equal to `max_load_factor()`.  Note that this method is
    /// effectively equivalent to:
    /// ```
    ///    rehash(bsl::ceil(numEntries / max_load_factor()))
    /// ```
    void reserve(bsl::size_t numEntries);

    /// Remove all entries from this table and release all memory from this
    /// table, returning the table to the zero-capacity state.
    void reset();

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
    /// If a key equivalent to the specified `key` already exists in this
    /// map, return a pair containing an iterator referring to the existing
    /// item, and `false`.  Otherwise, insert into this map a newly-created
    /// `ENTRY` object, constructed from `key` and the specified `args`, and
    /// return a pair containing an iterator referring to the newly-created
    /// entry and `true`.  This method requires that the (template
    /// parameter) types `KEY` and `VALUE` are `emplace-constructible` from
    /// `key` and `args` respectively.  For C++03, `VALUE` must also be
    /// `copy-constructible`.
    template< class... ARGS>
    bsl::pair<iterator, bool> try_emplace(const KEY& key, ARGS&&... args);

    /// If a key equivalent to the specified `key` already exists in this
    /// map, return a pair containing an iterator referring to the existing
    /// item, and `false`.  Otherwise, insert into this map a newly-created
    /// `ENTRY` object, constructed from `std::forward<KEY>(key)` and the
    /// specified `args`, and return a pair containing an iterator referring
    /// to the newly-created entry and `true`.  This method requires that
    /// the (template parameter) types `KEY` and `VALUE` are
    /// `emplace-constructible` from `key` and `args` respectively.  For
    /// C++03, `VALUE` must also be `copy-constructible`.
    template <class... ARGS>
    bsl::pair<iterator, bool> try_emplace(
                                     BloombergLP::bslmf::MovableRef<KEY> key,
                                     ARGS&&...                           args);
#endif

                          // Iterators

    /// Return an iterator representing the beginning of the sequence of
    /// entries held by this container.
    iterator begin();

    /// Return an iterator representing one past the end of the sequence of
    /// entries held by this container.
    iterator end();

                           // Aspects

    /// Efficiently exchange the value of this table with the value of the
    /// specified `other` table.  This method provides the no-throw
    /// exception-safety guarantee.  The behavior is undefined unless this
    /// array was created with the same allocator as `other`.
    void swap(FlatHashTable& other);

    // ACCESSORS

    /// Return the number of elements this table could hold if the load
    /// factor were 1.
    bsl::size_t capacity() const;

    /// Return `true` if this table contains an entry having the specified
    /// `key`, and `false` otherwise.
    bool contains(const KEY& key) const;

    /// Return the address of the first element of the underlying array of
    /// control values in this table, or 0 if this table is in the
    /// zero-capacity state.  An element of this array has the value
    /// `FlatHashTable_GroupControl::k_EMPTY`,
    /// `FlatHashTable_GroupControl::k_ERASED`, or a seven bit hashlet value
    /// for the in-use position (the highest-order bit is unset).
    const bsl::uint8_t *controls() const;

    /// Return the number of objects contained within this table having the
    /// specified `key`.  Note that since a table maintains unique keys, the
    /// returned value will be either 0 or 1.
    bsl::size_t count(const KEY& key) const;

    /// Return `true` if this table contains no entries, and `false`
    /// otherwise.
    bool empty() const;

    /// Return the address of the first element of the underlying array of
    /// entries in this table, or 0 if this table is in the zero-capacity
    /// state.  The behavior is undefined unless the address is verified
    /// in-use through use of the `controls` array before dereferencing an
    /// entry in this array.
    const ENTRY *entries() const;

    /// Return a pair of iterators providing non-modifiable access to the
    /// sequence of objects in this table having the specified `key`, where
    /// the first iterator is positioned at the start of the sequence, and
    /// the second is positioned one past the end of the sequence.  If this
    /// table contains no objects having `key`, then the two returned
    /// iterators will have the same value, `end()`.  Note that since a
    /// table maintains unique keys, the range will contain at most one
    /// entry.
    bsl::pair<const_iterator, const_iterator> equal_range(
                                                         const KEY& key) const;

    /// Return an iterator representing the position of the entry in this
    /// flat hash table having the specified `key`, or `end()` if no such
    /// entry exists in this table.
    const_iterator find(const KEY& key) const;

    /// Return (a copy of) the unary hash functor used by this flat hash
    /// table to generate a hash value (of type `bsl::size_t) for a `KEY'
    /// object.
    HASH hash_function() const;

    /// Return (a copy of) the binary key-equality functor used by this flat
    /// hash table that returns `true` if two `KEY` objects are equal, and
    /// `false` otherwise.
    EQUAL key_eq() const;

    /// Return the current ratio between the number of elements in this
    /// table and its capacity.
    float load_factor() const;

    /// Return the maximum load factor allowed for this table.  Note that if
    /// an insert operation would cause the load factor to exceed the
    /// `max_load_factor`, that same insert operation will increase the
    /// capacity and rehash the entries of the container (see `insert` and
    /// `rehash`).  Note that the value returned by `max_load_factor` is
    /// implementation dependent and cannot be changed by the user.
    float max_load_factor() const;

    /// Return the number of entries in this table.
    bsl::size_t size() const;

                          // Iterators

    const_iterator begin() const;

    /// Return an iterator representing the beginning of the sequence of
    /// entries held by this container.
    const_iterator cbegin() const;

    const_iterator cend() const;

    /// Return an iterator representing one past the end of the sequence of
    /// entries held by this container.
    const_iterator end() const;

                           // Aspects

    /// Return the allocator used by this hash table to supply memory.
    bslma::Allocator *allocator() const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` objects have the same
/// value, and `false` otherwise.  Two `FlatHashTable` objects have the same
/// value if they have the same number of entries, and for each entry that
/// is contained in `lhs` there is a entry contained in `rhs` having the
/// same value.  Note that this method requires the (template parameter)
/// type `ENTRY` to be equality-comparable.
template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
bool operator==(const FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>& lhs,
                const FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>& rhs);

/// Return `true` if the specified `lhs` and `rhs` objects do not have the
/// same value, and `false` otherwise.  Two `FlatHashTable` objects do not
/// have the same value if they do not have the same number of entries, or
/// that for some entry contained in `lhs` there is not a entry in `rhs`
/// having the same value.  Note that this method requires the (template
/// parameter) type `ENTRY` to be equality-comparable.
template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
bool operator!=(const FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>& lhs,
                const FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>& rhs);

// FREE FUNCTIONS

/// Exchange the values of the specified `a` and `b` objects.  This function
/// provides the no-throw exception-safety guarantee if the two objects were
/// created with the same allocator and the basic guarantee otherwise.
template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
void swap(FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>& a,
          FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>& b);

                       // =============================
                       // struct FlatHashTable_ImplUtil
                       // =============================

/// This component-private, utility `struct` provides a namespace for a
/// suite of operations used in the implementation of the `FlatHashTable`
/// class template.
struct FlatHashTable_ImplUtil {

  private:
    // PRIVATE TYPES
    typedef FlatHashTable_GroupControl GroupControl;

    /// This component-private, mechanism class template provides a proctor
    /// that, unless its `release` method has been previously invoked, on
    /// destruction automatically destroys the populated `ENTRY_TYPE`
    /// elements of an `ENTRY_TYPE` array supplied on construction as
    /// indicated by a "control" byte array supplied on construction.
    template <class ENTRY_TYPE>
    class DestroyEntryArrayProctor;

    // PRIVATE CLASS METHODS

    /// Copy the specified range `[firstSourceControl, lastSourceControl)`
    /// to the array specified by `firstDestinationControl`, and copy each
    /// element in the specified range `[firstSourceEntry, lastSourceEntry)`
    /// at the same index as each byte in the range '[firstSourceControl,
    /// lastSourceControl)' that has its most-significant bit unset, to the
    /// corresponding location in the contiguous storage for `ENTRY_TYPE`
    /// objects specified by `firstDestinationEntry`.  Use the specified
    /// `entryAllocator` as the object allocator for each newly-constructed
    /// `ENTRY_TYPE` object if `ENTRY_TYPE` is allocator-aware.  If
    /// `entryAllocator` is 0, the currently-installed default allocator is
    /// used.  No exception is thrown if the specified `ENTRY_TYPE` is
    /// nothrow-copyable, otherwise an exception may be thrown.  The
    /// behavior is undefined unless
    /// `bslmf::IsBitwiseCopyable<ENTRY_TYPE>::value` is equal to the
    /// `value` member of the type of the specified `tag`,
    /// `firstDestinationEntry` is a pointer to the first byte of
    /// uninitialized and correctly aligned storage for at least
    /// `bsl::distance(firstSourceEntry, lastSourceEntry)` `ENTRY_TYPE`
    /// objects, `firstDestinationControl` is a pointer to the first byte of
    /// `bsl::distance(firstSourceEntry, lastSourceEntry)` bytes of
    /// uninitialized storage, the range '[firstSourceEntry,
    /// lastSourceEntry)' denotes a contiguous array of storage for
    /// optionally-constructed `ENTRY_TYPE` objects, the range
    /// `[firstSourceControl, lastSourceControl)` denotes a contiguous array
    /// of `bsl::uint8_t` objects, an `ENTRY_TYPE` object exists at the same
    /// index in the `[firstSourceEntry, lastSourceEntry)` storage range as
    /// each byte in the range `[firstControlEntry, lastControlEntry)` that
    /// has its most significant bit unset, and
    /// `bsl::distance(firstSourceEntry, lastSourceEntry)` is equal to
    /// `bsl::distance(firstSourceControl, lastSourceControl)`.
    template <class ENTRY_TYPE>
    static void copyEntryAndControlArrays(
                      ENTRY_TYPE                      *firstDestinationEntry,
                      bsl::uint8_t                    *firstDestinationControl,
                      const ENTRY_TYPE                *firstSourceEntry,
                      const ENTRY_TYPE                *lastSourceEntry,
                      const bsl::uint8_t              *firstSourceControl,
                      const bsl::uint8_t              *lastSourceControl,
                      bslma::Allocator                *entryAllocator,
                      bsl::false_type                  bitwiseCopyable);
    template <class ENTRY_TYPE>
    static void copyEntryAndControlArrays(
                         ENTRY_TYPE                   *firstDestinationEntry,
                         bsl::uint8_t                 *firstDestinationControl,
                         const ENTRY_TYPE             *firstSourceEntry,
                         const ENTRY_TYPE             *lastSourceEntry,
                         const bsl::uint8_t           *firstSourceControl,
                         const bsl::uint8_t           *lastSourceControl,
                         bslma::Allocator             *entryAllocator,
                         bsl::true_type                bitwiseCopyable);


    /// Destroy each entry object in the storage specified by the range
    /// `[firstEntry, lastEntry)` if the most-significant bit is unset in
    /// the corresponding element in the specified range
    /// `[firstControl, lastControl)` (i.e., the most-significant bit of the
    /// element in the "control" array that has the same index as the
    /// element in the "entry" array is unset).  No exception is thrown.
    /// The behavior is undefined unless
    /// `bslmf::IsTriviallyDestructible<ENTRY_TYPE>::value` is equal to the
    /// `value` member of the type of the specified `tag`, the range
    /// `[firstEntry, lastEntry)` denotes a contiguous array of storage for
    /// optionally-constructed `ENTRY_TYPE` objects, the range
    /// `[firstControl, lastControl)` denotes a contiguous array of
    /// `bsl::uint8_t` objects, the two arrays have the same number of
    /// elements, and an `ENTRY_TYPE` object exists at the same index in the
    /// `[firstEntry, lastEntry)` storage range for each element in the
    /// `[firstControl, lastControl)` range that has its first bit unset.
    template <class ENTRY_TYPE>
    static void destroyEntryArray(ENTRY_TYPE         *firstEntry,
                                  ENTRY_TYPE         *lastEntry,
                                  const bsl::uint8_t *firstControl,
                                  const bsl::uint8_t *lastControl,
                                  bsl::false_type     triviallyDestructible);
    template <class ENTRY_TYPE>
    static void destroyEntryArray(ENTRY_TYPE         *firstEntry,
                                  ENTRY_TYPE         *lastEntry,
                                  const bsl::uint8_t *firstControl,
                                  const bsl::uint8_t *lastControl,
                                  bsl::true_type      triviallyDestructible);

  public:
    // CLASS METHODS

    /// Copy the specified range `[firstSourceControl, lastSourceControl)`
    /// to the array specified by `firstDestinationControl`, and copy each
    /// element in the specified range `[firstSourceEntry, lastSourceEntry)`
    /// at the same index as each byte in the range '[firstSourceControl,
    /// lastSourceControl)' that has its most-significant bit unset, to the
    /// corresponding location in the contiguous storage for `ENTRY_TYPE`
    /// objects specified by `firstDestinationEntry`.  Use the specified
    /// `entryAllocator` as the object allocator for each newly-constructed
    /// `ENTRY_TYPE` object if `ENTRY_TYPE` is allocator-aware.  If
    /// `entryAllocator` is 0, the currently-installed default allocator is
    /// used.  No exception is thrown if the specified `ENTRY_TYPE` is
    /// nothrow-copyable, otherwise an exception may be thrown.  The
    /// behavior is undefined unless `firstDestinationEntry` is a pointer to
    /// the first byte of uninitialized and correctly aligned storage for at
    /// least `bsl::distance(firstSourceEntry, lastSourceEntry)`
    /// `ENTRY_TYPE` objects, `firstDestinationControl` is a pointer to the
    /// first byte of `bsl::distance(firstSourceEntry, lastSourceEntry)`
    /// bytes of uninitialized storage, the range '[firstSourceEntry,
    /// lastSourceEntry)' denotes a contiguous array of storage for
    /// optionally-constructed `ENTRY_TYPE` objects, the range
    /// `[firstSourceControl, lastSourceControl)` denotes a contiguous array
    /// of `bsl::uint8_t` objects, an `ENTRY_TYPE` object exists at the same
    /// index in the `[firstSourceEntry, lastSourceEntry)` storage range as
    /// each byte in the range `[firstControlEntry, lastControlEntry)` that
    /// has its most significant bit unset, and
    /// `bsl::distance(firstSourceEntry, lastSourceEntry)` is equal to
    /// `bsl::distance(firstSourceControl, lastSourceControl)`.
    template <class ENTRY_TYPE>
    static void
    copyEntryAndControlArrays(ENTRY_TYPE         *firstDestinationEntry,
                              bsl::uint8_t       *firstDestinationControl,
                              const ENTRY_TYPE   *firstSourceEntry,
                              const ENTRY_TYPE   *lastSourceEntry,
                              const bsl::uint8_t *firstSourceControl,
                              const bsl::uint8_t *lastSourceControl,
                              bslma::Allocator   *entryAllocator);

    /// Destroy each entry object in the storage specified by the range
    /// `[firstEntry, lastEntry)` if the most-significant bit is unset in
    /// the corresponding element in the specified range '[firstControl,
    /// lastControl)' (i.e., the most-significant bit of the element in the
    /// "control" array that has the same index as the element in the
    /// "entry" array is unset).  No exception is thrown.  The behavior is
    /// undefined unless the range `[firstEntry, lastEntry)` denotes a
    /// contiguous array of storage for optionally-constructed `ENTRY_TYPE`
    /// objects, the range `[firstControl, lastControl)` denotes a
    /// contiguous array of `bsl::uint8_t` objects, the two arrays have the
    /// same number of elements, and an `ENTRY_TYPE` object exists at the
    /// same index in the `[firstEntry, lastEntry)` storage range for each
    /// element in the `[firstControl, lastControl)` range that has its
    /// first bit unset.
    template <class ENTRY_TYPE>
    static void destroyEntryArray(ENTRY_TYPE         *firstEntry,
                                  ENTRY_TYPE         *lastEntry,
                                  const bsl::uint8_t *firstControl,
                                  const bsl::uint8_t *lastControl);
};

           // ======================================================
           // class FlatHashTable_ImplUtil::DestroyEntryArrayProctor
           // ======================================================

/// This component-private, mechanism class template provides a proctor
/// that, unless its `release` method has been previously invoked, on
/// destruction automatically destroys the populated `ENTRY_TYPE` elements
/// of an `ENTRY_TYPE` array supplied on construction as indicated by a
/// "control" byte array supplied on construction.
template <class ENTRY_TYPE>
class FlatHashTable_ImplUtil::DestroyEntryArrayProctor {

    // PRIVATE TYPES
    typedef FlatHashTable_GroupControl GroupControl;
    typedef FlatHashTable_ImplUtil     ImplUtil;

    // DATA

    // pointer to first element of entry array
    ENTRY_TYPE         *d_firstEntry_p;

    // pointer to one-past-the-end of entry array
    ENTRY_TYPE         *d_lastEntry_p;

    // pointer to first element of control array
    const bsl::uint8_t *d_firstControl_p;

    // pointer to one-past-the-end of control array
    const bsl::uint8_t *d_lastControl_p;

    // NOT IMPLEMENTED
    DestroyEntryArrayProctor(const DestroyEntryArrayProctor&);
    DestroyEntryArrayProctor& operator=(const DestroyEntryArrayProctor&);

  public:
    // CREATORS

    /// Create a new `DestroyEntryArrayProctor` object for the contiguous
    /// `ENTRY_TYPE` storage delimited by the range specified by
    /// `[firstEntry, lastEntry)` controlled by the bytes in the range
    /// specified by `[firstControl, lastControl)`.  The behavior is
    /// undefined unless 'bsl::distance(firstEntry, lastEntry) ==
    /// bsl::distance(`firstControl, lastControl)`.  Note that these ranges
    /// may be valid sub-ranges (or "views") of larger arrays, and these
    /// sub-ranges can be grown or shrunk by `moveEnd`.
    DestroyEntryArrayProctor(ENTRY_TYPE         *firstEntry,
                             ENTRY_TYPE         *lastEntry,
                             const bsl::uint8_t *firstControl,
                             const bsl::uint8_t *lastControl);

    /// Destroy this object and each object in the entry storage array held
    /// by this object where the most-significant bit of the corresponding
    /// element in the control array held by this object is unset.
    ~DestroyEntryArrayProctor();

    // MANIPULATORS

    /// Move the end of the entry and control ranges held by this object by
    /// the specified `offset`.
    void moveEnd(bsl::ptrdiff_t offset);

    /// Release the entry and control ranges held by this object from
    /// management by this object, and set this object's held entry and
    /// control arrays to the corresponding empty arrays.  If the entry and
    /// control arrays held by this object are empty, this method has no
    /// effect.
    void release();
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                     // -------------------------------
                     // class FlatHashTable_IteratorImp
                     // -------------------------------

// CREATORS
template <class ENTRY>
inline
FlatHashTable_IteratorImp<ENTRY>::FlatHashTable_IteratorImp()
: d_entries_p(0)
, d_controls_p(0)
, d_additionalLength(0)
{
}

template <class ENTRY>
inline
FlatHashTable_IteratorImp<ENTRY>::FlatHashTable_IteratorImp(
                                          ENTRY              *entries,
                                          const bsl::uint8_t *controls,
                                          bsl::size_t         additionalLength)
: d_entries_p(entries)
, d_controls_p(controls)
, d_additionalLength(additionalLength)
{
}

template <class ENTRY>
inline
FlatHashTable_IteratorImp<ENTRY>::FlatHashTable_IteratorImp(
                                     const FlatHashTable_IteratorImp& original)
: d_entries_p(original.d_entries_p)
, d_controls_p(original.d_controls_p)
, d_additionalLength(original.d_additionalLength)
{
}

// MANIPULATORS
template <class ENTRY>
inline
FlatHashTable_IteratorImp<ENTRY>& FlatHashTable_IteratorImp<ENTRY>::operator=(
                                          const FlatHashTable_IteratorImp& rhs)
{
    d_entries_p        = rhs.d_entries_p;
    d_controls_p       = rhs.d_controls_p;
    d_additionalLength = rhs.d_additionalLength;

    return *this;
}

template <class ENTRY>
inline
void FlatHashTable_IteratorImp<ENTRY>::operator++()
{
    BSLS_ASSERT_SAFE(d_entries_p);
    BSLS_ASSERT_SAFE(d_controls_p);

    while (d_additionalLength) {
        ++d_entries_p;
        ++d_controls_p;
        --d_additionalLength;
        if (0 == (*d_controls_p & 0x80)) {
            return;                                                   // RETURN
        }
    }

    d_entries_p  = 0;
    d_controls_p = 0;
}

// ACCESSORS
template <class ENTRY>
inline
ENTRY& FlatHashTable_IteratorImp<ENTRY>::operator*() const
{
    BSLS_ASSERT_SAFE(d_entries_p);
    BSLS_ASSERT_SAFE(d_controls_p);

    return *d_entries_p;
}

}  // close package namespace

// FREE OPERATORS
template <class ENTRY>
inline
bool bdlc::operator==(const FlatHashTable_IteratorImp<ENTRY>& a,
                      const FlatHashTable_IteratorImp<ENTRY>& b)
{
    return a.d_entries_p        == b.d_entries_p
        && a.d_controls_p       == b.d_controls_p
        && a.d_additionalLength == b.d_additionalLength;
}

namespace bdlc {

                           // -------------------
                           // class FlatHashTable
                           // -------------------

// PRIVATE CLASS METHODS
template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
bsl::size_t FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::findAvailable(
                                                        bsl::uint8_t *controls,
                                                        bsl::size_t   index,
                                                        bsl::size_t   capacity)
{
    BSLS_ASSERT_SAFE(index < capacity);

    for (bsl::size_t i = 0; i < capacity; i += GroupControl::k_SIZE) {
        bsl::uint8_t *controlStart = controls + index;

        GroupControl  groupControl(controlStart);
        bsl::uint32_t candidates = groupControl.available();

        if (candidates) {
            return index
                 + bdlb::BitUtil::numTrailingUnsetBits(candidates);   // RETURN
        }

        index = (index + GroupControl::k_SIZE) & (capacity - 1);
    }

    BSLS_ASSERT_OPT(false && "execution should never reach this location");
    return capacity;
}

// PRIVATE MANIPULATORS
template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
bsl::size_t FlatHashTable<KEY,
                          ENTRY,
                          ENTRY_UTIL,
                          HASH,
                          EQUAL>::indexOfKey(bool        *notFound,
                                             const KEY&   key,
                                             bsl::size_t  hashValue)
{
    BSLS_ASSERT_SAFE(hashValue == d_hasher(key));

    bsl::size_t index = findKey(key, hashValue);

    if (index == d_capacity) {
        *notFound = true;

        if (d_size >= k_MAX_LOAD_FACTOR_NUMERATOR
                    * (d_capacity / k_MAX_LOAD_FACTOR_DENOMINATOR)) {
            rehashRaw(d_capacity > 0 ? 2 * d_capacity : k_MIN_CAPACITY);
        }

        index = (hashValue >> d_groupControlShift) * GroupControl::k_SIZE;
        index = findAvailable(d_controls_p, index, d_capacity);
    }
    else {
        *notFound = false;
    }

    return index;
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
void FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::rehashRaw(
                                                       bsl::size_t newCapacity)
{
    BSLS_ASSERT_SAFE(          0 <  newCapacity);
    BSLS_ASSERT_SAFE(newCapacity == minimumCompliantCapacity(newCapacity));

    FlatHashTable tmp(newCapacity,
                      d_hasher,
                      d_equal,
                      d_allocator_p);

    for (bsl::size_t i = 0; i < d_capacity; i += GroupControl::k_SIZE) {
        bsl::uint8_t *controlStart = d_controls_p + i;
        ENTRY        *entryStart   = d_entries_p  + i;

        GroupControl  groupControl(controlStart);
        bsl::uint32_t candidates = groupControl.inUse();
        while (candidates) {
            int   offset = bdlb::BitUtil::numTrailingUnsetBits(candidates);
            ENTRY *entry = entryStart + offset;

            // create a destructor proctor for the element to be moved
            bslma::DestructorProctor<ENTRY> proctor(entry);

            // perform book-keeping for the destruction
            *(controlStart + offset) = GroupControl::k_ERASED;
            --d_size;

            // place the element in the new container
            bsl::size_t hashValue = tmp.d_hasher(ENTRY_UTIL::key(*entry));
            bsl::size_t index     = (hashValue >> tmp.d_groupControlShift)
                                                        * GroupControl::k_SIZE;

            index = findAvailable(tmp.d_controls_p, index, tmp.d_capacity);

            bslma::ConstructionUtil::destructiveMove(tmp.d_entries_p + index,
                                                     tmp.d_allocator_p,
                                                     entry);

            // release destructor proctor
            proctor.release();

            tmp.d_controls_p[index] = static_cast<bsl::uint8_t>(
                                                   hashValue & k_HASHLET_MASK);

            ++tmp.d_size;

            candidates = bdlb::BitUtil::withBitCleared(candidates, offset);
        }
    }

    d_allocator_p->deallocate(d_entries_p);
    d_allocator_p->deallocate(d_controls_p);

    d_entries_p         = 0;
    d_controls_p        = 0;
    d_capacity          = 0;
    d_groupControlShift = 0;

    bslalg::SwapUtil::swap(&d_entries_p,         &tmp.d_entries_p);
    bslalg::SwapUtil::swap(&d_controls_p,        &tmp.d_controls_p);
    bslalg::SwapUtil::swap(&d_size,              &tmp.d_size);
    bslalg::SwapUtil::swap(&d_capacity,          &tmp.d_capacity);
    bslalg::SwapUtil::swap(&d_groupControlShift, &tmp.d_groupControlShift);
}

// PRIVATE ACCESSORS
template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
bsl::size_t FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::findKey(
                                                   const KEY&  key,
                                                   bsl::size_t hashValue) const
{
    BSLS_ASSERT_SAFE(hashValue == d_hasher(key));

    bsl::size_t  index   = (hashValue >> d_groupControlShift)
                                                        * GroupControl::k_SIZE;
    bsl::uint8_t hashlet = static_cast<bsl::uint8_t>(
                                                   hashValue & k_HASHLET_MASK);

    for (bsl::size_t i = 0; i < d_capacity; i += GroupControl::k_SIZE) {
        bsl::uint8_t *controlStart = d_controls_p + index;
        ENTRY        *entryStart   = d_entries_p  + index;

        GroupControl  groupControl(controlStart);
        bsl::uint32_t candidates = groupControl.match(hashlet);
        while (candidates) {
            int offset = bdlb::BitUtil::numTrailingUnsetBits(candidates);

            ENTRY *entry = entryStart + offset;

            if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
                                      d_equal(ENTRY_UTIL::key(*entry), key))) {
                return index + offset;                                // RETURN
            }
            candidates = bdlb::BitUtil::withBitCleared(candidates, offset);
        }
        if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(groupControl.neverFull())) {
            break;
        }

        index = (index + GroupControl::k_SIZE) & (d_capacity - 1);
    }

    return d_capacity;
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
bsl::size_t FlatHashTable<KEY,
                          ENTRY,
                          ENTRY_UTIL,
                          HASH,
                          EQUAL>::minimumCompliantCapacity(
                                             bsl::size_t minimumCapacity) const
{
    bsl::size_t minForEntries = ((d_size + k_MAX_LOAD_FACTOR_NUMERATOR - 1)
                                                 / k_MAX_LOAD_FACTOR_NUMERATOR)
                              * k_MAX_LOAD_FACTOR_DENOMINATOR;

    bsl::size_t capacity = minimumCapacity >= minForEntries
                         ? minimumCapacity
                         : minForEntries;

    if (0 < capacity) {
        capacity = capacity > k_MIN_CAPACITY
                ? static_cast<bsl::size_t>(bdlb::BitUtil::roundUpToBinaryPower(
                                         static_cast<bsl::uint64_t>(capacity)))
                 : k_MIN_CAPACITY;
    }

    return capacity;
}

// CREATORS
template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::FlatHashTable(
                                              bsl::size_t       capacity,
                                              const HASH&       hash,
                                              const EQUAL&      equal,
                                              bslma::Allocator *basicAllocator)
: d_entries_p(0)
, d_controls_p(0)
, d_size(0)
, d_capacity(0)
, d_groupControlShift(0)
, d_hasher(hash)
, d_equal(equal)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (0 < capacity) {
        d_capacity = capacity > k_MIN_CAPACITY
                ? static_cast<bsl::size_t>(bdlb::BitUtil::roundUpToBinaryPower(
                                         static_cast<bsl::uint64_t>(capacity)))
                   : k_MIN_CAPACITY;

        d_groupControlShift = static_cast<int>(
                                sizeof(bsl::size_t) * 8
                              - bdlb::BitUtil::log2(static_cast<bsl::uint64_t>(
                                                       d_capacity
                                                     / GroupControl::k_SIZE)));

        ENTRY *entries = static_cast<ENTRY *>(
                          d_allocator_p->allocate(d_capacity * sizeof(ENTRY)));

        bslma::DeallocatorProctor<bslma::Allocator> proctor(entries,
                                                            d_allocator_p);

        d_controls_p = static_cast<bsl::uint8_t *>(
                                          d_allocator_p->allocate(d_capacity));
        bsl::memset(d_controls_p, GroupControl::k_EMPTY, d_capacity);

        proctor.release();
        d_entries_p = entries;
    }
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::FlatHashTable(
                                          const FlatHashTable&  original,
                                          bslma::Allocator     *basicAllocator)
: d_entries_p(0)
, d_controls_p(0)
, d_size(0)
, d_capacity(0)
, d_groupControlShift(0)
, d_hasher(original.hash_function())
, d_equal(original.key_eq())
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (0 != original.d_capacity) {
        bsl::uint8_t *const controls = static_cast<bsl::uint8_t *>(
            d_allocator_p->allocate(original.d_capacity));
        bslma::DeallocatorProctor<bslma::Allocator> controlsProctor(
                                                                controls,
                                                                d_allocator_p);

        ENTRY *const entries = static_cast<ENTRY *>(
            d_allocator_p->allocate(original.d_capacity * sizeof(ENTRY)));
        bslma::DeallocatorProctor<bslma::Allocator> entriesProctor(
                                                                entries,
                                                                d_allocator_p);

        ImplUtil::copyEntryAndControlArrays(
            entries,
            controls,
            original.d_entries_p,
            original.d_entries_p + original.d_capacity,
            original.d_controls_p,
            original.d_controls_p + original.d_capacity,
            d_allocator_p);

        entriesProctor.release();
        controlsProctor.release();

        d_entries_p         = entries;
        d_controls_p        = controls;
        d_size              = original.d_size;
        d_capacity          = original.d_capacity;
        d_groupControlShift = original.d_groupControlShift;
    }
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::FlatHashTable(
                                     bslmf::MovableRef<FlatHashTable> original)
: d_entries_p(bslmf::MovableRefUtil::access(original).d_entries_p)
, d_controls_p(bslmf::MovableRefUtil::access(original).d_controls_p)
, d_size(bslmf::MovableRefUtil::access(original).d_size)
, d_capacity(bslmf::MovableRefUtil::access(original).d_capacity)
, d_groupControlShift(
                   bslmf::MovableRefUtil::access(original).d_groupControlShift)
, d_hasher(bslmf::MovableRefUtil::access(original).d_hasher)
, d_equal(bslmf::MovableRefUtil::access(original).d_equal)
, d_allocator_p(bslmf::MovableRefUtil::access(original).d_allocator_p)
{
    FlatHashTable& reference = original;

    reference.d_entries_p         = 0;
    reference.d_controls_p        = 0;
    reference.d_size              = 0;
    reference.d_capacity          = 0;
    reference.d_groupControlShift = 0;
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::FlatHashTable(
    bslmf::MovableRef<FlatHashTable>  original,
    bslma::Allocator                 *basicAllocator)
: d_entries_p(0)
, d_controls_p(0)
, d_size(0)
, d_capacity(0)
, d_groupControlShift(0)
, d_hasher(bslmf::MovableRefUtil::access(original).d_hasher)
, d_equal(bslmf::MovableRefUtil::access(original).d_equal)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    FlatHashTable& reference = original;
    if (d_allocator_p == reference.d_allocator_p) {
        bslalg::SwapUtil::swap(&d_entries_p, &reference.d_entries_p);
        bslalg::SwapUtil::swap(&d_controls_p, &reference.d_controls_p);
        bslalg::SwapUtil::swap(&d_size, &reference.d_size);
        bslalg::SwapUtil::swap(&d_capacity, &reference.d_capacity);
        bslalg::SwapUtil::swap(&d_groupControlShift,
                               &reference.d_groupControlShift);
    }
    else if (reference.d_capacity) {
        bsl::uint8_t *const controls = static_cast<bsl::uint8_t *>(
            d_allocator_p->allocate(reference.d_capacity));
        bslma::DeallocatorProctor<bslma::Allocator> controlsProctor(
                                                                controls,
                                                                d_allocator_p);

        ENTRY *const entries = static_cast<ENTRY *>(
            d_allocator_p->allocate(reference.d_capacity * sizeof(ENTRY)));
        bslma::DeallocatorProctor<bslma::Allocator> entriesProctor(
                                                                entries,
                                                                d_allocator_p);

        ImplUtil::copyEntryAndControlArrays(
            entries,
            controls,
            reference.d_entries_p,
            reference.d_entries_p + reference.d_capacity,
            reference.d_controls_p,
            reference.d_controls_p + reference.d_capacity,
            d_allocator_p);

        entriesProctor.release();
        controlsProctor.release();

        d_entries_p         = entries;
        d_controls_p        = controls;
        d_size              = reference.d_size;
        d_capacity          = reference.d_capacity;
        d_groupControlShift = reference.d_groupControlShift;
    }
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::~FlatHashTable()
{
    BSLS_ASSERT(
        (d_capacity == 0 && d_groupControlShift == 0) ||
        (d_groupControlShift ==
         static_cast<int>(sizeof(bsl::size_t) * 8 -
                          bdlb::BitUtil::log2(static_cast<bsl::uint64_t>(
                              d_capacity / GroupControl::k_SIZE)))));

    if (0 != d_entries_p) {
        ImplUtil::destroyEntryArray(d_entries_p,
                                    d_entries_p + d_capacity,
                                    d_controls_p,
                                    d_controls_p + d_capacity);

        d_allocator_p->deallocate(d_entries_p);
        d_allocator_p->deallocate(d_controls_p);
    }
}

// MANIPULATORS
template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>&
FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::operator=(
                                                      const FlatHashTable& rhs)
{
    if (this != &rhs) {
        FlatHashTable tmp(rhs, d_allocator_p);
        swap(tmp);
    }
    return *this;
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>&
FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::operator=(
                                          bslmf::MovableRef<FlatHashTable> rhs)
{
    FlatHashTable& reference = rhs;
    if (this != &reference) {
        FlatHashTable table(bslmf::MovableRefUtil::move(reference),
                            d_allocator_p);
        swap(table);
    }
    return *this;
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
template <class KEY_TYPE>
inline
ENTRY& FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::operator[](
                               BSLS_COMPILERFEATURES_FORWARD_REF(KEY_TYPE) key)
{
    bool        notFound;
    bsl::size_t hashValue = d_hasher(key);
    bsl::size_t index     = indexOfKey(&notFound, key, hashValue);

    if (notFound) {
        ENTRY_UTIL::constructFromKey(d_entries_p + index,
                                 d_allocator_p,
                                 BSLS_COMPILERFEATURES_FORWARD(KEY_TYPE, key));

        d_controls_p[index] = static_cast<bsl::uint8_t>(
                                                   hashValue & k_HASHLET_MASK);

        ++d_size;
    }

    return d_entries_p[index];
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
void FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::clear()
{
    ImplUtil::destroyEntryArray(d_entries_p,
                                d_entries_p + d_capacity,
                                d_controls_p,
                                d_controls_p + d_capacity);

    if (d_controls_p) {
        bsl::memset(d_controls_p, GroupControl::k_EMPTY, d_capacity);
    }

    d_size = 0;
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
bsl::pair<
         typename FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::iterator,
         typename FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::iterator>
FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::equal_range(const KEY& key)
{
    iterator it1 = find(key);
    if (it1 == end()) {
        return bsl::make_pair(it1, it1);                              // RETURN
    }
    iterator it2 = it1;
    ++it2;
    return bsl::make_pair(it1, it2);
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
template< class... ARGS>
inline
bsl::pair<typename
                  FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::iterator,
          bool>
FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::emplace(ARGS&&... args)
{
    bsls::ObjectBuffer<entry_type> value;
    ENTRY_UTIL::construct(value.address(),
                          d_allocator_p,
                          BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);

    bslma::DestructorGuard<entry_type> guard(value.address());
    return this->insert(bslmf::MovableRefUtil::move(value.object()));
}
#endif


template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
bsl::size_t FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::erase(
                                                                const KEY& key)
{
    iterator it = find(key);
    if (it == end()) {
        return 0;                                                     // RETURN
    }
    erase(it);
    return 1;
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
typename FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::iterator
FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::erase(
                        typename FlatHashTable<KEY,
                                               ENTRY,
                                               ENTRY_UTIL,
                                               HASH,
                                               EQUAL>::const_iterator position)
{
    BSLS_ASSERT_SAFE(position != end());

    bsl::size_t index = &*position - d_entries_p;
    bslma::DestructionUtil::destroy(d_entries_p + index);
    d_controls_p[index] = GroupControl::k_ERASED;
    --d_size;

    if (d_size) {
        for (bsl::size_t i = index + 1; i < d_capacity; ++i) {
            if (0 == (d_controls_p[i] & GroupControl::k_EMPTY)) {
                return iterator(IteratorImp(d_entries_p  + i,
                                            d_controls_p + i,
                                            d_capacity   - i - 1));   // RETURN
            }
        }
    }
    return iterator();
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
typename FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::iterator
FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::erase(
                        typename FlatHashTable<KEY,
                                               ENTRY,
                                               ENTRY_UTIL,
                                               HASH,
                                               EQUAL>::iterator position)
{
    // Note that this overload is necessary to avoid ambiguity when the key is
    // a table iterator.

    BSLS_ASSERT_SAFE(position != end());

    bsl::size_t index = &*position - d_entries_p;
    bslma::DestructionUtil::destroy(d_entries_p + index);
    d_controls_p[index] = GroupControl::k_ERASED;
    --d_size;

    if (d_size) {
        for (bsl::size_t i = index + 1; i < d_capacity; ++i) {
            if (0 == (d_controls_p[i] & GroupControl::k_EMPTY)) {
                return iterator(IteratorImp(d_entries_p  + i,
                                            d_controls_p + i,
                                            d_capacity   - i - 1));   // RETURN
            }
        }
    }
    return iterator();
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
typename FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::iterator
FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::erase(
                           typename FlatHashTable<KEY,
                                                  ENTRY,
                                                  ENTRY_UTIL,
                                                  HASH,
                                                  EQUAL>::const_iterator first,
                           typename FlatHashTable<KEY,
                                                  ENTRY,
                                                  ENTRY_UTIL,
                                                  HASH,
                                                  EQUAL>::const_iterator last)
{
    iterator rv;
    {
        if (last != end()) {
            bsl::size_t index = &*last - d_entries_p;
            rv = iterator(IteratorImp(d_entries_p  + index,
                                      d_controls_p + index,
                                      d_capacity   - index - 1));
        }
        else {
            rv = end();
        }
    }

    for (; first != last; ++first) {
        erase(first);
    }

    return rv;
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
typename FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::iterator
FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::find(const KEY& key)
{
    bsl::size_t index = findKey(key, d_hasher(key));
    if (index < d_capacity) {
        return iterator(IteratorImp(d_entries_p  + index,
                                    d_controls_p + index,
                                    d_capacity   - index - 1));       // RETURN
    }
    return end();
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
template <class INPUT_ITERATOR>
inline
void FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::insert(
                                                          INPUT_ITERATOR first,
                                                          INPUT_ITERATOR last)
{
    for (; first != last; ++first) {
        insert(*first);
    }
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
void FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::rehash(
                                                   bsl::size_t minimumCapacity)
{
    minimumCapacity = minimumCompliantCapacity(minimumCapacity);

    if (0 < minimumCapacity) {
        rehashRaw(minimumCapacity);
    }
    else {
        d_allocator_p->deallocate(d_entries_p);
        d_allocator_p->deallocate(d_controls_p);

        d_entries_p         = 0;
        d_controls_p        = 0;
        d_capacity          = 0;
        d_groupControlShift = 0;
    }
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
void FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::reserve(
                                                        bsl::size_t numEntries)
{
    if (0 == d_capacity && 0 == numEntries) {
        // From DRQS 167247593, 'reserve(0)' on an empty container is a
        // performance concern.

        return;                                                       // RETURN
    }

    bsl::size_t minForEntries = ((numEntries + k_MAX_LOAD_FACTOR_NUMERATOR - 1)
                                                 / k_MAX_LOAD_FACTOR_NUMERATOR)
                              * k_MAX_LOAD_FACTOR_DENOMINATOR;

    rehash(minForEntries);
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
void FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::reset()
{
    if (0 != d_entries_p) {
        ImplUtil::destroyEntryArray(d_entries_p,
                                    d_entries_p + d_capacity,
                                    d_controls_p,
                                    d_controls_p + d_capacity);

        d_allocator_p->deallocate(d_entries_p);
        d_allocator_p->deallocate(d_controls_p);

        d_entries_p         = 0;
        d_controls_p        = 0;
        d_capacity          = 0;
        d_size              = 0;
        d_groupControlShift = 0;
    }
}

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
/// Note: `args` contains `key`
template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
template< class... ARGS>
bsl::pair<typename
                  FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::iterator,
          bool>
FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::try_emplace(
                                                               const KEY& key,
                                                               ARGS&&...  args)
{
    bool        notFound;
    bsl::size_t hashValue = d_hasher(key);
    bsl::size_t index     = indexOfKey(&notFound, key, hashValue);

    if (notFound) {
        ENTRY_UTIL::construct(d_entries_p + index,
                              d_allocator_p,
                              BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);

        d_controls_p[index] = static_cast<bsl::uint8_t>(
                                                   hashValue & k_HASHLET_MASK);
        ++d_size;
    }

    return bsl::pair<iterator, bool>(IteratorImp(d_entries_p  + index,
                                                 d_controls_p + index,
                                                 d_capacity   - index - 1),
                                     notFound);
}

/// Note: `args` contains `key`
template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
template< class... ARGS>
bsl::pair<typename
                  FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::iterator,
          bool>
FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::try_emplace(
                                      BloombergLP::bslmf::MovableRef<KEY> key,
                                      ARGS&&...                           args)
{
    const KEY&  k = key;
    bool        notFound;
    bsl::size_t hashValue = d_hasher(k);
    bsl::size_t index     = indexOfKey(&notFound, k, hashValue);

    if (notFound) {
        ENTRY_UTIL::construct(d_entries_p + index,
                              d_allocator_p,
                              BSLS_COMPILERFEATURES_FORWARD(ARGS, args)...);

        d_controls_p[index] = static_cast<bsl::uint8_t>(
                                                   hashValue & k_HASHLET_MASK);
        ++d_size;
    }
    return bsl::pair<iterator, bool>(IteratorImp(d_entries_p  + index,
                                                 d_controls_p + index,
                                                 d_capacity   - index - 1),
                                     notFound);
}
#endif

                            // Iterators

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
typename FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::iterator
FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::begin()
{
    if (d_size) {
        for (bsl::size_t i = 0; i < d_capacity; ++i) {
            if (0 == (d_controls_p[i] & GroupControl::k_EMPTY)) {
                return iterator(IteratorImp(d_entries_p  + i,
                                            d_controls_p + i,
                                            d_capacity   - i - 1));   // RETURN
            }
        }
    }
    return iterator();
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
typename FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::iterator
FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::end()
{
    return iterator();
}

                           // Aspects

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
void FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::swap(
                                                          FlatHashTable& other)
{
    BSLS_ASSERT_SAFE(allocator() == other.allocator());

    bslalg::SwapUtil::swap(&d_entries_p, &other.d_entries_p);
    bslalg::SwapUtil::swap(&d_controls_p, &other.d_controls_p);
    bslalg::SwapUtil::swap(&d_size, &other.d_size);
    bslalg::SwapUtil::swap(&d_capacity, &other.d_capacity);
    bslalg::SwapUtil::swap(&d_groupControlShift, &other.d_groupControlShift);
    bslalg::SwapUtil::swap(&d_hasher, &other.d_hasher);
    bslalg::SwapUtil::swap(&d_equal, &other.d_equal);
}

// ACCESSORS
template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
bsl::size_t FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::
                                                               capacity() const
{
    return d_capacity;
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
bool FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::contains(
                                                          const KEY& key) const
{
    return find(key) != end();
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
const bsl::uint8_t *FlatHashTable<KEY,
                                  ENTRY,
                                  ENTRY_UTIL,
                                  HASH,
                                  EQUAL>::controls() const
{
    return d_controls_p;
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
bsl::size_t FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::count(
                                                          const KEY& key) const
{
    return contains(key) ? 1 : 0;
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
bool FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::empty() const
{
    return 0 == d_size;
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
const ENTRY *FlatHashTable<KEY,
                           ENTRY,
                           ENTRY_UTIL,
                           HASH,
                           EQUAL>::entries() const
{
    return d_entries_p;
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
bsl::pair<typename FlatHashTable<KEY,
                                 ENTRY,
                                 ENTRY_UTIL,
                                 HASH,
                                 EQUAL>::const_iterator,
          typename FlatHashTable<KEY,
                                 ENTRY,
                                 ENTRY_UTIL,
                                 HASH,
                                 EQUAL>::const_iterator>
FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::equal_range(
                                                          const KEY& key) const
{
    const_iterator cit1 = find(key);
    const_iterator cit2 = cit1;
    if (cit1 != end()) {
        ++cit2;
    }
    return bsl::make_pair(cit1, cit2);
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
typename FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::const_iterator
FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::find(const KEY& key) const
{
    bsl::size_t index = findKey(key, d_hasher(key));
    if (index < d_capacity) {
        return const_iterator(IteratorImp(d_entries_p  + index,
                                          d_controls_p + index,
                                          d_capacity   - index - 1)); // RETURN
    }
    return end();
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
HASH FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::hash_function() const
{
    return d_hasher;
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
EQUAL FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::key_eq() const
{
    return d_equal;
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
float bdlc::FlatHashTable<KEY,
                          ENTRY,
                          ENTRY_UTIL,
                          HASH,
                          EQUAL>::load_factor() const
{
    return d_capacity > 0
         ? static_cast<float>(d_size) / static_cast<float>(d_capacity)
         : 0;
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
float bdlc::FlatHashTable<KEY,
                          ENTRY,
                          ENTRY_UTIL,
                          HASH,
                          EQUAL>::max_load_factor() const
{
    return static_cast<float>(k_MAX_LOAD_FACTOR_NUMERATOR)
         / static_cast<float>(k_MAX_LOAD_FACTOR_DENOMINATOR);
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
bsl::size_t FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::size() const
{
    return d_size;
}

                            // Iterators

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
typename FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::const_iterator
FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::begin() const
{
    if (d_size) {
        for (bsl::size_t i = 0; i < d_capacity; ++i) {
            if (0 == (d_controls_p[i] & GroupControl::k_EMPTY)) {
                return const_iterator(
                                 IteratorImp(d_entries_p  + i,
                                             d_controls_p + i,
                                             d_capacity   - i - 1));  // RETURN
            }
        }
    }
    return const_iterator();
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
typename FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::const_iterator
FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::cbegin() const
{
    return begin();
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
typename FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::const_iterator
FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::cend() const
{
    return end();
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
typename FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::const_iterator
FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::end() const
{
    return const_iterator();
}

                           // Aspects

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
bslma::Allocator *FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>::
                                                              allocator() const
{
    return d_allocator_p;
}

}  // close package namespace

// FREE OPERATORS
template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
bool bdlc::operator==(const FlatHashTable<KEY,
                                          ENTRY,
                                          ENTRY_UTIL,
                                          HASH,
                                          EQUAL>& lhs,
                      const FlatHashTable<KEY,
                                          ENTRY,
                                          ENTRY_UTIL,
                                          HASH,
                                          EQUAL>& rhs)
{
    typedef typename FlatHashTable<KEY,
                                   ENTRY,
                                   ENTRY_UTIL,
                                   HASH,
                                   EQUAL>::const_iterator ConstIterator;

    if (lhs.size() == rhs.size()) {
        ConstIterator lhsEnd = lhs.end();
        ConstIterator rhsEnd = rhs.end();

        if (lhs.capacity() <= rhs.capacity()) {
            for (ConstIterator it = lhs.begin(); it != lhsEnd; ++it) {
                ConstIterator i = rhs.find(ENTRY_UTIL::key(*it));
                if (i == rhsEnd || *i != *it) {
                    return false;                                     // RETURN
                }
            }
            return true;                                              // RETURN
        }
        else {
            for (ConstIterator it = rhs.begin(); it != rhsEnd; ++it) {
                ConstIterator i = lhs.find(ENTRY_UTIL::key(*it));
                if (i == lhsEnd || *i != *it) {
                    return false;                                     // RETURN
                }
            }
            return true;                                              // RETURN
        }
    }
    return false;
}

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
bool bdlc::operator!=(const FlatHashTable<KEY,
                                          ENTRY,
                                          ENTRY_UTIL,
                                          HASH,
                                          EQUAL>& lhs,
                      const FlatHashTable<KEY,
                                          ENTRY,
                                          ENTRY_UTIL,
                                          HASH,
                                          EQUAL>& rhs)
{
    return !(lhs == rhs);
}

// FREE FUNCTIONS
template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
inline
void bdlc::swap(FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>& a,
                FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL>& b)
{
    if (a.allocator() == b.allocator()) {
        a.swap(b);

        return;                                                       // RETURN
    }

    typedef FlatHashTable<KEY, ENTRY, ENTRY_UTIL, HASH, EQUAL> Table;

    Table futureA(b, a.allocator());
    Table futureB(a, b.allocator());

    futureA.swap(a);
    futureB.swap(b);
}

namespace bslma {

template <class KEY, class ENTRY, class ENTRY_UTIL, class HASH, class EQUAL>
struct UsesBslmaAllocator<bdlc::FlatHashTable<KEY,
                                              ENTRY,
                                              ENTRY_UTIL,
                                              HASH,
                                              EQUAL> > : bsl::true_type {
};

}  // close namespace bslma

namespace bdlc {

                       // -----------------------------
                       // struct FlatHashTable_ImplUtil
                       // -----------------------------

// PRIVATE CLASS METHODS
template <class ENTRY_TYPE>
inline
void FlatHashTable_ImplUtil::copyEntryAndControlArrays(
                                   ENTRY_TYPE         *firstDestinationEntry,
                                   bsl::uint8_t       *firstDestinationControl,
                                   const ENTRY_TYPE   *firstSourceEntry,
                                   const ENTRY_TYPE   *lastSourceEntry,
                                   const bsl::uint8_t *firstSourceControl,
                                   const bsl::uint8_t *lastSourceControl,
                                   bslma::Allocator   *entryAllocator,
                                   bsl::false_type     isBitwiseCopyable)
{
    (void) isBitwiseCopyable;

    BSLMF_ASSERT((! bslmf::IsBitwiseCopyable<ENTRY_TYPE>::value));

    bsl::memcpy(firstDestinationControl,
                firstSourceControl,
                bsl::distance(firstSourceControl, lastSourceControl) *
                    sizeof(bsl::uint8_t));

    const bsl::size_t numEntries = static_cast<bsl::size_t>(
        bsl::distance(firstSourceEntry, lastSourceEntry));

    DestroyEntryArrayProctor<ENTRY_TYPE> destroyEntriesProctor(
                                                      firstDestinationEntry,
                                                      firstDestinationEntry,
                                                      firstDestinationControl,
                                                      firstDestinationControl);

    for (bsl::size_t idx = 0; idx != numEntries; ++idx) {
        ENTRY_TYPE&         destinationEntry = *(firstDestinationEntry + idx);
        const bsl::uint8_t& sourceControl    = *(firstSourceControl + idx);
        const ENTRY_TYPE&   sourceEntry      = *(firstSourceEntry + idx);

        if (0 == (sourceControl & GroupControl::k_EMPTY)) {
            bslma::ConstructionUtil::construct(
                &destinationEntry, entryAllocator, sourceEntry);
        }

        destroyEntriesProctor.moveEnd(1);
    }

    destroyEntriesProctor.release();
}

template <class ENTRY_TYPE>
inline
void FlatHashTable_ImplUtil::copyEntryAndControlArrays(
                         ENTRY_TYPE                   *firstDestinationEntry,
                         bsl::uint8_t                 *firstDestinationControl,
                         const ENTRY_TYPE             *firstSourceEntry,
                         const ENTRY_TYPE             *lastSourceEntry,
                         const bsl::uint8_t           *firstSourceControl,
                         const bsl::uint8_t           *lastSourceControl,
                         bslma::Allocator             *,
                         bsl::true_type                isBitwiseCopyable)
{
    (void) isBitwiseCopyable;

    BSLMF_ASSERT((bslmf::IsBitwiseCopyable<ENTRY_TYPE>::value));

    bsl::memcpy(firstDestinationControl,
                firstSourceControl,
                bsl::distance(firstSourceControl, lastSourceControl) *
                    sizeof(bsl::uint8_t));

#if defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VERSION >= 80000
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif

    bsl::memcpy(firstDestinationEntry,
                firstSourceEntry,
                bsl::distance(firstSourceEntry, lastSourceEntry) *
                    sizeof(ENTRY_TYPE));

#if defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VERSION >= 80000
#pragma GCC diagnostic pop
#endif
}

template <class ENTRY_TYPE>
inline
void FlatHashTable_ImplUtil::destroyEntryArray(
                                     ENTRY_TYPE         *firstEntry,
                                     ENTRY_TYPE         *lastEntry,
                                     const bsl::uint8_t *firstControl,
                                     const bsl::uint8_t *lastControl,
                                     bsl::false_type     triviallyDestructible)
{
    (void) triviallyDestructible;

    ///Implementation Note
    ///-------------------
    // The implementation of this function uses the
    // 'FlatHashTable_GroupControl' facilities to bulk search through the
    // control range and look for entries that need to be destroyed.  However,
    // the size of the ranges passed to this function are not always a multiple
    // of 'GroupControl::k_SIZE', and so a second loop is needed to
    // sequentially destroy the up-to 'GroupControl::k_SIZE - 1' number of
    // entries that cannot be destroyed as part of a bulk operation on
    // 'GroupControl::k_SIZE' elements of the entry range.
    //
    // It may be surprising that the size of these ranges is not always a
    // multiple of 'GroupControl::k_SIZE' despite the fact that the capacity of
    // a 'FlatHashTable' is always a power of 2.  However, this operation can
    // be invoked midway through copying one entry array to another if copying
    // an entry throws and the already-copied elements need to then be
    // destroyed as part of cleaning up during exception handling.

    static_cast<void>(lastControl); // silence unused variable warnings

    const bsl::size_t numEntries =
        static_cast<bsl::size_t>(bsl::distance(firstEntry, lastEntry));
    const bsl::size_t numGroupedEntries =
        numEntries - (numEntries % GroupControl::k_SIZE);

    for (bsl::size_t idx  = 0;
                     idx != numGroupedEntries;
                     idx += GroupControl::k_SIZE) {
        GroupControl  groupControl(firstControl + idx);
        bsl::uint32_t candidates = groupControl.inUse();
        while (candidates) {
            const int offset = bdlb::BitUtil::numTrailingUnsetBits(candidates);
            bslma::DestructionUtil::destroy(firstEntry + idx + offset);
            candidates = bdlb::BitUtil::withBitCleared(candidates, offset);
        }
    }

    for (bsl::size_t idx = numGroupedEntries; idx != numEntries; ++idx) {
        ENTRY_TYPE&         entry   = *(firstEntry + idx);
        const bsl::uint8_t& control = *(firstControl + idx);

        if (0 == (control & GroupControl::k_EMPTY)) {
            bslma::DestructionUtil::destroy(&entry);
        }
    }
}

template <class ENTRY_TYPE>
inline
void FlatHashTable_ImplUtil::destroyEntryArray(
                                     ENTRY_TYPE         *,
                                     ENTRY_TYPE         *,
                                     const bsl::uint8_t *,
                                     const bsl::uint8_t *,
                                     bsl::true_type      triviallyDestructible)
{
    (void) triviallyDestructible;

    // Do nothing, in just the right way.
}

// CLASS METHODS
template <class ENTRY_TYPE>
inline
void FlatHashTable_ImplUtil::copyEntryAndControlArrays(
                                   ENTRY_TYPE         *firstDestinationEntry,
                                   bsl::uint8_t       *firstDestinationControl,
                                   const ENTRY_TYPE   *firstSourceEntry,
                                   const ENTRY_TYPE   *lastSourceEntry,
                                   const bsl::uint8_t *firstSourceControl,
                                   const bsl::uint8_t *lastSourceControl,
                                   bslma::Allocator   *entryAllocator)
{
    BSLS_ASSERT_SAFE(bsl::distance(firstSourceEntry, lastSourceEntry) ==
                     bsl::distance(firstSourceControl, lastSourceControl));

    FlatHashTable_ImplUtil::copyEntryAndControlArrays(
                                       firstDestinationEntry,
                                       firstDestinationControl,
                                       firstSourceEntry,
                                       lastSourceEntry,
                                       firstSourceControl,
                                       lastSourceControl,
                                       entryAllocator,
                                       bslmf::IsBitwiseCopyable<ENTRY_TYPE>());
}

template <class ENTRY_TYPE>
inline
void FlatHashTable_ImplUtil::destroyEntryArray(
                                              ENTRY_TYPE         *firstEntry,
                                              ENTRY_TYPE         *lastEntry,
                                              const bsl::uint8_t *firstControl,
                                              const bsl::uint8_t *lastControl)
{
    BSLS_ASSERT_SAFE(bsl::distance(firstEntry, lastEntry) ==
                     bsl::distance(firstControl, lastControl));

    ///Implementation Note
    ///-------------------
    // If a type is trivially copyable then it is also trivially destructible.
    // The type trait 'bslmf::IsTriviallyCopyable' is available in C++03
    // mode and later, however 'bsl::is_trivially_destructible' is only
    // available in C++11 and later and not always on clang.  So, this utility
    // 'struct' uses bitwise copyability as a stand-in for trivial
    // destructibility in order to provide certain optimizations uniformly
    // across all C++ versions.

    typedef bslmf::IsBitwiseCopyable<ENTRY_TYPE>
                                              IsEntryTypeTriviallyDestructible;

    FlatHashTable_ImplUtil::destroyEntryArray(
                                           firstEntry,
                                           lastEntry,
                                           firstControl,
                                           lastControl,
                                           IsEntryTypeTriviallyDestructible());
}

           // ------------------------------------------------------
           // class FlatHashTable_ImplUtil::DestroyEntryArrayProctor
           // ------------------------------------------------------

// CREATORS
template <class ENTRY_TYPE>
inline
FlatHashTable_ImplUtil::DestroyEntryArrayProctor<
    ENTRY_TYPE>::DestroyEntryArrayProctor(ENTRY_TYPE         *firstEntry,
                                          ENTRY_TYPE         *lastEntry,
                                          const bsl::uint8_t *firstControl,
                                          const bsl::uint8_t *lastControl)
: d_firstEntry_p(firstEntry)
, d_lastEntry_p(lastEntry)
, d_firstControl_p(firstControl)
, d_lastControl_p(lastControl)
{
}

template <class ENTRY_TYPE>
inline
FlatHashTable_ImplUtil::DestroyEntryArrayProctor<
    ENTRY_TYPE>::~DestroyEntryArrayProctor()
{
    ImplUtil::destroyEntryArray(d_firstEntry_p,
                                d_lastEntry_p,
                                d_firstControl_p,
                                d_lastControl_p);
}

// MANIPULATORS
template <class ENTRY_TYPE>
inline
void FlatHashTable_ImplUtil::DestroyEntryArrayProctor<ENTRY_TYPE>::moveEnd(
                                                         bsl::ptrdiff_t offset)
{
    d_lastEntry_p += offset;
    d_lastControl_p += offset;
}

template <class ENTRY_TYPE>
inline
void FlatHashTable_ImplUtil::DestroyEntryArrayProctor<ENTRY_TYPE>::release()
{
    d_firstEntry_p   = 0;
    d_lastEntry_p    = 0;
    d_firstControl_p = 0;
    d_lastControl_p  = 0;
}

}  // close package namespace
}  // close enterprise namespace

#endif // End C++11 code

#endif

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
// Copyright 2018 The Abseil Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License.  You may obtain a copy
// of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
// License for the specific language governing permissions and limitations
// under the License.
// ----------------------------- END-OF-FILE ----------------------------------
