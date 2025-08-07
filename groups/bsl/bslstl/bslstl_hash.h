// bslstl_hash.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLSTL_HASH
#define INCLUDED_BSLSTL_HASH

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace for hash functions.
//
//@CLASSES:
//  bsl::hash: hash function for fundamental types
//
//@CANONICAL_HEADER: bsl_functional.h
//
//@SEE_ALSO: package bos+stdhdrs in the bos package group
//
//@DESCRIPTION: This component provides a template unary functor, `bsl::hash`,
// implementing the `std::hash` functor.  `bsl::hash` applies a C++ standard
// compliant, implementation defined, hash function to fundamental types
// returning the result of such application.
//
///Standard Hash Function
///----------------------
// According to the C++ standard the requirements of a standard hash function
// `h` are:
//
// 1. Return a `size_t` value between 0 and
//    `numeric_limits<std::size_t>::max()`.
// 2. The value returned must depend only on the argument `k`.  For multiple
//    evaluations with the same argument `k`, the value returned must be
//    always the same.
// 3. The function should not modify its argument.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Creating and Using a Hash Cross Reference
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we already have an array of unique values of type `TYPE`, for which
// `operator==` is defined, and we want to be able to quickly look up whether
// an element is in the array, without exhaustively applying `operator==` to
// all the elements in sequence.  The array itself is guaranteed not to change
// for the duration of our interest in it.
//
// The problem is much simpler than building a general-purpose hash table,
// because we know how many elements our cross reference will contain in
// advance, so we will never have to dynamically grow the number of `buckets`.
// We do not need to copy the values into our own area, so we don't have to
// create storage for them, or require that a copy constructor or destructor be
// available.  We only require that they have a transitive, symmetric
// equivalence operation `bool operator==` and that a hash function be
// provided.
//
// We will need a hash function -- the hash function is a function that will
// take as input an object of the type stored in our array, and yield a
// `size_t` value that will be very randomized.  Ideally, the slightest change
// in the value of the `TYPE` object will result in a large change in the value
// returned by the hash function.  In a good hash function, typically half the
// bits of the return value will change for a 1-bit change in the hashed value.
// We then use the result of the hash function to index into our array of
// `buckets`.  Each `bucket` is simply a pointer to a value in our original
// array of `TYPE` objects.  We will resolve hash collisions in our array
// through `linear probing`, where we will search consecutive buckets following
// the bucket where the collision occurred, testing occupied buckets for
// equality with the value we are searching on, and concluding that the value
// is not in the table if we encounter an empty bucket before we encounter one
// referring to an equal element.
//
// An important quality of the hash function is that if two values are
// equivalent, they must yield the same hash value.
//
// First, we define our `HashCrossReference` template class, with the two type
// parameters `TYPE` (the type being referenced) and `HASHER`, which defaults
// to `bsl::hash<TYPE>`.  For common types of `TYPE` such as `int`, a
// specialization of `bsl::hash` is already defined:
// ```
// /// This table leverages a hash table to provide a fast lookup of an
// /// external, non-owned, array of values of configurable type.
// ///
// /// The only requirement for `TYPE` is that it have a transitive,
// /// symmetric `operator==` function.  There is no requirement that it
// /// have any kind of creator defined.
// ///
// /// The `HASHER` template parameter type must be a functor with a
// /// function of the following signature:
// /// ```
// /// size_t operator()(const TYPE)  const; or
// /// size_t operator()(const TYPE&) const; or
// /// ```
// /// and `HASHER` must have a publicly available default constructor and
// /// destructor.
// template <class TYPE, class HASHER = bsl::hash<TYPE> >
// class HashCrossReference {
//
//     // DATA
//     const TYPE       *d_values;             // Array of values table is to
//                                             // cross-reference.  Held, not
//                                             // owned.
//     size_t            d_numValues;          // Length of 'd_values'.
//     const TYPE      **d_bucketArray;        // Contains pointers into
//                                             // 'd_values'.
//     size_t            d_bucketArrayMask;    // Will always be '2^N - 1'.
//     HASHER            d_hasher;
//     bool              d_valid;              // Object was properly
//                                             // initialized.
//     bslma::Allocator *d_allocator_p;        // Held, not owned.
//
//   private:
//     // PRIVATE ACCESSORS
//
//     /// Look up the specified `value`, having hash value `hashValue`,
//     /// and return its index in `d_bucketArray` stored in the specified
//     /// `index`.  If not found, return the vacant entry in
//     /// `d_bucketArray` where it should be inserted.  Return `true` if
//     /// `value` is found and `false` otherwise.
//     bool lookup(size_t      *index,
//                 const TYPE&  value,
//                 size_t       hashValue) const
//     {
//         const TYPE *ptr;
//         for (*index = hashValue & d_bucketArrayMask;
//              static_cast<bool>(ptr = d_bucketArray[*index]);
//              *index = (*index + 1) & d_bucketArrayMask) {
//             if (value == *ptr) {
//                 return true;                                      // RETURN
//             }
//         }
//         // value was not found in table
//
//         return false;
//     }
//
//     // NOT IMPLEMENTED
//     HashCrossReference(const HashCrossReference&);
//     HashCrossReference& operator=(const HashCrossReference&);
//
//   public:
//     // CREATORS
//
//     /// Create a hash table refering to the specified `valuesArray`
//     /// containing the specified `numValues` elements.  Optionally
//     /// specify `basicAllocator` or the default allocator will be used.
//     HashCrossReference(const TYPE       *valuesArray,
//                        size_t            numValues,
//                        bslma::Allocator *basicAllocator = 0)
//     : d_values(valuesArray)
//     , d_numValues(numValues)
//     , d_hasher()
//     , d_valid(true)
//     , d_allocator_p(bslma::Default::allocator(allocator))
//     {
//         size_t bucketArrayLength = 4;
//         while (bucketArrayLength < numValues * 4) {
//             bucketArrayLength *= 2;
//             BSLS_ASSERT_OPT(bucketArrayLength);
//         }
//         d_bucketArrayMask = bucketArrayLength - 1;
//         d_bucketArray = (const TYPE **) d_allocator_p->allocate(
//                                       bucketArrayLength * sizeof(TYPE **));
//         memset(d_bucketArray,  0, bucketArrayLength * sizeof(TYPE *));
//
//         for (unsigned i = 0; i < numValues; ++i) {
//             const TYPE& value = d_values[i];
//
//             size_t idx;
//             if (lookup(&idx, value, d_hasher(value))) {
//                 // Duplicate value.  Fail.
//
//                 printf("Error: entries %u and %u have the same value\n",
//                                i, unsigned(d_bucketArray[idx] - d_values));
//                 d_valid = false;
//
//                 // don't return, continue reporting other redundant
//                 // entries.
//             }
//             else {
//                 d_bucketArray[idx] = &d_values[i];
//             }
//         }
//     }
//
//     /// Free up memory used by this cross-reference.
//     ~HashCrossReference()
//     {
//         d_allocator_p->deallocate(d_bucketArray);
//     }
//
//     // ACCESSORS
//
//     /// Return 1 if the specified `value` is found in the cross
//     /// reference and 0 otherwise.
//     int count(const TYPE& value) const
//     {
//         BSLS_ASSERT_OPT(d_valid);
//
//         size_t idx;
//         return lookup(&idx, value, d_hasher(value));
//     }
//
//     /// Return `true` if this cross reference was successfully
//     /// constructed and `false` otherwise.
//     bool isValid() const
//     {
//         return d_valid;
//     }
// };
// ```
// Then, In `main`, we will first use our cross-reference to cross-reference a
// collection of integer values.  We define our array and take its length:
// ```
// const int ints[] = { 23, 42, 47, 56, 57, 61, 62, 63, 70, 72, 79 };
// enum { NUM_INTS = sizeof ints / sizeof *ints };
// ```
// Now, we create our cross-reference `hcri` and verify it constructed
// properly.  Note that we don't specify the second template parameter `HASHER`
// and let it default to `bsl::hash<int>`, which is already defined by
// bslstl_hash:
// ```
// HashCrossReference<int> hcri(ints, NUM_INTS);
// assert(hcri.isValid());
// ```
// Finally, we use `hcri` to verify numbers that were and were not in the
// collection:
// ```
// assert(1 == hcri.count(23));
// assert(1 == hcri.count(42));
// assert(1 == hcri.count(47));
// assert(1 == hcri.count(56));
// assert(0 == hcri.count( 3));
// assert(0 == hcri.count(31));
// assert(0 == hcri.count(37));
// assert(0 == hcri.count(58));
// ```
//
///Example 2: Using `hashAppend` from `bslh` with `HashCrossReference`
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// We want to specialize `bsl::hash` for a custom class.  We can use the
// modular hashing system implemented in `bslh` rather than explicitly
// specializing `bsl::hash`.  We will re-use the `HashCrossReference` template
// class defined in Example 1.
//
// First, we declare `Point`, a class that allows us to identify a location on
// a two dimensional Cartesian plane.
// ```
// /// This class is a value-semantic type that represents a two-dimensional
// /// location on a Cartesian plane.
// class Point {
//
//   private:
//     int    d_x;
//     int    d_y;
//     double d_distToOrigin; // This value will be accessed a lot, so we
//                            // cache it rather than recalculating every
//                            // time.
//
//   public:
//
//     /// Create a `Point` with the specified `x` and `y` coordinates
//     Point (int x, int y);
//
//     /// Return the distance from the origin (0, 0) to this point.
//     double distanceToOrigin();
// ```
// Then, we declare `operator==` as a friend so that we will be able to compare
// two points.
// ```
//     friend bool operator==(const Point &left, const Point &right);
//
// ```
// Next, we declare `hashAppend` as a friend so that we will be able hash a
// `Point`.
// ```
//     /// Apply the specified `hashAlg` to the specified `point`.
//     template <class HASH_ALGORITHM>
//     friend
//     void hashAppend(HASH_ALGORITHM &hashAlg, const Point &point);
// };
//
// Point::Point(int x, int y)
// : d_x(x)
// , d_y(y)
// {
//     d_distToOrigin = sqrt(static_cast<double>(d_x) * d_x +
//                           static_cast<double>(d_y) * d_y);
// }
//
// double Point::distanceToOrigin()
// {
//     return d_distToOrigin;
// }
//
// ```
// Then, we define `operator==`.  Notice how it only checks salient attributes
// - attributes that contribute to the value of the class.  We ignore
// `d_distToOrigin` which is not required to determine equality.
// ```
// bool operator==(const Point &left, const Point &right)
// {
//     return (left.d_x == right.d_x) && (left.d_y == right.d_y);
// }
//
// ```
// Next, we define `hashAppend`.  This method will allow any hashing algorithm
// to be applied to `Point`.  This is the extent of the work that needs to be
// done by type creators.  They do not need to implement any algorithms, they
// just need to call out the salient attributes (which have already been
// determined by `operator==`) by calling `hashAppend` on them.
// ```
// template <class HASH_ALGORITHM>
// void hashAppend(HASH_ALGORITHM &hashAlg, const Point &point)
// {
//     using ::BloombergLP::bslh::hashAppend;
//     hashAppend(hashAlg, point.d_x);
//     hashAppend(hashAlg, point.d_y);
// }
//
// ```
// Then, we declare another value-semantic type, `Box`, that has a `Point` as
// one of its salient attributes.
// ```
// /// This class is a value-semantic type that represents a box drawn on to
// /// a Cartesian plane.
// class Box {
//
//   private:
//     Point d_position;
//     int   d_length;
//     int   d_width;
//
//   public:
//     /// Create a box with the specified `length` and `width`, with its
//     /// upper left corner at the specified `position`
//     Box(Point position, int length, int width);
//
// ```
// Next, we declare `operator==` and `hashAppend` as we did before.
// ```
//     friend bool operator==(const Box &left, const Box &right);
//
//     /// Apply the specified `hashAlg` to the specified `box`.
//     template <class HASH_ALGORITHM>
//     friend
//     void hashAppend(HASH_ALGORITHM &hashAlg, const Box &box);
// };
//
// Box::Box(Point position, int length, int width) : d_position(position),
//                                                   d_length(length),
//                                                   d_width(width) { }
//
// ```
// Then, we define `operator==`.  This time all of the data members contribute
// to equality.
// ```
// bool operator==(const Box &left, const Box &right)
// {
//     return (left.d_position == right.d_position) &&
//            (left.d_length   == right.d_length) &&
//            (left.d_width    == right.d_width);
// }
//
// ```
// Next, we define `hashAppend` for `Box`.  Notice how as well as calling
// `hashAppend` on fundamental types, we can also call it on our user-defined
// type `Point`.  Calling `hashAppend` on `Point` will propagate the hashing
// algorithm functor `hashAlg` down to the fundamental types that make up
// `Point`, and those types will then be passed into the algorithm functor.
// ```
// template <class HASH_ALGORITHM>
// void hashAppend(HASH_ALGORITHM &hashAlg, const Box &box)
// {
//     hashAppend(hashAlg, box.d_position);
//     hashAppend(hashAlg, box.d_length);
//     hashAppend(hashAlg, box.d_width);
// }
// ```
// Then, we want to use our cross reference on a `Box`.  We create an array of
// unique `Box`s and take its length:
// ```
//     Box boxes[] = { Box(Point(0, 0), 2, 3),
//                     Box(Point(1, 0), 1, 1),
//                     Box(Point(0, 1), 1, 5),
//                     Box(Point(1, 1), 5, 6),
//                     Box(Point(2, 1), 1, 13),
//                     Box(Point(0, 4), 3, 3),
//                     Box(Point(3, 2), 2, 17) };
//     enum { NUM_BOXES = sizeof boxes / sizeof *boxes };
// ```
// Next, we create our cross-reference `hcrsts` and verify that it constructed
// properly.  Note we don't pass a second parameter template argument and let
// `HASHER` default to `bsl::hash<TYPE>`.  Since we have not specialized
// `bsl::hash` for `Box`, `bsl::hash<TYPE>` will attempt to use `bslh::hash<>`
// to hash `Box`.
// ```
//     HashCrossReference<Box> hcrsts(boxes, NUM_BOXES);
//     ASSERT(hcrsts.isValid());
// ```
// Now, we verify that each element in our array registers with count:
// ```
//     for(int i = 0; i < NUM_BOXES; ++i) {
//         ASSERT(1 == hcrsts.count(boxes[i]));
//     }
// ```
// Finally, we verify that elements not in our original array are correctly
// identified as not being in the set:
// ```
//     ASSERT(0 == hcrsts.count(Box(Point(3, 3), 3, 3)));
//     ASSERT(0 == hcrsts.count(Box(Point(3, 2), 1, 0)));
//     ASSERT(0 == hcrsts.count(Box(Point(1, 2), 3, 4)));
//     ASSERT(0 == hcrsts.count(Box(Point(33, 23), 13, 3)));
//     ASSERT(0 == hcrsts.count(Box(Point(30, 37), 34, 13)));
// ```

