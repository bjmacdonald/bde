// bdlb_printmethods.h                                                -*-C++-*-
#ifndef INCLUDED_BDLB_PRINTMETHODS
#define INCLUDED_BDLB_PRINTMETHODS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide methods for uniform printing of value-semantic types.
//
//@CLASSES:
//  bdlb::PrintMethods: templates for uniform printing of value-semantic types
//  bdlb::HasPrintMethod: trait indicating existence of `print` method
//  bdlb::TypeTraitHasPrintMethod: old-style version of `bdlb::HasPrintMethod`
//
//@SEE_ALSO: bslalg_nestedtraitdeclaration
//
//@DESCRIPTION: This component provides a namespace for print utilities that
// support uniform `ostream` printing across all printable types, including
// template types and containers.  The `bdlb::PrintMethods` namespace enables
// clients to output the value of any printable object according to the
// standard BDE `print` protocol.  If the parameterized `TYPE` does not provide
// a `print` method, `TYPE::operator<<` is used.  Availability of a `print`
// method is determined by testing for the `bdlb::HasPrintMethod` and
// `bdlb::TypeTraitHasPrintMethod` traits.
//
///Traits Affecting Printing
///-------------------------
// By default, `bdlb::PrintMethods::print` uses the `<<` stream output operator
// to print a value.  This formats the entire output on one line, suppressing
// all indentation.  A class can override this behavior by declaring certain
// traits related to printing.  This component detects these traits and invokes
// an appropriate print operation.  The following lists the traits recognized
// by this component:
// ```
// bdlb::HasPrintMethod       ( highest precedence )
// bslalg::HasStlIterators
// bslmf::IsPair              ( lowest precedence  )
// ```
// Since a class may declare multiple traits (see the component-level
// documentation of {`bslalg_nestedtraitdeclaration`} for information about
// declaring traits), the relative precedence of the traits is shown above.
// The next sub-sections describe these traits and their effects on printing.
//
///Effect of `bdlb::TypeTraitHasPrintMethod` Trait
///- - - - - - - - - - - - - - - - - - - - - - - -
// If a class `X` declares the `bdlb::TypeTraitHasPrintMethod` trait, then it
// must provide a `print` method with the following signature:
// ```
// bsl::ostream& print(bsl::ostream& stream,
//                     int           level          = 0,
//                     int           spacesPerLevel = 4) const;
// ```
// To output an `X` object with this trait declared, the
// `bdlb::PrintMethods::print` method simply forwards to this method.  This
// means that the print operation is completely defined by the class.  Ideally,
// it should behave according to the standard BDE `print` protocol that is
// documented as follows:
// ```
// Format this object to the specified output `stream` at the (absolute value
// of) the optionally specified indentation `level` and return a reference to
// `stream`.  If `level` is specified, optionally specify `spacesPerLevel`,
// the number of spaces per indentation level for this and all of its nested
// objects.  If `level` is negative, suppress indentation of the first line.
// If `spacesPerLevel` is negative, format the entire output on one line,
// suppressing all but the initial indentation (as governed by `level`).  If
// `stream` is not valid on entry, this operation has no effect.
// ```
//
///Effect of `bslalg::HasStlIterators` Trait
///- - - - - - - - - - - - - - - - - - - - -
// If a class `X` declares the `bslalg::HasStlIterators` trait, then it must
// provide access to iterators using the standard STL protocol.  The BDE
// implementation of STL declares this trait for all STL container types that
// have STL iterators.  Other containers that provide STL iterators should
// declare this trait to get correct printing behavior.
//
// When an `X` object with this trait is printed using
// `bdlb::PrintMethods::print`, the contents of the object is traversed via an
// iterator and the output is formatted according to the standard BDE `print`
// protocol, as documented above.  Additionally, an opening `[` character is
// prepended at the beginning of the output and a closing `]` character is
// appended at the end of the output.  Each iterated element is printed using
// its own print method, and with an indentation level one higher than that of
// the container.
//
///Effect of `bslmf::IsPair` Trait
///- - - - - - - - - - - - - - - -
// If a class `X` declares the `bslmf::IsPair` trait, then the class must
// contain two `public` data members named `first` and `second`.  The BDE
// implementation of STL declares this trait for the `bsl::pair` `struct`.
// Other classes that have `public` `first` and `second` data members may
// declare this trait to get printing behavior similar to that of `bsl::pair`.
//
// When an `X` object with this trait is printed using
// `bdlb::PrintMethods::print`, its output is formatted based on the standard
// BDE `print` protocol, as documented above.  Additionally, an opening `[`
// character is prepended at the beginning of the output and a closing `]`
// character is appended at the end of the output.  The `first` and `second`
// elements are printed using their own `print` methods, and with an
// indentation level one higher than that of the pair object.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Supplying a `print` Method for a Parameterized Class
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we must create a value-semantic class that holds an object of
// parameterized `TYPE` and, per BDE convention for VSTs, provides a `print`
// method that shows the value in some human-readable format.
//
// First, we define the wrapper class:
// ```
// /// An example wrapper class for a `TYPE` object.
// template <class TYPE>
// class MyWrapper {
//
//   // PRIVATE DATA MEMBERS
//   TYPE d_obj;  // wrapped object
//
//   public:
//     // TRAITS
//     BSLMF_NESTED_TRAIT_DECLARATION(MyWrapper, bdlb::HasPrintMethod);
//
//     // CREATORS
//     MyWrapper(): d_obj() {};
//     MyWrapper(const TYPE& value) : d_obj(value) { }
//     // ... other constructors and destructor ...
//
//     // MANIPULATORS
//     // ... assignment operator, etc. ...
//
//     // ACCESSORS
//
//     /// Format the contained `TYPE` to the specified output `stream` at
//     /// the (absolute value of) the optionally specified indentation
//     /// `level` and return a reference to `stream`.  If `level` is
//     /// specified, optionally specify `spacesPerLevel`, the number of
//     /// spaces per indentation level for this and all of its nested
//     /// objects.  If `level` is negative, suppress indentation of the
//     /// first line.  If `spacesPerLevel` is negative, format the entire
//     /// output on one line, suppressing all but the initial indentation
//     /// (as governed by `level`).  If `stream` is not valid on entry,
//     /// this operation has no effect.
//     bsl::ostream& print(bsl::ostream& stream,
//                         int           level          = 0,
//                         int           spacesPerLevel = 4) const;
// };
// ```
// Now, we implement the `print` method of `MyWrapper` using the
// `bdlb::PrintMethods` utility.  Doing so gives us a method that produces
// results both when `TYPE` defines a `print` method and when it does not.  In
// the latter case `TYPE::operator<<` is used.
// ```
// template <class TYPE>
// bsl::ostream& MyWrapper<TYPE>::print(bsl::ostream& stream,
//                                      int           level,
//                                      int           spacesPerLevel) const
// {
//     return bdlb::PrintMethods::print(stream, d_obj, level, spacesPerLevel);
// }
// ```
// Finally, we exercise our `MyWrapper` class using several representative
// types, starting with `MyDate` (not shown) a class that implements a `print`
// method.
// ```
// static void usingMyWrapper()
// {
//     BSLMF_ASSERT(bdlb::HasPrintMethod<MyDate>::value);
//
//     MyDate            myDate;
//     MyWrapper<MyDate> myWrapperForMyDate(myDate);
//
//     BSLMF_ASSERT(!bdlb::HasPrintMethod<int>::value);
//
//     bsl::ostringstream oss1;
//     myWrapperForMyDate.print(oss1); // No problem expected since
//                                     // `bsls::TimeInterval` has a `print`
//                                     // method.
//     assert("01JAN0001\n" == oss1.str());
// ```
// Using an `int` type shows how `bdlb::PrintMethods::print` transparently
// handles types that do not provide `print` methods:
// ```
//     int            myInt = 123;
//     MyWrapper<int> myWrapperForInt(myInt);
//
//     bsl::ostringstream oss2;
//     myWrapperForInt.print(oss2);    // `int` has no `print` method.
//                                     // Problem?
//     assert("123\n" == oss2.str());  // No problem!
// ```
// Lastly, since `MyWrapper` itself is a type that implements `print` -- and
// sets the `bdlb::TypeTraitHasPrintMethod` trait -- one instance of the
// `MyWrapper` type can be wrapped by another.
// ```
//     BSLMF_ASSERT(bdlb::HasPrintMethod<MyWrapper<int> >::value);
//
//     MyWrapper<MyWrapper<int> > myWrappedWrapper;
//
//     bsl::ostringstream oss3;
//     myWrappedWrapper.print(oss3);
//     assert("0\n" == oss3.str());
// }
// ```
// See the {`bslmf_nestedtraitdeclaration`} component for more information
// about declaring traits for user-defined classes.

