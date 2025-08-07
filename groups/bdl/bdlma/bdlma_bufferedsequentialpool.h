// bdlma_bufferedsequentialpool.h                                     -*-C++-*-
#ifndef INCLUDED_BDLMA_BUFFEREDSEQUENTIALPOOL
#define INCLUDED_BDLMA_BUFFEREDSEQUENTIALPOOL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide sequential memory using an external buffer and a fallback.
//
//@CLASSES:
//  bdlma::BufferedSequentialPool: pool using an external buffer and a fallback
//
//@SEE_ALSO: bdlma_buffermanager, bdlma_sequentialpool
//
//@DESCRIPTION: This component provides a maximally efficient sequential memory
// pool, `bdlma::BufferedSequentialPool`, that dispenses heterogeneous memory
// blocks (of varying, user-specified sizes) from an external buffer.  If an
// allocation request exceeds the remaining free memory space in the external
// buffer, the pool will fall back to a sequence of dynamically allocated
// buffers.  Users can optionally specify a growth strategy at construction
// that governs the growth rate of the dynamically-allocated buffers.  If no
// growth strategy is specified at construction, geometric growth is used.
// Users can also optionally specify an alignment strategy at construction that
// governs the alignment of allocated memory blocks.  If no alignment strategy
// is specified, natural alignment is used.  The `release` method releases all
// memory allocated through the pool, as does the destructor.  The `rewind`
// method releases all memory allocated through the pool and returns to the
// underlying allocator *only* memory that was allocated outside of the typical
// internal buffer growth of the pool (i.e., large blocks).  Note that
// individually allocated memory blocks cannot be separately deallocated.
//
// A `bdlma::BufferedSequentialPool` is typically used when users have a
// reasonable estimation of the amount of memory needed.  This amount of memory
// would typically be created directly on the program stack, and used as the
// initial external buffer of the pool for fast memory allocation.  While the
// buffer has sufficient capacity, memory allocations using the pool will not
// trigger *any* dynamic memory allocation, will have optimal locality of
// reference, and will not require deallocation upon destruction.
//
// Once the external buffer is exhausted, subsequent allocation requests
// require dynamic memory allocation, and the performance of the pool degrades.
//
///Optional `maxBufferSize` Parameter
/// - - - - - - - - - - - - - - - - -
// An optional `maxBufferSize` parameter can be supplied at construction to
// specify the maximum size (in bytes) of the dynamically-allocated buffers for
// geometric growth.  Once the internal buffer grows up to the `maxBufferSize`,
// further requests that exceed this size will be served by a separate memory
// block instead of the internal buffer.  The behavior is undefined unless
// `size <= maxBufferSize`, where `size` is the extent (in bytes) of the
// external buffer supplied at construction.
//
///Warning
///-------
// Note that, even when a buffer having `n` bytes of memory is supplied at
// construction, it does *not* mean that `n` bytes of memory are available
// before dynamic memory allocation is triggered.  This is due to memory
// alignment requirements.  If the buffer supplied is not aligned, the first
// call to the `allocate` method may automatically skip one or more bytes such
// that the memory allocated is properly aligned.  The number of bytes that are
// wasted depends on whether natural alignment, maximum alignment, or 1-byte
// alignment is used (see `bsls_alignment` for more details).
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using `bdlma::BufferedSequentialPool` for Efficient Allocations
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we define a container class, `my_BufferedIntDoubleArray`, that holds
// both `int` and `double` values.  The class can be implemented using two
// parallel arrays: one storing the type information, and the other storing
// pointers to the `int` and `double` values.  Furthermore, if we can
// approximate the amount of memory needed, we can use a
// `bdlma::BufferedSequentialPool` for memory allocation for maximum
// efficiency:
// ```
// // my_bufferedintdoublearray.h
//
// /// This class implements an efficient container for an array that
// /// stores both `int` and `double` values.
// class my_BufferedIntDoubleArray {
//
//     // DATA
//     char  *d_typeArray_p;   // array indicating the type of corresponding
//                             // values stored in 'd_valueArray_p'
//
//     void **d_valueArray_p;  // array of pointers to the values stored
//
//     int    d_length;        // number of values stored
//
//     int    d_capacity;      // physical capacity of the type and value
//                             // arrays
//
//     bdlma::BufferedSequentialPool
//            d_pool;          // buffered sequential memory pool used to
//                             // supply memory
//
//   private:
//     // NOT IMPLEMENTED
//     my_BufferedIntDoubleArray(const my_BufferedIntDoubleArray&);
//
//   private:
//     // PRIVATE MANIPULATORS
//
//     /// Increase the capacity of the internal arrays used to store
//     /// elements added to this array by at least one element.
//     void increaseCapacity();
//
//   public:
//     // TYPES
//     enum Type { k_MY_INT, k_MY_DOUBLE };
//
//     // CREATORS
//
//     /// Create a fast `int`-`double` array that initially allocates
//     /// memory sequentially from the specified `buffer` having the
//     /// specified `size` (in bytes).  Optionally specify a
//     /// `basicAllocator` used to supply memory if `buffer` capacity is
//     /// exceeded.  If `basicAllocator` is 0, the currently installed
//     /// default allocator is used.
//     my_BufferedIntDoubleArray(char             *buffer,
//                               int               size,
//                               bslma::Allocator *basicAllocator = 0);
//
//     /// Destroy this array and all elements held by it.
//     ~my_BufferedIntDoubleArray();
//
//     // ...
//
//     // MANIPULATORS
//
//     /// Append the specified `int` `value` to this array.
//     void appendInt(int value);
//
//     /// Append the specified `double` `value` to this array.
//     void appendDouble(double value);
//
//     /// Remove all elements from this array.
//     void removeAll();
//
//     // ...
// };
// ```
// The use of a buffered sequential pool and the `release` method allows the
// `removeAll` method to quickly deallocate memory of all elements:
// ```
// // MANIPULATORS
// inline
// void my_BufferedIntDoubleArray::removeAll()
// {
//     d_pool.release();  // *very* efficient if 'd_pool' has not exhausted
//                        // the buffer supplied at construction
//
//     d_length = 0;
// }
// ```
// The buffered sequential pool optimizes the allocation of memory by using a
// buffer supplied at construction.  As described in the "DESCRIPTION" section,
// the need for *all* dynamic memory allocations are eliminated provided that
// the buffer is not exhausted.  The pool provides maximal memory allocation
// efficiency:
// ```
// // my_bufferedintdoublearray.cpp
//
// enum { k_INITIAL_SIZE = 1 };
//
// // PRIVATE MANIPULATORS
// void my_BufferedIntDoubleArray::increaseCapacity()
// {
//     // Implementation elided.
//     // ...
// }
//
// // CREATORS
// my_BufferedIntDoubleArray::my_BufferedIntDoubleArray(
//                                           char             *buffer,
//                                           int               size,
//                                           bslma::Allocator *basicAllocator)
// : d_length(0)
// , d_capacity(k_INITIAL_SIZE)
// , d_pool(buffer, size, basicAllocator)
// {
//     d_typeArray_p  = static_cast<char *>(
//                      d_pool.allocate(d_capacity * sizeof *d_typeArray_p));
//     d_valueArray_p = static_cast<void **>(
//                      d_pool.allocate(d_capacity * sizeof *d_valueArray_p));
// }
// ```
// Note that in the destructor, all outstanding memory blocks are deallocated
// automatically when `d_pool` is destroyed:
// ```
// my_BufferedIntDoubleArray::~my_BufferedIntDoubleArray()
// {
//     assert(0 <= d_length);
//     assert(0 <= d_capacity);
//     assert(d_length <= d_capacity);
// }
//
// // MANIPULATORS
// void my_BufferedIntDoubleArray::appendDouble(double value)
// {
//     if (d_length >= d_capacity) {
//         increaseCapacity();
//     }
//
//     double *item = static_cast<double *>(d_pool.allocate(sizeof *item));
//     *item = value;
//
//     d_typeArray_p[d_length]  = static_cast<char>(k_MY_DOUBLE);
//     d_valueArray_p[d_length] = item;
//
//     ++d_length;
// }
//
// void my_BufferedIntDoubleArray::appendInt(int value)
// {
//     if (d_length >= d_capacity) {
//         increaseCapacity();
//     }
//
//     int *item = static_cast<int *>(d_pool.allocate(sizeof *item));
//     *item = value;
//
//     d_typeArray_p[d_length]  = static_cast<char>(k_MY_INT);
//     d_valueArray_p[d_length] = item;
//
//     ++d_length;
// }
// ```
//
///Example 2: Implementing an Allocator Using `bdlma::BufferedSequentialPool`
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// `bslma::Allocator` is used throughout the interfaces of BDE components.
// Suppose we would like to create a fast allocator, `my_FastAllocator`, that
// allocates memory from a buffer in a similar fashion to
// `bdlma::BufferedSequentialPool`.  `bdlma::BufferedSequentialPool` can be
// used directly to implement such an allocator.
//
// Note that the documentation for this class is simplified for this usage
// example.  Please see `bdlma_bufferedsequentialallocator` for full
// documentation of a similar class.
// ```
// /// This class implements the `bslma::Allocator` protocol to provide a
// /// fast allocator of heterogeneous blocks of memory (of varying,
// /// user-specified sizes) from an external buffer whose address and size
// /// are supplied at construction.
// class my_FastAllocator : public bslma::Allocator {
//
//     // DATA
//     bdlma::BufferedSequentialPool d_pool;  // memory manager for allocated
//                                            // memory blocks
//
//     // CREATORS
//
//     /// Create an allocator for allocating memory blocks from the
//     /// specified external `buffer` of the specified `size` (in bytes).
//     /// Optionally specify a `basicAllocator` used to supply memory
//     /// should the capacity of `buffer` be exhausted.  If
//     /// `basicAllocator` is 0, the currently installed default allocator
//     /// is used.
//     my_FastAllocator(char             *buffer,
//                      int               size,
//                      bslma::Allocator *basicAllocator = 0);
//
//      /// Destroy this allocator.  All memory allocated from this
//      /// allocator is released.
//     ~my_FastAllocator();
//
//     // MANIPULATORS
//
//     /// Return the address of a contiguous block of memory of the
//     /// specified `size` (in bytes).
//     virtual void *allocate(size_type size);
//
//     /// This method has no effect on the memory block at the specified
//     /// `address` as all memory allocated by this allocator is managed.
//     /// The behavior is undefined unless `address` was allocated by this
//     /// allocator, and has not already been deallocated.
//     virtual void deallocate(void *address);
// };
//
// // CREATORS
// inline
// my_FastAllocator::my_FastAllocator(char             *buffer,
//                                    int               size,
//                                    bslma::Allocator *basicAllocator)
// : d_pool(buffer, size, basicAllocator)
// {
// }
//
// inline
// my_FastAllocator::~my_FastAllocator()
// {
//     d_pool.release();
// }
//
// // MANIPULATORS
// inline
// void *my_FastAllocator::allocate(size_type size)
// {
//     return d_pool.allocate(size);
// }
//
// inline
// void my_FastAllocator::deallocate(void *)
// {
// }
// ```

