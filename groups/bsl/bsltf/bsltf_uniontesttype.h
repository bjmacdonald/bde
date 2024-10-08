// bsltf_uniontesttype.h                                              -*-C++-*-
#ifndef INCLUDED_BSLTF_UNIONTESTTYPE
#define INCLUDED_BSLTF_UNIONTESTTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an union test type.
//
//@CLASSES:
//   bsltf::UnionTestType: union test type
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@DESCRIPTION: This component provides a union type, `UnionTestType`, that
// supports the set of operations for a single unconstrained (value-semantic)
// attribute type.  `UnionTestType` can be used during testing as the
// parameterized type of a class templates to ensure `union` types will
// function correctly.  Note that this is particular valuable when testing a
// container template that supports different types of contained elements.
//
///Attributes
///----------
// ```
// Name                Type         Default
// ------------------  -----------  -------
// data                int          0
// ```
// * `data`: value of this type
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Constructing And Using The Union
///- - - - - - - - - - - - - - - - - - - - - -
// Suppose we wanted to show how objects of `UnionTestType` can be created and
// used.
// ```
// UnionTestType X;
//
// X.setData(-1);
// assert(X.data() == -1);
//
// UnionTestType Y = X;
// assert(Y == X);
//
// UnionTestType Z(X);
// assert(Z == X);
// ```

#include <bslscm_version.h>

#include <cstring>

namespace BloombergLP {
namespace bsltf {

                        // ===================
                        // union UnionTestType
                        // ===================

/// This union provides a union test type to facilitate testing of
/// templates.
union UnionTestType {

    // DATA
  private:
    int    d_data;    // integer data value
    char   d_char;    // the char in this union
    float  d_float;   // the float in this union
    double d_double;  // the double in this union

  public:
    // CREATORS

    /// Construct a `UnionTestType` union having all members set to 0.
    UnionTestType();

    /// Construct a `UnionTestType` having the `d_data` member set to the
    /// specified `value`.
    explicit UnionTestType(int value);

    /// Destroy this object.
    ~UnionTestType();

    // MANIPULATORS

    /// Set the `data` attribute of this object to the specified `value`.
    void setData(int value);

    // ACCESSORS

    /// Return the value of the `data` attribute of this object.
    int data() const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` unions have the same
/// value, and `false` otherwise.  Two `UnionTestType` objects have the same
/// if their `data` attributes are the same.
bool operator==(const UnionTestType& lhs, const UnionTestType& rhs);

/// Return `true` if the specified `lhs` and `rhs` objects do not have the
/// same value, and `false` otherwise.  Two `UnionTestType` objects do not
/// have the same value if their `data` attributes are not the same.
bool operator!=(const UnionTestType& lhs, const UnionTestType& rhs);

// ============================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

                        // -------------------
                        // class UnionTestType
                        // -------------------

// CREATORS
inline
UnionTestType::UnionTestType()
: d_double(0.0)
{
    d_data = 0;
}

inline
UnionTestType::UnionTestType(int value)
: d_double(0.0)
{
    d_data = value;
}

// MANIPULATORS
inline
void UnionTestType::setData(int value)
{
    d_data = value;
}

inline
UnionTestType::~UnionTestType()
{
    std::memset(this, 0xa5, sizeof(*this));
}

// ACCESSORS
inline
int UnionTestType::data() const
{
    return d_data;
}

// FREE OPERATORS
inline
bool operator==(const bsltf::UnionTestType& lhs,
                const bsltf::UnionTestType& rhs)
{
    return lhs.data() == rhs.data();
}

inline
bool operator!=(const bsltf::UnionTestType& lhs,
                const bsltf::UnionTestType& rhs)
{
    return lhs.data() != rhs.data();
}

}  // close package namespace
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