#include <bdlscm_version.h>

#include <bdlb_print.h>

#include <bslalg_hasstliterators.h>

#include <bslmf_ispair.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_selecttrait.h>

#include <bsls_libraryfeatures.h>

#include <bsl_iomanip.h>
#include <bsl_ostream.h>
#include <bsl_vector.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslalg_typetraithasstliterators.h>
#include <bslalg_typetraitpair.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
#include <optional>
#include <variant>
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

namespace bsl {

template <class CHAR_TYPE, class CHAR_TRAITS, class ALLOCATOR>
class basic_string;

}  // close namespace bsl

namespace BloombergLP {
namespace bdlb {

                           // =====================
                           // struct HasPrintMethod
                           // =====================

/// A class, `TYPE`, should specialize this trait to derive from `true_type`
/// if it has a `print` method with the following signature:
/// ```
/// bsl::ostream& print(bsl::ostream& stream,
///                     int           level          = 0,
///                     int           spacesPerLevel = 4) const;
/// ```
template <class TYPE>
struct HasPrintMethod :
        bslmf::DetectNestedTrait<TYPE, HasPrintMethod>::type {
};

                       // ==============================
                       // struct TypeTraitHasPrintMethod
                       // ==============================

/// A class should declare this trait if it has a `print` method with the
/// following signature:
/// ```
/// bsl::ostream& print(bsl::ostream& stream,
///                     int           level          = 0,
///                     int           spacesPerLevel = 4) const;
/// ```
struct TypeTraitHasPrintMethod {