#include <bdlscm_version.h>

#include <bdlma_buffermanager.h>
#include <bdlma_sequentialpool.h>

#include <bslma_allocator.h>

#include <bsls_alignment.h>
#include <bsls_assert.h>
#include <bsls_blockgrowth.h>
#include <bsls_platform.h>
#include <bsls_performancehint.h>
#include <bsls_types.h>

#include <bsl_cstddef.h>

namespace BloombergLP {
namespace bdlma {

                       // ============================
                       // class BufferedSequentialPool
                       // ============================

/// This class implements a fast memory pool that efficiently dispenses
/// heterogeneous blocks of memory (of varying, user-specified sizes) from
/// an external buffer whose address and size (in bytes) are supplied at
/// construction.  If an allocation request exceeds the remaining free
/// memory space in the external buffer, memory will be supplied by an
/// (optional) allocator supplied also at construction; if no allocator is
/// supplied, the currently installed default allocator will be used.  This
/// class is *exception* *neutral*: If memory cannot be allocated, the
/// behavior is defined by the (optional) allocator supplied at
/// construction.  Note that in no case will the buffered sequential pool
/// attempt to deallocate the external buffer.
class BufferedSequentialPool {

    // DATA
    BufferManager           d_bufferManager;    // memory manager for current
                                                // buffer

