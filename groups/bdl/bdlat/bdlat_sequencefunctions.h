// bdlat_sequencefunctions.h                                          -*-C++-*-
#ifndef INCLUDED_BDLAT_SEQUENCEFUNCTIONS
#define INCLUDED_BDLAT_SEQUENCEFUNCTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace defining sequence functions.
//
//@CLASSES:
//  bdlat_SequenceFunctions: namespace for calling sequence functions
//
//@SEE_ALSO: bdlat_attributeinfo
//
//@DESCRIPTION: The `bdlat_SequenceFunctions` `namespace` provided in this
// component defines parameterized functions that expose "sequence" behavior
// for "sequence" types.  See the `bdlat` package-level documentation for a
// brief description of "sequence" types.
//
// The functions in this namespace allow users to:
// * manipulate an attribute by attribute id or attribute name using a
//   parameterized manipulator (`manipulateAttribute`).
// * manipulate all attributes sequentially using a parameterized manipulator
//   (`manipulateAttributes`).
// * access an attribute by attribute id or attribute name using a
//   parameterized accessor (`accessAttribute`).
// * access all attributes sequentially using a parameterized accessor
//   (`accessAttributes`).
//
// Also, the meta-function `IsSequence` contains a compile-time constant
// `value` that is non-zero if the parameterized `TYPE` exposes "sequence"
// behavior through the `bdlat_SequenceFunctions` `namespace`.
//
// This component specializes all of these functions for types that have the
// `bdlat_TypeTraitBasicSequence` trait.
//
// Types that do not have the `bdlat_TypeTraitBasicSequence` trait can be
// plugged into the `bdlat` framework.  This is done by overloading the
// `bdlat_sequence*` functions inside the namespace of the plugged in type.
// Note that the placeholder `YOUR_TYPE` is not a template argument and should
// be replaced with the name of the type being plugged into the framework.
// ```
// // MANIPULATORS
//
// /// Invoke the specified `manipulator` on the address of the (modifiable)
// /// attribute indicated by the specified `attributeName` and
// /// `attributeNameLength` of the specified `object`, supplying `manipulator`
// /// with the corresponding attribute information structure.  The supplied
// /// `manipulator` must be a callable type that can be called as if it had
// /// the following signature:
// /// ```
// /// template <class t_INFO>
// /// int manipulator(ATTRIBUTE_TYPE *attribute, const t_INFO& info);
// /// ```
// /// Return non-zero value if the attribute is not found, and the value
// /// returned from the invocation of `manipulator` otherwise.
// template <typename MANIPULATOR>
// int bdlat_sequenceManipulateAttribute(YOUR_TYPE    *object,
//                                       MANIPULATOR&  manipulator,
//                                       const char   *attributeName,
//                                       int           attributeNameLength);
//
// /// Invoke the specified `manipulator` on the address of the (modifiable)
// /// attribute indicated by the specified `attributeId` of the specified
// /// `object`, supplying `manipulator` with the corresponding attribute
// /// information structure.  The supplied `manipulator` must be a callable
// /// type that can be called as if it had the following signature:
// /// ```
// /// template <class t_INFO>
// /// int manipulator(ATTRIBUTE_TYPE *attribute, const t_INFO& info);
// /// ```
// /// Return non-zero value if the attribute is not found, and the value
// /// returned from the invocation of `manipulator` otherwise.
// template <typename MANIPULATOR>
// int bdlat_sequenceManipulateAttribute(YOUR_TYPE    *object,
//                                       MANIPULATOR&  manipulator,
//                                       int           attributeId);
//
// /// Invoke the specified `manipulator` sequentially on the address of each
// /// (modifiable) attribute of the specified `object`, supplying
// /// `manipulator` with the corresponding attribute information structure
// /// until such invocation returns non-zero value.  The supplied
// /// `manipulator` must be a callable type that can be called as if it had
// /// the following signature:
// /// ```
// /// template <class t_INFO>
// /// int manipulator(ATTRIBUTE_TYPE *attribute, const t_INFO& info);
// /// ```
// /// Return the value from the last invocation of `manipulator` (i.e., the
// /// invocation that terminated the sequence).
// template <typename MANIPULATOR>
// int bdlat_sequenceManipulateAttributes(YOUR_TYPE    *object,
//                                        MANIPULATOR&  manipulator);
//
// // ACCESSORS
//
// /// Invoke the specified `accessor` on the (non-modifiable) attribute of the
// /// specified `object` indicated by the specified `attributeName` and
// /// `attributeNameLength`, supplying `accessor` with the corresponding
// /// attribute information structure.  The supplied `accessor` must be a
// /// callable type that can be called as if it had the following signature:
// /// ```
// /// template <class t_INFO>
// /// int accessor(const ATTRIBUTE_TYPE& attribute, const t_INFO& info);
// /// ```
// /// Return non-zero value if the attribute is not found, and the value
// /// returned from the invocation of `accessor` otherwise.
// template <typename ACCESSOR>
// int bdlat_sequenceAccessAttribute(const YOUR_TYPE&  object,
//                                   ACCESSOR&         accessor,
//                                   const char       *attributeName,
//                                   int               attributeNameLength);
//
// /// Invoke the specified `accessor` on the attribute of the specified
// /// `object` with the given `attributeId`, supplying `accessor` with the
// /// corresponding attribute information structure.  The supplied `accessor`
// /// must be a callable type that can be called as if it had the following
// /// signature:
// /// ```
// /// template <class t_INFO>
// /// int accessor(const ATTRIBUTE_TYPE& attribute, const t_INFO& info);
// /// ```
// /// Return non-zero if the attribute is not found, and the value returned
// /// from the invocation of `accessor` otherwise.
// template <typename ACCESSOR>
// int bdlat_sequenceAccessAttribute(const YOUR_TYPE& object,
//                                   ACCESSOR&        accessor,
//                                   int              attributeId);
//
// /// Invoke the specified `accessor` sequentially on each attribute of the
// /// specified `object`, supplying `accessor` with the corresponding
// /// attribute information structure until such invocation returns a non-zero
// /// value.  The supplied `accessor` must be a callable type that can be
// /// called as if it had the following signature:
// /// ```
// /// template <class t_INFO>
// /// int accessor(const ATTRIBUTE_TYPE& attribute, const t_INFO& info);
// /// ```
// /// Return the value from the last invocation of `accessor` (i.e., the
// /// invocation that terminated the sequence).
// template <typename ACCESSOR>
// int bdlat_sequenceAccessAttributes(const YOUR_TYPE& object,
//                                    ACCESSOR&        accessor);
//
// /// Return true if the specified `object` has an attribute with the
// /// specified `attributeName` of the specified `attributeNameLength`, and
// /// false otherwise.
// bool bdlat_sequenceHasAttribute(const YOUR_TYPE&  object,
//                                 const char       *attributeName,
//                                 int               attributeNameLength);
//
// /// Return true if the specified `object` has an attribute with the
// /// specified `attributeId`, and false otherwise.
// bool bdlat_sequenceHasAttribute(const YOUR_TYPE& object,
//                                 int              attributeId);
// ```
// Also, the `IsSequence` meta-function must be specialized for the
// `mine::MySequence` type in the `bdlat_SequenceFunctions` namespace.
//
// An example of plugging in a user-defined sequence type into the `bdlat`
// framework is shown in the [](#Usage) section of this document.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// Suppose you had a `struct` that contains three members:
// ```
// namespace BloombergLP {
//
// namespace mine {
//
// /// This struct represents a sequence containing a `string` member, an `int`
// /// member, and a `float` member.
// struct MySequence {
//     // CONSTANTS
//     enum {
//         NAME_ATTRIBUTE_ID   = 1,
//         AGE_ATTRIBUTE_ID    = 2,
//         SALARY_ATTRIBUTE_ID = 3
//     };
//
//     // DATA MEMBERS
//     bsl::string d_name;
//     int         d_age;
//     float       d_salary;
// };
//
// }  // close namespace mine
// ```
// We can now make `mine::MySequence` expose "sequence" behavior by
// implementing the necessary `bdlat_sequence*` functions for `MySequence`
// inside the `mine` namespace.  First, we should forward declare all the
// functions that we will implement inside the `mine` namespace:
// ```
// namespace mine {
//
// template <class MANIPULATOR>
// int bdlat_sequenceManipulateAttribute(MySequence   *object,
//                                       MANIPULATOR&  manipulator,
//                                       const char   *attributeName,
//                                       int           attributeNameLength);
// template <class MANIPULATOR>
// int bdlat_sequenceManipulateAttribute(MySequence   *object,
//                                       MANIPULATOR&  manipulator,
//                                       int           attributeId);
// template <class MANIPULATOR>
// int bdlat_sequenceManipulateAttributes(MySequence   *object,
//                                        MANIPULATOR&  manipulator);
// template <class ACCESSOR>
// int bdlat_sequenceAccessAttribute(const MySequence&  object,
//                                   ACCESSOR&          accessor,
//                                   const char        *attributeName,
//                                   int                attributeNameLength);
// template <class ACCESSOR>
// int bdlat_sequenceAccessAttribute(const MySequence& object,
//                                   ACCESSOR&         accessor,
//                                   int               attributeId);
// template <class ACCESSOR>
// int bdlat_sequenceAccessAttributes(const MySequence& object,
//                                    ACCESSOR&         accessor);
// bool bdlat_sequenceHasAttribute(const MySequence&  object,
//                                 const char        *attributeName,
//                                 int                attributeNameLength);
// bool bdlat_sequenceHasAttribute(const MySequence& object,
//                                 int               attributeId);
//
// }  // close namespace mine
// ```
// Now, we will implement these functions.  Note that for this implementation,
// we will create a temporary `bdlat_AttributeInfo` object and pass it along
// when invoking the manipulator or accessor.  See the {`bdlat_attributeinfo`}
// component-level documentation for more information.  The implementation of
// the functions are as follows:
// ```
// template <class MANIPULATOR>
// int mine::bdlat_sequenceManipulateAttribute(
//                                       MySequence   *object,
//                                       MANIPULATOR&  manipulator,
//                                       const char   *attributeName,
//                                       int           attributeNameLength)
// {
//     enum { NOT_FOUND = -1 };
//
//     if (bdlb::String::areEqualCaseless("name",
//                                        attributeName,
//                                        attributeNameLength)) {
//         return bdlat_sequenceManipulateAttribute(
//                                             object,
//                                             manipulator,
//                                             MySequence::NAME_ATTRIBUTE_ID);
//                                                                   // RETURN
//     }
//
//     if (bdlb::String::areEqualCaseless("age",
//                                        attributeName,
//                                        attributeNameLength)) {
//         return bdlat_sequenceManipulateAttribute(
//                                              object,
//                                              manipulator,
//                                              MySequence::AGE_ATTRIBUTE_ID);
//                                                                   // RETURN
//     }
//
//     if (bdlb::String::areEqualCaseless("salary",
//                                        attributeName,
//                                        attributeNameLength)) {
//         return bdlat_sequenceManipulateAttribute(
//                                           object,
//                                           manipulator,
//                                           MySequence::SALARY_ATTRIBUTE_ID);
//                                                                   // RETURN
//     }
//
//     return NOT_FOUND;
// }
//
// template <class MANIPULATOR>
// int mine::bdlat_sequenceManipulateAttribute(MySequence   *object,
//                                             MANIPULATOR&  manipulator,
//                                             int           attributeId)
// {
//     enum { NOT_FOUND = -1 };
//
//     switch (attributeId) {
//       case MySequence::NAME_ATTRIBUTE_ID: {
//         bdlat_AttributeInfo info;
//
//         info.annotation()     = "Name of employee";
//         info.formattingMode() = bdlat_FormattingMode::e_DEFAULT;
//         info.id()             = MySequence::NAME_ATTRIBUTE_ID;
//         info.name()           = "name";
//         info.nameLength()     = 4;
//
//         return manipulator(&object->d_name, info);                // RETURN
//       }
//       case MySequence::AGE_ATTRIBUTE_ID: {
//         bdlat_AttributeInfo info;
//
//         info.annotation()     = "Age of employee";
//         info.formattingMode() = bdlat_FormattingMode::e_DEFAULT;
//         info.id()             = MySequence::AGE_ATTRIBUTE_ID;
//         info.name()           = "age";
//         info.nameLength()     = 3;
//
//         return manipulator(&object->d_age, info);                 // RETURN
//       }
//       case MySequence::SALARY_ATTRIBUTE_ID: {
//         bdlat_AttributeInfo info;
//
//         info.annotation()     = "Salary of employee";
//         info.formattingMode() = bdlat_FormattingMode::e_DEFAULT;
//         info.id()             = MySequence::SALARY_ATTRIBUTE_ID;
//         info.name()           = "salary";
//         info.nameLength()     = 6;
//
//         return manipulator(&object->d_salary, info);              // RETURN
//       }
//       default: {
//         return NOT_FOUND;                                         // RETURN
//       }
//     }
// }
//
// template <class MANIPULATOR>
// int mine::bdlat_sequenceManipulateAttributes(MySequence   *object,
//                                              MANIPULATOR&  manipulator)
// {
//     int retVal;
//
//     retVal = bdlat_sequenceManipulateAttribute(
//                                             object,
//                                             manipulator,
//                                             MySequence::NAME_ATTRIBUTE_ID);
//
//     if (0 != retVal) {
//         return retVal;                                            // RETURN
//     }
//
//     retVal = bdlat_sequenceManipulateAttribute(
//                                              object,
//                                              manipulator,
//                                              MySequence::AGE_ATTRIBUTE_ID);
//
//     if (0 != retVal) {
//         return retVal;                                            // RETURN
//     }
//
//     retVal = bdlat_sequenceManipulateAttribute(
//                                           object,
//                                           manipulator,
//                                           MySequence::SALARY_ATTRIBUTE_ID);
//
//     return retVal;
// }
//
// // ACCESSORS
//
// template <class ACCESSOR>
// int mine::bdlat_sequenceAccessAttribute(
//                                     const MySequence&  object,
//                                     ACCESSOR&          accessor,
//                                     const char        *attributeName,
//                                     int                attributeNameLength)
// {
//     enum { NOT_FOUND = -1 };
//
//     if (bdlb::String::areEqualCaseless("name",
//                                        attributeName,
//                                        attributeNameLength)) {
//         return bdlat_sequenceAccessAttribute(
//                                             object,
//                                             accessor,
//                                             MySequence::NAME_ATTRIBUTE_ID);
//                                                                   // RETURN
//     }
//
//     if (bdlb::String::areEqualCaseless("age",
//                                        attributeName,
//                                        attributeNameLength)) {
//         return bdlat_sequenceAccessAttribute(object,
//                                              accessor,
//                                              MySequence::AGE_ATTRIBUTE_ID);
//                                                                   // RETURN
//     }
//
//     if (bdlb::String::areEqualCaseless("salary",
//                                        attributeName,
//                                        attributeNameLength)) {
//         return bdlat_sequenceAccessAttribute(
//                                           object,
//                                           accessor,
//                                           MySequence::SALARY_ATTRIBUTE_ID);
//                                                                   // RETURN
//     }
//
//     return NOT_FOUND;
// }
//
// template <class ACCESSOR>
// int mine::bdlat_sequenceAccessAttribute(const MySequence& object,
//                                         ACCESSOR&         accessor,
//                                         int               attributeId)
// {
//     enum { NOT_FOUND = -1 };
//
//     switch (attributeId) {
//       case MySequence::NAME_ATTRIBUTE_ID: {
//         bdlat_AttributeInfo info;
//
//         info.annotation()     = "Name of employee";
//         info.formattingMode() = bdlat_FormattingMode::e_DEFAULT;
//         info.id()             = MySequence::NAME_ATTRIBUTE_ID;
//         info.name()           = "name";
//         info.nameLength()     = 4;
//
//         return accessor(object.d_name, info);                     // RETURN
//       }
//       case MySequence::AGE_ATTRIBUTE_ID: {
//         bdlat_AttributeInfo info;
//
//         info.annotation()     = "Age of employee";
//         info.formattingMode() = bdlat_FormattingMode::e_DEFAULT;
//         info.id()             = MySequence::AGE_ATTRIBUTE_ID;
//         info.name()           = "age";
//         info.nameLength()     = 3;
//
//         return accessor(object.d_age, info);                      // RETURN
//       }
//       case MySequence::SALARY_ATTRIBUTE_ID: {
//         bdlat_AttributeInfo info;
//
//         info.annotation()     = "Salary of employee";
//         info.formattingMode() = bdlat_FormattingMode::e_DEFAULT;
//         info.id()             = MySequence::SALARY_ATTRIBUTE_ID;
//         info.name()           = "salary";
//         info.nameLength()     = 6;
//
//         return accessor(object.d_salary, info);                   // RETURN
//       }
//       default: {
//       return NOT_FOUND;                                           // RETURN
//       }
//     }
// }
//
// template <class ACCESSOR>
// int mine::bdlat_sequenceAccessAttributes(const MySequence& object,
//                                          ACCESSOR&         accessor)
// {
//     int retVal;
//
//     retVal = bdlat_sequenceAccessAttribute(object,
//                                            accessor,
//                                            MySequence::NAME_ATTRIBUTE_ID);
//
//     if (0 != retVal) {
//     return retVal;                                                // RETURN
//     }
//
//     retVal = bdlat_sequenceAccessAttribute(object,
//                                            accessor,
//                                            MySequence::AGE_ATTRIBUTE_ID);
//
//     if (0 != retVal) {
//         return retVal;                                            // RETURN
//     }
//
//     retVal = bdlat_sequenceAccessAttribute(
//                                           object,
//                                           accessor,
//                                           MySequence::SALARY_ATTRIBUTE_ID);
//
//     return retVal;
// }
//
// bool mine::bdlat_sequenceHasAttribute(
//                                     const MySequence&  ,
//                                     const char        *attributeName,
//                                     int                attributeNameLength)
// {
//     return bdlb::String::areEqualCaseless("name",
//                                           attributeName,
//                                           attributeNameLength)
//         || bdlb::String::areEqualCaseless("age",
//                                           attributeName,
//                                           attributeNameLength)
//         || bdlb::String::areEqualCaseless("salary",
//                                           attributeName,
//                                           attributeNameLength);
// }
//
// bool mine::bdlat_sequenceHasAttribute(const MySequence& , int attributeId)
// {
//     return MySequence::NAME_ATTRIBUTE_ID   == attributeId
//         || MySequence::AGE_ATTRIBUTE_ID    == attributeId
//         || MySequence::SALARY_ATTRIBUTE_ID == attributeId;
// }
// ```
// Finally, we need to specialize the `IsSequence` meta-function in the
// `bdlat_SequenceFunctions` namespace for the `mine::MySequence` type.  This
// makes the `bdlat` infrastructure recognize `mine::MySequence` as a sequence
// abstraction:
// ```
// namespace bdlat_SequenceFunctions {
//
// template <>
// struct IsSequence<mine::MySequence> : bsl::true_type {
// };
//
// }  // close namespace bdlat_SequenceFunctions
// }  // close enterprise namespace
// ```
// The `bdlat` infrastructure (and any component that uses this infrastructure)
// will now recognize `mine::MySequence` as a "sequence" type.  For example,
// suppose we have the following XML data:
// ```
// <?xml version='1.0' encoding='UTF-8' ?>
// <MySequence>
//     <name>John Doe</name>
//     <age>29</age>
//     <salary>12345.00</salary>
// </MySequence>
// ```
// Using the `balxml_decoder` component, we can now load this XML data into a
// `mine::MySequence` object:
// ```
// #include <balxml_decoder.h>
//
// void decodeMySequenceFromXML(bsl::istream& inputData)
// {
//     mine::MySequence object;
//
//     balxml::DecoderOptions options;
//     balxml::MiniReader     reader;
//     balxml::ErrorInfo      errInfo;
//
//     balxml::Decoder decoder(&options, &reader, &errInfo);
//     int result = decoder.decode(inputData, &object);
//
//     assert(0          == result);
//     assert("John Doe" == object.d_name);
//     assert(29         == object.d_age);
//     assert(12345.00   == object.d_salary);
// }
// ```
// Note that the `bdlat` framework can be used for functionality other than
// encoding/decoding into XML.  When `mine::MySequence` is plugged into the
// framework, then it will be automatically usable within the framework.  For
// example, the following snippets of code will print out all the attributes of
// a sequence object:
// ```
// /// Print each visited object to the bound `d_stream_p` object.
// struct PrintAttribute {
//     // DATA MEMBERS
//     bsl::ostream *d_stream_p;
//
//     template <class TYPE, class INFO>
//     int operator()(const TYPE& object, const INFO& info)
//     {
//         (*d_stream_p) << info.name() << ": " << object << bsl::endl;
//         return 0;
//     }
// };
//
// template <class TYPE>
// void printSequenceAttributes(bsl::ostream& stream, const TYPE& object)
// {
//     PrintAttribute accessor;
//     accessor.d_stream_p = &stream;
//
//     bdlat_SequenceFunctions::accessAttributes(object, accessor);
// }
// ```
// Now we have a generic function that takes an output stream and a sequence
// object, and prints out each attribute with its name and value.  We can use
// this generic function as follows:
// ```
// void printMySequence(bsl::ostream& stream)
// {
//     mine::MySequence object;
//
//     object.d_name   = "John Doe";
//     object.d_age    = 25;
//     object.d_salary = 12345.00;
//
//     stream << bsl::fixed << bsl::setprecision(2);
//
//     printSequenceAttributes(stream, object);
// }
// ```
// The function above will print the following to provided stream:
// ```
// name: John Doe
// age: 25
// salary: 12345.00
// ```

