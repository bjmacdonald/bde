// bdlat_choicefunctions.h                                            -*-C++-*-
#ifndef INCLUDED_BDLAT_CHOICEFUNCTIONS
#define INCLUDED_BDLAT_CHOICEFUNCTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace defining choice functions.
//
//@CLASSES:
//  bdlat_ChoiceFunctions: namespace for calling choice functions
//
//@SEE_ALSO: bdlat_selectioninfo
//
//@DESCRIPTION: The `bdlat_ChoiceFunctions` `namespace` provided in this
// component defines parameterized functions that expose "choice" behavior for
// "choice" types.  See the {`bdlat`} package-level documentation for a full
// description of "choice" types.
//
// The functions in this namespace allow users to:
// * make a selection using either a selection id or a selection name
//   (`makeSelection`),
// * manipulate the current selection using a parameterized manipulator
//   (`manipulateSelection`),
// * access the current selection using a parameterized accessor
//   (`accessSelection`), and
// * obtain the id for the current selection (`selectionId`).
//
// Also, the meta-function `IsChoice` contains a compile-time constant `value`
// that is non-zero if the parameterized `TYPE` exposes "choice" behavior
// through the `bdlat_ChoiceFunctions` `namespace`.
//
// This component specializes all of these functions for types that have the
// `bdlat_TypeTraitBasicChoice` trait.
//
// Types that do not have the `bdlat_TypeTraitBasicChoice` trait can be plugged
// into the `bdlat` framework.  This is done by overloading the `bdlat_choice*`
// functions inside the namespace of the plugged in type.  For example, suppose
// there is a type called `mine::MyChoice`.  In order to plug this type into
// the `bdlat` framework as a "Choice", the following functions must be
// declared and implemented in the `mine` namespace:
// ```
// namespace mine {
//
// // MANIPULATORS
// int bdlat_choiceMakeSelection(MyChoice *object, int selectionId);
//     // Set the value of the specified 'object' to be the default for
//     // the selection indicated by the specified 'selectionId'.  Return
//     // 0 on success, and non-zero value otherwise (i.e., the selection
//     // is not found).
//
// int bdlat_choiceMakeSelection(MyChoice  *object,
//                               const char *selectionName,
//                               int         selectionNameLength);
//     // Set the value of the specified 'object' to be the default for
//     // the selection indicated by the specified 'selectionName' of the
//     // specified 'selectionNameLength'.  Return 0 on success, and
//     // non-zero value otherwise (i.e., the selection is not found).
//
// template <typename MANIPULATOR>
// int bdlat_choiceManipulateSelection(MyChoice *object,
//                                     MANIPULATOR& manipulator);
//     // Invoke the specified 'manipulator' on the address of the
//     // (modifiable) selection of the specified 'object', supplying
//     // 'manipulator' with the corresponding selection information
//     // structure.  Return -1 if the selection is undefined, and the
//     // value returned from the invocation of 'manipulator' otherwise.
//
// // ACCESSORS
// template <typename ACCESSOR>
// int bdlat_choiceAccessSelection(const MyChoice& object,
//                                 ACCESSOR& accessor);
//     // Invoke the specified 'accessor' on the (non-modifiable)
//     // selection of the specified 'object', supplying 'accessor' with
//     // the corresponding selection information structure.  Return -1 if
//     // the selection is undefined, and the value returned from the
//     // invocation of 'accessor' otherwise.
//
// int bdlat_choiceSelectionId(const MyChoice& object);
//     // Return the id of the current selection if the selection is
//     // defined, and 0 otherwise.
//
// }  // close namespace mine
// ```
// Also, the `IsChoice` meta-function must be specialized for the
// `mine::MyChoice` type in the `bdlat_ChoiceFunctions` namespace.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// Suppose you had a `union` embedded inside a `struct`.  The `struct` also
// contains a `d_selectionId` member that specifies which member of the `union`
// is selected.  The default constructor of the `struct` makes the selection
// undefined:
// ```
// namespace BloombergLP {
//
// namespace mine {
//
// struct MyChoice {
//     // This struct represents a choice between a 'char' value, an 'int'
//     // value, and a 'float' value.
//
//     // CONSTANTS
//     enum {
//         UNDEFINED_SELECTION_ID = -1,
//         CHAR_SELECTION_ID      = 0,
//         INT_SELECTION_ID       = 1,
//         FLOAT_SELECTION_ID     = 2
//     };
//
//     // DATA MEMBERS
//     union {
//         char  d_charValue;
//         int   d_intValue;
//         float d_floatValue;
//     };
//     int d_selectionId;
//
//     // CREATORS
//     MyChoice()
//     : d_selectionId(UNDEFINED_SELECTION_ID)
//     {
//     }
// };
// ```
// We can now make `MyChoice` expose "choice" behavior by implementing
// `bdlat_ChoiceFunctions` for `MyChoice`.  First, we should forward declare
// all the functions that we will implement inside the `mine` namespace:
// ```
// // MANIPULATORS
// int bdlat_choiceMakeSelection(MyChoice *object, int selectionId);
//     // Set the value of the specified 'object' to be the default for the
//     // selection indicated by the specified 'selectionId'.  Return 0 on
//     // success, and non-zero value otherwise (i.e., the selection is not
//     // found).
//
// int bdlat_choiceMakeSelection(MyChoice   *object,
//                               const char *selectionName,
//                               int         selectionNameLength);
//     // Set the value of the specified 'object' to be the default for the
//     // selection indicated by the specified 'selectionName' of the
//     // specified 'selectionNameLength'.  Return 0 on success, and non-zero
//     // value otherwise (i.e., the selection is not found).
//
// template <class MANIPULATOR>
// int bdlat_choiceManipulateSelection(MyChoice     *object,
//                                     MANIPULATOR&  manipulator);
//     // Invoke the specified 'manipulator' on the address of the
//     // (modifiable) selection of the specified 'object', supplying
//     // 'manipulator' with the corresponding selection information
//     // structure.  Return -1 if the selection is undefined, and the value
//     // returned from the invocation of 'manipulator' otherwise.
//
// // ACCESSORS
// template <class ACCESSOR>
// int bdlat_choiceAccessSelection(const MyChoice& object,
//                                 ACCESSOR&       accessor);
//     // Invoke the specified 'accessor' on the (non-modifiable) selection of
//     // the specified 'object', supplying 'accessor' with the corresponding
//     // selection information structure.  Return -1 if the selection is
//     // undefined, and the value returned from the invocation of 'accessor'
//     // otherwise.
//
// int bdlat_choiceSelectionId(const MyChoice& object);
//     // Return the id of the current selection if the selection is defined,
//     // and 0 otherwise.
//
// }  // close namespace mine
// ```
// Now, we provide the definitions for each of these functions:
// ```
// // MANIPULATORS
// int mine::bdlat_choiceMakeSelection(MyChoice *object,
//                                     int       selectionId)
// {
//     enum { SUCCESS = 0, NOT_FOUND = -1 };
//
//     switch (selectionId) {
//       case MyChoice::CHAR_SELECTION_ID: {
//         object->d_selectionId = selectionId;
//         object->d_charValue   = 0;
//
//         return SUCCESS;                                           // RETURN
//       }
//       case MyChoice::INT_SELECTION_ID: {
//         object->d_selectionId = selectionId;
//         object->d_intValue    = 0;
//
//         return SUCCESS;                                           // RETURN
//       }
//       case MyChoice::FLOAT_SELECTION_ID: {
//         object->d_selectionId = selectionId;
//         object->d_floatValue  = 0;
//
//         return SUCCESS;                                           // RETURN
//       }
//       case MyChoice::UNDEFINED_SELECTION_ID: {
//         object->d_selectionId = selectionId;
//
//         return SUCCESS;                                           // RETURN
//       }
//       default: {
//         return NOT_FOUND;                                         // RETURN
//       }
//     }
// }
//
// int mine::bdlat_choiceMakeSelection(MyChoice   *object,
//                                     const char *selectionName,
//                                     int         selectionNameLength)
// {
//     enum { NOT_FOUND = -1 };
//
//     if (bdlb::String::areEqualCaseless("charValue",
//                                       selectionName,
//                                       selectionNameLength)) {
//         return bdlat_choiceMakeSelection(object,
//                                          MyChoice::CHAR_SELECTION_ID);
//                                                                   // RETURN
//     }
//
//     if (bdlb::String::areEqualCaseless("intValue",
//                                       selectionName,
//                                       selectionNameLength)) {
//         return bdlat_choiceMakeSelection(object,
//                                          MyChoice::INT_SELECTION_ID);
//                                                                   // RETURN
//     }
//
//     if (bdlb::String::areEqualCaseless("floatValue",
//                                       selectionName,
//                                       selectionNameLength)) {
//         return bdlat_choiceMakeSelection(object,
//                                          MyChoice::FLOAT_SELECTION_ID);
//                                                                   // RETURN
//     }
//
//     return NOT_FOUND;
// }
// ```
// For the `manipulateSelection` and `accessSelection` functions, we need to
// create a temporary `bdlat_SelectionInfo` object and pass it along when
// invoking the manipulator or accessor.  See the {`bdlat_selectioninfo`}
// component-level documentation for more information.  The implementation of
// the remaining functions are as follows:
// ```
// template <class MANIPULATOR>
// int mine::bdlat_choiceManipulateSelection(MyChoice     *object,
//                                           MANIPULATOR&  manipulator)
// {
//     switch (object->d_selectionId) {
//       case MyChoice::CHAR_SELECTION_ID: {
//         bdlat_SelectionInfo info;
//
//         info.annotation()     = "Char Selection";
//         info.formattingMode() = bdlat_FormattingMode::e_DEFAULT;
//         info.id()             = MyChoice::CHAR_SELECTION_ID;
//         info.name()           = "charValue";
//         info.nameLength()     = 9;
//
//         return manipulator(&object->d_charValue, info);           // RETURN
//       }
//       case MyChoice::INT_SELECTION_ID: {
//         bdlat_SelectionInfo info;
//
//         info.annotation()     = "Int Selection";
//         info.formattingMode() = bdlat_FormattingMode::e_DEFAULT;
//         info.id()             = MyChoice::INT_SELECTION_ID;
//         info.name()           = "intValue";
//         info.nameLength()     = 8;
//
//         return manipulator(&object->d_intValue, info);            // RETURN
//       }
//       case MyChoice::FLOAT_SELECTION_ID: {
//         bdlat_SelectionInfo info;
//
//         info.annotation()     = "Float Selection";
//         info.formattingMode() = bdlat_FormattingMode::e_DEFAULT;
//         info.id()             = MyChoice::FLOAT_SELECTION_ID;
//         info.name()           = "floatValue";
//         info.nameLength()     = 10;
//
//         return manipulator(&object->d_floatValue, info);          // RETURN
//       }
//       default:
//         BSLS_ASSERT_SAFE(0 == "Invalid selection!");
//     }
//     return 0;
// }
//
// // ACCESSORS
//
// template <class ACCESSOR>
// int mine::bdlat_choiceAccessSelection(const MyChoice& object,
//                                       ACCESSOR&       accessor)
// {
//     switch (object.d_selectionId) {
//       case MyChoice::CHAR_SELECTION_ID: {
//         bdlat_SelectionInfo info;
//
//         info.annotation()     = "Char Selection";
//         info.formattingMode() = bdlat_FormattingMode::e_DEFAULT;
//         info.id()             = MyChoice::CHAR_SELECTION_ID;
//         info.name()           = "charValue";
//         info.nameLength()     = 9;
//
//         return accessor(object.d_charValue, info);                // RETURN
//       }
//       case MyChoice::INT_SELECTION_ID: {
//         bdlat_SelectionInfo info;
//
//         info.annotation()     = "Int Selection";
//         info.formattingMode() = bdlat_FormattingMode::e_DEFAULT;
//         info.id()             = MyChoice::INT_SELECTION_ID;
//         info.name()           = "intValue";
//         info.nameLength()     = 8;
//
//         return accessor(object.d_intValue, info);                 // RETURN
//       }
//       case MyChoice::FLOAT_SELECTION_ID: {
//         bdlat_SelectionInfo info;
//
//         info.annotation()     = "Float Selection";
//         info.formattingMode() = bdlat_FormattingMode::e_DEFAULT;
//         info.id()             = MyChoice::FLOAT_SELECTION_ID;
//         info.name()           = "floatValue";
//         info.nameLength()     = 10;
//
//         return accessor(object.d_floatValue, info);               // RETURN
//       }
//       default:
//         BSLS_ASSERT_SAFE(0 == "Invalid selection!");
//     }
//     return 0;
// }
//
// int mine::bdlat_choiceSelectionId(const MyChoice& object)
// {
//     return object.d_selectionId;
// }
// ```
// Finally, we need to specialize the `IsChoice` meta-function in the
// `bdlat_ChoiceFunctions` namespace for the `mine::MyChoice` type.  This makes
// the `bdlat` infrastructure recognize `mine::MyChoice` as a choice
// abstraction:
// ```
// namespace bdlat_ChoiceFunctions {
//
// template <>
// struct IsChoice<mine::MyChoice> : bsl::true_type {
// };
//
// }  // close namespace bdlat_ChoiceFunctions
// }  // close enterprise namespace
// ```
// The `bdlat` infrastructure (and any component that uses this infrastructure)
// will now recognize `MyChoice` as a "choice" type.  For example, suppose we
// have the following XML data:
// ```
// <?xml version='1.0' encoding='UTF-8' ?>
// <MyChoice>
//     <intValue>321</intValue>
// </MyChoice>
// ```
// Using the `balxml_decoder` component, we can load this XML data into a
// `MyChoice` object:
// ```
// void usageExample(bsl::istream& inputData)
// {
//   using namespace BloombergLP;
//
//   MyChoice object;
//
//   assert(MyChoice::UNDEFINED_SELECTION_ID == object.d_selectionId);
//
//   balxml::DecoderOptions options;
//   balxml::MiniReader     reader;
//   balxml::ErrorInfo      errInfo;
//
//   balxml::Decoder decoder(&options, &reader, &errInfo);
//   int result = decoder.decode(inputData, &object);
//
//   assert(0                           == result);
//   assert(MyChoice::INT_SELECTION_ID  == object.d_selectionId);
//   assert(321                         == object.d_intValue);
// }
// ```
// Note that the `bdlat` framework can be used for functionality other than
// encoding/decoding into XML.  When `mine::MyChoice` is plugged into the
// framework, then it will be automatically usable within the framework.  For
// example, the following snippets of code will print out the selection value
// of a choice object:
// ```
// struct PrintSelection {
//     // Print each visited object to the bound 'd_stream_p' object.
//
//     // DATA MEMBERS
//     bsl::ostream *d_stream_p;
//
//     template <class TYPE, class INFO>
//     int operator()(const TYPE& object, const INFO& info)
//     {
//       (*d_stream_p) << info.name() << ": " << object << bsl::endl;
//       return 0;                                                   // RETURN
//     }
// };
//
// template <class TYPE>
// void printChoiceSelection(bsl::ostream& stream, const TYPE& object)
// {
//     using namespace BloombergLP;
//
//     PrintSelection accessor;
//     accessor.d_stream_p = &stream;
//
//     bdlat_choiceAccessSelection(object, accessor);
// }
// ```
// Now we have a generic function that takes an output stream and a choice
// object, and prints out each choice seletion with its name and value.  We can
// use this generic function as follows:
// ```
// void printMyChoice(bsl::ostream& stream)
// {
//     using namespace BloombergLP;
//
//     mine::MyChoice object;
//
//     object.d_selectionId = mine::MyChoice::INT_SELECTION_ID;
//     object.d_intValue    = 321;
//
//     printChoiceSelection(stream, object);
// }
// ```
// The function above will print the following to provided stream:
// ```
// intValue: 321
// ```