    /// This class template ties the `bdlb::TypeTraitHasPrintMethod` trait
    /// tag to the `bdlb::HasPrintMethod` trait metafunction.
    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE, HasPrintMethod>
    {
    };

    template <class TYPE>
    struct Metafunction : HasPrintMethod<TYPE>::type { };
};

                        // ======================
                        // namespace PrintMethods
                        // ======================

/// This `namespace` contains parameterized `print` methods having the
/// standard BDE signature for such methods.
namespace PrintMethods {

/// Format the specified `object` to the specified output `stream` at the
/// (absolute value of) the optionally specified indentation `level` and
/// return a reference to `stream`.  If `level` is specified, optionally
/// specify `spacesPerLevel`, the number of spaces per indentation level for
/// this and all of its nested objects.  If `level` is negative, suppress
/// indentation of the first line.  If `spacesPerLevel` is negative, format
/// the entire output on one line, suppressing all but the initial
/// indentation (as governed by `level`).  If `stream` is not valid on
/// entry, this operation has no effect.
template <class TYPE>
bsl::ostream& print(bsl::ostream& stream,
                    const TYPE&   object,
                    int           level          = 0,
                    int           spacesPerLevel = 4);
bsl::ostream& print(bsl::ostream& stream,
                    char          object,
                    int           level          = 0,
                    int           spacesPerLevel = 4);
bsl::ostream& print(bsl::ostream& stream,
                    unsigned char object,
                    int           level          = 0,
                    int           spacesPerLevel = 4);

/// Format the specified `object` to the specified output `stream` at the
/// (absolute value of) the optionally specified indentation `level` and
/// return a reference to stream.  Note that output will be formatted on one
/// line.  If `stream` is not valid on entry, this operation has no effect.
template <class CHAR_T, class CHAR_TRAITS_T, class ALLOC>
bsl::ostream& print(bsl::ostream&                          stream,
                    const bsl::basic_string<CHAR_T,
                                            CHAR_TRAITS_T,
                                            ALLOC>&        object,
                    int                                    level          = 0,
                    int                                    spacesPerLevel = 4);

/// Format the specified `object` to the specified output `stream` at the
/// (absolute value of) the optionally specified indentation `level` and
/// return a reference to stream.  Note that output will be formatted on one
/// line.  Also note that non-printable characters in `object` will be
/// printed using their hexadecimal representation.  If `stream` is not
/// valid on entry, this operation has no effect.
template <class ALLOC>
bsl::ostream& print(bsl::ostream&                   stream,
                    const bsl::vector<char, ALLOC>& object,
                    int                             level          = 0,
                    int                             spacesPerLevel = 4);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

/// Format the specified `object` to the specified output `stream` at the
/// (absolute value of) the optionally specified indentation `level` and
/// return a reference to `stream`.  If `level` is specified, optionally
/// specify `spacesPerLevel`, the number of spaces per indentation level for
/// this and all of its nested objects.  If `level` is negative, suppress
/// indentation of the first line.  If `spacesPerLevel` is negative, format
/// the entire output on one line, suppressing all but the initial
/// indentation (as governed by `level`).  If `stream` is not valid on
/// entry, this operation has no effect.  A descriptive, human-readable
/// message, indented according to `level` and `spacesPerLevel`, is output
/// for objects having no value (i.e., `false == object.has_value()`).
template <class TYPE>
bsl::ostream& print(bsl::ostream&              stream,
                    const std::optional<TYPE>& object,
                    int                        level          = 0,
                    int                        spacesPerLevel = 4);

/// Format the specified `object` to the specified output `stream` at the
/// (absolute value of) the optionally specified indentation `level` and
/// return a reference to `stream`.  If `level` is specified, optionally
/// specify `spacesPerLevel`, the number of spaces per indentation level for
/// this and all of its nested objects.  If `level` is negative, suppress
/// indentation of the first line.  If `spacesPerLevel` is negative, format
/// the entire output on one line, suppressing all but the initial
/// indentation (as governed by `level`).  If `stream` is not valid on
/// entry, this operation has no effect.  A descriptive, human-readable
/// message, indented according to `level` and `spacesPerLevel`, is output
/// for objects holding the value of type `std::monostate`.  Note that a
/// `std::variant` object can hold the `std::monostate` value only if its
/// template parameters explicitly mention the `std::monostate` type.
template <class ... TYPE>
bsl::ostream& print(bsl::ostream&                 stream,
                    const std::variant<TYPE ...>& object,
                    int                           level          = 0,
                    int                           spacesPerLevel = 4);

/// Format the specified `object` to the specified output `stream` at the
/// (absolute value of) the optionally specified indentation `level` and
/// return a reference to `stream`.  If `level` is specified, optionally
/// specify `spacesPerLevel`, the number of spaces per indentation level for
/// this and all of its nested objects.  If `level` is negative, suppress
/// indentation of the first line.  If `spacesPerLevel` is negative, format
/// the entire output on one line, suppressing all but the initial
/// indentation (as governed by `level`).  If `stream` is not valid on
/// entry, this operation has no effect.  As all objects of this type have
/// the same value, `object` is *ignored* and a descriptive, human-readable
/// message is output for all objects of this type.
bsl::ostream& print(bsl::ostream&         stream,
                    const std::monostate& object,
                    int                   level          = 0,
                    int                   spacesPerLevel = 4);

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

}  // close namespace PrintMethods
}  // close package namespace

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

namespace bdlb {