#include <bslscm_version.h>

#include <bslh_hash.h>

#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>
#include <bslmf_assert.h>

#include <bsls_compilerfeatures.h>
#include <bsls_deprecatefeature.h>
#include <bsls_platform.h>

#include <cstddef>  // for 'std::size_t'

#define BSLSTL_HASH_DEPRECATED_CPP17                                          \
    BSLS_DEPRECATE_FEATURE(                                                   \
        "bsl", "deprecated_cpp17_standard_library_features", "do not use")

namespace bsl {

                          // ==================
                          // class bslstl::hash
                          // ==================

/// Empty base class for hashing.  This class, and all explicit and partial
/// specializations of this class, shall conform to the C++11 Hash
/// Requirements (C++11 17.6.3.4, [hash.requirements]).  Unless this
/// template is explicitly specialized, it will use the default hash
/// algorithm provided by `bslh::Hash<>` to supply hash values.  In order to
/// hash a user-defined type using `bsl::hash`, `bsl::hash` must be
/// explicitly specialized for the type, or, preferably, `hashAppend` must
/// be implemented for the type.  For more details on `hashAppend` and
/// `bslh::Hash` see the component `bslh_hash`.
template <class TYPE>
struct hash : ::BloombergLP::bslh::Hash<> {

    // PUBLIC ACCESSORS

