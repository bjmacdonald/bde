// bdlma_infrequentdeleteblocklist.h                                  -*-C++-*-
#ifndef INCLUDED_BDLMA_INFREQUENTDELETEBLOCKLIST
#define INCLUDED_BDLMA_INFREQUENTDELETEBLOCKLIST

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide allocation and management of infrequently deleted blocks.
//
//@CLASSES:
//  bdlma::InfrequentDeleteBlockList: manager of infrequently deleted blocks
//
//@SEE_ALSO: bdlma_blocklist
//
//@DESCRIPTION: This component implements a low-level memory manager,
// `bdlma::InfrequentDeleteBlockList`, that allocates and manages a sequence of
// memory blocks, each of a potentially different size as specified during the
// `allocate` method's invocation.  The `release` method of a
// `bdlma::InfrequentDeleteBlockList` object deallocates the entire sequence of
// outstanding memory blocks, as does its destructor.  Note that, in contrast
// to `bdlma::BlockList`, the `bdlma::InfrequentDeleteBlockList` class does
// *not* support the deallocation of individual items.  In particular, although
// `bdlma::InfrequentDeleteBlockList` has a `deallocate` method, that method
// has no effect.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating a Memory Pools for Strings
/// - - - - - - - - - - - - - - - - - - - - - - -
// A `bdlma::InfrequentDeleteBlockList` object is commonly used to supply
// memory to more elaborate memory managers that distribute parts of each
// (larger) allocated memory block supplied by the
// `bdlma::InfrequentDeleteBlockList` object.  The `my_StrPool` memory pool
// manager shown below requests relatively large blocks of memory from its
// `bdlma::InfrequentDeleteBlockList` member object and distributes memory
// chunks of varying sizes from each block on demand:
// ```
// // my_strpool.h
//
// class my_StrPool {
//
//     // DATA
//     char                   *d_block_p;    // current memory block
//
//     bsls::Types::size_type  d_blockSize;  // size of current memory block
//
//     bsls::Types::IntPtr     d_cursor;     // offset to next available byte
//                                           // in block
//
//     bdlma::InfrequentDeleteBlockList
//                             d_blockList;  // supplies managed memory blocks
//
//   private:
//     // PRIVATE MANIPULATORS
//
//     // Request a memory block of at least the specified `numBytes` size
//     // and allocate the initial `numBytes` from this block.  Return the
//     // address of the allocated memory.  The behavior is undefined
//     // unless `0 < numBytes`.
//     void *allocateBlock(bsls::Types::size_type numBytes);
//
//   private:
//     // NOT IMPLEMENTED
//     my_StrPool(const my_StrPool&);
//     my_StrPool& operator=(const my_StrPool&);
//
//   public:
//     // CREATORS
//
//     /// Create a string pool.  Optionally specify a `basicAllocator`
//     /// used to supply memory.  If `basicAllocator` is 0, the currently
//     /// installed default allocator is used.
//     explicit
//     my_StrPool(bslma::Allocator *basicAllocator = 0);
//
//     /// Destroy this object and release all associated memory.
//     ~my_StrPool();
//
//     // MANIPULATORS
//
//     /// Return the address of a contiguous block of memory of the
//     /// specified `size` (in bytes).  If `size` is 0, no memory is
//     /// allocated and 0 is returned.
//     void *allocate(bsls::Types::size_type size);
//
//     /// Release all memory currently allocated through this object.
//     void release();
// };
//
// // MANIPULATORS
// inline
// void my_StrPool::release()
// {
//     d_blockList.release();
//     d_block_p = 0;
// }
//
// // ...
//
// // my_strpool.cpp
//
// enum {
//     k_INITIAL_SIZE  = 128,  // initial block size
//
//     k_GROWTH_FACTOR =   2,  // multiplicative factor by which to grow block
//
//     k_THRESHOLD     = 128   // size beyond which an individual block may be
//                             // allocated if it doesn't fit in current block
// };
//
// // PRIVATE MANIPULATORS
// void *my_StrPool::allocateBlock(bsls::Types::size_type numBytes)
// {
//     assert(0 < numBytes);
//
//     if (k_THRESHOLD < numBytes) {
//         // Allocate separate block if above threshold.
//
//         return reinterpret_cast<char *>(
//                                 d_blockList.allocate(numBytes));  // RETURN
//     }
//
//     if (d_block_p) {
//         // Do not increase block size if no current block.
//
//         d_blockSize *= k_GROWTH_FACTOR;
//     }
//     d_block_p = reinterpret_cast<char*>(d_blockList.allocate(d_blockSize));
//     d_cursor  = numBytes;
//
//     return d_block_p;
// }
//
// // CREATORS
// my_StrPool::my_StrPool(bslma::Allocator *basicAllocator)
// : d_block_p(0)
// , d_blockSize(k_INITIAL_SIZE)
// , d_cursor(0)
// , d_blockList(basicAllocator)  // the blocklist knows about 'bslma_default'
// {
// }
//
// my_StrPool::~my_StrPool()
// {
//     assert(k_INITIAL_SIZE <= d_blockSize);
//     assert(!d_block_p || (0 <= d_cursor && d_cursor <=
//                            static_cast<bsls::Types::IntPtr>(d_blockSize)));
// }
//
// // MANIPULATORS
// void *my_StrPool::allocate(bsls::Types::size_type size)
// {
//     if (0 == size) {
//         return 0;                                                 // RETURN
//     }
//
//     if (d_block_p && size + d_cursor <= d_blockSize) {
//         char *p = d_block_p + d_cursor;
//         d_cursor += size;
//         return p;                                                 // RETURN
//     }
//     else {
//         return allocateBlock(size);                               // RETURN
//     }
// }
// ```
// In the code shown above, the `my_StrPool` memory manager allocates from its
// `bdlma::InfrequentDeleteBlockList` member object an initial memory block of
// size `k_INITIAL_SIZE`.  This size is multiplied by `k_GROWTH_FACTOR` each
// time a depleted memory block is replaced by a newly-allocated block.  The
// `allocate` method distributes memory from the current memory block
// piecemeal, except when the requested size (1) is not available in the
// current block, or (2) exceeds the `k_THRESHOLD`, in which case a separate
// memory block is allocated and returned.  When the `my_StrPool` memory
// manager is destroyed, its `bdlma::InfrequentDeleteBlockList` member object
// is also destroyed, which in turn automatically deallocates all of its
// managed memory blocks.