                // --------------------------------------------
                // struct bdlb::PrintMethods_Imp<TYPE, SELECTOR>
                // --------------------------------------------

template <class TYPE, class SELECTOR>
struct PrintMethods_Imp;

/// Component-private `struct`.  Do not use outside of this component.  This
/// `struct` provides a `print` function that prints objects of
/// parameterized `TYPE` that do not declare any of the traits recognized by
/// this component.
template <class TYPE>
struct PrintMethods_Imp<TYPE, bslmf::SelectTraitCase<> > {

    // CLASS METHODS

    /// Print the specified `object` to the specified `stream` using its
    /// `<<` output stream operator.  Note that a compiler error will result
    /// if the specified `TYPE` does not have a `<<` output stream operator.
    static bsl::ostream& print(bsl::ostream& stream,
                               const TYPE&   object,
                               int           level,
                               int           spacesPerLevel);
};

              // ----------------------------------------------------
              // struct PrintMethods_Imp<TYPE, HasPrintMethod<TYPE> >
              // ----------------------------------------------------

/// Component-private `struct`.  Do not use outside of this component.  This
/// `struct` provides a `print` function that prints objects of the
/// parameterized `TYPE` that are associated with the `HasPrintMethod`
/// trait.
template <class TYPE>
struct PrintMethods_Imp<TYPE, bslmf::SelectTraitCase<HasPrintMethod> >
{

    // CLASS METHODS
    static bsl::ostream& print(bsl::ostream& stream,
                               const TYPE&   object,
                               int           level,
                               int           spacesPerLevel);
};