    bsls::Types::size_type  d_maxBufferSize;    // max buffer size parameter to
                                                // be passed to the sequential
                                                // allocator upon construction

    unsigned char           d_growthStrategy;   // the growth strategy to be
                                                // passed to the sequential
                                                // pool

    bool                    d_sequentialPoolIsCreated;
                                                // indicates whether the
                                                // sequential pool has been
                                                // created yet

    union {
        bslma::Allocator   *d_allocator_p;      // allocator we were
                                                // constructed with if the
                                                // sequential pool hasn't been
                                                // created

        SequentialPool     *d_pool_p;           // memory manager for
    };                                          // allocations not from the
                                                // buffer, if the sequential
                                                // pool has been created

  private:
    // NOT IMPLEMENTED
    BufferedSequentialPool(const BufferedSequentialPool&);
    BufferedSequentialPool& operator=(const BufferedSequentialPool&);

  private:
    // PRIVATE MANIPULATORS

    /// Allocate and construct the sequential pool, using a block size of
    /// `bufferManager.bufferSize()`.  Use the specified
    /// `currentAllocationSize` to determine if the current allocation
    /// attempt will fit in the first block, and thus what boolean value to
    /// pass to the sequental pool's `allocateInitialBuffer` argument.
    void createSequentialPool(bsls::Types::size_type currentAllocationSize);

