// balcl_typeinfo.cpp                                                 -*-C++-*-
#include <balcl_typeinfo.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balcl_typeinfo_cpp,"$Id$ $CSID$")

#include <balcl_optionvalue.h>

#include <bdlb_numericparseutil.h>
#include <bdlb_print.h>
#include <bdlb_stringviewutil.h>

#include <bdlt_iso8601util.h>

#include <bslalg_constructorproxy.h>

#include <bslmf_allocatorargt.h>
#include <bslmf_assert.h>
#include <bslmf_movableref.h>

#include <bsla_fallthrough.h>

#include <bsls_assert.h>
#include <bsls_keyword.h> // 'BSLS_KEYWORD_OVERRIDE'

#include <bsl_algorithm.h>
#include <bsl_cstddef.h>  // 'bsl::size_t'
#include <bsl_cstdlib.h>  // 'bsl::atoi'
#include <bsl_cstring.h>  // 'bsl::strlen'
#include <bsl_ostream.h>
#include <bsl_sstream.h>

#include <bsl_c_ctype.h>    // 'isdigit', 'isspace'

namespace BloombergLP {
namespace balcl {

                   // ========================
                   // class TypeInfoConstraint
                   // ========================

/// This class defines a protocol for objects implementing constraints on
/// option values.  Note that although this class not visible in the header
/// of this component, the class is publicly known by name to allow the
/// sharing of a constraint as an opaque object by use of the `constraint`
/// accessor and `setConstraint` manipulator methods.
class TypeInfoConstraint {

  public:
    // PUBLIC TYPES
    typedef TypeInfo::ParseInputSource ParseInputSource;

    // CREATORS

    /// Destroy this polymorphic constraint object.
    virtual ~TypeInfoConstraint();

    // ACCESSORS

    /// Load into the specified `element` the result of parsing the
    /// specified `input`, parsed as a value of the `balcl::OptionType` of
    /// this constraint, and output to the specified `stream` any error
    /// message, using the specified `inputSource` to determine how `input`
    /// for boolean options should be parsed.  Return `true` if this value,
    /// parsed without error, satisfies this constraint, and `false` if
    /// parsing fails or the value does not satisfy the constraint.  If
    /// parsing fails, `element` is unchanged.  The specified `inputSource`
    /// is used in handling boolean options.  Boolean options supplied by
    /// command line should not have associated text, where as boolean
    /// options supplied by the environment should have the values "0", "1",
    /// "true", or "false".  The behavior is undefined unless
    /// `type() == element->type()`.
    virtual bool parse(OptionValue               *element,
                       bsl::ostream&              stream,
                       const bsl::string_view&    input,
                       ParseInputSource           inputSource) const = 0;

    /// Return the `balcl::OptionType` of this constraint object.
    virtual OptionType::Enum type() const = 0;

    /// Return `true` if the specified `element` satisfies this constraint,
    /// and `false` otherwise.  Optionally specify a `stream` to which a
    /// descriptive error message is written if this constraint is not
    /// satisfied.  The behavior is undefined unless `element`
    /// `type() == element->type()`.
    virtual bool validate(const OptionValue& element) const = 0;
    virtual bool validate(const OptionValue& element,
                          bsl::ostream&      stream)  const = 0;
};
                       // ------------------------
                       // class TypeInfoConstraint
                       // ------------------------

// CREATORS
TypeInfoConstraint::~TypeInfoConstraint()
{
}

}  // close namespace balcl
}  // close namespace BloombergLP

namespace {
namespace u {

using namespace BloombergLP;
using namespace balcl;

typedef bslmf::MovableRefUtil MoveUtil;

bsl::size_t npos = bsl::string::npos;

                            // ==============
                            // struct Ordinal
                            // ==============

/// This `struct` assists in printing numbers as ordinals (1st, 2nd, etc.).
struct Ordinal {

    bsl::size_t d_rank;  // rank (starting at 0)

    // CREATORS

    /// Create an ordinal for the specified position `n` (starting at 0).
    explicit Ordinal(bsl::size_t n);
};

// FREE OPERATORS

/// Output the specified `position` (starting at 0) to the specified
/// `stream` as an ordinal, mapping 0 to "1st", 1 to "2nd", 3 to "3rd", 4 to
/// "4th", etc. following correct English usage.
bsl::ostream& operator<<(bsl::ostream& stream, Ordinal position);

                            // --------------
                            // struct Ordinal
                            // --------------

// CREATORS
Ordinal::Ordinal(bsl::size_t n)
: d_rank(n)
{
}

} // close namespace u

bsl::ostream& u::operator<<(bsl::ostream& stream, Ordinal position)
{
    // ranks start at 0, but are displayed as 1st, 2nd, etc.
    int n = static_cast<int>(position.d_rank + 1);
    switch (n % 10) {
      case 1: {
        if (n % 100 == 11) {
            stream << n << "th";
        } else {
            stream << n << "st";
        }
      } break;
      case 2: {
        if (n % 100 == 12) {
            stream << n << "th";
        } else {
            stream << n << "nd";
        }
      } break;
      case 3: {
        if (n % 100 == 13) {
            stream << n << "th";
        } else {
            stream << n << "rd";
        }
      } break;
      default: {
        stream << n << "th";
      } break;
    }
    return stream;
}

 namespace u {

                         // ===================
                         // function parseValue
                         // ===================

/// Load, to the specified `value`, the value obtained by parsing the
/// parsing the specified `input` that is (assumed) in the format expected
/// for values of the specified `type`.  Return `true` if the parse
/// succeeds, and `false` otherwise.  Note that on success `value` can be
/// legitimately cast to a pointer of the type associated with `type`.
bool parseValue(void                    *value,
                const bsl::string_view&  input,
                OptionType::Enum         type)
{
    BSLS_ASSERT(value);

    bool result = false;

    switch (type) {
      case OptionType::e_CHAR:   BSLA_FALLTHROUGH;
      case OptionType::e_INT:    BSLA_FALLTHROUGH;
      case OptionType::e_INT64:  BSLA_FALLTHROUGH;
      case OptionType::e_DOUBLE: BSLA_FALLTHROUGH;
      case OptionType::e_STRING: {
        switch (type) {
          case OptionType::e_CHAR: {
            *static_cast<char *>(value) = input.empty() ? '\0' : input[0];
            result = !input.empty() && input[0];
          } break;
          case OptionType::e_INT: {
            result = !bdlb::NumericParseUtil::parseInt(
                                                     static_cast<int *>(value),
                                                     input);
          } break;
          case OptionType::e_INT64: {
            result = !bdlb::NumericParseUtil::parseInt64(
                                      static_cast<bsls::Types::Int64 *>(value),
                                      input);
          } break;
          case OptionType::e_DOUBLE: {
            result = !bdlb::NumericParseUtil::parseDouble(
                                                  static_cast<double *>(value),
                                                  input);
          } break;
          case OptionType::e_STRING: {
            *static_cast<bsl::string *>(value) = input;
            result = true;
          } break;
          default: {
            BSLS_ASSERT(0);
          } break;
        }
      } break;
      case OptionType::e_DATETIME: BSLA_FALLTHROUGH;
      case OptionType::e_DATE:     BSLA_FALLTHROUGH;
      case OptionType::e_TIME: {
        bsl::string inputStr(input);

        switch (type) {
          case OptionType::e_DATETIME: {
            result = !bdlt::Iso8601Util::parse(
                                         static_cast<bdlt::Datetime *>(value),
                                         inputStr.c_str(),
                                         static_cast<int>(input.length()));
          } break;
          case OptionType::e_DATE: {
            result = !bdlt::Iso8601Util::parse(
                                         static_cast<bdlt::Date *>(value),
                                         inputStr.c_str(),
                                         static_cast<int>(input.length()));
          } break;
          case OptionType::e_TIME: {
            result = !bdlt::Iso8601Util::parse(
                                         static_cast<bdlt::Time *>(value),
                                         inputStr.c_str(),
                                         static_cast<int>(input.length()));
          } break;
          default: {
            BSLS_ASSERT(0);
          } break;
        }
      } break;
      default: {
        BSLS_ASSERT(0);
      } break;
    }
    return result;
}
                         // =========================
                         // function elemTypeToString
                         // =========================

/// Return the address of a string literal that describes the specified
/// `elemType` value.
const char *elemTypeToString(OptionType::Enum elemType)
{
    const char *typeString;

    switch (elemType) {
      case OptionType::e_CHAR: {
        typeString = "A 'char'";
      } break;
      case OptionType::e_INT: {
        typeString = "An 'int'";
      } break;
      case OptionType::e_INT64: {
        typeString = "A 64-bit integer";
      } break;
      case OptionType::e_DOUBLE: {
        typeString = "A 'double'";
      } break;
      case OptionType::e_STRING: {
        typeString = "A string";
      } break;
      case OptionType::e_DATETIME: {
        typeString = "A 'bdlt::Datetime'";
      } break;
      case OptionType::e_DATE: {
        typeString = "A 'bdlt::Date'";
      } break;
      case OptionType::e_TIME: {
        typeString = "A 'bdlt::Time'";
      } break;
      default: {
        BSLS_ASSERT(0);
        typeString = "An unknown type";
      } break;
    }

    return typeString;
}