#include <bdlscm_version.h>

#include <bdlat_bdeatoverrides.h>
#include <bdlat_typetraits.h>

#include <bslalg_hastrait.h>

#include <bslmf_assert.h>
#include <bslmf_integralconstant.h>
#include <bslmf_matchanytype.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

namespace BloombergLP {

                      // =================================
                      // namespace bdlat_SequenceFunctions
                      // =================================

/// This `namespace` provides methods that expose "sequence" behavior for
/// "sequence" types.  See the component-level documentation for more
/// information.
namespace bdlat_SequenceFunctions {
    // META-FUNCTIONS

    /// This `struct` should be specialized for third-party types that need
    /// to expose "sequence" behavior.  See the component-level
    /// documentation for further information.
    template <class TYPE>
    struct IsSequence
    : public bsl::integral_constant<
        bool,
        bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicSequence>::value> {
    };

    // MANIPULATORS

    /// Invoke the specified `manipulator` on the address of the (modifiable)
    /// attribute indicated by the specified `attributeName` and
    /// `attributeNameLength` of the specified `object`, supplying
    /// `manipulator` with the corresponding attribute information structure.
    /// The supplied `manipulator` must be a callable type that can be called
    /// as if it had the following signature:
    /// ```
    /// template <class t_INFO>
    /// int manipulator(ATTRIBUTE_TYPE *attribute, const t_INFO& info);
    /// ```
    /// Return non-zero value if the attribute is not found, and the value
    /// returned from the invocation of `manipulator` otherwise.
    template <class TYPE, class MANIPULATOR>
    int manipulateAttribute(TYPE         *object,
                            MANIPULATOR&  manipulator,
                            const char   *attributeName,
                            int           attributeNameLength);