#include <bdlscm_version.h>

#include <bdlat_bdeatoverrides.h>
#include <bdlat_selectioninfo.h>
#include <bdlat_typetraits.h>

#include <bslalg_hastrait.h>

#include <bslmf_assert.h>
#include <bslmf_integralconstant.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

namespace BloombergLP {

                      // ===============================
                      // namespace bdlat_ChoiceFunctions
                      // ===============================

namespace bdlat_ChoiceFunctions {
    // This 'namespace' provides functions that expose "choice" behavior for
    // "choice" types.  See the component-level documentation for more
    // information.

    // CONSTANTS
    enum {
        k_UNDEFINED_SELECTION_ID = -1  // indicates selection not made

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , UNDEFINED_SELECTION_ID       = k_UNDEFINED_SELECTION_ID
      , BDEAT_UNDEFINED_SELECTION_ID = k_UNDEFINED_SELECTION_ID
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

    // META-FUNCTIONS

    /// This `struct` should be specialized for third-party types that need
    /// to expose "choice" behavior.  See the component-level documentation
    /// for further information.
    template <class TYPE>
    struct IsChoice
    : public bsl::integral_constant<
          bool,
          bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicChoice>::value> {
    };

    // MANIPULATORS

    /// Set the value of the specified `object` to be the default for the
    /// selection indicated by the specified `selectionId`.  Return 0 on
    /// success, and non-zero value otherwise (i.e., the selection is not
    /// found).
    template <class TYPE>
    int makeSelection(TYPE *object, int selectionId);