                         // ====================
                         // class BoolConstraint
                         // ====================

/// This concrete implementation of the `Constraint` protocol provides a
/// `bool` empty constraint that always returns `true` after this option has
/// been parsed.  The individual contracts for each method are identical to
/// the protocol and not repeated here.
class BoolConstraint : public TypeInfoConstraint {

    // NOT IMPLEMENTED
    BoolConstraint(const BoolConstraint&);
    BoolConstraint& operator=(const BoolConstraint&);

  public:
    // CREATORS

    /// Create a `bool` constraint object.  Optionally specify a
    /// `basicAllocator`, which is ignored.
    explicit BoolConstraint(bslma::Allocator *basicAllocator = 0);

    /// Destroy this object.
    ~BoolConstraint() BSLS_KEYWORD_OVERRIDE;

    // ACCESSORS

    /// Return `OptionType::e_BOOL`.
    OptionType::Enum type() const BSLS_KEYWORD_OVERRIDE;

    bool parse(OptionValue             *element,
               bsl::ostream&            stream,
               const bsl::string_view&  input,
               ParseInputSource         inputSource) const
                                                         BSLS_KEYWORD_OVERRIDE;
        // Store the value 'true' into the specified 'element'.  If there are
        // any issues with the specified 'input' a warning message may be
        // written to the specified 'stream'.  The specified 'inputSource' is
        // used in handling boolean options.  Boolean options supplied by
        // command line should not have associated text, where as boolean
        // options supplied by the environment should have the values "0", "1",
        // "true", or "false".  Return 'true' if parsing is successful and
        // 'false' otherwise.  The behavior is undefined unless
        // 'OptionType::e_BOOL == element->type()'.

    bool validate(const OptionValue& element) const BSLS_KEYWORD_OVERRIDE;

    /// Return `true`, and leave the optionally specified `stream`
    /// unchanged.  The behavior is undefined unless the specified `element`
    /// satisfies `OptionType::e_BOOL == element->type()`.
    bool validate(const OptionValue& element,
                  bsl::ostream&      stream)  const BSLS_KEYWORD_OVERRIDE;
};

                         // --------------------
                         // class BoolConstraint
                         // --------------------

// CREATORS
BoolConstraint::BoolConstraint(bslma::Allocator *)
{
}

BoolConstraint::~BoolConstraint()
{
}

// BDE_VERIFY pragma: -FABC01  // not in alphabetic order

// ACCESSORS
OptionType::Enum BoolConstraint::type() const
{
    return OptionType::e_BOOL;
}

bool BoolConstraint::parse(OptionValue               *element,
                           bsl::ostream&              stream,
                           const bsl::string_view&    input,
                           ParseInputSource           inputSource) const
{
    BSLS_ASSERT(element);
    BSLS_ASSERT(element->hasNonVoidType());
    BSLS_ASSERT(OptionType::e_BOOL == element->type());

    if (TypeInfo::e_COMMAND_LINE == inputSource) {
        if (!input.empty()) {
            // WARNING: A value is provided for flag, using equality sign.
            // Values are not acceptable for flags, except when set by
            // environment variables.

            stream << "Warning: A string value has been provided for a boolean"
                      " option on the command line -- the string value was"
                      " ignored." << bsl::endl;

            BSLS_REVIEW_INVOKE(
                             "String value pass to bool command line option.");

            // We would prefer to return 'false' here, but cannot do so for
            // backward compatibility.

            // return false;                                          // RETURN
        }

        element->set(true);

        return true;                                                  // RETURN
    }

    BSLS_ASSERT(TypeInfo::e_ENVIRONMENT_VARIABLE == inputSource);

    bsl::string_view envInput(bdlb::StringViewUtil::trim(input));

    if ("1" == envInput || "true" == envInput) {
        element->set(true);
    }
    else if ("0" == envInput || "false" == envInput) {
        element->set(false);
    }
    else {
        stream << "Error parsing boolean environment variable with"
                " value \"" << envInput << "\" -- a value of \"1\", \"true\","
                " \"0\", or \"false\" is required." << std::endl;

        return false;                                                 // RETURN
    }

    return true;
}

bool BoolConstraint::validate(const OptionValue&) const
{
    return true;
}

bool BoolConstraint::validate(const OptionValue&, std::ostream&)
                                                                          const
{
    return true;
}

// BDE_VERIFY pragma: +FABC01  // not in alphabetic order

                   // ===========================
                   // class ScalarConstraint<...>
                   // ===========================

/// This concrete implementation of the `Constraint` protocol provides a
/// constraint of the parameterized `CONSTRAINT_TYPE`, which enables the
/// parsing and validation of values of parameterized `TYPE`.
template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
class ScalarConstraint : public TypeInfoConstraint {

    // DATA
    CONSTRAINT_TYPE d_constraint;

    // NOT IMPLEMENTED
    ScalarConstraint(const ScalarConstraint&);
    ScalarConstraint& operator=(const ScalarConstraint&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(ScalarConstraint,
                                   bslma::UsesBslmaAllocator);

    // CREATORS

    /// Create a constraint object storing the specified `constraint`
    /// functor.  Optionally specify a `basicAllocator` used to supply
    /// memory.  If `basicAllocator` is 0, the currently-installed default
    /// allocator is used.
    explicit ScalarConstraint(const CONSTRAINT_TYPE&  constraint,
                              bslma::Allocator       *basicAllocator = 0);

    /// Destroy this polymorphic constraint object.
    ~ScalarConstraint() BSLS_KEYWORD_OVERRIDE;

    // ACCESSORS

    /// Return the `balcl::OptionType` element type of this constraint.
    OptionType::Enum type() const BSLS_KEYWORD_OVERRIDE;

    /// Load into the instance of parameterized `TYPE` stored in the
    /// specified `element` the result of parsing the specified `input`,
    /// interpreted as an instance of `TYPE`.  Return `true` if parsing
    /// succeeds and the parsed value satisfies the constraint.  Return
    /// `false` otherwise, and write to the specified `stream` a descriptive
    /// error message.  The behavior is undefined unless
    /// `type() == element->type()`.
    bool parse(OptionValue               *element,
               std::ostream&              stream,
               const bsl::string_view&    input,
               ParseInputSource           ) const BSLS_KEYWORD_OVERRIDE;

    bool validate(const OptionValue& element) const BSLS_KEYWORD_OVERRIDE;

    /// Return `true` if the specified `element` satisfies this constraint,
    /// and `false` otherwise.  Optionally specify a `stream` to which a
    /// descriptive error message is written if this constraint is not
    /// satisfied.  The behavior is undefined unless
    /// `type() == element->type()`.
    bool validate(const OptionValue& element,
                  bsl::ostream&      stream)  const BSLS_KEYWORD_OVERRIDE;
};