    /// Invoke the specified `manipulator` on the address of the (modifiable)
    /// attribute indicated by the specified `attributeId` of the specified
    /// `object`, supplying `manipulator` with the corresponding attribute
    /// information structure.  The supplied `manipulator` must be a callable
    /// type that can be called as if it had the following signature:
    /// ```
    /// template <class t_INFO>
    /// int manipulator(ATTRIBUTE_TYPE *attribute, const t_INFO& info);
    /// ```
    /// Return non-zero value if the attribute is not found, and the value
    /// returned from the invocation of `manipulator` otherwise.
    template <class TYPE, class MANIPULATOR>
    int manipulateAttribute(TYPE         *object,
                            MANIPULATOR&  manipulator,
                            int           attributeId);

    /// Invoke the specified `manipulator` sequentially on the address of each
    /// (modifiable) attribute of the specified `object`, supplying
    /// `manipulator` with the corresponding attribute information structure
    /// until such invocation returns non-zero value.  The supplied
    /// `manipulator` must be a callable type that can be called as if it had
    /// the following signature:
    /// ```
    /// template <class t_INFO>
    /// int manipulator(ATTRIBUTE_TYPE *attribute, const t_INFO& info);
    /// ```
    /// Return the value from the last invocation of `manipulator` (i.e., the
    /// invocation that terminated the sequence).
    template <class TYPE, class MANIPULATOR>
    int manipulateAttributes(TYPE *object, MANIPULATOR& manipulator);