    // PRIVATE ACCESSORS

    /// Return the alignment strategy to be used by the sequential pool.
    bsls::Alignment::Strategy alignmentStrategy() const;

    /// Return the growth strategy to be used by the sequential pool.
    bsls::BlockGrowth::Strategy growthStrategy() const;

  public:
    // CREATORS

    /// Create a buffered sequential pool for allocating memory blocks from
    /// the specified external `buffer` having the specified `size` (in
    /// bytes), or from an internal buffer (after the external `buffer` is
    /// exhausted).  Optionally specify a `growthStrategy` used to control
    /// buffer growth.  If a `growthStrategy` is not specified, geometric
    /// growth is used.  Optionally specify an `alignmentStrategy` used to
    /// align allocated memory blocks.  If an `alignmentStrategy` is not
    /// specified, natural alignment is used.  Optionally specify a
    /// `basicAllocator` used to supply memory should the capacity of
    /// `buffer` be exhausted.  If `basicAllocator` is 0, the currently
    /// installed default allocator is used.  The behavior is undefined
    /// unless `0 < size`, and `buffer` has at least `size` bytes.  Note
    /// that, due to alignment effects, it is possible that not all `size`
    /// bytes of memory in `buffer` can be used for allocation.  Also note
    /// that no limit is imposed on the size of the internal buffers when
    /// geometric growth is used.  Also note that when constant growth is
    /// used, the size of the internal buffers will always be the same as
    /// `size`.
    BufferedSequentialPool(char                   *buffer,
                           bsls::Types::size_type  size,
                           bslma::Allocator       *basicAllocator = 0);
    BufferedSequentialPool(char                        *buffer,
                           bsls::Types::size_type       size,
                           bsls::BlockGrowth::Strategy  growthStrategy,
                           bslma::Allocator            *basicAllocator = 0);
    BufferedSequentialPool(char                      *buffer,
                           bsls::Types::size_type     size,
                           bsls::Alignment::Strategy  alignmentStrategy,
                           bslma::Allocator          *basicAllocator = 0);
    BufferedSequentialPool(char                        *buffer,
                           bsls::Types::size_type       size,
                           bsls::BlockGrowth::Strategy  growthStrategy,
                           bsls::Alignment::Strategy    alignmentStrategy,
                           bslma::Allocator            *basicAllocator = 0);

    /// Create a buffered sequential pool for allocating memory blocks from
    /// the specified external `buffer` having the specified `size` (in
    /// bytes), or from an internal buffer (after the external `buffer` is
    /// exhausted) where the buffer growth is limited to the specified
    /// `maxBufferSize` (in bytes).  Optionally specify a `growthStrategy`
    /// used to control buffer growth.  If a `growthStrategy` is not
    /// specified, geometric growth is used.  Optionally specify an
    /// `alignmentStrategy` used to align allocated memory blocks.  If an
    /// `alignmentStrategy` is not specified, natural alignment is used.
    /// Optionally specify a `basicAllocator` used to supply memory should
    /// the capacity of `buffer` be exhausted.  If `basicAllocator` is 0,
    /// the currently installed default allocator is used.  The behavior is
    /// undefined unless `0 < size`, `size <= maxBufferSize`, and `buffer`
    /// has at least `size` bytes.  Note that, due to alignment effects, it
    /// is possible that not all `size` bytes of memory in `buffer` can be
    /// used for allocation.  Also note that when constant growth is used,
    /// the size of the internal buffers will always be the same as `size`.
    BufferedSequentialPool(char                   *buffer,
                           bsls::Types::size_type  size,
                           bsls::Types::size_type  maxBufferSize,
                           bslma::Allocator       *basicAllocator = 0);
    BufferedSequentialPool(char                        *buffer,
                           bsls::Types::size_type       size,
                           bsls::Types::size_type       maxBufferSize,
                           bsls::BlockGrowth::Strategy  growthStrategy,
                           bslma::Allocator            *basicAllocator = 0);
    BufferedSequentialPool(char                      *buffer,
                           bsls::Types::size_type     size,
                           bsls::Types::size_type     maxBufferSize,
                           bsls::Alignment::Strategy  alignmentStrategy,
                           bslma::Allocator          *basicAllocator = 0);
    BufferedSequentialPool(char                        *buffer,
                           bsls::Types::size_type       size,
                           bsls::Types::size_type       maxBufferSize,
                           bsls::BlockGrowth::Strategy  growthStrategy,
                           bsls::Alignment::Strategy    alignmentStrategy,
                           bslma::Allocator            *basicAllocator = 0);