    /// Compute and return the hash of the specified `value`.  This
    /// implementation forwards to the call operator of the base class, but
    /// with the parameter guaranteed to be of type `TYPE`.
    std::size_t operator()(const TYPE &value) const;
};

// ============================================================================
//                  SPECIALIZATIONS FOR FUNDAMENTAL TYPES
// ============================================================================

/// This class provides hashing functionality for constant key types, by
/// delegating to the same function for non-constant key types.
template <class BSLSTL_KEY>
struct hash<const BSLSTL_KEY> : hash<BSLSTL_KEY> {
};

/// Specialization of `hash` for `bool` values.
template <>
struct hash<bool> {

    // STANDARD TYPEDEFS

    /// @DEPRECATED: This typedef is depreacted in C++17, for details see
    /// https://isocpp.org/files/papers/p0005r4.html.
    BSLSTL_HASH_DEPRECATED_CPP17
    typedef bool argument_type;

    /// @DEPRECATED: This typedef is depreacted in C++17, for details see
    /// https://isocpp.org/files/papers/p0005r4.html.
    BSLSTL_HASH_DEPRECATED_CPP17
    typedef std::size_t result_type;

    /// Create a `hash` object.
    //! hash() = default;

    /// Create a `hash` object.  Note that as `hash` is an empty (stateless)
    /// type, this operation has no observable effect.
    //! hash(const hash& original) = default;