#include <bdlscm_version.h>

#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bsls_alignmentutil.h>
#include <bsls_types.h>

namespace BloombergLP {
namespace bdlma {

                     // ===============================
                     // class InfrequentDeleteBlockList
                     // ===============================

/// This class implements a low-level memory manager that allocates and
/// manages a sequence of memory blocks -- each potentially of a different
/// size as specified during the invocation of the `allocate` method.  The
/// `release` method deallocates the entire sequence of memory blocks, as
/// does the destructor.  Note that memory blocks cannot be deallocated
/// individually.
class InfrequentDeleteBlockList {

    // PRIVATE TYPES

    /// This `struct` overlays the beginning of each managed block of
    /// allocated memory, implementing a singly-linked list of managed
    /// blocks, and thereby enabling constant-time additions to the list of
    /// blocks.
    struct Block {

        Block                               *d_next_p;  // next pointer
        bsls::AlignmentUtil::MaxAlignedType  d_memory;  // force alignment
    };

    // DATA
    Block            *d_head_p;       // address of 1st block of memory (or 0)
    bslma::Allocator *d_allocator_p;  // memory allocator (held, not owned)

  private:
    // NOT IMPLEMENTED
    InfrequentDeleteBlockList(const InfrequentDeleteBlockList&);
    InfrequentDeleteBlockList& operator=(const InfrequentDeleteBlockList&);

  public:
    // CREATORS

    /// Create an empty block list suitable for managing memory blocks of
    /// varying sizes.  Optionally specify a `basicAllocator` used to supply
    /// memory.  If `basicAllocator` is 0, the currently installed default
    /// allocator is used.
    explicit
    InfrequentDeleteBlockList(bslma::Allocator *basicAllocator = 0);

    /// Destroy this object and deallocate all outstanding memory blocks
    /// managed by this object.
    ~InfrequentDeleteBlockList();

    // MANIPULATORS

    /// Return the address of a contiguous block of memory of the specified
    /// `size` (in bytes).  If `size` is 0, no memory is allocated and 0 is
    /// returned.  The returned memory is guaranteed to be maximally
    /// aligned.
    void *allocate(bsls::Types::size_type size);

    /// This method has no effect on the memory block at the specified
    /// `address` as all memory allocated by this object is managed.  The
    /// behavior is undefined unless `address` was allocated by this object,
    /// and has not already been released.
    void deallocate(void *address);

    /// Deallocate all memory blocks managed by this object, returning this
    /// object to its default-constructed state.
    void release();

    /// Deallocate all except the most-recently obtained block of the memory
    /// blocks managed by this object.  If no blocks are managed, this
    /// method has no effect.
    void releaseAllButLastBlock();

    // ACCESSORS
                                  // Aspects

    /// Return the allocator used by this object to supply memory.
    bslma::Allocator *allocator() const;
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                     // -------------------------------
                     // class InfrequentDeleteBlockList
                     // -------------------------------

// CREATORS
inline
InfrequentDeleteBlockList::InfrequentDeleteBlockList(
                                              bslma::Allocator *basicAllocator)
: d_head_p(0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

// MANIPULATORS
inline
void InfrequentDeleteBlockList::deallocate(void *)
{
}

// ACCESSORS
                                  // Aspects

inline
bslma::Allocator *InfrequentDeleteBlockList::allocator() const
{
    return d_allocator_p;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