         // -------------------------------------------------------------
         // struct PrintMethods_Imp<TYPE, bslalg::HasStlIterators<TYPE> >
         // -------------------------------------------------------------

/// Component-private `struct`.  Do not use outside of this component.  This
/// `struct` provides a `print` function that prints objects of the
/// parameterized `TYPE` that have the `bslalg::HasStlIterators` trait
/// declared.
template <class TYPE>
struct PrintMethods_Imp<TYPE, bslmf::SelectTraitCase<bslalg::HasStlIterators> >
{

    // CLASS METHODS
    static bsl::ostream& print(bsl::ostream& stream,
                               const TYPE&   object,
                               int           level,
                               int           spacesPerLevel);
};

                  // ---------------------------------------------------
                  // struct PrintMethods_Imp<TYPE, bslmf::IsPair<TYPE> >
                  // ---------------------------------------------------

/// Component-private `struct`.  Do not use outside of this component.  This
/// `struct` provides a `print` function that prints objects of
/// parameterized `TYPE` that declare the `bslmf::IsPair` trait.
template <class TYPE>
struct PrintMethods_Imp<TYPE, bslmf::SelectTraitCase<bslmf::IsPair> >
{

    // CLASS METHODS
    static bsl::ostream& print(bsl::ostream& stream,
                               const TYPE&   object,
                               int           level,
                               int           spacesPerLevel);
};

// ============================================================================
//                 TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                // ---------------------------------------
                // struct PrintMethods_Imp<TYPE, SELECTOR>
                // ---------------------------------------

// CLASS METHODS
template <class TYPE>
bsl::ostream& PrintMethods_Imp<TYPE, bslmf::SelectTraitCase<> >::print(
                                                  bsl::ostream& stream,
                                                  const TYPE&   object,
                                                  int           level,
                                                  int           spacesPerLevel)
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    Print::indent(stream, level, spacesPerLevel);

    // A compilation error indicating the next line of code implies the `TYPE`
    // parameter does not have the `<<` output stream operator.

    stream << object;

    if (0 <= spacesPerLevel) {
        stream << '\n';
    }

    return stream;
}

          // ----------------------------------------------------
          // struct PrintMethods_Imp<TYPE, HasPrintMethod<TYPE> >
          // ----------------------------------------------------

// CLASS METHODS
template <class TYPE>
inline
bsl::ostream& PrintMethods_Imp<TYPE, bslmf::SelectTraitCase<HasPrintMethod> >::
                                            print(bsl::ostream& stream,
                                                  const TYPE&   object,
                                                  int           level,
                                                  int           spacesPerLevel)
{
    // A compilation error indicating the next line of code implies the 'TYPE'
    // parameter does not have a 'print' method with the expected signature.

    return object.print(stream, level, spacesPerLevel);
}

         // -------------------------------------------------------------
         // struct PrintMethods_Imp<TYPE, bslalg::HasStlIterators<TYPE> >
         // -------------------------------------------------------------

// CLASS METHODS
template <class TYPE>
bsl::ostream& PrintMethods_Imp<TYPE,
                               bslmf::SelectTraitCase<bslalg::HasStlIterators>
                              >::print(bsl::ostream& stream,
                                       const TYPE&   object,
                                       int           level,
                                       int           spacesPerLevel)
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    Print::indent(stream, level, spacesPerLevel);

    // A compilation error indicating the next line of code implies the 'TYPE'
    // parameter does not have STL-compliant iterators.

    typedef typename TYPE::const_iterator Iterator;

    if (0 <= spacesPerLevel) {
        // Multi-line output.

        if (level < 0) {
            level = -level;
        }

        stream << "[\n";

        const int levelPlus1 = level + 1;

        for (Iterator it = object.begin(); it != object.end(); ++it) {
            PrintMethods::print(stream,
                                *it,
                                levelPlus1,
                                spacesPerLevel);
        }

        Print::indent(stream, level, spacesPerLevel);

        stream << "]\n";
    }
    else {
        // Output on a single line and suppress any further indentation.

        stream << "[ ";

        for (Iterator it = object.begin(); it != object.end(); ++it) {
            PrintMethods::print(stream, *it, 0, -1);
            stream << ' ';
        }

        stream << ']';
    }

    return stream << bsl::flush;
}

                  // ---------------------------------------------------
                  // struct PrintMethods_Imp<TYPE, bslmf::IsPair<TYPE> >
                  // ---------------------------------------------------