    /// Destroy this object.
    //! ~hash() = default;

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.  Note
    /// that as `hash` is an empty (stateless) type, this operation has no
    /// observable effect.
    //! hash& operator=(const hash& rhs) = default;

    // ACCESSORS

    /// Return a hash value computed using the specified `x`.
    std::size_t operator()(bool x) const;
};

/// Specialization of `hash` for `char` values.
template <>
struct hash<char> {

    // STANDARD TYPEDEFS

    /// @DEPRECATED: This typedef is depreacted in C++17, for details see
    /// https://isocpp.org/files/papers/p0005r4.html.
    BSLSTL_HASH_DEPRECATED_CPP17
    typedef char argument_type;

    /// @DEPRECATED: This typedef is depreacted in C++17, for details see
    /// https://isocpp.org/files/papers/p0005r4.html.
    BSLSTL_HASH_DEPRECATED_CPP17
    typedef std::size_t result_type;

    /// Create a `hash` object.
    //! hash() = default;

    /// Create a `hash` object.  Note that as `hash` is an empty (stateless)
    /// type, this operation has no observable effect.
    //! hash(const hash& original) = default;

    /// Destroy this object.
    //! ~hash() = default;

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.  Note
    /// that as `hash` is an empty (stateless) type, this operation has no
    /// observable effect.
    //! hash& operator=(const hash& rhs) = default;