                   // ---------------------------
                   // class ScalarConstraint<...>
                   // ---------------------------

// CREATORS
template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
ScalarConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::ScalarConstraint(
                                        const CONSTRAINT_TYPE&  constraint,
                                        bslma::Allocator       *basicAllocator)
: d_constraint(bsl::allocator_arg, basicAllocator, constraint)
{
}

template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
ScalarConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::~ScalarConstraint()
{
}

// BDE_VERIFY pragma: -FABC01  // not in alphabetic order

// ACCESSORS
template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
OptionType::Enum
ScalarConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::type() const
{
    BSLMF_ASSERT(OptionType::e_VOID <= static_cast<OptionType::Enum>(
                                                                   ELEM_TYPE));

    BSLMF_ASSERT(static_cast<OptionType::Enum>(ELEM_TYPE) <=
                                                     OptionType::e_TIME_ARRAY);

    return static_cast<OptionType::Enum>(ELEM_TYPE);
}

template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
bool
ScalarConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::parse(
                                    OptionValue               *element,
                                    bsl::ostream&              stream,
                                    const bsl::string_view&    input,
                                    ParseInputSource           ) const
{
    BSLS_ASSERT(element);
    BSLS_ASSERT(element->hasNonVoidType());

    const OptionType::Enum elemType = element->type();

    BSLS_ASSERT(ELEM_TYPE == elemType);

    TYPE value = TYPE();
    if (!parseValue(&value, input, elemType)) {
        stream << elemTypeToString(elemType)
               << " value was expected, instead of \""
               << input << "\"" << '\n' << bsl::flush;
        return false;                                                 // RETURN
    }

    element->set<TYPE>(value);
    return validate(*element, stream);
}

template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
bool
ScalarConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::validate(
                                              const OptionValue& element) const
{
    bsl::ostringstream oss;
    return validate(element, oss);
}

template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
bool
ScalarConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::validate(
                                               const OptionValue& element,
                                               bsl::ostream&      stream) const
{
    BSLS_ASSERT(element.hasNonVoidType());
    BSLS_ASSERT(ELEM_TYPE == element.type());

    return d_constraint
         ? d_constraint(&element.the<TYPE>(), stream)
         : true;
}

// BDE_VERIFY pragma: +FABC01  // not in alphabetic order

// SPECIALIZATIONS
template
class ScalarConstraint<char,
                       Constraint::CharConstraint,
                       OptionType::e_CHAR>;
template
class ScalarConstraint<int,
                       Constraint::IntConstraint,
                       OptionType::e_INT>;
template
class ScalarConstraint<bsls::Types::Int64,
                       Constraint::Int64Constraint,
                       OptionType::e_INT64>;
template
class ScalarConstraint<double,
                       Constraint::DoubleConstraint,
                       OptionType::e_DOUBLE>;
template
class ScalarConstraint<bsl::string,
                       Constraint::StringConstraint,
                       OptionType::e_STRING>;
template
class ScalarConstraint<bdlt::Datetime,
                       Constraint::DatetimeConstraint,
                       OptionType::e_DATETIME>;
template
class ScalarConstraint<bdlt::Date,
                       Constraint::DateConstraint,
                       OptionType::e_DATE>;
template
class ScalarConstraint<bdlt::Time,
                       Constraint::TimeConstraint,
                       OptionType::e_TIME>;
    // Explicit specializations (to force instantiation of all variants).

                 // ==============================
                 // class template ArrayConstraint
                 // ==============================

/// This concrete implementation of the `Constraint` protocol provides a
/// constraint of the parameterized `CONSTRAINT_TYPE`, which enables the
/// parsing and validation of values of parameterized `bsl::vector<TYPE>`
/// type.
template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
class ArrayConstraint : public TypeInfoConstraint {

    // DATA
    CONSTRAINT_TYPE d_constraint;

    // NOT IMPLEMENTED
    ArrayConstraint(const ArrayConstraint&);
    ArrayConstraint& operator=(const ArrayConstraint&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(ArrayConstraint,
                                   bslma::UsesBslmaAllocator);

    // CREATORS

    /// Create a constraint object storing the specified `constraint`
    /// functor.  Optionally specify a `basicAllocator` used to supply
    /// memory.  If `basicAllocator` is 0, the currently-installed default
    /// allocator is used.
    explicit ArrayConstraint(const CONSTRAINT_TYPE&  constraint,
                             bslma::Allocator       *basicAllocator);

    /// Destroy this polymorphic constraint object.
    ~ArrayConstraint() BSLS_KEYWORD_OVERRIDE;

    // ACCESSORS

    /// Return the `balcl::OptionType` element type of this constraint.
    OptionType::Enum type() const BSLS_KEYWORD_OVERRIDE;

    /// Append to the instance of `bsl::vector<TYPE>` stored in the
    /// specified `element` the result of parsing the specified `input`,
    /// interpreted as an instance of parameterized `TYPE`.  Return `true`
    /// if parsing succeeds and the parsed value satisfies the constraint.
    /// Return `false` otherwise, and write to the specified `stream` a
    /// descriptive error message.  The behavior is undefined unless
    /// `OptionType::toArrayType(type()) == element->type()`.
    bool parse(OptionValue               *element,
               bsl::ostream&              stream,
               const bsl::string_view&    input,
               ParseInputSource           ) const BSLS_KEYWORD_OVERRIDE;

    bool validate(const OptionValue& element) const BSLS_KEYWORD_OVERRIDE;

    /// Return `true` if the specified `element` satisfies this constraint,
    /// and `false` otherwise.  Optionally specify a `stream` to which a
    /// descriptive error message is written if this constraint is not
    /// satisfied.  The behavior is undefined unless `element`
    /// `type() == element->type()`.
    bool validate(const OptionValue& element,
                  bsl::ostream&      stream)  const BSLS_KEYWORD_OVERRIDE;
};

                 // ------------------------------
                 // class template ArrayConstraint
                 // ------------------------------

// CREATORS
template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
ArrayConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::ArrayConstraint(
                                        const CONSTRAINT_TYPE&  constraint,
                                        bslma::Allocator       *basicAllocator)
: d_constraint(bsl::allocator_arg, basicAllocator, constraint)
{
}

template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
ArrayConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::~ArrayConstraint()
{
}

// BDE_VERIFY pragma: -FABC01  // not in alphabetic order

// ACCESSORS
template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
OptionType::Enum
ArrayConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::type() const
{
    BSLMF_ASSERT(OptionType::e_VOID <=
                          static_cast<OptionType::Enum>(ELEM_TYPE));

    BSLMF_ASSERT(static_cast<OptionType::Enum>(ELEM_TYPE) <=
                                          OptionType::e_TIME_ARRAY);

    return static_cast<OptionType::Enum>(ELEM_TYPE);
}

template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
bool
ArrayConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::parse(
                                             OptionValue              *element,
                                             bsl::ostream&             stream,
                                             const bsl::string_view&   input,
                                             ParseInputSource         ) const
{
    BSLS_ASSERT(element);
    BSLS_ASSERT(element->hasNonVoidType());

    const OptionType::Enum elemType = element->type();

    BSLS_ASSERT(ELEM_TYPE == elemType);

    OptionType::Enum scalarType = OptionType::fromArrayType(elemType);

    TYPE value;
    if (!parseValue(&value, input, scalarType)) {
        stream << elemTypeToString(scalarType)
               << " value was expected, instead of \""
               << input << "\"" << '\n' << bsl::flush;
        return false;                                                 // RETURN
    }

// BDE_VERIFY pragma: -ADC01  // Passing address of char 'value' where a
                              // null-terminated string may be expected
    if (d_constraint && !d_constraint(&value, stream)) {
        return false;                                                 // RETURN
    }
// BDE_VERIFY pragma: +ADC01

    if (element->isNull()) {
        bsl::vector<TYPE> tmp;
        element->set(tmp);
    }

    element->the<bsl::vector<TYPE> >().push_back(value);

    return true;
}

template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
bool
ArrayConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::validate(
                                              const OptionValue& element) const
{
    bsl::ostringstream oss;
    return validate(element, oss);
}

template <class TYPE, class CONSTRAINT_TYPE, int ELEM_TYPE>
bool
ArrayConstraint<TYPE, CONSTRAINT_TYPE, ELEM_TYPE>::validate(
                                               const OptionValue& element,
                                               bsl::ostream&      stream) const
{
    BSLS_ASSERT(element.hasNonVoidType());
    BSLS_ASSERT(ELEM_TYPE == element.type());

    typedef typename bsl::vector<TYPE>::size_type size_type;
    const bsl::vector<TYPE>& vec = element.the<bsl::vector<TYPE> >();

    if (d_constraint) {
        for (size_type i = 0; i < vec.size(); ++i) {
            if (!d_constraint(&vec[i], stream)) {
                stream << "The above error occurred while parsing the "
                       << u::Ordinal(i)
                       << " element of the vector."
                       << '\n' << bsl::flush;
                return false;                                         // RETURN
            }
        }
    }
    return true;
}

// BDE_VERIFY pragma: -FABC01  // not in alphabetic order

// SPECIALIZATIONS
template
class ArrayConstraint<char,
                      Constraint::CharConstraint,
                      OptionType::e_CHAR_ARRAY>;
template
class ArrayConstraint<int,
                      Constraint::IntConstraint,
                      OptionType::e_INT_ARRAY>;
template
class ArrayConstraint<bsls::Types::Int64,
                      Constraint::Int64Constraint,
                      OptionType::e_INT64_ARRAY>;
template
class ArrayConstraint<double,
                      Constraint::DoubleConstraint,
                      OptionType::e_DOUBLE_ARRAY>;
template
class ArrayConstraint<bsl::string,
                      Constraint::StringConstraint,
                      OptionType::e_STRING_ARRAY>;
template
class ArrayConstraint<bdlt::Datetime,
                      Constraint::DatetimeConstraint,
                      OptionType::e_DATETIME_ARRAY>;
template
class ArrayConstraint<bdlt::Date,
                      Constraint::DateConstraint,
                      OptionType::e_DATE_ARRAY>;
template
class ArrayConstraint<bdlt::Time,
                      Constraint::TimeConstraint,
                      OptionType::e_TIME_ARRAY>;
    // Explicit specializations (to force instantiation of all variants).

