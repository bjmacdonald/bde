// bsltf_movabletesttype.h                                            -*-C++-*-
#ifndef INCLUDED_BSLTF_MOVABLETESTTYPE
#define INCLUDED_BSLTF_MOVABLETESTTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a non-allocating test class that records when moved from.
//
//@CLASSES:
//   bsltf::MovableTestType: non-allocating test class that records moves
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@DESCRIPTION: This component provides a single, unconstrained, non-allocating
// (value-semantic) attribute class, `MovableTestType`, that records when
// the move constructor or assignment operator is called with the instance as
// the source argument.  This class is not bitwise-moveable, and will assert on
// destruction if it has been copied (or moved) without calling a constructor.
// This class is primarily provided to facilitate testing of templates where
// move semantics need to be differentiated versus copy semantics.
//
///Attributes
///----------
// ```
// Name                Type             Default
// ------------------  ---------------  -------
// data                int              0
// movedInto           MoveState::Enum  e_NOT_MOVED
// movedFrom           MoveState::Enum  e_NOT_MOVED
// ```
// * `data`: representation of the object's value
// * `movedInto`: indicates whether a move constructor or move assignment
//   operator was used to set the value of this object.
// * `movedFrom`: indicates whether a move constructor or move assignment
//   operator was used to move out the value of this object.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Printing the Supported Traits
/// - - - - - - - - - - - - - - - - - - - -
// Suppose we wanted to print the supported traits of this test type.
//
// First, we create a function template `printTypeTraits` with a parameterized
// `TYPE`:
// ```
// template <class TYPE>
// void printTypeTraits()
//     // Prints the traits of the parameterized 'TYPE' to the console.
// {
//     if (bslma::UsesBslmaAllocator<TYPE>::value) {
//         printf("Type defines bslma::UsesBslmaAllocator.\n");
//     }
//     else {
//         printf(
//             "Type does not define bslma::UsesBslmaAllocator.\n");
//     }
//
//     if (bslmf::IsBitwiseMoveable<TYPE>::value) {
//         printf("Type defines bslmf::IsBitwiseMoveable.\n");
//     }
//     else {
//         printf("Type does not define bslmf::IsBitwiseMoveable.\n");
//     }
// }
// ```
// Now, we invoke the `printTypeTraits` function template using
// `MovableTestType` as the parameterized `TYPE`:
// ```
// printTypeTraits<MovableTestType>();
// ```
// Finally, we observe the console output:
// ```
// Type does not define bslma::UsesBslmaAllocator.
// Type does not define bslmf::IsBitwiseMoveable.
// ```

#include <bslscm_version.h>

#include <bsltf_movestate.h>

#include <bslmf_isnothrowmoveconstructible.h>
#include <bslmf_movableref.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>

namespace BloombergLP {
namespace bsltf {

                        // =====================
                        // class MovableTestType
                        // =====================

/// This class provides an unconstrained (value-semantic) attribute type
/// that records when move semantics have been invoked with the object
/// instance as the source parameter.  This class is primarily provided
/// to facilitate testing of templates where move semantics need to be
/// differentiated versus copy semantics.  See the `Attributes` section
/// under @DESCRIPTION in the component-level documentation for information
/// on the class attributes.
class MovableTestType {

    // DATA
    int                d_data;         // data value

    MovableTestType   *d_self_p;       // pointer to self (to verify this
                                       // object is not bit-wise moved
    MoveState::Enum    d_movedFrom;    // moved-from state
    MoveState::Enum    d_movedInto;    // moved-into state

#ifndef BSLS_COMPILERFEATURES_SUPPORT_NOEXCEPT
  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(MovableTestType,
                                   bsl::is_nothrow_move_constructible);
#endif

  public:
    // CREATORS

    /// Create a `MovableTestType` object having the (default)
    /// attribute values:
    /// ```
    /// data()      == -1
    /// movedInto() == e_NOT_MOVED
    /// movedFrom() == e_NOT_MOVED
    /// ```
    MovableTestType();