    // ACCESSORS

    /// Return a hash value computed using the specified `x`.
    std::size_t operator()(char x) const;
};

/// Specialization of `hash` for `signed` `char` values.
template <>
struct hash<signed char> {

    // STANDARD TYPEDEFS

    /// @DEPRECATED: This typedef is depreacted in C++17, for details see
    /// https://isocpp.org/files/papers/p0005r4.html.
    BSLSTL_HASH_DEPRECATED_CPP17
    typedef signed char argument_type;

    /// @DEPRECATED: This typedef is depreacted in C++17, for details see
    /// https://isocpp.org/files/papers/p0005r4.html.
    BSLSTL_HASH_DEPRECATED_CPP17
    typedef std::size_t result_type;

    /// Create a `hash` object.
    //! hash() = default;

    /// Create a `hash` object.  Note that as `hash` is an empty (stateless)
    /// type, this operation has no observable effect.
    //! hash(const hash& original) = default;

    /// Destroy this object.
    //! ~hash() = default;

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.  Note
    /// that as `hash` is an empty (stateless) type, this operation has no
    /// observable effect.
    //! hash& operator=(const hash& rhs) = default;

    // ACCESSORS

    /// Return a hash value computed using the specified `x`.
    std::size_t operator()(signed char x) const;
};

/// Specialization of `hash` for `unsigned` `char` values.
template <>
struct hash<unsigned char> {

    // STANDARD TYPEDEFS

    /// @DEPRECATED: This typedef is depreacted in C++17, for details see
    /// https://isocpp.org/files/papers/p0005r4.html.
    BSLSTL_HASH_DEPRECATED_CPP17
    typedef unsigned char argument_type;

    /// @DEPRECATED: This typedef is depreacted in C++17, for details see
    /// https://isocpp.org/files/papers/p0005r4.html.
    BSLSTL_HASH_DEPRECATED_CPP17
    typedef std::size_t result_type;

    /// Create a `hash` object.
    //! hash() = default;

    /// Create a `hash` object.  Note that as `hash` is an empty (stateless)
    /// type, this operation has no observable effect.
    //! hash(const hash& original) = default;

    /// Destroy this object.
    //! ~hash() = default;

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.  Note
    /// that as `hash` is an empty (stateless) type, this operation has no
    /// observable effect.
    //! hash& operator=(const hash& rhs) = default;

    // ACCESSORS

    /// Return a hash value computed using the specified `x`.
    std::size_t operator()(unsigned char x) const;
};

/// Specialization of `hash` for `wchar_t` values.
template <>
struct hash<wchar_t> {

    // STANDARD TYPEDEFS

    /// @DEPRECATED: This typedef is depreacted in C++17, for details see
    /// https://isocpp.org/files/papers/p0005r4.html.
    BSLSTL_HASH_DEPRECATED_CPP17
    typedef wchar_t argument_type;

    /// @DEPRECATED: This typedef is depreacted in C++17, for details see
    /// https://isocpp.org/files/papers/p0005r4.html.
    BSLSTL_HASH_DEPRECATED_CPP17
    typedef std::size_t result_type;