    /// Set the value of the specified `object` to be the default for the
    /// selection indicated by the specified `selectionName` of the
    /// specified `selectionNameLength`.  Return 0 on success, and non-zero
    /// value otherwise (i.e., the selection is not found).
    template <class TYPE>
    int makeSelection(TYPE       *object,
                      const char *selectionName,
                      int         selectionNameLength);

    /// Invoke the specified `manipulator` on the address of the
    /// (modifiable) selection of the specified `object`, supplying
    /// `manipulator` with the corresponding selection information
    /// structure.  Return the value returned from the invocation of
    /// `manipulator`.  The behavior is undefined unless
    /// `k_UNDEFINED_SELECTION_ID != selectionId(*object)`.
    template <class TYPE, class MANIPULATOR>
    int manipulateSelection(TYPE *object, MANIPULATOR& manipulator);

    // ACCESSORS

    /// Invoke the specified `accessor` on the (non-modifiable) selection of
    /// the specified `object`, supplying `accessor` with the corresponding
    /// selection information structure.  Return the value returned from the
    /// invocation of `accessor`.  The behavior is undefined unless
    /// `k_UNDEFINED_SELECTION_ID != selectionId(object)`.
    template <class TYPE, class ACCESSOR>
    int accessSelection(const TYPE& object, ACCESSOR& accessor);