    /// Create a `MovableTestType` object having the specified `data`
    /// attribute value.
    explicit MovableTestType(int data);

    /// Create a `MovableTestType` object having the same value as the
    /// specified `original` object.  Note that `movedInto()` for this
    /// object will be `e_NOT_MOVED`, and `original.movedFrom()` will also
    /// be `e_NOT_MOVED`.
    MovableTestType(const MovableTestType&  original);

    /// Create a `MovableTestType` object having the same value as the
    /// specified `original` object.  Note that `movedInto()` for this
    /// object will be `e_MOVED`, and `original.movedFrom()` will also be
    /// `e_MOVED`.
    MovableTestType(bslmf::MovableRef<MovableTestType>
                                               original) BSLS_KEYWORD_NOEXCEPT;

    /// Destroy this object.
    ~MovableTestType();

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.  Note
    /// that `movedInto()` for this object will be `e_NOT_MOVED`, and
    /// `rhs.movedFrom()` will also be `e_NOT_MOVED`.
    MovableTestType& operator=(const MovableTestType& rhs);

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.
    /// Note that `movedInto()` for this object will be `e_MOVED`, and
    /// `rhs.movedFrom()` will also be `e_MOVED`.
    MovableTestType& operator=(bslmf::MovableRef<MovableTestType> rhs);

    /// Set the `data` attribute of this object to the specified `value`.
    void setData(int value);

    /// Set the moved-into state of this object to the specified `value`.
    void setMovedInto(MoveState::Enum value);

    // ACCESSORS

    /// Return the value of the `data` attribute of this object.
    int data() const;

    /// Return the move state of this object as target of a move operation.
    MoveState::Enum movedInto() const;

    /// Return the move state of this object as source of a move operation.
    MoveState::Enum movedFrom() const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` objects have the same
/// `data()` value, and `false` otherwise.  Two `MovableTestType` objects
/// have the same value if their `data` attributes are the same.
/// TBD: think about the behavior when specified on an object that was
/// moved-from on this as well as other functions/methods if appropriate.
bool operator==(const MovableTestType& lhs,
                const MovableTestType& rhs);

/// Return `true` if the specified `lhs` and `rhs` objects do not have the
/// same `data()` value, and `false` otherwise.  Two `MovableTestType`
/// objects do not have the same value if their `data` attributes are not
/// the same.
bool operator!=(const MovableTestType& lhs,
                const MovableTestType& rhs);

// FREE FUNCTIONS

/// Return the move-from state of the specified `object`.
MoveState::Enum getMovedFrom(const MovableTestType& object);

/// Return the move-into state of the specified `object`.
MoveState::Enum getMovedInto(const MovableTestType& object);

/// Set the moved-into state of the specified `object` to the specified
/// `value`.
void setMovedInto(MovableTestType *object, MoveState::Enum value);

// ============================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ============================================================================

                        // ---------------------
                        // class MovableTestType
                        // ---------------------

// MANIPULATORS
inline
void MovableTestType::setMovedInto(MoveState::Enum value)
{
    d_movedInto = value;
}

// ACCESSORS
inline
int MovableTestType::data() const
{
    return d_data;
}

inline
MoveState::Enum MovableTestType::movedFrom() const
{
    return d_movedFrom;
}

inline
MoveState::Enum MovableTestType::movedInto() const
{
    return d_movedInto;
}

// FREE FUNCTIONS
inline
MoveState::Enum getMovedFrom(const MovableTestType& object)
{
    return object.movedFrom();
}

inline
MoveState::Enum getMovedInto(const MovableTestType& object)
{
    return object.movedInto();
}

inline
void setMovedInto(MovableTestType *object, MoveState::Enum value)
{
    object->setMovedInto(value);
}

}  // close package namespace

// FREE OPERATORS
inline
bool bsltf::operator==(const MovableTestType& lhs,
                       const MovableTestType& rhs)
{
    return lhs.data() == rhs.data();
}

inline
bool bsltf::operator!=(const MovableTestType& lhs,
                       const MovableTestType& rhs)
{
    return lhs.data() != rhs.data();
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