                      // ===============================
                      // typedef template TYPEConstraint
                      // ===============================

// Note: for 'BoolConstraint', see non-'typedef' implementations.

/// The type `CharConstraint` is an alias for a scalar constraint on type
/// `char` using the `Constraint::CharConstraint` functor.
typedef ScalarConstraint<char,
                         Constraint::CharConstraint,
                         OptionType::e_CHAR> CharConstraint;

/// The type `IntConstraint` is an alias for a scalar constraint on type
/// `int` using the `Constraint::IntConstraint` functor.
typedef ScalarConstraint<int,
                         Constraint::IntConstraint,
                         OptionType::e_INT> IntConstraint;

/// The type `Int64Constraint` is an alias for a scalar constraint on type
/// `bsls::Types::Int64` using the `Constraint::Int64Constraint` functor.
typedef ScalarConstraint<bsls::Types::Int64,
                         Constraint::Int64Constraint,
                         OptionType::e_INT64>  Int64Constraint;

/// The type `DoubleConstraint` is an alias for a scalar constraint on type
/// `double` using the `Constraint::DoubleConstraint` functor.
typedef ScalarConstraint<double,
                         Constraint::DoubleConstraint,
                         OptionType::e_DOUBLE> DoubleConstraint;

/// The type `StringConstraint` is an alias for a scalar constraint on type
/// `bsl::string` using the `Constraint::StringConstraint` functor.
typedef ScalarConstraint<bsl::string,
                         Constraint::StringConstraint,
                         OptionType::e_STRING> StringConstraint;

/// The type `DatetimeConstraint` is an alias for a scalar constraint of
/// type `bdlt::Datetime` using the `Constraint::DatetimeConstraint`
/// functor.
typedef ScalarConstraint<bdlt::Datetime,
                         Constraint::DatetimeConstraint,
                         OptionType::e_DATETIME> DatetimeConstraint;

/// The type `DateConstraint` is an alias for a scalar constraint on type
/// `bdlt::Date` using the `Constraint::DateConstraint` functor.
typedef ScalarConstraint<bdlt::Date,
                         Constraint::DateConstraint,
                         OptionType::e_DATE> DateConstraint;

/// The type `TimeConstraint` is an alias for a scalar constraint on type
/// `bdlt::Time` using the `Constraint::TimeConstraint` functor.
typedef ScalarConstraint<bdlt::Time,
                         Constraint::TimeConstraint,
                         OptionType::e_TIME> TimeConstraint;

                    // ================================
                    // typedef template ArrayConstraint
                    // ================================

/// The type `CharArrayConstraint` is an alias for a constraint on type
/// `bsl::vector<char>` using the `Constraint::CharConstraint` functor.
typedef ArrayConstraint<char,
                        Constraint::CharConstraint,
                        OptionType::e_CHAR_ARRAY> CharArrayConstraint;

/// The type `IntArrayConstraint` is an alias for a constraint on type
/// `bsl::vector<int>` using the `Constraint::IntConstraint` functor.
typedef ArrayConstraint<int,
                        Constraint::IntConstraint,
                        OptionType::e_INT_ARRAY> IntArrayConstraint;

/// The type `Int64ArrayConstraint` is an alias for a constraint on type
/// `bsl::vector<bsls::Types::Int64>` using the
/// `Constraint::Int64Constraint` functor.
typedef ArrayConstraint<bsls::Types::Int64,
                       Constraint::Int64Constraint,
                       OptionType::e_INT64_ARRAY> Int64ArrayConstraint;

/// The type `DoubleArrayConstraint` is an alias for a constraint on type
/// `bsl::vector<double>` using the `Constraint::DoubleConstraint` functor.
typedef ArrayConstraint<double,
                        Constraint::DoubleConstraint,
                        OptionType::e_DOUBLE_ARRAY> DoubleArrayConstraint;

/// The type `StringArrayConstraint` is an alias for a constraint on type
/// `bsl::vector<bsl::string>` using the `Constraint::StringConstraint`
/// functor.
typedef ArrayConstraint<bsl::string,
                        Constraint::StringConstraint,
                        OptionType::e_STRING_ARRAY> StringArrayConstraint;

/// The type `DatetimeArrayConstraint` is an alias for a constraint on type
/// `bsl::vector<bdlt::Datetime>` using the `Constraint::DatetimeConstraint`
/// functor.
typedef ArrayConstraint<bdlt::Datetime,
                        Constraint::DatetimeConstraint,
                        OptionType::e_DATETIME_ARRAY> DatetimeArrayConstraint;

/// The type `DateArrayConstraint` is an alias for a constraint on type
/// `bsl::vector<bdlt::Date>` using the `Constraint::DateConstraint`
/// functor.
typedef ArrayConstraint<bdlt::Date,
                        Constraint::DateConstraint,
                        OptionType::e_DATE_ARRAY> DateArrayConstraint;

/// The type `TimeArrayConstraint` is an alias for a constraint on type
/// `bsl::vector<bdlt::Time>` using the `Constraint::TimeConstraint`
/// functor.
typedef ArrayConstraint<bdlt::Time,
                        Constraint::TimeConstraint,
                        OptionType::e_TIME_ARRAY> TimeArrayConstraint;

                        // ======================
                        // struct OptionValueUtil
                        // ======================

/// This `struct` provides a namespace for utility functions on
/// `OptionValue` objects.
struct OptionValueUtil {

    // CLASS METHODS

    /// Assign to the specified `*dst` value found at the specified `src`.
    /// The behavior is undefined unless `OptionType::e_VOID != src.type()`
    /// and `src` can be cast to a pointer to
    /// `OptionType::EnumToType<dst.type()>::type`.
    static void setValue(OptionValue *dst, const void  *src);
};

                        // ---------------------
                        // class OptionValueUtil
                        // ---------------------

// CLASS METHODS
void OptionValueUtil::setValue(OptionValue *dst, const void  *src)
{
    BSLS_ASSERT(dst);
    BSLS_ASSERT(src);

    OptionType::Enum type = dst->type();

    typedef OptionType  Ot;

    switch (type) {
      case Ot::e_VOID: {
        BSLS_ASSERT_INVOKE_NORETURN("Not reachable.");
      } break;
      case Ot::e_BOOL: {
        dst->set(*(static_cast<const Ot::Bool          *>(src)));
      } break;
      case Ot::e_CHAR: {
        dst->set(*(static_cast<const Ot::Char          *>(src)));
      } break;
      case Ot::e_INT: {
        dst->set(*(static_cast<const Ot::Int           *>(src)));
      } break;
      case Ot::e_INT64: {
        dst->set(*(static_cast<const Ot::Int64         *>(src)));
      } break;
      case Ot::e_DOUBLE: {
        dst->set(*(static_cast<const Ot::Double        *>(src)));
      } break;
      case Ot::e_STRING: {
        dst->set(*(static_cast<const Ot::String        *>(src)));
      } break;
      case Ot::e_DATETIME: {
        dst->set(*(static_cast<const Ot::Datetime      *>(src)));
      } break;
      case Ot::e_DATE: {
        dst->set(*(static_cast<const Ot::Date          *>(src)));
      } break;
      case Ot::e_TIME: {
        dst->set(*(static_cast<const Ot::Time          *>(src)));
      } break;
      case Ot::e_CHAR_ARRAY: {
        dst->set(*(static_cast<const Ot::CharArray     *>(src)));
      } break;
      case Ot::e_INT_ARRAY: {
        dst->set(*(static_cast<const Ot::IntArray      *>(src)));
      } break;
      case Ot::e_INT64_ARRAY: {
        dst->set(*(static_cast<const Ot::Int64Array    *>(src)));
      } break;
      case Ot::e_DOUBLE_ARRAY: {
        dst->set(*(static_cast<const Ot::DoubleArray   *>(src)));
      } break;
      case Ot::e_STRING_ARRAY: {
        dst->set(*(static_cast<const Ot::StringArray   *>(src)));
      } break;
      case Ot::e_DATETIME_ARRAY: {
        dst->set(*(static_cast<const Ot::DatetimeArray *>(src)));
      } break;
      case Ot::e_DATE_ARRAY: {
        dst->set(*(static_cast<const Ot::DateArray     *>(src)));
      } break;
      case Ot::e_TIME_ARRAY: {
        dst->set(*(static_cast<const Ot::TimeArray     *>(src)));
      } break;
    }
}

}  // close namespace u
}  // close unnamed namespace

namespace BloombergLP {
namespace balcl {