    /// Return true if the specified `object` has a selection with the
    /// specified `selectionName` of the specified `selectionNameLength`,
    /// and false otherwise.
    template <class TYPE>
    bool hasSelection(const TYPE&  object,
                      const char  *selectionName,
                      int          selectionNameLength);

    /// Return true if the specified `object` has a selection with the
    /// specified `selectionId`, and false otherwise.
    template <class TYPE>
    bool hasSelection(const TYPE& object,
                      int         selectionId);

    /// Return the id of the current selection if the selection is defined,
    /// and k_UNDEFINED_SELECTION_ID otherwise.
    template <class TYPE>
    int selectionId(const TYPE& object);

}  // close namespace bdlat_ChoiceFunctions

                            // ====================
                            // default declarations
                            // ====================

namespace bdlat_ChoiceFunctions {
    // This namespace declaration adds the default implementations of the
    // "choice" customization-point functions to 'bdlat_ChoiceFunctions'. These
    // default implementations assume the type of the acted-upon object is a
    // basic-choice type.  For more information about basic-choice types, see
    // {'bdlat_typetraits'}.

    // MANIPULATORS
    template <class TYPE>
    int bdlat_choiceMakeSelection(TYPE *object, int selectionId);

    template <class TYPE>
    int bdlat_choiceMakeSelection(TYPE       *object,
                                  const char *selectionName,
                                  int         selectionNameLength);