    // ACCESSORS

    /// Invoke the specified `accessor` on the (non-modifiable) attribute of
    /// the specified `object` indicated by the specified `attributeName` and
    /// `attributeNameLength`, supplying `accessor` with the corresponding
    /// attribute information structure.  The supplied `accessor` must be a
    /// callable type that can be called as if it had the following signature:
    /// ```
    /// template <class t_INFO>
    /// int accessor(const ATTRIBUTE_TYPE& attribute, const t_INFO& info);
    /// ```
    /// Return non-zero value if the attribute is not found, and the value
    /// returned from the invocation of `accessor` otherwise.
    template <class TYPE, class ACCESSOR>
    int accessAttribute(const TYPE&  object,
                        ACCESSOR&    accessor,
                        const char  *attributeName,
                        int          attributeNameLength);

    /// Invoke the specified `accessor` on the attribute of the specified
    /// `object` with the given `attributeId`, supplying `accessor` with the
    /// corresponding attribute information structure.  The supplied `accessor`
    /// must be a callable type that can be called as if it had the following
    /// signature:
    /// ```
    /// template <class t_INFO>
    /// int accessor(const ATTRIBUTE_TYPE& attribute, const t_INFO& info);
    /// ```
    /// Return non-zero if the attribute is not found, and the value returned
    /// from the invocation of `accessor` otherwise.
    template <class TYPE, class ACCESSOR>
    int accessAttribute(const TYPE& object,
                        ACCESSOR&   accessor,
                        int         attributeId);