                       // --------------
                       // class TypeInfo
                       // --------------

// CREATORS
TypeInfo::TypeInfo(bslma::Allocator *basicAllocator)
: d_elemType(OptionType::e_STRING)
, d_linkedVariable_p(0)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

TypeInfo::TypeInfo(bool            *variable,
                  bslma::Allocator *basicAllocator)
: d_elemType(OptionType::e_BOOL)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

TypeInfo::TypeInfo(char             *variable,
                   bslma::Allocator *basicAllocator)
: d_elemType(OptionType::e_CHAR)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

TypeInfo::TypeInfo(char                              *variable,
                   const Constraint::CharConstraint&  constraint,
                   bslma::Allocator                  *basicAllocator)
: d_elemType(OptionType::e_CHAR)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

TypeInfo::TypeInfo(int              *variable,
                   bslma::Allocator *basicAllocator)
: d_elemType(OptionType::e_INT)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

TypeInfo::TypeInfo(int                              *variable,
                   const Constraint::IntConstraint&  constraint,
                   bslma::Allocator                 *basicAllocator)
: d_elemType(OptionType::e_INT)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

TypeInfo::TypeInfo(bsls::Types::Int64 *variable,
                   bslma::Allocator   *basicAllocator)
: d_elemType(OptionType::e_INT64)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

TypeInfo::TypeInfo(bsls::Types::Int64                 *variable,
                   const Constraint::Int64Constraint&  constraint,
                   bslma::Allocator                   *basicAllocator)
: d_elemType(OptionType::e_INT64)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

TypeInfo::TypeInfo(double           *variable,
                   bslma::Allocator *basicAllocator)
: d_elemType(OptionType::e_DOUBLE)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

TypeInfo::TypeInfo(double                              *variable,
                   const Constraint::DoubleConstraint&  constraint,
                   bslma::Allocator                    *basicAllocator)
: d_elemType(OptionType::e_DOUBLE)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

TypeInfo::TypeInfo(bsl::string      *variable,
                   bslma::Allocator *basicAllocator)
: d_elemType(OptionType::e_STRING)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

TypeInfo::TypeInfo(bsl::string                         *variable,
                   const Constraint::StringConstraint&  constraint,
                   bslma::Allocator                    *basicAllocator)
: d_elemType(OptionType::e_STRING)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

TypeInfo::TypeInfo(bdlt::Datetime   *variable,
                   bslma::Allocator *basicAllocator)
: d_elemType(OptionType::e_DATETIME)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

TypeInfo::TypeInfo(bdlt::Datetime                        *variable,
                   const Constraint::DatetimeConstraint&  constraint,
                   bslma::Allocator                      *basicAllocator)
: d_elemType(OptionType::e_DATETIME)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

TypeInfo::TypeInfo(bdlt::Date       *variable,
                   bslma::Allocator *basicAllocator)
: d_elemType(OptionType::e_DATE)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

TypeInfo::TypeInfo(bdlt::Date                        *variable,
                   const Constraint::DateConstraint&  constraint,
                   bslma::Allocator                  *basicAllocator)
: d_elemType(OptionType::e_DATE)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

TypeInfo::TypeInfo(bdlt::Time       *variable,
                   bslma::Allocator *basicAllocator)
: d_elemType(OptionType::e_TIME)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

TypeInfo::TypeInfo(bdlt::Time                        *variable,
                   const Constraint::TimeConstraint&  constraint,
                   bslma::Allocator                  *basicAllocator)
: d_elemType(OptionType::e_TIME)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

TypeInfo::TypeInfo(bsl::vector<char> *variable,
                   bslma::Allocator  *basicAllocator)
: d_elemType(OptionType::e_CHAR_ARRAY)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

TypeInfo::TypeInfo(bsl::vector<char>                 *variable,
                   const Constraint::CharConstraint&  constraint,
                   bslma::Allocator                  *basicAllocator)
: d_elemType(OptionType::e_CHAR_ARRAY)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

TypeInfo::TypeInfo(bsl::vector<int> *variable,
                   bslma::Allocator *basicAllocator)
: d_elemType(OptionType::e_INT_ARRAY)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

TypeInfo::TypeInfo(bsl::vector<int>                 *variable,
                   const Constraint::IntConstraint&  constraint,
                   bslma::Allocator                 *basicAllocator)
: d_elemType(OptionType::e_INT_ARRAY)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

TypeInfo::TypeInfo(bsl::vector<bsls::Types::Int64> *variable,
                   bslma::Allocator                *basicAllocator)
: d_elemType(OptionType::e_INT64_ARRAY)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

TypeInfo::TypeInfo(bsl::vector<bsls::Types::Int64>    *variable,
                   const Constraint::Int64Constraint&  constraint,
                   bslma::Allocator                   *basicAllocator)
: d_elemType(OptionType::e_INT64_ARRAY)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

TypeInfo::TypeInfo(bsl::vector<double> *variable,
                   bslma::Allocator    *basicAllocator)
: d_elemType(OptionType::e_DOUBLE_ARRAY)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

TypeInfo::TypeInfo(bsl::vector<double>                 *variable,
                   const Constraint::DoubleConstraint&  constraint,
                   bslma::Allocator                    *basicAllocator)
: d_elemType(OptionType::e_DOUBLE_ARRAY)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

TypeInfo::TypeInfo(bsl::vector<bsl::string> *variable,
                   bslma::Allocator         *basicAllocator)
: d_elemType(OptionType::e_STRING_ARRAY)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

TypeInfo::TypeInfo(bsl::vector<bsl::string>            *variable,
                   const Constraint::StringConstraint&  constraint,
                   bslma::Allocator                    *basicAllocator)
: d_elemType(OptionType::e_STRING_ARRAY)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

TypeInfo::TypeInfo(bsl::vector<bdlt::Datetime> *variable,
                   bslma::Allocator            *basicAllocator)
: d_elemType(OptionType::e_DATETIME_ARRAY)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

TypeInfo::TypeInfo(bsl::vector<bdlt::Datetime>           *variable,
                   const Constraint::DatetimeConstraint&  constraint,
                   bslma::Allocator                      *basicAllocator)
: d_elemType(OptionType::e_DATETIME_ARRAY)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

TypeInfo::TypeInfo(bsl::vector<bdlt::Date> *variable,
                   bslma::Allocator        *basicAllocator)
: d_elemType(OptionType::e_DATE_ARRAY)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

TypeInfo::TypeInfo(bsl::vector<bdlt::Date>           *variable,
                   const Constraint::DateConstraint&  constraint,
                   bslma::Allocator                  *basicAllocator)
: d_elemType(OptionType::e_DATE_ARRAY)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

TypeInfo::TypeInfo(bsl::vector<bdlt::Time> *variable,
                   bslma::Allocator        *basicAllocator)
: d_elemType(OptionType::e_TIME_ARRAY)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

TypeInfo::TypeInfo(bsl::vector<bdlt::Time>           *variable,
                   const Constraint::TimeConstraint&  constraint,
                   bslma::Allocator                  *basicAllocator)
: d_elemType(OptionType::e_TIME_ARRAY)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(false)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

TypeInfo::TypeInfo(bsl::optional<char> *variable,
                   bslma::Allocator    *basicAllocator)
: d_elemType(OptionType::e_CHAR)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(variable != 0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

TypeInfo::TypeInfo(bsl::optional<char>               *variable,
                   const Constraint::CharConstraint&  constraint,
                   bslma::Allocator                  *basicAllocator)
: d_elemType(OptionType::e_CHAR)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(variable != 0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

TypeInfo::TypeInfo(bsl::optional<int> *variable,
                   bslma::Allocator   *basicAllocator)
: d_elemType(OptionType::e_INT)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(variable != 0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

TypeInfo::TypeInfo(bsl::optional<int>               *variable,
                   const Constraint::IntConstraint&  constraint,
                   bslma::Allocator                 *basicAllocator)
: d_elemType(OptionType::e_INT)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(variable != 0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

TypeInfo::TypeInfo(bsl::optional<bsls::Types::Int64> *variable,
                   bslma::Allocator                  *basicAllocator)
: d_elemType(OptionType::e_INT64)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(variable != 0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

TypeInfo::TypeInfo(bsl::optional<bsls::Types::Int64>  *variable,
                   const Constraint::Int64Constraint&  constraint,
                   bslma::Allocator                   *basicAllocator)
: d_elemType(OptionType::e_INT64)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(variable != 0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

TypeInfo::TypeInfo(bsl::optional<double> *variable,
                   bslma::Allocator      *basicAllocator)
: d_elemType(OptionType::e_DOUBLE)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(variable != 0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

TypeInfo::TypeInfo(bsl::optional<double>               *variable,
                   const Constraint::DoubleConstraint&  constraint,
                   bslma::Allocator                    *basicAllocator)
: d_elemType(OptionType::e_DOUBLE)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(variable != 0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

TypeInfo::TypeInfo(bsl::optional<bsl::string> *variable,
                   bslma::Allocator           *basicAllocator)
: d_elemType(OptionType::e_STRING)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(variable != 0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

TypeInfo::TypeInfo(bsl::optional<bsl::string>          *variable,
                   const Constraint::StringConstraint&  constraint,
                   bslma::Allocator                    *basicAllocator)
: d_elemType(OptionType::e_STRING)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(variable != 0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

TypeInfo::TypeInfo(bsl::optional<bdlt::Datetime> *variable,
                   bslma::Allocator              *basicAllocator)
: d_elemType(OptionType::e_DATETIME)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(variable != 0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

TypeInfo::TypeInfo(bsl::optional<bdlt::Datetime>         *variable,
                   const Constraint::DatetimeConstraint&  constraint,
                   bslma::Allocator                      *basicAllocator)
: d_elemType(OptionType::e_DATETIME)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(variable != 0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

TypeInfo::TypeInfo(bsl::optional<bdlt::Date> *variable,
                   bslma::Allocator          *basicAllocator)
: d_elemType(OptionType::e_DATE)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(variable != 0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

TypeInfo::TypeInfo(bsl::optional<bdlt::Date>         *variable,
                   const Constraint::DateConstraint&  constraint,
                   bslma::Allocator                  *basicAllocator)
: d_elemType(OptionType::e_DATE)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(variable != 0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

TypeInfo::TypeInfo(bsl::optional<bdlt::Time> *variable,
                   bslma::Allocator          *basicAllocator)
: d_elemType(OptionType::e_TIME)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(variable != 0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    resetConstraint();
}

TypeInfo::TypeInfo(bsl::optional<bdlt::Time>         *variable,
                   const Constraint::TimeConstraint&  constraint,
                   bslma::Allocator                  *basicAllocator)
: d_elemType(OptionType::e_TIME)
, d_linkedVariable_p(variable)
, d_isOptionalLinkedVariable(variable != 0)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    setConstraint(constraint);
}

TypeInfo::TypeInfo(const TypeInfo&   original,
                   bslma::Allocator *basicAllocator)
: d_elemType                (original.d_elemType)
, d_linkedVariable_p        (original.d_linkedVariable_p)
, d_isOptionalLinkedVariable(original.d_isOptionalLinkedVariable)
, d_constraint_p            (original.d_constraint_p)  // share constraint
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

TypeInfo::~TypeInfo()
{
}

// MANIPULATORS
TypeInfo& TypeInfo::operator=(const TypeInfo& rhs)
{
    if (this != &rhs) {
        d_elemType                 = rhs.d_elemType;
        d_linkedVariable_p         = rhs.d_linkedVariable_p;
        d_isOptionalLinkedVariable = rhs.d_isOptionalLinkedVariable;
        d_constraint_p             = rhs.d_constraint_p;
    }

    return *this;
}

void TypeInfo::resetConstraint()
{
    static bslma::Allocator *ga = bslma::Default::globalAllocator(0);

    // STATIC DATA
    static u::BoolConstraint          defaultBoolConstraint(               ga);

    static u::CharConstraint          defaultCharConstraint(
                                         Constraint::CharConstraint(),     ga);

    static u::IntConstraint           defaultIntConstraint(
                                         Constraint::IntConstraint(),      ga);

    static u::Int64Constraint         defaultInt64Constraint(
                                         Constraint::Int64Constraint(),    ga);

    static u::DoubleConstraint        defaultDoubleConstraint(
                                         Constraint::DoubleConstraint(),   ga);

    static u::StringConstraint        defaultStringConstraint(
                                         Constraint::StringConstraint(),   ga);

    static u::DatetimeConstraint      defaultDatetimeConstraint(
                                         Constraint::DatetimeConstraint(), ga);

    static u::DateConstraint          defaultDateConstraint(
                                         Constraint::DateConstraint(),     ga);

    static u::TimeConstraint          defaultTimeConstraint(
                                         Constraint::TimeConstraint(),     ga);

    static u::CharArrayConstraint     defaultCharArrayConstraint(
                                         Constraint::CharConstraint(),     ga);

    static u::IntArrayConstraint      defaultIntArrayConstraint(
                                         Constraint::IntConstraint(),      ga);

    static u::Int64ArrayConstraint    defaultInt64ArrayConstraint(
                                         Constraint::Int64Constraint(),    ga);

    static u::DoubleArrayConstraint   defaultDoubleArrayConstraint(
                                         Constraint::DoubleConstraint(),   ga);

    static u::StringArrayConstraint   defaultStringArrayConstraint(
                                         Constraint::StringConstraint(),   ga);

    static u::DatetimeArrayConstraint defaultDatetimeArrayConstraint(
                                         Constraint::DatetimeConstraint(), ga);

    static u::DateArrayConstraint     defaultDateArrayConstraint(
                                         Constraint::DateConstraint(),     ga);

    // These static variables, default-initialized, are shared among all
    // type information objects that do not have constraints.  Note that
    // these objects are initialized with the global allocator and should
    // never be deleted.
    static u::TimeArrayConstraint     defaultTimeArrayConstraint(
                                         Constraint::TimeConstraint(),     ga);

    switch (d_elemType) {
      case OptionType::e_BOOL: {
        d_constraint_p.reset(&defaultBoolConstraint,
                             bslstl::SharedPtrNilDeleter(),
                             d_allocator_p);
      } break;
      case OptionType::e_CHAR: {
        d_constraint_p.reset(&defaultCharConstraint,
                             bslstl::SharedPtrNilDeleter(),
                             d_allocator_p);
      } break;
      case OptionType::e_INT: {
        d_constraint_p.reset(&defaultIntConstraint,
                             bslstl::SharedPtrNilDeleter(),
                             d_allocator_p);
      } break;
      case OptionType::e_INT64: {
        d_constraint_p.reset(&defaultInt64Constraint,
                             bslstl::SharedPtrNilDeleter(),
                             d_allocator_p);
      } break;
      case OptionType::e_DOUBLE: {
        d_constraint_p.reset(&defaultDoubleConstraint,
                             bslstl::SharedPtrNilDeleter(),
                             d_allocator_p);
      } break;
      case OptionType::e_STRING: {
        d_constraint_p.reset(&defaultStringConstraint,
                             bslstl::SharedPtrNilDeleter(),
                             d_allocator_p);
      } break;
      case OptionType::e_DATETIME: {
        d_constraint_p.reset(&defaultDatetimeConstraint,
                             bslstl::SharedPtrNilDeleter(),
                             d_allocator_p);
      } break;
      case OptionType::e_DATE: {
        d_constraint_p.reset(&defaultDateConstraint,
                             bslstl::SharedPtrNilDeleter(),
                             d_allocator_p);
      } break;
      case OptionType::e_TIME: {
        d_constraint_p.reset(&defaultTimeConstraint,
                             bslstl::SharedPtrNilDeleter(),
                             d_allocator_p);
      } break;
      case OptionType::e_CHAR_ARRAY: {
        d_constraint_p.reset(&defaultCharArrayConstraint,
                             bslstl::SharedPtrNilDeleter(),
                             d_allocator_p);
      } break;
      case OptionType::e_INT_ARRAY: {
        d_constraint_p.reset(&defaultIntArrayConstraint,
                             bslstl::SharedPtrNilDeleter(),
                             d_allocator_p);
      } break;
      case OptionType::e_INT64_ARRAY: {
        d_constraint_p.reset(&defaultInt64ArrayConstraint,
                             bslstl::SharedPtrNilDeleter(),
                             d_allocator_p);
      } break;
      case OptionType::e_DOUBLE_ARRAY: {
        d_constraint_p.reset(&defaultDoubleArrayConstraint,
                             bslstl::SharedPtrNilDeleter(),
                             d_allocator_p);
      } break;
      case OptionType::e_STRING_ARRAY: {
        d_constraint_p.reset(&defaultStringArrayConstraint,
                             bslstl::SharedPtrNilDeleter(),
                             d_allocator_p);
      } break;
      case OptionType::e_DATETIME_ARRAY: {
        d_constraint_p.reset(&defaultDatetimeArrayConstraint,
                             bslstl::SharedPtrNilDeleter(),
                             d_allocator_p);
      } break;
      case OptionType::e_DATE_ARRAY: {
        d_constraint_p.reset(&defaultDateArrayConstraint,
                             bslstl::SharedPtrNilDeleter(),
                             d_allocator_p);
      } break;
      case OptionType::e_TIME_ARRAY: {
        d_constraint_p.reset(&defaultTimeArrayConstraint,
                             bslstl::SharedPtrNilDeleter(),
                             d_allocator_p);
      } break;
      default: {
        BSLS_ASSERT(0);
      } break;
    }
}

void TypeInfo::resetLinkedVariableAndConstraint()
{
    d_linkedVariable_p         = 0;
    d_isOptionalLinkedVariable = false;
    resetConstraint();
}

void TypeInfo::setConstraint(const Constraint::CharConstraint& constraint)
{
    if (OptionType::e_CHAR == d_elemType) {
        d_constraint_p.reset(new (*d_allocator_p) u::CharConstraint(
                                                                constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else if (OptionType::e_CHAR_ARRAY == d_elemType) {
        d_constraint_p.reset(new (*d_allocator_p) u::CharArrayConstraint(
                                                                constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else {
        BSLS_ASSERT(0);
    }
}

void TypeInfo::setConstraint(const Constraint::IntConstraint& constraint)
{
    if (OptionType::e_INT == d_elemType) {
        d_constraint_p.reset(new (*d_allocator_p) u::IntConstraint(
                                                                constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else if (OptionType::e_INT_ARRAY == d_elemType) {
        d_constraint_p.reset(new (*d_allocator_p) u::IntArrayConstraint(
                                                                constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else {
        BSLS_ASSERT(0);
    }
}

void TypeInfo::setConstraint(const Constraint::Int64Constraint& constraint)
{
    if (OptionType::e_INT64 == d_elemType) {
        d_constraint_p.reset(new (*d_allocator_p) u::Int64Constraint(
                                                                constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else if (OptionType::e_INT64_ARRAY == d_elemType) {
        d_constraint_p.reset(new (*d_allocator_p) u::Int64ArrayConstraint(
                                constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else {
        BSLS_ASSERT(0);
    }
}

void TypeInfo::setConstraint(const Constraint::DoubleConstraint& constraint)
{
    if (OptionType::e_DOUBLE == d_elemType) {
        d_constraint_p.reset(new (*d_allocator_p) u::DoubleConstraint(
                                                                constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else if (OptionType::e_DOUBLE_ARRAY == d_elemType) {
        d_constraint_p.reset(new (*d_allocator_p) u::DoubleArrayConstraint(
                                                                constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else {
        BSLS_ASSERT(0);
    }
}

void TypeInfo::setConstraint(const Constraint::StringConstraint& constraint)
{
    if (OptionType::e_STRING == d_elemType) {
        d_constraint_p.reset(new (*d_allocator_p) u::StringConstraint(
                                                                constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else if (OptionType::e_STRING_ARRAY == d_elemType) {
        d_constraint_p.reset(new (*d_allocator_p) u::StringArrayConstraint(
                                                                constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else {
        BSLS_ASSERT(0);
    }
}

void TypeInfo::setConstraint(const Constraint::DatetimeConstraint& constraint)
{
    if (OptionType::e_DATETIME == d_elemType) {
        d_constraint_p.reset(new (*d_allocator_p) u::DatetimeConstraint(
                                                                constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else if (OptionType::e_DATETIME_ARRAY == d_elemType) {
        d_constraint_p.reset(new (*d_allocator_p) u::DatetimeArrayConstraint(
                                                                constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else {
        BSLS_ASSERT(0);
    }
}

void TypeInfo::setConstraint(const Constraint::DateConstraint& constraint)
{
    if (OptionType::e_DATE == d_elemType) {
        d_constraint_p.reset(new (*d_allocator_p) u::DateConstraint(
                                                                constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else if (OptionType::e_DATE_ARRAY == d_elemType) {
        d_constraint_p.reset(new (*d_allocator_p) u::DateArrayConstraint(
                                                                constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else {
        BSLS_ASSERT(0);
    }
}

void TypeInfo::setConstraint(const Constraint::TimeConstraint& constraint)
{
    if (OptionType::e_TIME == d_elemType) {
        d_constraint_p.reset(new (*d_allocator_p) u::TimeConstraint(
                                                                constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else if (OptionType::e_TIME_ARRAY == d_elemType) {
        d_constraint_p.reset(new (*d_allocator_p) u::TimeArrayConstraint(
                                                                constraint,
                                                                d_allocator_p),
                             d_allocator_p);
    } else {
        BSLS_ASSERT(0);
    }
}

void TypeInfo::setConstraint(
                         const bsl::shared_ptr<TypeInfoConstraint>& constraint)
{
    d_constraint_p = constraint;
}

void TypeInfo::setLinkedVariable(bool *variable)
{
    d_elemType                 = OptionType::e_BOOL;
    d_linkedVariable_p         = variable;
    d_isOptionalLinkedVariable = false;
    resetConstraint();
}

void TypeInfo::setLinkedVariable(char *variable)
{
    d_elemType                 = OptionType::e_CHAR;
    d_linkedVariable_p         = variable;
    d_isOptionalLinkedVariable = false;
    resetConstraint();
}

void TypeInfo::setLinkedVariable(int *variable)
{
    d_elemType                 = OptionType::e_INT;
    d_linkedVariable_p         = variable;
    d_isOptionalLinkedVariable = false;
    resetConstraint();
}

void TypeInfo::setLinkedVariable(bsls::Types::Int64 *variable)
{
    d_elemType                 = OptionType::e_INT64;
    d_linkedVariable_p         = variable;
    d_isOptionalLinkedVariable = false;
    resetConstraint();
}

void TypeInfo::setLinkedVariable(double *variable)
{
    d_elemType                 = OptionType::e_DOUBLE;
    d_linkedVariable_p         = variable;
    d_isOptionalLinkedVariable = false;
    resetConstraint();
}

void TypeInfo::setLinkedVariable(bsl::string *variable)
{
    d_elemType                 = OptionType::e_STRING;
    d_linkedVariable_p         = variable;
    d_isOptionalLinkedVariable = false;
    resetConstraint();
}

void TypeInfo::setLinkedVariable(bdlt::Datetime *variable)
{
    d_elemType                 = OptionType::e_DATETIME;
    d_linkedVariable_p         = variable;
    d_isOptionalLinkedVariable = false;
    resetConstraint();
}

void TypeInfo::setLinkedVariable(bdlt::Date *variable)
{
    d_elemType                 = OptionType::e_DATE;
    d_linkedVariable_p         = variable;
    d_isOptionalLinkedVariable = false;
    resetConstraint();
}

void TypeInfo::setLinkedVariable(bdlt::Time *variable)
{
    d_elemType                 = OptionType::e_TIME;
    d_linkedVariable_p         = variable;
    d_isOptionalLinkedVariable = false;
    resetConstraint();
}

void TypeInfo::setLinkedVariable(bsl::vector<char> *variable)
{
    d_elemType                 = OptionType::e_CHAR_ARRAY;
    d_linkedVariable_p         = variable;
    d_isOptionalLinkedVariable = false;
    resetConstraint();
}

void TypeInfo::setLinkedVariable(bsl::vector<int> *variable)
{
    d_elemType                 = OptionType::e_INT_ARRAY;
    d_linkedVariable_p         = variable;
    d_isOptionalLinkedVariable = false;
    resetConstraint();
}

void TypeInfo::setLinkedVariable(bsl::vector<bsls::Types::Int64> *variable)
{
    d_elemType                 = OptionType::e_INT64_ARRAY;
    d_linkedVariable_p         = variable;
    d_isOptionalLinkedVariable = false;
    resetConstraint();
}

void TypeInfo::setLinkedVariable(bsl::vector<double> *variable)
{
    d_elemType                 = OptionType::e_DOUBLE_ARRAY;
    d_linkedVariable_p         = variable;
    d_isOptionalLinkedVariable = false;
    resetConstraint();
}

void TypeInfo::setLinkedVariable(bsl::vector<bsl::string> *variable)
{
    d_elemType                 = OptionType::e_STRING_ARRAY;
    d_linkedVariable_p         = variable;
    d_isOptionalLinkedVariable = false;
    resetConstraint();
}

void TypeInfo::setLinkedVariable(bsl::vector<bdlt::Datetime> *variable)
{
    d_elemType                 = OptionType::e_DATETIME_ARRAY;
    d_linkedVariable_p         = variable;
    d_isOptionalLinkedVariable = false;
    resetConstraint();
}

void TypeInfo::setLinkedVariable(bsl::vector<bdlt::Date> *variable)
{
    d_elemType                 = OptionType::e_DATE_ARRAY;
    d_linkedVariable_p         = variable;
    d_isOptionalLinkedVariable = false;
    resetConstraint();
}

void TypeInfo::setLinkedVariable(bsl::vector<bdlt::Time> *variable)
{
    d_elemType                 = OptionType::e_TIME_ARRAY;
    d_linkedVariable_p         = variable;
    d_isOptionalLinkedVariable = false;
    resetConstraint();
}

void TypeInfo::setLinkedVariable(bsl::optional<char> *variable)
{
    d_elemType                 = OptionType::e_CHAR;
    d_linkedVariable_p         = variable;
    d_isOptionalLinkedVariable = variable != 0;
    resetConstraint();
}

void TypeInfo::setLinkedVariable(bsl::optional<int> *variable)
{
    d_elemType                 = OptionType::e_INT;
    d_linkedVariable_p         = variable;
    d_isOptionalLinkedVariable = variable != 0;
    resetConstraint();
}

void TypeInfo::setLinkedVariable(bsl::optional<bsls::Types::Int64> *variable)
{
    d_elemType                 = OptionType::e_INT64;
    d_linkedVariable_p         = variable;
    d_isOptionalLinkedVariable = variable != 0;
    resetConstraint();
}

void TypeInfo::setLinkedVariable(bsl::optional<double> *variable)
{
    d_elemType                 = OptionType::e_DOUBLE;
    d_linkedVariable_p         = variable;
    d_isOptionalLinkedVariable = variable != 0;
    resetConstraint();
}

void TypeInfo::setLinkedVariable(bsl::optional<bsl::string> *variable)
{
    d_elemType                 = OptionType::e_STRING;
    d_linkedVariable_p         = variable;
    d_isOptionalLinkedVariable = variable != 0;
    resetConstraint();
}

void TypeInfo::setLinkedVariable(bsl::optional<bdlt::Datetime> *variable)
{
    d_elemType                 = OptionType::e_DATETIME;
    d_linkedVariable_p         = variable;
    d_isOptionalLinkedVariable = variable != 0;
    resetConstraint();
}

void TypeInfo::setLinkedVariable(bsl::optional<bdlt::Date> *variable)
{
    d_elemType                 = OptionType::e_DATE;
    d_linkedVariable_p         = variable;
    d_isOptionalLinkedVariable = variable != 0;
    resetConstraint();
}

void TypeInfo::setLinkedVariable(bsl::optional<bdlt::Time> *variable)
{
    d_elemType                 = OptionType::e_TIME;
    d_linkedVariable_p         = variable;
    d_isOptionalLinkedVariable = variable != 0;
    resetConstraint();
}

// ACCESSORS
bsl::shared_ptr<TypeInfoConstraint> TypeInfo::constraint() const
{
    return d_constraint_p;
}

void *TypeInfo::linkedVariable() const
{
    return d_linkedVariable_p;
}

bool TypeInfo::isOptionalLinkedVariable() const
{
    return d_isOptionalLinkedVariable;
}

OptionType::Enum TypeInfo::type() const
{
    return d_elemType;
}

                                  // Aspects

bslma::Allocator *TypeInfo::allocator() const
{
    return d_allocator_p;
}

bsl::ostream& TypeInfo::print(bsl::ostream& stream,
                              int           level,
                              int           spacesPerLevel) const
{
    BSLS_ASSERT(0 <= spacesPerLevel);

    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    if (level < 0) {
        level = -level;
    }
    else {
        bdlb::Print::indent(stream, level, spacesPerLevel);
    }

    stream << "{";
    bdlb::Print::newlineAndIndent(stream, ++level, spacesPerLevel);
    stream << "TYPE       " << type();
    if (linkedVariable()) {
        bdlb::Print::newlineAndIndent(stream, level, spacesPerLevel);
        stream << "VARIABLE   " << static_cast<void *>(linkedVariable());
    }
    if (constraint()) {
        bdlb::Print::newlineAndIndent(stream, level, spacesPerLevel);
        stream << "CONSTRAINT " << static_cast<void *>(constraint().get());
    }
    bdlb::Print::newlineAndIndent(stream, --level, spacesPerLevel);
    stream << "}";

    return stream;
}

}  // close package namespace

// FREE OPERATORS
bool balcl::operator==(const TypeInfo& lhs, const TypeInfo& rhs)
{
    return lhs.type()           == rhs.type()
        && lhs.linkedVariable() == rhs.linkedVariable()
        && lhs.constraint()     == rhs.constraint();
}

bool balcl::operator!=(const TypeInfo& lhs, const TypeInfo& rhs)
{
    return !(lhs == rhs);
}

bsl::ostream&
balcl::operator<<(bsl::ostream& stream, const TypeInfo& rhs)
{
    return rhs.print(stream);
}

namespace balcl {

                       // ------------------
                       // class TypeInfoUtil
                       // ------------------

bool TypeInfoUtil::satisfiesConstraint(const OptionValue& element,
                                       const TypeInfo&    typeInfo)
{
    BSLS_ASSERT(typeInfo.type() == element.type());

    return typeInfo.constraint()->validate(element);
}

bool TypeInfoUtil::satisfiesConstraint(const OptionValue& element,
                                       const TypeInfo&    typeInfo,
                                       bsl::ostream&      stream)
{
    BSLS_ASSERT(typeInfo.type() == element.type());

    return typeInfo.constraint()->validate(element, stream);
}

bool TypeInfoUtil::satisfiesConstraint(const void      *variable,
                                       const TypeInfo&  typeInfo)
{
    BSLS_ASSERT(variable);

    OptionValue value(typeInfo.type());
    u::OptionValueUtil::setValue(&value, variable);
    return typeInfo.constraint()->validate(value);
}

bool TypeInfoUtil::satisfiesConstraint(const void      *variable,
                                       const TypeInfo&  typeInfo,
                                       bsl::ostream&    stream)
{
    BSLS_ASSERT(variable);

    OptionValue value(typeInfo.type());
    u::OptionValueUtil::setValue(&value, variable);
    return typeInfo.constraint()->validate(value, stream);
}

bool TypeInfoUtil::parseAndValidate(OptionValue                *element,
                                    const bsl::string_view&     input,
                                    const TypeInfo&             typeInfo,
                                    bsl::ostream&               stream,
                                    TypeInfo::ParseInputSource  inputSource)
{
    BSLS_ASSERT(element);
    BSLS_ASSERT(element->type() == typeInfo.type());

    return typeInfo.constraint()->parse(element, stream, input, inputSource);
}

int TypeInfoUtil::tokenizeArrayEnvironmentVariable(
                                              bsl::vector<bsl::string> *tokens,
                                              const bsl::string_view&   input)
{
    // Implementation note: This tokenization function hardcodes ' ' (space) as
    // the array element separator character.  As is typical of white-space
    // characters, this is treated as a "soft delimiter", where as a visible
    // character is typically treated as a "hard delimiter" (see bdlb_tokenizer
    // for more informaiton on delimiter types).  As a result of using a soft
    // delimiter, zero-length tokens are not added to the output vector.

    BSLS_ASSERT(tokens->empty());

    static const char patternStr[] = { "\\ " };

    bsl::string_view remaining(input);
    bsl::string      nextToken(tokens->get_allocator());

    // This may be an over-estimate if some of the ' 's are escaped or
    // adjacent, but that's not really much of a problem.

    tokens->reserve(1 + bsl::count(input.cbegin(),
                                   input.cend(),
                                   ' '));

    while (true) {
        const bsl::size_t pos = remaining.find_first_of(patternStr);
        nextToken += remaining.substr(0, pos);

        if (u::npos == pos) {
            if (!nextToken.empty()) {
                tokens->push_back(u::MoveUtil::move(nextToken));
            }

            break;
        }

        if (' ' == remaining[pos]) {
            if (!nextToken.empty()) {
                tokens->push_back(u::MoveUtil::move(nextToken));
                nextToken.clear();
            }

            remaining = remaining.substr(pos + 1);
        }
        else {
            BSLS_ASSERT(remaining[pos] == '\\');
            if (remaining.size() <= pos + 1) {
                // incomplete escape sequence

                return -1;                                            // RETURN
            }

            const char escapedChar = remaining[pos + 1];
            if (escapedChar != '\\' && escapedChar != ' ') {
                // unsupported escape sequence

                return -1;                                            // RETURN
            }

            nextToken += escapedChar;
            remaining = remaining.substr(pos + 2);
        }
    }

    return 0;
}

}  // close package namespace
}  // close enterprise namespace

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