    template <class TYPE, class MANIPULATOR>
    int bdlat_choiceManipulateSelection(TYPE         *object,
                                        MANIPULATOR&  manipulator);

    // ACCESSORS
    template <class TYPE, class ACCESSOR>
    int bdlat_choiceAccessSelection(const TYPE& object, ACCESSOR& accessor);

    template <class TYPE>
    bool bdlat_choiceHasSelection(const TYPE&  object,
                                  const char  *selectionName,
                                  int          selectionNameLength);

    template <class TYPE>
    bool bdlat_choiceHasSelection(const TYPE& object,
                                  int         selectionId);

    template <class TYPE>
    int bdlat_choiceSelectionId(const TYPE& object);

}  // close namespace bdlat_ChoiceFunctions

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                      // -------------------------------
                      // namespace bdlat_ChoiceFunctions
                      // -------------------------------

// MANIPULATORS
template <class TYPE>
inline
int bdlat_ChoiceFunctions::makeSelection(TYPE *object, int selectionId)
{
    return bdlat_choiceMakeSelection(object, selectionId);
}

template <class TYPE>
inline
int bdlat_ChoiceFunctions::makeSelection(TYPE       *object,
                                         const char *selectionName,
                                         int         selectionNameLength)
{
    return bdlat_choiceMakeSelection(object,
                                     selectionName,
                                     selectionNameLength);
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_ChoiceFunctions::manipulateSelection(TYPE         *object,
                                               MANIPULATOR&  manipulator)
{
    BSLS_ASSERT_SAFE(k_UNDEFINED_SELECTION_ID
                                          != bdlat_choiceSelectionId(*object));

    return bdlat_choiceManipulateSelection(object, manipulator);
}

// ACCESSORS
template <class TYPE, class ACCESSOR>
inline
int bdlat_ChoiceFunctions::accessSelection(const TYPE& object,
                                           ACCESSOR&   accessor)
{
    BSLS_ASSERT_SAFE(k_UNDEFINED_SELECTION_ID
                                           != bdlat_choiceSelectionId(object));

    return bdlat_choiceAccessSelection(object, accessor);
}

template <class TYPE>
inline
bool bdlat_ChoiceFunctions::hasSelection(const TYPE&  object,
                                         const char  *selectionName,
                                         int          selectionNameLength)
{
    return bdlat_choiceHasSelection(object,
                                    selectionName,
                                    selectionNameLength);
}

template <class TYPE>
inline
bool bdlat_ChoiceFunctions::hasSelection(const TYPE& object,
                                         int         selectionId)
{
    return bdlat_choiceHasSelection(object, selectionId);
}

template <class TYPE>
inline
int bdlat_ChoiceFunctions::selectionId(const TYPE& object)
{
    return bdlat_choiceSelectionId(object);
}

                            // -------------------
                            // default definitions
                            // -------------------

// MANIPULATORS
template <class TYPE>
inline
int bdlat_ChoiceFunctions::bdlat_choiceMakeSelection(TYPE *object,
                                                     int   selectionId)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicChoice>::value));

