// bdlma_memoryblockdescriptor.h                                      -*-C++-*-
#ifndef INCLUDED_BDLMA_MEMORYBLOCKDESCRIPTOR
#define INCLUDED_BDLMA_MEMORYBLOCKDESCRIPTOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a class describing a block of memory.
//
//@CLASSES:
//  bdlma::MemoryBlockDescriptor: describes a block of memory
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines an in-core value-semantic class for
// describing a block of memory, namely `bdlma::MemoryBlockDescriptor`.  Each
// descriptor object contains the address of the block of memory and the size
// of the block.  The distinguished "null" descriptor contains an address and a
// size that are both 0.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// This example demonstrates how to create and test the state of a
// `bdlma::MemoryBlockDescriptor`.
// ```
//  char buffer[100];
//
//  bdlma::MemoryBlockDescriptor a(buffer, sizeof buffer);
//                              assert(!a.isNull());
//                              assert(buffer         == a.address());
//                              assert(sizeof buffer  == a.size());
//
//  bdlma::MemoryBlockDescriptor b;
//                              assert( b.isNull());
//                              assert(0              == b.address());
//                              assert(0              == b.size());
//                              assert(a              != b);
//
//  b = a;
//                              assert(!b.isNull());
//                              assert(buffer         == b.address());
//                              assert(sizeof buffer  == b.size());
//                              assert(a              == b);
//
// ```

#include <bdlscm_version.h>

#include <bsls_assert.h>
#include <bsls_review.h>
#include <bsls_types.h>

namespace BloombergLP {
namespace bdlma {

                       // ===========================
                       // class MemoryBlockDescriptor
                       // ===========================

/// This is an in-core value-semantic class describing the size and address
/// of a block of memory.  A null block descriptor is defined as a
/// descriptor having an address and size of 0.  The behavior is undefined
/// for a descriptor whose address is 0, but whose size if not also 0.
class MemoryBlockDescriptor {

  public:
    // PUBLIC TYPES
    typedef bsls::Types::size_type size_type;  // type for block size

  private:
    // DATA
    void      *d_address_p;  // address of the memory block
    size_type  d_size;       // size of the block

  public:
    // CREATORS

    /// Create a memory block descriptor having an address and size of 0.
    MemoryBlockDescriptor();

    /// Create a memory block descriptor having the specified `address` and
    /// `size`.  The behavior is undefined if `address` is 0 but `size` is
    /// not also 0.
    MemoryBlockDescriptor(void *address, size_type size);

    /// Create a memory block descriptor having the same value as the
    /// specified `original` descriptor.  Two descriptors have the same
    /// value if and only if they have the same address and size.
    MemoryBlockDescriptor(const MemoryBlockDescriptor& original);

    // Destroy this object.
    //! ~MemoryBlockDescriptor() = default;

    // MANIPULATORS

    /// Assign to this memory block descriptor the value of the specified `rhs`
    /// descriptor and return a reference to this modifiable descriptor.
    MemoryBlockDescriptor& operator=(const MemoryBlockDescriptor& rhs);

    /// Set the address and size of the memory block described by this object
    /// to the specified `address` and `size`.  The behavior is undefined if
    /// `address' is 0 but `size` is not also 0.
    void setAddressAndSize(void *address, size_type size);

    // ACCESSORS

    /// Return `true` if this memory block descriptor describes a null
    /// memory block and `false` otherwise.  A null memory block has an
    /// address and size of 0.
    bool isNull() const;

    /// Return the address of the modifiable memory block described by this
    /// object, or 0 if this is a null descriptor.
    void *address() const;

    /// Return the size of the memory block described by this object.
    size_type size() const;

    /// Format the attributes of this memory block descriptor to `stdout` in
    /// some reasonable (single-line) format.
    void print() const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` memory block descriptors
/// have the same value and `false` otherwise.  Two descriptors have the same
/// value if and only if they have the same address and size.
bool operator==(const MemoryBlockDescriptor& lhs,
                const MemoryBlockDescriptor& rhs);

/// Return `true` if the specified `lhs` and `rhs` memory block descriptors do
/// not have the same value and `false` otherwise.  Two descriptors differ in
/// value if they differ in either their address or size.
bool operator!=(const MemoryBlockDescriptor& lhs,
                const MemoryBlockDescriptor& rhs);

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                       // ---------------------------
                       // class MemoryBlockDescriptor
                       // ---------------------------

// CREATORS
inline
MemoryBlockDescriptor::MemoryBlockDescriptor()
: d_address_p(0)
, d_size(0)
{
}

inline
MemoryBlockDescriptor::MemoryBlockDescriptor(void      *address,
                                             size_type  size)
: d_address_p(address)
, d_size(size)
{
    BSLS_ASSERT(address || 0 == size);
}

inline
MemoryBlockDescriptor::MemoryBlockDescriptor(
                                         const MemoryBlockDescriptor& original)
: d_address_p(original.d_address_p)
, d_size(original.d_size)
{
}

// MANIPULATORS
inline
MemoryBlockDescriptor& MemoryBlockDescriptor::operator=(
                                              const MemoryBlockDescriptor& rhs)
{
    d_address_p = rhs.d_address_p;
    d_size      = rhs.d_size;

    return *this;
}

inline
void MemoryBlockDescriptor::setAddressAndSize(void *address, size_type size)
{
    BSLS_ASSERT(address || 0 == size);

    d_address_p = address;
    d_size      = size;
}

// ACCESSORS
inline
void *MemoryBlockDescriptor::address() const
{
    return d_address_p;
}

inline
MemoryBlockDescriptor::size_type
MemoryBlockDescriptor::size() const
{
    return d_size;
}

inline
bool MemoryBlockDescriptor::isNull() const
{
    return 0 == d_address_p;
}

}  // close package namespace

// FREE OPERATORS
inline
bool bdlma::operator==(const MemoryBlockDescriptor& lhs,
                       const MemoryBlockDescriptor& rhs)
{
    return lhs.address() == rhs.address() && lhs.size() == rhs.size();
}

inline
bool bdlma::operator!=(const MemoryBlockDescriptor& lhs,
                       const MemoryBlockDescriptor& rhs)
{
    return !(lhs == rhs);
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