    /// Invoke the specified `accessor` sequentially on each attribute of
    /// the specified `object`, supplying `accessor` with the corresponding
    /// attribute information structure until such invocation returns a
    /// non-zero value.  The supplied `accessor` must be a callable type that
    /// can be called as if it had the following signature:
    /// ```
    /// template <class t_INFO>
    /// int accessor(const ATTRIBUTE_TYPE& attribute, const t_INFO& info);
    /// ```
    /// Return the value from the last invocation of `accessor` (i.e., the
    /// invocation that terminated the sequence).
    template <class TYPE, class ACCESSOR>
    int accessAttributes(const TYPE& object, ACCESSOR& accessor);

    /// Return true if the specified `object` has an attribute with the
    /// specified `attributeName` of the specified `attributeNameLength`,
    /// and false otherwise.
    template <class TYPE>
    bool hasAttribute(const TYPE&  object,
                      const char  *attributeName,
                      int          attributeNameLength);

    /// Return true if the specified `object` has an attribute with the
    /// specified `attributeId`, and false otherwise.
    template <class TYPE>
    bool hasAttribute(const TYPE& object, int attributeId);

}  // close namespace bdlat_SequenceFunctions

                            // ====================
                            // default declarations
                            // ====================

/// This namespace declaration adds the default implementations of the
/// "sequence" customization-point functions to `bdlat_SequenceFunctions`.
/// These default implementations assume the type of the acted-upon object is a
/// basic-sequence type.  For more information about basic-sequence types, see
/// `bdlat_typetraits`.
namespace bdlat_SequenceFunctions {
    // MANIPULATORS
    template <class TYPE, class MANIPULATOR>
    int bdlat_sequenceManipulateAttribute(TYPE         *object,
                                          MANIPULATOR&  manipulator,
                                          const char   *attributeName,
                                          int           attributeNameLength);