    /// Destroy this buffered sequential pool.  All memory allocated from
    /// this pool is released.
    ~BufferedSequentialPool();

    // MANIPULATORS

    /// Return the address of a contiguous block of memory of the specified
    /// `size` (in bytes) according to the alignment strategy specified at
    /// construction.  If `size` is 0, no memory is allocated and 0 is
    /// returned.  If the allocation request exceeds the remaining free
    /// memory space in the external buffer supplied at construction, use
    /// memory obtained from the allocator supplied at construction.
    void *allocate(bsls::Types::size_type size);

    /// Destroy the specified `object`.  Note that this method has the same
    /// effect as the `deleteObjectRaw` method (since no deallocation is
    /// involved), and exists for consistency across memory pools.
    template <class TYPE>
    void deleteObject(const TYPE *object);

    /// Destroy the specified `object`.  Note that memory associated with
    /// `object` is not deallocated because there is no `deallocate` method
    /// in `BufferedSequentialPool`.
    template <class TYPE>
    void deleteObjectRaw(const TYPE *object);

    /// Release all memory allocated through this pool and return to the
    /// underlying allocator *all* memory except the external buffer
    /// supplied at construction.  The pool is reset to its
    /// default-constructed state, making the memory from the entire
    /// external buffer supplied at construction available for subsequent
    /// allocations, retaining the alignment and growth strategies, and the
    /// initial and maximum buffer sizes in effect following construction.
    /// The effect of subsequently - to this invokation of `release` - using
    /// a pointer obtained from this object prior to this call to `release`
    /// is undefined.
    void release();

    /// Release all memory allocated through this pool and return to the
    /// underlying allocator *only* memory that was allocated outside of the
    /// typical internal buffer growth of this pool (i.e., large blocks).
    /// All retained memory will be used to satisfy subsequent allocations.
    /// The effect of subsequently - to this invokation of `rewind` - using
    /// a pointer obtained from this object prior to this call to `rewind`
    /// is undefined.
    void rewind();

    // ACCESSORS

    /// Return the allocator used by this object to allocate memory.  Note
    /// that this allocator can not be used to deallocate memory allocated
    /// through this pool.
    bslma::Allocator *allocator() const;
};

}  // close package namespace
}  // close enterprise namespace

// Note that the `new` and `delete` operators are declared outside the
// `BloombergLP` namespace so that they do not hide the standard placement
// `new` and `delete` operators (i.e.,
// `void *operator new(bsl::size_t, void *)` and
// `void operator delete(void *)`).
//
// Also note that only the scalar versions of operators `new` and `delete` are
// provided, because overloading `new` (and `delete`) with their array versions
// would cause dangerous ambiguity.  Consider what would have happened had we
// overloaded the array version of `operator new`:
//..
//   void *operator new[](bsl::size_t                                 size,
//                        BloombergLP::bdlma::BufferedSequentialPool& pool);
//..
// The user of the pool class would have expected to be able to use operator
// `new` as follows:
//..
//   new (*pool) my_Type[...];
//..
// The problem is that this expression returns an array that cannot be safely
// deallocated.  On the one hand, there is no syntax in C++ to invoke an
// overloaded `operator delete`; on the other hand, the pointer returned by
// `operator new` cannot be passed to the `deallocate` method directly because
// the pointer is different from the one returned by the `allocate` method.
// The compiler offsets the value of this pointer by a header, which is used to
// maintain the number of objects in the array (so that the `operator delete`
// can destroy the right number of objects).

// FREE OPERATORS