    /// Create a `hash` object.
    //! hash() = default;

    /// Create a `hash` object.  Note that as `hash` is an empty (stateless)
    /// type, this operation has no observable effect.
    //! hash(const hash& original) = default;

    /// Destroy this object.
    //! ~hash() = default;

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.  Note
    /// that as `hash` is an empty (stateless) type, this operation has no
    /// observable effect.
    //! hash& operator=(const hash& rhs) = default;

    // ACCESSORS

    /// Return a hash value computed using the specified `x`.
    std::size_t operator()(wchar_t x) const;
};

/// Specialization of `hash` for `short` values.
template <>
struct hash<short> {

    // STANDARD TYPEDEFS

    /// @DEPRECATED: This typedef is depreacted in C++17, for details see
    /// https://isocpp.org/files/papers/p0005r4.html.
    BSLSTL_HASH_DEPRECATED_CPP17
    typedef short argument_type;

    /// @DEPRECATED: This typedef is depreacted in C++17, for details see
    /// https://isocpp.org/files/papers/p0005r4.html.
    BSLSTL_HASH_DEPRECATED_CPP17
    typedef std::size_t result_type;

    /// Create a `hash` object.
    //! hash() = default;

    /// Create a `hash` object.  Note that as `hash` is an empty (stateless)
    /// type, this operation has no observable effect.
    //! hash(const hash& original) = default;

    /// Destroy this object.
    //! ~hash() = default;

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.  Note
    /// that as `hash` is an empty (stateless) type, this operation has no
    /// observable effect.
    //! hash& operator=(const hash& rhs) = default;

    // ACCESSORS

    /// Return a hash value computed using the specified `x`.
    std::size_t operator()(short x) const;
};

/// Specialization of `hash` for `unsigned` `short` values.
template <>
struct hash<unsigned short> {

    // STANDARD TYPEDEFS

    /// @DEPRECATED: This typedef is depreacted in C++17, for details see
    /// https://isocpp.org/files/papers/p0005r4.html.
    BSLSTL_HASH_DEPRECATED_CPP17
    typedef unsigned short argument_type;

    /// @DEPRECATED: This typedef is depreacted in C++17, for details see
    /// https://isocpp.org/files/papers/p0005r4.html.
    BSLSTL_HASH_DEPRECATED_CPP17
    typedef std::size_t result_type;

    /// Create a `hash` object.
    //! hash() = default;

    /// Create a `hash` object.  Note that as `hash` is an empty (stateless)
    /// type, this operation has no observable effect.
    //! hash(const hash& original) = default;

    /// Destroy this object.
    //! ~hash() = default;

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.  Note
    /// that as `hash` is an empty (stateless) type, this operation has no
    /// observable effect.
    //! hash& operator=(const hash& rhs) = default;

    // ACCESSORS

    /// Return a hash value computed using the specified `x`.
    std::size_t operator()(unsigned short x) const;
};

/// Specialization of `hash` for `int` values.
template <>
struct hash<int> {

    // STANDARD TYPEDEFS

    /// @DEPRECATED: This typedef is depreacted in C++17, for details see
    /// https://isocpp.org/files/papers/p0005r4.html.
    BSLSTL_HASH_DEPRECATED_CPP17
    typedef int argument_type;

    /// @DEPRECATED: This typedef is depreacted in C++17, for details see
    /// https://isocpp.org/files/papers/p0005r4.html.
    BSLSTL_HASH_DEPRECATED_CPP17
    typedef std::size_t result_type;

    /// Create a `hash` object.
    //! hash() = default;

    /// Create a `hash` object.  Note that as `hash` is an empty (stateless)
    /// type, this operation has no observable effect.
    //! hash(const hash& original) = default;

    /// Destroy this object.
    //! ~hash() = default;

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.  Note
    /// that as `hash` is an empty (stateless) type, this operation has no
    /// observable effect.
    //! hash& operator=(const hash& rhs) = default;

    // ACCESSORS

    /// Return a hash value computed using the specified `x`.
    std::size_t operator()(int x) const;
};

/// Specialization of `hash` for `unsigned` `int` values.
template <>
struct hash<unsigned int> {

    // STANDARD TYPEDEFS

    /// @DEPRECATED: This typedef is depreacted in C++17, for details see
    /// https://isocpp.org/files/papers/p0005r4.html.
    BSLSTL_HASH_DEPRECATED_CPP17
    typedef unsigned int argument_type;