    template <class TYPE, class MANIPULATOR>
    int bdlat_sequenceManipulateAttribute(TYPE         *object,
                                          MANIPULATOR&  manipulator,
                                          int           attributeId);

    template <class TYPE, class MANIPULATOR>
    int bdlat_sequenceManipulateAttributes(TYPE         *object,
                                           MANIPULATOR&  manipulator);

    // ACCESSORS
    template <class TYPE, class ACCESSOR>
    int bdlat_sequenceAccessAttribute(const TYPE&  object,
                                      ACCESSOR&    accessor,
                                      const char  *attributeName,
                                      int          attributeNameLength);

    template <class TYPE, class ACCESSOR>
    int bdlat_sequenceAccessAttribute(const TYPE& object,
                                      ACCESSOR&   accessor,
                                      int         attributeId);

    template <class TYPE, class ACCESSOR>
    int bdlat_sequenceAccessAttributes(const TYPE& object, ACCESSOR& accessor);

    template <class TYPE>
    bool bdlat_sequenceHasAttribute(const TYPE&  object,
                                    const char  *attributeName,
                                    int          attributeNameLength);

    template <class TYPE>
    bool bdlat_sequenceHasAttribute(const TYPE& object,
                                    int         attributeId);

}  // close namespace bdlat_SequenceFunctions

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                     // ---------------------------------
                     // namespace bdlat_SequenceFunctions
                     // ---------------------------------

// MANIPULATORS
template <class TYPE, class MANIPULATOR>
inline
int bdlat_SequenceFunctions::manipulateAttribute(
                                             TYPE         *object,
                                             MANIPULATOR&  manipulator,
                                             const char   *attributeName,
                                             int           attributeNameLength)
{
    return bdlat_sequenceManipulateAttribute(object,
                                             manipulator,
                                             attributeName,
                                             attributeNameLength);
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_SequenceFunctions::manipulateAttribute(TYPE         *object,
                                                 MANIPULATOR&  manipulator,
                                                 int           attributeId)
{
    return bdlat_sequenceManipulateAttribute(object, manipulator, attributeId);
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_SequenceFunctions::manipulateAttributes(TYPE         *object,
                                                  MANIPULATOR&  manipulator)
{
    return bdlat_sequenceManipulateAttributes(object, manipulator);
}

// ACCESSORS
template <class TYPE, class ACCESSOR>
inline
int bdlat_SequenceFunctions::accessAttribute(const TYPE&  object,
                                             ACCESSOR&    accessor,
                                             const char  *attributeName,
                                             int          attributeNameLength)
{
    return bdlat_sequenceAccessAttribute(object,
                                         accessor,
                                         attributeName,
                                         attributeNameLength);
}

template <class TYPE, class ACCESSOR>
inline
int bdlat_SequenceFunctions::accessAttribute(const TYPE& object,
                                             ACCESSOR&   accessor,
                                             int         attributeId)
{
    return bdlat_sequenceAccessAttribute(object, accessor, attributeId);
}

template <class TYPE, class ACCESSOR>
inline
int bdlat_SequenceFunctions::accessAttributes(const TYPE& object,
                                              ACCESSOR&   accessor)
{
    return bdlat_sequenceAccessAttributes(object, accessor);
}

template <class TYPE>
inline
bool bdlat_SequenceFunctions::hasAttribute(const TYPE&  object,
                                           const char  *attributeName,
                                           int          attributeNameLength)
{
    return bdlat_sequenceHasAttribute(object,
                                      attributeName,
                                      attributeNameLength);
}

template <class TYPE>
inline
bool bdlat_SequenceFunctions::hasAttribute(const TYPE& object,
                                           int         attributeId)
{
    return bdlat_sequenceHasAttribute(object, attributeId);
}


                            // -------------------
                            // default definitions
                            // -------------------

// MANIPULATORS
template <class TYPE, class MANIPULATOR>
inline
int bdlat_SequenceFunctions::bdlat_sequenceManipulateAttribute(
                                             TYPE         *object,
                                             MANIPULATOR&  manipulator,
                                             const char   *attributeName,
                                             int           attributeNameLength)
{
    BSLMF_ASSERT(
                (bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicSequence>::value));

    return object->manipulateAttribute(manipulator,
                                       attributeName,
                                       attributeNameLength);
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_SequenceFunctions::bdlat_sequenceManipulateAttribute(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator,
                                                     int           attributeId)
{
    BSLMF_ASSERT(
                (bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicSequence>::value));

    return object->manipulateAttribute(manipulator, attributeId);
}

template <class TYPE, class MANIPULATOR>
inline
int bdlat_SequenceFunctions::bdlat_sequenceManipulateAttributes(
                                                     TYPE         *object,
                                                     MANIPULATOR&  manipulator)
{
    BSLMF_ASSERT(
                (bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicSequence>::value));

    return object->manipulateAttributes(manipulator);
}

// ACCESSORS
template <class TYPE, class ACCESSOR>
inline
int bdlat_SequenceFunctions::bdlat_sequenceAccessAttribute(
                                              const TYPE&  object,
                                              ACCESSOR&    accessor,
                                              const char  *attributeName,
                                              int          attributeNameLength)
{
    BSLMF_ASSERT(
                (bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicSequence>::value));

    return object.accessAttribute(accessor,
                                  attributeName,
                                  attributeNameLength);
}

template <class TYPE, class ACCESSOR>
inline
int bdlat_SequenceFunctions::bdlat_sequenceAccessAttribute(
                                                       const TYPE& object,
                                                       ACCESSOR&   accessor,
                                                       int         attributeId)
{
    BSLMF_ASSERT(
                (bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicSequence>::value));

    return object.accessAttribute(accessor, attributeId);
}

template <class TYPE, class ACCESSOR>
inline
int bdlat_SequenceFunctions::bdlat_sequenceAccessAttributes(
                                                          const TYPE& object,
                                                          ACCESSOR&   accessor)
{
    BSLMF_ASSERT(
                (bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicSequence>::value));

    return object.accessAttributes(accessor);
}

template <class TYPE>
inline
bool bdlat_SequenceFunctions::bdlat_sequenceHasAttribute(
                                              const TYPE&  object,
                                              const char  *attributeName,
                                              int          attributeNameLength)
{
    BSLMF_ASSERT(
                (bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicSequence>::value));

    return 0 != object.lookupAttributeInfo(attributeName, attributeNameLength);
}

template <class TYPE>
inline
bool bdlat_SequenceFunctions::bdlat_sequenceHasAttribute(
                                                       const TYPE& object,
                                                       int         attributeId)
{
    BSLMF_ASSERT(
                (bslalg::HasTrait<TYPE, bdlat_TypeTraitBasicSequence>::value));

    return 0 != object.lookupAttributeInfo(attributeId);
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