/// Return a block of memory of the specified `size` (in bytes) allocated
/// from the specified `pool`.  Note that an object may allocate additional
/// memory internally, requiring the allocator to be passed in as a
/// constructor argument:
/// ```
/// my_Type *newMyType(bdlma::BufferedSequentialPool *pool,
///                    bslma::Allocator              *basicAllocator)
/// {
///     return new (*pool) my_Type(..., basicAllocator);
/// }
/// ```
/// Also note that the analogous version of `operator delete` should not be
/// called directly.  Instead, this component provides a static template
/// member function, `deleteObject`, parameterized by `TYPE` that performs
/// the following:
/// ```
/// void deleteMyType(bdlma::BufferedSequentialPool *pool, my_Type *t)
/// {
///     t->~my_Type();
/// }
/// ```
void *operator new(bsl::size_t                                 size,
                   BloombergLP::bdlma::BufferedSequentialPool& pool);

/// Use the specified `pool` to deallocate the memory at the specified
/// `address`.  The behavior is undefined unless `address` was allocated
/// using `pool` and has not already been deallocated.  This operator is
/// supplied solely to allow the compiler to arrange for it to be called in
/// case of an exception.
void operator delete(void                                        *address,
                     BloombergLP::bdlma::BufferedSequentialPool&  pool);

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

namespace BloombergLP {
namespace bdlma {

                       // ----------------------------
                       // class BufferedSequentialPool
                       // ----------------------------

// PRIVATE ACCESSORS
inline
bsls::Alignment::Strategy BufferedSequentialPool::alignmentStrategy() const
{
    return d_bufferManager.alignmentStrategy();
}

inline
bsls::BlockGrowth::Strategy BufferedSequentialPool::growthStrategy() const
{
    return static_cast<bsls::BlockGrowth::Strategy>(d_growthStrategy);
}

// CREATORS
inline
BufferedSequentialPool::~BufferedSequentialPool()
{
    // 'd_bufferManager' doesn't need to be released, it will destroy itself
    // just fine.

    if (d_sequentialPoolIsCreated) {
        d_pool_p->allocator()->deleteObjectRaw(d_pool_p);
    }
}

// MANIPULATORS
inline
void *BufferedSequentialPool::allocate(bsls::Types::size_type size)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(0 == size)) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return 0;                                                     // RETURN
    }

    void *result = d_bufferManager.allocate(size);
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(result)) {
        return result;                                                // RETURN
    }

    if (false == d_sequentialPoolIsCreated) {
        this->createSequentialPool(size);
    }

    return d_pool_p->allocate(size);
}

template <class TYPE>
inline
void BufferedSequentialPool::deleteObject(const TYPE *object)
{
    this->deleteObjectRaw(object);
}

template <class TYPE>
inline
void BufferedSequentialPool::deleteObjectRaw(const TYPE *object)
{
    if (0 != object) {
#ifndef BSLS_PLATFORM_CMP_SUN
        object->~TYPE();
#else
        const_cast<TYPE *>(object)->~TYPE();
#endif
    }
}

inline
void BufferedSequentialPool::release()
{
    d_bufferManager.release();  // Reset the internal cursor in the current
                                // block.

    if (d_sequentialPoolIsCreated) {
        // Note that 'd_pool_p' and 'd_allocator_p' fit in the same footprint
        // in an anonymous union.

        bslma::Allocator *alloc_p = d_pool_p->allocator();
        alloc_p->deleteObjectRaw(d_pool_p);
        d_allocator_p = alloc_p;

        d_sequentialPoolIsCreated = false;
    }
}

inline
void BufferedSequentialPool::rewind()

{
    d_bufferManager.release();  // Reset the internal cursor in the current
                                // block.

    if (d_sequentialPoolIsCreated) {
        d_pool_p->rewind();
    }
}

// ACCESSORS
inline
bslma::Allocator *BufferedSequentialPool::allocator() const
{
    return d_sequentialPoolIsCreated ? d_pool_p->allocator()
                                     : d_allocator_p;
}

}  // close package namespace
}  // close enterprise namespace

// FREE OPERATORS
inline
void *operator new(bsl::size_t                                 size,
                   BloombergLP::bdlma::BufferedSequentialPool& pool)
{
    return pool.allocate(size);
}

inline
void operator delete(void *, BloombergLP::bdlma::BufferedSequentialPool&)
{
    // NOTE: there is no deallocation from this allocation mechanism.
}

#endif

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