    /// @DEPRECATED: This typedef is depreacted in C++17, for details see
    /// https://isocpp.org/files/papers/p0005r4.html.
    BSLSTL_HASH_DEPRECATED_CPP17
    typedef std::size_t result_type;

    /// Create a `hash` object.
    //! hash() = default;

    /// Create a `hash` object.  Note that as `hash` is an empty (stateless)
    /// type, this operation has no observable effect.
    //! hash(const hash& original) = default;

    /// Destroy this object.
    //! ~hash() = default;

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.  Note
    /// that as `hash` is an empty (stateless) type, this operation has no
    /// observable effect.
    //! hash& operator=(const hash& rhs) = default;

    // ACCESSORS

    /// Return a hash value computed using the specified `x`.
    std::size_t operator()(unsigned int x) const;
};

/// Specialization of `hash` for `long` values.
template <>
struct hash<long> {

    // STANDARD TYPEDEFS

    /// @DEPRECATED: This typedef is depreacted in C++17, for details see
    /// https://isocpp.org/files/papers/p0005r4.html.
    BSLSTL_HASH_DEPRECATED_CPP17
    typedef long argument_type;

    /// @DEPRECATED: This typedef is depreacted in C++17, for details see
    /// https://isocpp.org/files/papers/p0005r4.html.
    BSLSTL_HASH_DEPRECATED_CPP17
    typedef std::size_t result_type;

    /// Create a `hash` object.
    //! hash() = default;

    /// Create a `hash` object.  Note that as `hash` is an empty (stateless)
    /// type, this operation has no observable effect.
    //! hash(const hash& original) = default;

    /// Destroy this object.
    //! ~hash() = default;

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.  Note
    /// that as `hash` is an empty (stateless) type, this operation has no
    /// observable effect.
    //! hash& operator=(const hash& rhs) = default;

    // ACCESSORS

    /// Return a hash value computed using the specified `x`.
    std::size_t operator()(long x) const;
};

/// Specialization of `hash` for `unsigned` `long` values.
template <>
struct hash<unsigned long> {

    // STANDARD TYPEDEFS

    /// @DEPRECATED: This typedef is depreacted in C++17, for details see
    /// https://isocpp.org/files/papers/p0005r4.html.
    BSLSTL_HASH_DEPRECATED_CPP17
    typedef unsigned long argument_type;

    /// @DEPRECATED: This typedef is depreacted in C++17, for details see
    /// https://isocpp.org/files/papers/p0005r4.html.
    BSLSTL_HASH_DEPRECATED_CPP17
    typedef std::size_t result_type;

    /// Create a `hash` object.
    //! hash() = default;

    /// Create a `hash` object.  Note that as `hash` is an empty (stateless)
    /// type, this operation has no observable effect.
    //! hash(const hash& original) = default;

    /// Destroy this object.
    //! ~hash() = default;

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.  Note
    /// that as `hash` is an empty (stateless) type, this operation has no
    /// observable effect.
    //! hash& operator=(const hash& rhs) = default;

    // ACCESSORS

    /// Return a hash value computed using the specified `x`.
    std::size_t operator()(unsigned long x) const;
};

/// Specialization of `hash` for `long long` values.
template <>
struct hash<long long> {

    // STANDARD TYPEDEFS

    /// @DEPRECATED: This typedef is depreacted in C++17, for details see
    /// https://isocpp.org/files/papers/p0005r4.html.
    BSLSTL_HASH_DEPRECATED_CPP17
    typedef long long argument_type;

    /// @DEPRECATED: This typedef is depreacted in C++17, for details see
    /// https://isocpp.org/files/papers/p0005r4.html.
    BSLSTL_HASH_DEPRECATED_CPP17
    typedef std::size_t result_type;

    /// Create a `hash` object.
    //! hash() = default;

    /// Create a `hash` object.  Note that as `hash` is an empty (stateless)
    /// type, this operation has no observable effect.
    //! hash(const hash& original) = default;

    /// Destroy this object.
    //! ~hash() = default;

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.  Note
    /// that as `hash` is an empty (stateless) type, this operation has no
    /// observable effect.
    //! hash& operator=(const hash& rhs) = default;

    // ACCESSORS

    /// Return a hash value computed using the specified `x`.
    std::size_t operator()(long long x) const;
};

/// Specialization of `hash` for `unsigned` `long long` values.
template <>
struct hash<unsigned long long> {

    // STANDARD TYPEDEFS