// CLASS METHODS
template <class TYPE>
bsl::ostream& PrintMethods_Imp<TYPE, bslmf::SelectTraitCase<bslmf::IsPair> >::
                                            print(bsl::ostream& stream,
                                                  const TYPE&   object,
                                                  int           level,
                                                  int           spacesPerLevel)
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    Print::indent(stream, level, spacesPerLevel);

    if (0 <= spacesPerLevel) {
        // Multi-line output.

        if (level < 0) {
            level = -level;
        }

        stream << "[\n";

        const int levelPlus1 = level + 1;

        // A compilation error indicating the next line of code implies the
        // 'TYPE' parameter is not a pair.

        PrintMethods::print(stream,
                            object.first,
                            levelPlus1,
                            spacesPerLevel);
        PrintMethods::print(stream,
                            object.second,
                            levelPlus1,
                            spacesPerLevel);

        Print::indent(stream, level, spacesPerLevel);

        stream << "]\n";
    }
    else {
        // Output on a single line and suppress any further indentation.

        stream << "[ ";

        // A compilation error indicating the next line of code implies the
        // 'TYPE' parameter is not a pair.

        PrintMethods::print(stream, object.first, 0, -1);
        stream << ' ';

        PrintMethods::print(stream, object.second, 0, -1);
        stream << " ]";
    }

    return stream << bsl::flush;
}

                        // ----------------------
                        // namespace PrintMethods
                        // ----------------------

// CLASS METHODS
template <class TYPE>
bsl::ostream& PrintMethods::print(bsl::ostream& stream,
                                  const TYPE&   object,
                                  int           level,
                                  int           spacesPerLevel)
{
    typedef typename bslmf::SelectTrait<TYPE,
                                        HasPrintMethod,
                                        bslalg::HasStlIterators,
                                        bslmf::IsPair>::Type BdlbSelector;

    return PrintMethods_Imp<TYPE, BdlbSelector>::print(stream,
                                                       object,
                                                       level,
                                                       spacesPerLevel);
}

template <class CHAR_T, class CHAR_TRAITS_T, class ALLOC>
bsl::ostream& PrintMethods::print(
         bsl::ostream&                                          stream,
         const bsl::basic_string<CHAR_T, CHAR_TRAITS_T, ALLOC>& object,
         int                                                    level,
         int                                                    spacesPerLevel)
{
    return PrintMethods_Imp<bsl::basic_string<CHAR_T, CHAR_TRAITS_T, ALLOC>,
                            bslmf::SelectTraitCase<> >::print(stream,
                                                              object,
                                                              level,
                                                              spacesPerLevel);
}

template <class ALLOC>
bsl::ostream&
PrintMethods::print(bsl::ostream&                   stream,
                    const bsl::vector<char, ALLOC>& object,
                    int                             level,
                    int                             spacesPerLevel)
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    Print::indent(stream, level, spacesPerLevel);

    stream << "\"";

    const int len = static_cast<int>(object.size());

    if (0 < len) {
        Print::printString(stream, &object[0], len, false);
    }

    stream << "\"";

    if (0 <= spacesPerLevel) {
        stream << '\n';
    }

    return stream;
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

template <class TYPE>
bsl::ostream&
PrintMethods::print(bsl::ostream&              stream,
                    const std::optional<TYPE>& object,
                    int                        level,
                    int                        spacesPerLevel)
{

    if (object.has_value()) {
        return PrintMethods::print(stream,
                                   object.value(),
                                   level,
                                   spacesPerLevel);                   // RETURN

    } else {
        return PrintMethods::print(stream,
                                   "EMPTY",
                                   level,
                                   spacesPerLevel);                   // RETURN
    }
}

template <class ... TYPE>
bsl::ostream&
PrintMethods::print(bsl::ostream&                 stream,
                    const std::variant<TYPE ...>& object,
                    int                           level,
                    int                           spacesPerLevel)
{
    const auto lambda = [&](const auto& x) -> bsl::ostream& {
                            return PrintMethods::print(stream,
                                                       x,
                                                       level,
                                                       spacesPerLevel);
                        };
    return std::visit(lambda, object);
}

inline
bsl::ostream&
PrintMethods::print(bsl::ostream&         stream,
                    const std::monostate& ,
                    int                   level,
                    int                   spacesPerLevel)
{
    return PrintMethods::print(stream,
                               "MONOSTATE",
                               level,
                               spacesPerLevel);                       // RETURN
}
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

}  // close package namespace
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