    return object->makeSelection(selectionId);
}

template <class TYPE>
inline
int bdlat_ChoiceFunctions::bdlat_choiceMakeSelection(
                                               TYPE       *object,
                                               const char *selectionName,
                                               int         selectionNameLength)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicChoice>::value));

    return object->makeSelection(selectionName, selectionNameLength);
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_ChoiceFunctions::bdlat_choiceManipulateSelection(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicChoice>::value));

    return object->manipulateSelection(manipulator);
}

// ACCESSORS
template <class TYPE, class ACCESSOR>
inline
int bdlat_ChoiceFunctions::bdlat_choiceAccessSelection(const TYPE& object,
                                                       ACCESSOR&   accessor)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicChoice>::value));

    return object.accessSelection(accessor);
}

template <class TYPE>
inline
bool bdlat_ChoiceFunctions::bdlat_choiceHasSelection(
                                              const TYPE&  object,
                                              const char  *selectionName,
                                              int          selectionNameLength)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicChoice>::value));

    return 0 != object.lookupSelectionInfo(selectionName, selectionNameLength);
}

template <class TYPE>
inline
bool bdlat_ChoiceFunctions::bdlat_choiceHasSelection(const TYPE& object,
                                                     int         selectionId)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicChoice>::value));

    return 0 != object.lookupSelectionInfo(selectionId);
}

template <class TYPE>
inline
int bdlat_ChoiceFunctions::bdlat_choiceSelectionId(const TYPE& object)
{
    BSLMF_ASSERT((bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicChoice>::value));

    return object.selectionId();
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