    /// @DEPRECATED: This typedef is depreacted in C++17, for details see
    /// https://isocpp.org/files/papers/p0005r4.html.
    BSLSTL_HASH_DEPRECATED_CPP17
    typedef unsigned long long argument_type;

    /// @DEPRECATED: This typedef is depreacted in C++17, for details see
    /// https://isocpp.org/files/papers/p0005r4.html.
    BSLSTL_HASH_DEPRECATED_CPP17
    typedef std::size_t result_type;

    /// Create a `hash` object.
    //! hash() = default;

    /// Create a `hash` object.  Note that as `hash` is an empty (stateless)
    /// type, this operation has no observable effect.
    //! hash(const hash& original) = default;

    /// Destroy this object.
    //! ~hash() = default;

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.  Note
    /// that as `hash` is an empty (stateless) type, this operation has no
    /// observable effect.
    //! hash& operator=(const hash& rhs) = default;

    // ACCESSORS

    /// Return a hash value computed using the specified `x`.
    std::size_t operator()(unsigned long long x) const;
};

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

template <class TYPE>
inline
std::size_t hash<TYPE>::operator()(const TYPE& value) const
{
    return ::BloombergLP::bslh::Hash<>::operator()(value);
}

inline
std::size_t hash<bool>::operator()(bool x) const
{
    return x;
}

inline
std::size_t hash<char>::operator()(char x) const
{
    return x;
}

inline
std::size_t hash<signed char>::operator()(signed char x) const
{
    return x;
}

inline
std::size_t hash<unsigned char>::operator()(unsigned char x) const
{
    return x;
}

inline
std::size_t hash<wchar_t>::operator()(wchar_t x) const
{
    return x;
}

inline
std::size_t hash<short>::operator()(short x) const
{
    return x;
}

inline
std::size_t hash<unsigned short>::operator()(unsigned short x) const
{
    return x;
}

inline
std::size_t hash<int>::operator()(int x) const
{
    return x;
}

inline
std::size_t hash<unsigned int>::operator()(unsigned int x) const
{
    return x;
}

inline
std::size_t hash<long>::operator()(long x) const
{
    return x;
}

inline
std::size_t hash<unsigned long>::operator()(unsigned long x) const
{
    return x;
}


#ifdef BSLS_PLATFORM_CPU_64_BIT
inline
std::size_t hash<long long>::operator()(long long x) const
{
    BSLMF_ASSERT(sizeof (long long) == sizeof (std::size_t));
    return x;
}

inline
std::size_t hash<unsigned long long>::operator()(unsigned long long x) const
{
    BSLMF_ASSERT(sizeof (long long) == sizeof (std::size_t));
    return x;
}

#else // BSLS_PLATFORM_CPU_32_BIT

inline
std::size_t hash<long long>::operator()(long long x) const
{
    BSLMF_ASSERT(sizeof (long long) > sizeof (std::size_t));

    // The mangling algorithm won't work unless these conditions hold:

    BSLMF_ASSERT(sizeof (std::size_t) * 8 == 32);
    BSLMF_ASSERT(sizeof (long long) * 8 == 64);

    // Return a simple mangling of the 64-bits of 'x' to generate a 32-bit hash
    // value (xor the high and low 32 bits together).

    return static_cast<std::size_t>((x ^ (x >> 32)) & 0xFFFFFFFF);
}

inline
std::size_t hash<unsigned long long>::operator()(unsigned long long x) const
{
    BSLMF_ASSERT(sizeof (long long) > sizeof (std::size_t));

    // The mangling algorithm won't work unless these conditions hold:

    BSLMF_ASSERT(sizeof (std::size_t) * 8 == 32);
    BSLMF_ASSERT(sizeof (unsigned long long) * 8 == 64);

    // Return a simple mangling of the 64-bits of 'x' to generate a 32-bit hash
    // value (xor the high and low 32 bits together).

    return static_cast<std::size_t>((x ^ (x >> 32)) & 0xFFFFFFFF);
}
#endif

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

// Type traits for STL 'hash'
//: o 'bsl::hash<TYPE>' is trivially default constructible.
//: o 'bsl::hash<TYPE>' is trivially copyable.
//: o 'bsl::hash<TYPE>' is bitwise movable.

template <class TYPE>
struct is_trivially_default_constructible<hash<TYPE> >
: bsl::true_type
{};

template <class TYPE>
struct is_trivially_copyable<hash<TYPE> >
: bsl::true_type
{};

}  // close namespace bsl

#undef BSLSTL_HASH_DEPRECATED_CPP17

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
