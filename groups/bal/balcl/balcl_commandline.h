// balcl_commandline.h                                                -*-C++-*-
#ifndef INCLUDED_BALCL_COMMANDLINE
#define INCLUDED_BALCL_COMMANDLINE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide command line parsing, validation, and access.
//
//@CLASSES:
//               balcl::CommandLine: value-semantic command-line arguments
//  balcl::CommandLineOptionsHandle: references to parsed options
//
//@SEE_ALSO: balcl_optionvalue, balcl_optiontype
//
//@DESCRIPTION: This component provides a value-semantic class,
// `balcl::CommandLine`, used to represent the command-line arguments passed to
// a process.  Also provided is `balcl::CommandLineOptionsHandle`, an
// optionally-used class that provides an alternate means of access to the
// options (and associative values, if any) found in a `balcl::CommandLine`
// object in a "parsed" state.
//
// The constructor of `balcl::CommandLine` takes a specification describing the
// command-line arguments.  Once created, `printUsage` can be invoked to print
// the usage syntax.  The `parse` method takes command-line arguments and
// validates them against the specification supplied at construction, printing
// suitable messages on an optionally-specified stream in case of a parsing
// error.  Once parsed, options and values can be accessed using various access
// methods.  The class provides a set of *theType* access methods (for example,
// `theString`, `theInt`) that return the value of the specified option name.
// It is also possible to link a variable with an option in the specification;
// doing so will cause the variable to be loaded with the option value once
// `parse` has been invoked and was successful.
//
///Component Features Summary
///--------------------------
// This component offers the following features:
//
// 1. Validation of command-line arguments against the provided specification.
// 2. The ability to specify sophisticated constraints easily.  Users can also
//    build their own constraints and use them in the specification.
// 3. The ability to automatically generate usage syntax.
// 4. The ability to directly link a variable to an option.  Parsing a command
//    line loads such variables with the option value as specified on the
//    command line (or a default value if one is configured).  Linked variables
//    provide a sometimes convenient alternative to using the accessor methods.
//    See {Type-and-Constraint Field}.  Linked variables can be `bsl::optional`
//    objects for each of the scalar option types except `bool`.
// 5. The ability to access options and their corresponding values through
//    various accessor methods.
// 6. The ability to parse Unix-style command lines (for example, grouping of
//    flags, allowing any ordering of options, or a mix between options and
//    non-option arguments, short tag and long tag forms, etc.).
// 7. The ability to have multiple non-option arguments of possibly different
//    types.  Note that only the last non-option argument may be multi-valued,
//    and that if a non-option has a default value, then all subsequent
//    non-options must also have a default value.
// 8. The ability to specify option values through environment variables.
// 9. The ability to specify defaults for options, to be used when options are
//    not specified on the command line or through environment variables.
//
// A lower bound can be placed on the number of multi-valued non-option
// arguments (e.g., two or more values) can be achieved by explicitly
// specifying the required number of single-valued non-option arguments of the
// same type before the unrestricted multi-value non-option of that same type.
//
///Background for Unix-Style Command-Line Arguments and Definitions
///----------------------------------------------------------------
// This section provides background on Unix-style command-line arguments, as
// well as definitions of terms used frequently in this documentation (such as
// "option", "flag", "non-option", "tag", "short tag", "long tag").  Readers
// familiar with Unix command lines can skim this section or omit entirely.
//
// Command-line arguments can be classified as:
//
// * command name (there is only one, and it is always the first argument)
// * options (tags and associated values)
// * flags (boolean options)
// * non-option arguments
//
// For example, in the following command line:
// ```
// $ mybuildcommand -e -c CC64 myproject
// ```
// the command name is `mybuildcommand`.  There is one option, described by
// `-c CC64`: `c` is the tag name, and `CC64` is the option value.  There is
// also one boolean option (flag): `-e` is a flag, `e` is the flag name.  The
// last parameter, `myproject`, is a non-option argument.
//
// Sometimes *option* is also used where "flag" or "non-option" would be more
// accurate.  What is actually intended should be clear from context.
//
// A user specifies an option on a command line by entering one of the tag
// values configured for that option.  Each option has a mandatory long tag and
// an optional short tag.  The short tag, if specified, must be a single
// character; the long tag generally must follow the same rules applicable to
// C/C++ identifiers, except that `-` is allowed (but not as the leading
// character).  When a short tag is used on a command line, it must be preceded
// by `-`, and when a long tag is used it must be preceded by `--`.  Flags have
// no corresponding values; they are either present or absent.  Option tags
// *must* be followed by a corresponding option value.  An option can have
// multiple values (such options are called multi-valued options).  When
// multiple values are provided for an option, the tag must appear with each
// value (see the section {Multi-Valued Options and How to Specify Them}).
// Arguments that are not the command name, options, or flags are called
// "non-option" arguments and can be either single-valued or multi-valued.
// They do not have any tag associated with them.
//
// Consider the syntax of a typical Unix-style command whose options are
// described by the usage string:
// ```
// usage: mysort [-r|reverse] [-o|outputfile <outfile>] files...
// ```
// Here:
// ```
// '-r|reverse' is a flag: 'r' is the short tag, 'reverse' is the long tag.
//
// '-o|outputfile' is an option: 'o' is the short tag, 'outputfile' is the
//                 long tag.  The value is parsed from the string 'outfile'.
//
// 'files...' describes the multi-valued non-option argument.
// ```
// The command can be invoked as follows:
// ```
// $ mysort -r -o myoutfile file1 file2 file3
// ```
// and an equivalent command line is:
// ```
// $ mysort --reverse --outputfile myoutfile file1 file2 file3
// ```
// Note that short tags must be prepended with `-` and long tags with `--`.  To
// specify a non-option argument beginning with `-`, use a single `--`
// separator (not followed by a long tag).
//
///Specifying Option Values
///------------------------
// This component supports a variety of forms for specifying option values.
// They are best described by example.  Consider the command-line specification
// described by the following usage string:
// ```
// usage: myserver [-p|port <portNumber>]
// ```
// The following (valid) forms can be used with equivalent meaning:
// ```
// $ myserver -p 13
// $ myserver -p=13  # option value is '13' and *not* '=13'
// $ myserver -p13
// $ myserver --port 13
// $ myserver --port=13
// ```
// Note that if `=13` is desired as an option value, then whitespace must be
// used as in:
// ```
// $ myserver -p =13  # option value *is* '=13'
// ```
// All of the following are invalid:
// ```
// $ myserver -port13
// $ myserver --p 13
// $ myserver -port 13
// ```
//
///Flag Grouping
///-------------
// Flags can be grouped (i.e., expressed more succinctly like `-ctv` instead of
// `-c -t -v`).  While grouping flags, short tags must be used.  For example,
// given the command-line specification described by the following usage
// string:
// ```
// usage: mysort [-r|reverse] [-i|insensitiveToCase] [-u|uniq]
// ```
// the following command lines are valid and equivalent:
// ```
// $ mysort -r -i -u
// $ cmd -riu
// $ cmd -uir
// $ cmd -i -ru
// ```
// Note that the last character in a group need not be a flag; it could be an
// option.  Any character that is the short tag of an option signals the end of
// the flag group, and it must be followed by the value of the option.  For
// example, given the command-line specification described by the following
// usage string:
// ```
// usage: mysort [-r|reverse] [-i|insensitiveToCase] [-u|uniq]
//               [-o|outfile <outputfile>]
// ```
// the following command lines are valid and equivalent:
// ```
// $ mysort -riu -o myoutfile
// $ mysort -riuo myoutfile
// $ mysort -riuomyoutfile
// $ mysort -riuo=myoutfile
// ```
//
///Multi-Valued Options and How to Specify Them
///--------------------------------------------
// Options can have several values.  For example, in the command-line
// specification described by the following usage string, `*` denotes a
// multi-valued option, and `+` denotes a multivalued option that must occur at
// least once.
// ```
// usage: mycompiler [-l|library <libName>]* [-o|out outFile] [<object>]+
// ```
// multiple values can be given as follows:
// ```
// $ mycompiler -l lib1 -l lib2 -l lib3 -o main a.o b.o
// ```
// They need not be supplied contiguously.  For example, the following command
// line is valid and equivalent to the above:
// ```
// $ mycompiler -l lib1 -o main -l lib2 -l lib3 a.o b.o
// ```
// Note that the tag needs to be repeated for every option value.  For example,
// the following command line is invalid (because `-l` must be repeated before
// both `lib2` and `lib3`):
// ```
// $ mycompiler -l lib1 lib2 lib3 -o main a.o b.o
// ```
// Short and long forms can be used in mixed fashion, however:
// ```
// $ mycompiler -l lib1 -o main --library lib2 -l lib3 a.o b.o
// ```
//
///Order of Arguments
///------------------
// Command-line arguments can appear in any order.  For example, given the
// command-line specification described by the following usage string:
// ```
// usage: mysort [-r|reverse] [-o|outputfile <outfile>] [<file>]+
// ```
// all the following command lines are valid (and equivalent):
// ```
// $ mysort -r -o myoutfile file1 file2 file3
// $ mysort file1 file2 file3 -r -o myoutfile
// $ mysort file1 -o myoutfile file2 -r file3
// $ mysort file1 -o=myoutfile file2 -r file3
// ```
// There are three exceptions to the above rule on argument order:
//
// 1. An option tag must be followed by that option's value, if any (either in
//    the next argument, or in the same argument using "=value").
// 2. When a non-option argument starts with a `-` then it must not appear
//    before any option or flag *and* a `--` must be put on the command line to
//    indicate the end of all options and flags.
// 3. Non-option arguments are parsed and assigned in the order they appear on
//    the command line.
//
// For example, the following is invalid because of rule (1) above (because
// `-o` should be followed by `myoutfile`):
// ```
// $ mysort -o -r myoutfile file1 file2 file3
// ```
// and the following is incorrect because of rule (2) (because
// `-weirdfilename`, which starts with `-`, must appear after `--`):
// ```
// $ mysort -weirdfilename file2 file3 -r -o myoutfile
// ```
// The previous examples can be corrected in either of the following ways:
// ```
// $ mysort -r -o myoutfile -- -weirdfilename file2 file3
// $ mysort file2 file3 -r -o myoutfile -- -weirdfilename
// ```
// Note that the order of values within the sequence of multi-valued non-option
// arguments differs in both examples, as per rule (3).  In the first example,
// the non-option arguments have for a value the (ordered) sequence:
// ```
// -weirdfilename file2 file3
// ```
// while in the second example, the non-option argument value is the sequence:
// ```
// file2 file3 -weirdfilename
// ```
// This order may or may not matter to the application.
//
///Setting Options via Environment Variables
///-----------------------------------------
// Sometimes users may wish to supply configuration via environment variables,
// which is common practice, for example, for services deployed via Docker
// containers in cloud environments.
//
// By default, parsed command line option values are unaffected by the
// environment.  If an environment variable name is specified for an option,
// then, during parsing, if an option is not set on the command line, and the
// specified environment variable has a value, that value is used for the
// command line option.  If a value is not supplied, either via the command
// line, or via an environment variable, then the default value for the option,
// if any, is used.
//
///Setting Boolean Options via Environment Variables
///- - - - - - - - - - - - - - - - - - - - - - - - -
// Boolean options when set via the environment variable must be associated
// with a string.
// ```
// +---------------+--------------+
// | desired value |  valid text  |
// +---------------+--------------+
// |     true      | "true",  "1" |
// |     false     | "false", "0" |
// +---------------+--------------+
// ```
// Any other text, including an empty string, is treated as invalid input.
//
///Setting Array Options via Environement Variables
/// - - - - - - - - - - - - - - - - - - - - - - - -
// Array options can be set by environment variable.  When an array option is
// supplied via an environment variable string, the discrete values must be
// separated using the space character.  For example, if the program takes an
// integer array of years as an option, and that option is associated with the
// environment variable name "MYPROG_YEARS", to supply a value via the command
// line one could specify:
// ```
// $ export MYPROG_YEARS="2010 2014 2018 2022"
// ```
// The '\' is used as an escape-character if an element of an array contains a
// space.  For example:
// ```
// $ export MYPROG_WINPATH='C:\\Program\ Files\\MyProg D:\\Another\\Path'
// ```
// would configure an array containing "C:\Program Files\MyProg" and
// "D:\Another\Path".
//
///Best Practices for Environment Variable Naming
/// - - - - - - - - - - - - - - - - - - - - - - -
// Environment variable exist in a single namespace accessed by all programs
// run in the shell, and many environment variable names are used by many
// different systems for different purposes.  For that reason, we strongly
// encourage selecting a (ideally) unique prefix for the environment variables
// used by your application, to reduce the likelihood of collisions with
// environment variables used by other applications for different purposes.
//
// For example, if someone were writing a trading application `tradesvc`, they
// might choose to use "TRADESVC_" as a common prefix for the environment
// variables.
//
///Specifying Options
///------------------
// A command line is described by an *option* *table* (supplied as an array of
// `balcl::OptionInfo`).  Each entry (row) of the table describes an option
// (i.e., an option, flag, or non-option argument).  Each entry has several
// fields, specified in the following order:
// ```
// Field name                     Main purpose (see below for more details)
// ============================   ===========================================
// tag field                      Specify tags (short and long) for options
//                                and flags.  A non-option argument is
//                                indicated by an empty string.
//
// name field                     Specify the name through which the option
//                                value can be accessed.
//
// description field              Used in printing usage.
//
// type-and-constraint field      (1) Specify the type of the option value(s).
//                                (2) Specify a variable to be linked to the
//                                    option.
//                                (3) Specify other constraints on individual
//                                    value(s).
//
// occurrence information field   (1) Specify a default value for an option.
//                                (2) Specify whether an option is required on
//                                    the command line or is optional (by
//                                    default, an option is optional).
//
// environment variable name      Specify an environment variable name that
//                                can be used to set the option.
// ```
// The first three fields *must* be specified.  The type-and-constraint field
// can be omitted (meaning no constraint), and the occurrence information field
// likewise can be omitted (meaning that the option is not required on the
// command line).
//
// The following sections provide a more detailed description of each field,
// including example values for each field.
//
///`balcl::OptionInfo` versus `balcl::Option`
/// - - - - - - - - - - - - - - - - - - - - -
// In some applications, command-line specifications *must* *be* defined using
// a statically-initialized array.  For that reason, there are two classes that
// serve the same purpose: `balcl::OptionInfo` is a statically-initializable
// class but it does not conform to the `bslma` allocator protocol, while
// `balcl::Option` is convertible from `balcl::OptionInfo`, takes allocators,
// and is suitable for storing into containers.
//
///Tag Field
///- - - - -
// The tag field specifies the (optional) short tag and long tag for the
// corresponding option or flag, except that non-option arguments are indicated
// by an empty string for a tag field.  There can only be one multi-valued
// entry for non-option arguments, and it must be listed last among the
// non-options.
//
// The general format is either:
// 1. "" (empty string) for non-option arguments;
// 2. "<s>|<long>" for options and flags, where `<s>` is the short tag, and
//    `<long>` is the long tag; or
// 3. "<long>" for options and flags where a short tag is not specified.
//
// Note that for short tags (`<s>`), `s` must be a single character (different
// from `-` and `|`); for long tags ("<long>"), `long` must have 2 or more
// characters (which may contain `-`, but not as the first character, and
// cannot contain `|`).  Also note that either no tag (empty string), both
// short and long tags, or only a long tag, may be specified.
//
// The tag field cannot be omitted, but it can be the empty string.
//
///Name Field
/// - - - - -
// The name field specifies the name through which the option value can be
// accessed either through one of the *theType* methods.
//
// The general format is any non-empty string.  In most cases, the name will be
// used as-is.  Note that any suffix starting at the first occurrence of `=`,
// if any, is removed from the name before storing in the `balcl::Option`.
// Thus, if a name having such a suffix is specified in a `balcl::OptionInfo`
// (e.g., "nameOption=someAttribute"), the correct name to use for querying
// this option by name does not include the suffix (e.g.,
// `cmdLine.numSpecified("nameOption=someAttribute")` will always return 0, but
// `cmdLine.numSpecified("nameOption")` will return the appropriate value).
//
// This field cannot be omitted, and it cannot be an empty string.
//
///Description Field
///- - - - - - - - -
// The description field is used when printing the usage string.
//
// The general format is any non-empty string.
//
// This field cannot be omitted, and it cannot be an empty string.
//
///Type-and-Constraint Field
///- - - - - - - - - - - - -
// The type-and-constraint field specifies the type and constraints for the
// option values.  Flags are identified by having the boolean type; note that
// flags cannot have constraints.  Multiple values (for multi-valued options
// and multi-valued non-option arguments) can be specified by using array
// types.  The list of the supported types is provided in the section
// [](#Supported Types) below.
//
// Other constraints can be placed on individual value(s).  When the type is an
// array type, then those constraints are placed on the individual value(s)
// held in the array and not on the entire array.  A list of useful constraints
// is provided in the section [](#Supported Constraint Values).  Also see the
// section [](#Building New Constraints) to see how new constraints can be built
// so that they can be used in the same manner as the available constraints.
//
// Additionally, this field allows a specified variable to be linked to the
// option.  In that case, after parsing, the variable is loaded with the option
// value specified on the command line (or its default value, if any, if the
// option is absent from the command line).  [](#Occurrence Information Field)
// describes how to configure a default value.
//
// The general format can be one of either:
// ```
// /// Link the option with the specified `variable`.  Note that the option
// /// type is inferred from the type of `variable`.  Optionally place the
// /// user-specified `constraint`, of a type defined in
// /// `balcl::Constraint`, on the value.
// balcl::TypeInfo(&variable)
// balcl::TypeInfo(&variable, constraint)
//
// /// Specify the type of this option to be of the specified `type`.
// /// Optionally place the user-specified `constraint`, of a type defined
// /// in `balcl::Constraint`, on the value.  Don't link this
// /// option with any variable.  `type` must be one of the static
// /// variables (null pointers) listed in [](balcl_optiontype#Enumerators).
// balcl::TypeInfo(type)
// balcl::TypeInfo(type, constraint)
// ```
// This field can be omitted.  If so, the type is assumed to be of string type
// with no constraints and no variable is linked to the option.  No occurrence
// information field can then be specified; if such a field is desired, then
// the type-and-constraint field needs to be set explicitly.
//
///Linked Variables
///-  -  -  -  -  -
// Linked variables are updated by the `parse` method of `balcl::CommandLine`
// should that method determine a value for an option; otherwise, the linked
// variable is left unchanged.  The value for an option is determined either
// from the command-line arguments passed to `parse` or obtained from a
// pre-configured default value, if any (see
// [](#Occurrence Information Field)).
//
// Linked variables can be `bsl::optional` objects that wrap any of the
// non-array option types except for `bool` (see [](#Supported Types)).  Also,
// a link to a `bsl::optional` object is disallowed if the option is "required"
// or has a default value (see [](#Occurrence Information Field)).
//
///Occurrence Information Field
/// - - - - - - - - - - - - - -
// The occurrence information field is used to specify a default value for an
// option, and whether an option is required on the command line or is
// optional.  An option may also be "hidden" (i.e., not displayed by
// `printUsage`).
//
// The general format of this field is one of the following:
// ```
// balcl::OccurrenceInfo::e_HIDDEN
// balcl::OccurrenceInfo::e_OPTIONAL
// balcl::OccurrenceInfo::e_REQUIRED
// a default value
// ```
// If a default value is specified, the option is assumed to be optional; in
// addition, the default value must satisfy the type and constraint indicated
// by the specified type-and-constraint field.
//
// This field can be omitted, and is always omitted if the type-and-constraint
// field is not specified.  If omitted, the option is not required on the
// command line and has no default value; furthermore, if the option is not
// present on the command line, the linked variable, if any, is unaffected.
//
///Environment Variable Name Field
///- - - - - - - - - - - - - - - -
// The environment variable name field is used to specify a string which is the
// name of an environment variable which, if set, allows the environment to be
// searched for a value of the option.  If an option is specified on the
// command line, the environment is never searched for that option.  If an
// option has a default value, and a value is specified in the environment, the
// value in the environment takes precedence over the default value.
//
///Example Field Values
///--------------------
// The following tables give examples of field values.
//
///Example: Tag Field
/// - - - - - - - - -
// The tag field may be declared using the following forms:
// ```
//    Usage                              Meaning
// ==============    =======================================================
// "o|outputfile"    The option being defined is either an option or a flag.
//                   The short tag is "o" and the long tag is "outputfile".
//
//                   Note that "o" alone is invalid since a long tag must be
//                   specified.
//
// "outputfile"      The option being defined is either an option or a flag.
//                   There is no short tag and the long tag is "outputfile".
//
//     ""            Specifies a non-option argument.
// ```
//
///Example: Name Field
///- - - - - - - - - -
// The name field may be declared using the following form:
// ```
//     Usage                              Meaning
// ===============   =======================================================
//    "xyz"          The option value can be accessed by "xyz".
// ```
//
///Example: Type-and-Constraint Field
/// - - - - - - - - - - - - - - - - -
// Suppose, for example, that our application has the following parameters:
// ```
// int                      portNum;   // a variable to be linked to an
//                                     // option
//
// bool                     isVerbose; // a variable to be linked to a flag
//
// bsl::vector<bsl::string> fileNames; // a variable to be linked to a
//                                     // multi-valued option
// ```
// The type and constraint fields may be declared using the following values:
// ```
//    Usage                               Meaning
// ===============   =======================================================
// balcl::TypeInfo(&portNum)
//                   (1) Link the variable 'portNum' with this option value.
//                   That is, after successful parsing, this variable will
//                   contain the option value specified on the command line.
//                   (2) An integer value must be provided on the command
//                   line for this option (the type 'int' is inferred
//                   implicitly from the type of 'portNum').
//
// balcl::TypeInfo(balcl::OptionType::k_INT)
//                   This option value must be an integer.  Since there is no
//                   linked variable, the value of the option must be obtained
//                   using the field name passed to the 'the<TYPE>' accessor
//                   (in this case 'theInt').
//
// balcl::TypeInfo(&isVerbose)
//                   Load the variable 'isVerbose' with this option value.
//
// balcl::TypeInfo(balcl::OptionType::k_BOOL)
//                   This option is a flag.
//
// balcl::TypeInfo(&fileNames)
//                   Load the variable 'fileNames' with the values specified
//                   for this multi-valued option (or multi-valued
//                   non-option argument).  That is, after successful parsing,
//                   this variable will contain the sequence of values
//                   specified on the command line, in the same order.
//
// balcl::TypeInfo(balcl::OptionType::k_STRING_ARRAY)
//                   This option value consists of a sequence of string
//                   values specified on the command line, in the same order.
// ```
//
///Example: Occurrence Information Field
///- - - - - - - - - - - - - - - - - - -
// The following values may be used for this field:
// ```
//    Usage                               Meaning
// ===============   ========================================================
// balcl::OccurrenceInfo::e_REQUIRED
//                   The value(s) for this option *must* be provided on the
//                   command line.  For options that are of an array type, at
//                   least one value must be provided.  Omission manifests
//                   as a parse error.
//
// balcl::OccurrenceInfo::e_OPTIONAL
//                   Value(s) for this option may be omitted on the command
//                   line.
//
// balcl::OccurrenceInfo::e_HIDDEN
//                   Same as 'e_OPTIONAL'; in addition, this option will not
//                   be displayed by 'printUsage'.
//
// 13                The default value for this option is 13 and the option
//                   is not required on the command line.  If no value is
//                   provided, then 13 is used.  If the type described by the
//                   type-and-constraint field is not integer, then it is an
//                   error.
// ```
// *Note*: If an option is optional *and* no value is provided on the command
// line or through an environment variable, `isSpecified` will will return
// `false and `numSpecified' will return 0.  If no default value is provided
// and if the variable is a linked variable, it will be unmodified by the
// parsing.  If the variable is accessed through
// `CommandLineOptionsHandle::value`, it will be in a null state (defined type
// but *no* defined value).
//
///Example: Environment Variable Name Field
/// - - - - - - - - - - - - - - - - - - - -
// The name field may be declared using the following form:
// ```
//     Usage                              Meaning
// ===============   =======================================================
//   "MYPROG_XYZ"    The option can be set by "export MYPROG_XYZ=<value>"
// ```
//
///Supported Types
///---------------
// The following types are supported.  The type is specified by an enumeration
// value (see {`balcl_optiontype`}) supplied as the first argument to:
// ```
// balcl::TypeInfo(type, constraint)
// ```
// which is used to create the type-and-constraint field value in the
// command-line specification.  When the constraint need only specify the type
// of the option value (i.e., no linked variable or programmatic constraint),
// one can supply any of the public data members of `balcl::OptionType` shown
// below:
// ```
// Type                            Specifier
// -----------------------------   -------------------------
// bool                            OptionType::k_BOOL
// char                            OptionType::k_CHAR
// int                             OptionType::k_INT
// bsls::Types::Int64              OptionType::k_INT64
// double                          OptionType::k_DOUBLE
// bsl::string                     OptionType::k_STRING
// bdlt::Datetime                  OptionType::k_DATETIME
// bdlt::Date                      OptionType::k_DATE
// bdlt::Time                      OptionType::k_TIME
// bsl::vector<char>               OptionType::k_CHAR_ARRAY
// bsl::vector<int>                OptionType::k_INT_ARRAY
// bsl::vector<bsls::Types::Int64> OptionType::k_INT64_ARRAY
// bsl::vector<double>             OptionType::k_DOUBLE_ARRAY
// bsl::vector<bsl::string>        OptionType::k_STRING_ARRAY
// bsl::vector<bdlt::Datetime>     OptionType::k_DATETIME_ARRAY
// bsl::vector<bdlt::Date>         OptionType::k_DATE_ARRAY
// bsl::vector<bdlt::Time>         OptionType::k_TIME_ARRAY
// ```
// The ASCII representation of these values (i.e., the actual format of the
// values on command lines) depends on the type:
// * Numeric Values: see {`bdlb_numericparseutil`}.
// * Date/Time Values: see {`bdlt_iso8601util`}.
//
///Supported Constraint Values
///---------------------------
// This component supports constraint values for each supported type except
// `bool`.  Specifically, the utility `struct` `balcl::Constraint` defines
// `TYPEConstraint` types (for instance, `StringConstraint`, `IntConstraint`)
// that can be used to define a constraint suitable for the `balcl::TypeInfo`
// class.
//
///Building New Constraints
///------------------------
// A constraint is simply a function object that takes as its first argument
// the (address of the) data to be constrained and as its second argument the
// stream that should be written to with an appropriate error message when the
// data does not follow the constraint.  The functor should return a `bool`
// value indicating whether or not the data abides by the constraint (with
// `true` indicating success).  A constraint for a given option whose value has
// the given type must be convertible to one of the `TYPEConstraint` types
// defined in the utility `struct` `balcl::Constraint`.  Note that when passing
// a function as a constraint, the *address* of the function must be passed.
//
///Valid `balcl::OptionInfo` Specifications
///----------------------------------------
// The `balcl::CommandLine` class has a complex set of preconditions on the
// option specification table (array of `balcl::OptionInfo` objects) passed to
// each of its constructors.  There are requirements on individual elements, on
// elements relative to each other, and on the entire set of elements.  If
// these preconditions are not met, the behavior of the constructor is
// undefined.
//
// The preconditions (some previously mentioned) are given in their entirety
// below.  Moreover, an overloaded class method,
// `balcl::CommandLine::isValidOptionSpecification`, is provided to allow
// programmatic checking without risk of incurring undefined behavior.
//
///Tag/Name/Description Fields
///- - - - - - - - - - - - - -
// The tag, name, and description fields must pass the `isTagValid`,
// `isNameValid, and `isDescriptionValid' methods of `balcl::Option`,
// respectively.
//
// * The tag field:
//   - If empty (a non-option argument), the option must not be a flag.
//   - If non-empty, see {Tag Field} above for details.
// * The name field must be non-empty.
// * The description field must be non-empty.
//
// Collectively, each non-empty short tag, each long tag, and each name must be
// unique in the specification.
//
///Default Values
/// - - - - - - -
// * Default values are disallowed for flags.
// * The type of a default value must match the type of its option.
// * The default value must satisfy the user-defined constraint on the option
//   value, if any.
//
///Non-Option Arguments
/// - - - - - - - - - -
// * Cannot be a flag (see {Tag/Name/Description Fields}).
// * Cannot be a hidden option.
// * Only the last non-option argument can be multi-valued (i.e., an array
//   type).
// * If a non-option argument has a default value, all subsequent non-option
//   arguments must also have default values.
//
///Boolean Options (Flags)
///- - - - - - - - - - - -
// Options having the `bool` type (also known as "flags") are distinguished
// from the other supported option types in several ways:
//
// * The command-line syntax does not allow an explicit value for flag types
//   (e.g., `-x=true` and `-xFlag false` are disallowed when setting booleans
//   on the command line).  The presence or absence of the tag name (either
//   long or short form) on the command line determines the value of the
//   option.
// * If providing a boolean option through an environment variable, an
//   explicit value is required, and that value must be either "true", "1",
//   "false", or "0" -- any other value, including the empty string, is
//   invalid (see {Setting Boolean Options via Environment Variables}).
// * The boolean option type has no "array" form (i.e., there is no
//   `BoolArray` option type).
//   - Multiple instances of the tag for a boolean option are allowed on the
//     command line (e.g., `-x -x -x`).  For other option types, specifying
//     multiple tags for a non-array option is an error.
//   - Multiple instances of a boolean tag idempotently set the flag to
//     `true`; however, one can determine the number of appearances by using
//     the `position` accessor.
// * A boolean option type cannot be configured to:
//   - have a default value, or
//   - have a constraint, or
//   - be linked to a `bsl::optional<bool>` variable.
// * A boolean option type *can* be configured to be a required parameter;
//   however, that combination defeats the purpose of having a flag option so
//   the requirement that the flag appear on the command line is *not*
//   enforced.
// * The `theBool` method returns the same value (`true` or `false`) as the
//   `isSpecified` method.  In contrast, the `the*` accessor methods for the
//   other option types have a precondition such that either `isSpecified()`
//   must be `true` or the option must have a default value.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Parsing Command Line Options Using Minimal Functionality
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to design a sorting utility named `mysort` that has the
// following syntax:
// ```
// Usage: mysort [-r|reverse] [-f|field-separator <fieldSeparator>]
//               -o|outputfile <outputFile> [<fileList>]+
//                           // Sort the specified files (in 'fileList'), and
//                           // write the output to the specified output file.
// ```
// The `<fileList>` argument is a `non-option`, meaning that its value or
// values appear on the command line unannounced by tags.  In this case, the
// `+` following the argument means that it is an array type of argument where
// at least one element is required, so its values are stored in a
// `bsl::vector`.
// ```
// int main(int argc, const char **argv)
// {
//     using balcl::TypeInfo;
//     using balcl::OccurrenceInfo;
// ```
// First, we define our variables to be initialized from the command line.  All
// values must be initialized to their default state:
// ```
//     bool reverse = false;
//     bsl::string outputFile;
//     char fieldSeparator = '|';
//     bsl::vector<bsl::string> files;
// ```
// Then, we define our `OptionInfo` table of attributes to be set.  The fields
// of the `OptionInfo` are:
//
// * tag - the tag on the command line for supplying the option
// * name - a one word description of the value to provide for the option
// * description - a short body of text describing the purpose of the option
//   etc.
// * TypeInfo - information about the type of the input expected (possibly the
//   variable in which to load the value)
// * OccurenceInfo - (optional) whether the option is required, optional, or
//   hidden (where hidden means hidden from the help text)
// * environment variable name - (optional) the name of an environment
//   variable to use for the option if the option is not provided on the
//   command line
// ```
//     static const balcl::OptionInfo specTable[] = {
//       {
//         "r|reverse",                                   // tag
//         "isReverse",                                   // name
//         "sort in reverse order",                       // description
//         TypeInfo(&reverse),                            // link
//         OccurrenceInfo(),                              // occurrence
//         ""                                             // env var name
//       },
//       {
//         "f|field-separator",                           // tag
//         "fieldSeparator",                              // name
//         "field separator character",                   // description
//         TypeInfo(&fieldSeparator),                     // link
//         OccurrenceInfo(),                              // occurrence
//         ""                                             // env var name
//       },
//       {
//         "o|outputfile",                                // tag
//         "outputFile",                                  // name
//         "output file",                                 // description
//         TypeInfo(&outputFile),                         // link
//         OccurrenceInfo::e_REQUIRED,                    // occurrence info
//                                                        // (not optional)
//         ""                                             // env var name
//       },
//       {
//         "",                                            // non-option
//         "fileList",                                    // name
//         "input files to be sorted",                    // description
//         TypeInfo(&files),                              // link
//         OccurrenceInfo::e_REQUIRED,                    // occurrence info
//                                                        // (at least one
//                                                        // file required)
//         ""                                             // env var name
//       }
//     };
// ```
// Now, we create a balcl command-line object, supplying it with the spec table
// that we have just defined:
// ```
//     balcl::CommandLine cmdLine(specTable);
// ```
// Parse the options and if an error occurred, print a usage message describing
// the options:
// ```
//     if (cmdLine.parse(argc, argv)) {
//         cmdLine.printUsage();
//         return -1;                                                // RETURN
//     }
// ```
// If there are no errors in the specification table and correct arguments are
// passed, `parse` will set any variables that were specified on the command
// line, return 0 and there will be no output.
//
// Finally, we show what will happen if `mysort` is called with invalid
// arguments.  We will call without specifying an input file to `fileList`,
// which will be an error.  `parse` streams a message describing the error and
// then returns non-zero, so our program will call `cmdLine.printUsage`, which
// prints a detailed usage message.
// ```
// $ mysort -r -o sorted.txt
// Error: No value supplied for the non-option argument "fileList".
//
// Usage: mysort [-r|reverse] [-f|field-separator <fieldSeparator>]
//               -o|outputfile <outputFile> [<fileList>]+
// Where:
//   -r | --reverse
//           sort in reverse order
//   -f | --field-separator  <fieldSeparator>
//           field separator character
//   -o | --outputfile       <outputFile>
//           output file
//                           <fileList>
//           input files to be sorted
// ```
//
///Example 2: Accessing Option Values Through `balcl::CommandLine` Accessors
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Imagine we defined the same `mysort` program with the same options.  After a
// successful `parse`, `balcl::Commandline` makes the state of every option
// available through accessors (in addition to setting external variables as
// shown in example 1).
//
// For every type that is supported, there is a `the<TYPE>` accessor which
// takes a single argument, the name of the argument.  In the above program, if
// parsing was successful, the following asserts will always pass:
// ```
//     assert(cmdLine.theBool("isReverse")       == reverse);
//     assert(cmdLine.theString("outputFile")    == outputFile);
//     assert(cmdLine.theStringArray("fileList") == files);
// ```
// The next accessors we'll discuss are `isSpecified` and `numSpecified`.
// Here, we use `isSpecified` to determine whether "fieldSeparator" was
// specified on the command line, and we use `numSpecified` to determine the
// number of times the "fieldSeparator" option appeared on the command line:
// ```
//     if (cmdLine.isSpecified("fieldSeparator")) {
//         const unsigned char uc = cmdLine.theChar("fieldSeparator");
//         if (!::isprint(uc)) {
//             bsl::cerr << "'fieldSeparator' must be printable.\n";
//
//             return -1;                                            // RETURN
//         }
//     }
//
//     if (1 < cmdLine.numSpecified("fieldSeparator")) {
//         bsl::cerr <<
//                  "'fieldSeparator' may not be specified more than once.\n";
//
//         return -1;                                                // RETURN
//     }
// ```
//
///Example 3: Default Values and Specifying Values Via The Environment
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are implementing `mysort` (from examples 1 & 2) again, but here
// we want to make use of default option values and the ability to supply
// options via the environment.
//
// In this example, we have decided not to link local variables, and instead
// access the option values via the `balcl::CommandLine` object.  Since we are
// not linking local variables, we specify `OptionType::k_<TYPE>` in the
// specification table below for each `TypeInfo` field.
//
// To specify default values, we pass the default value to the `OccurrenceInfo`
// field.  Boolean options always have a default value of `false`.
//
// We also choose to allow these options to be supplied through the
// environment.  To enable this, we specify an environment variable name as the
// 5th (optional) element of the `OptionInfo` specification for the option.  If
// no name is supplied for the environment variable, the option cannot be set
// via the environment.
//
// First, in `main`, we define our spec table:
// ```
// using balcl::TypeInfo;
// using balcl::OptionType;
// using balcl::OccurrenceInfo;
//
// // option specification table
// static const balcl::OptionInfo specTable[] = {
//   {
//     "r|reverse",                             // tag
//     "isReverse",                             // name
//     "sort in reverse order",                 // description
//     TypeInfo(OptionType::k_BOOL),            // type
//     OccurrenceInfo::e_OPTIONAL,              // optional
//     "MYSORT_REVERSE"                         // env var name
//   },
//   {
//     "",                                      // non-option
//     "fileList",                              // name
//     "input files to be sorted",              // description
//     TypeInfo(OptionType::k_STRING_ARRAY),    // type
//     OccurrenceInfo::e_REQUIRED,              // at least one file required
//     "MYSORT_FILES"                           // env var name
//   }
// };
//
// int main(int argc, const char **argv)
// {
// ```
// Then, we declare our `cmdLine` object.  This time, we pass it a stream, and
// messages will be written to that stream rather than `cerr` (the default).
// ```
//     balcl::CommandLine cmdLine(specTable, bsl::cout);
// ```
// Next, we call `parse` (just like in Example 1):
// ```
//     if (cmdLine.parse(argc, argv)) {
//         cmdLine.printUsage();
//         return -1;                                                // RETURN
//     }
// ```
//
// `balcl::CommandLine` uses the following precedence to determine the value of
// a command line option:
//
// 1. Use the option value on the command-line (if one was supplied)
// 2. Use the option value supplied by an environment variable (if one was
//    supplied)
// 3. Use the default value (if one was supplied, or `false` for booleans)
//
// Finally, if an option value is not supplied by either the command line or
// environment, and there is no default value, any linked variable will be
// unmodified, `cmdLine.hasValue` for the option will return `false`, and the
// behavior is undefined if `cmdLine.the<TYPE>` for the option is called.
//
// Note that `cmdLine.isSpecified` will be `true` only if an option was
// supplied by the command line or the environment.
//
// If an array options is set by an environment variable, the different
// elements of the array are separated by spaces by default.
//
// All these calling sequences are equivalent:
// ```
// $ mysort -r inputFile1 inputFile2 inputFile3
// ```
// or
// ```
// $ mysort inputFile1 --reverse inputFile2 inputFile3
// ```
// or
// ```
// $ mysort inputFile1 inputFile2 inputFile3 -r
// ```
// or the user can specify arguments through environment variables:
// ```
// $ export MYSORT_REVERSE=true
// $ export MYSORT_FILES="inputFile1 inputFile2 inputFile3"
// $ mysort
// ```
// or as a combination of command line arguments and environment variables:
// ```
// $ export MYSORT_FILES="inputFile1 inputFile2 inputFile3"
// $ mysort -r
// ```
// The '\' character is used as an escape character for array values provided
// via an environment variable.  So, for example, if we needed to encode file
// names that contain a space (` `), which is the element separator (by
// default), we would use "\ ":
// ```
// $ export MYSORT_FILES='C:\\file\ name\ 1 C:\\file\ name\ 2'
// ```
// Notice we used a single tick to avoid requiring a double escape when
// supplying the string to the shell (e.g., avoiding "C:\\\\file\\ name\\ 1").
//
///Example 4: Option Constraints
///- - - - - - - - - - - - - - -
// Suppose, we are again implementing `mysort`, and we want to introduce some
// constraints on the values supplied for the variables.  In this example, we
// will ensure that the supplied input files exist and are not directories, and
// that `fieldSeparator` is appropriate.
//
// First, we write a validation function for the file name.  A validation
// function supplied to `balcl::CommandLine` takes an argument of a const
// pointer to the input option type (with the user provided value) and a stream
// on which to write an error message, and the validation function returns a
// `bool` that is `true` if the option is valid, and `false` otherwise.
//
// Here, we implement a function to validate a file name, that returns `true`
// if the file exists and is a regular file, and `false` otherwise (writing an
// description of the error to the `stream`):
// ```
// bool isValidFileName(const bsl::string *fileName, bsl::ostream& stream)
// {
//     if (!bdls::FilesystemUtil::isRegularFile(*fileName, true)) {
//         stream << "Invalid file: " << *fileName << bsl::endl;
//
//         return false;                                             // RETURN
//     }
//
//     return true;
// }
// ```
// Then, we also want to make sure that the specified `fieldSeparator` is
// a non-whitespace printable ascii character, so we write a function for that:
// ```
// bool isValidFieldSeparator(const char    *fieldSeparator,
//                            bsl::ostream&  stream)
// {
//     const unsigned char uc = *fieldSeparator;
//     if (::isspace(uc) || !::isprint(uc)) {
//         stream << "Invalid field separator specified." << bsl::endl;
//
//         return false;                                             // RETURN
//     }
//
//     return true;
// }
// ```
// Next, we define `main` and declare the variables to be configured:
// ```
// int main(int argc, const char **argv)
// {
//     using balcl::Constraint;
//     using balcl::OptionType;
//     using balcl::OccurrenceInfo;
//     using balcl::TypeInfo;
//
//     bool                     reverse = false;
//     char                     fieldSeparator;
//     bsl::vector<bsl::string> files;
// ```
// Notice that `fieldSeparator` are in automatic storage with no constructor or
// initial value.  We can safely use an uninitialized variable in the
// `specTable` below because the `specTable` provides a default value for it,
// which will be assigned to the variable if an option value is not provided on
// the command line or through environment variables.  `reverse` has to be
// initialized because no default for it is provided in `specTable`.
//
// Then, we declare our `specTable`, providing function pointers for our
// constraint functions to the second argument of the `TypeInfo` constructor.
// ```
//     // option specification table
//
//     static const balcl::OptionInfo specTable[] = {
//       {
//         "r|reverse",                             // tag
//         "isReverse",                             // name
//         "sort in reverse order",                 // description
//         TypeInfo(&reverse),                      // linked variable
//         OccurrenceInfo(),                        // default value
//         ""                                       // env variable name
//       },
//       {
//         "f|field-separator",                     // tag
//         "fieldSeparator",                        // name
//         "field separator character",             // description
//         TypeInfo(&fieldSeparator,                // linked variable
//                  &isValidFieldSeparator),        // constraint
//         OccurrenceInfo('|'),
//         ""                                       // env variable name
//       },
//       {
//         "",                                      // non-option
//         "fileList",                              // name
//         "input files to be sorted",              // description
//         TypeInfo(&files, &isValidFileName),      // linked variable and
//                                                  // constraint
//         OccurrenceInfo::e_REQUIRED,              // at least one file
//                                                  // required
//         ""                                       // env variable name
//       }
//     };
//
//     balcl::CommandLine cmdLine(specTable);
//     if (cmdLine.parse(argc, argv)) {
//         cmdLine.printUsage();
//         return -1;                                                // RETURN
//     }
// ```
// If the constraint functions return `false`, `cmdLine.parse` will return
// non-zero, and the output will contain the message from the constraint
// function followed by the usage message.
//
///Example 5: Using `bsl::optional` for Optional Command Line Parameters
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// We can use a `bsl::optional` variables when providing optional command line
// parameters.  Suppose we want to write a command line that takes an optional
// input file, and if the file is not supplied, take input from a
// `bsl::stringstream`.
//
// To represent the optional file name parameter, we link a variable of type
// `bsl::optional<bsl::string>`.  In general, when linking a variable to an
// option, we can choose to use `bsl::optonal<TYPE>` in place of `TYPE`, for
// any option type other than `bool`.
// ```
// int main(int argc, const char **argv)
// {
//     bsl::optional<bsl::string> optionalFileName;
//
//     const balcl::OptionInfo specTable[] = {
//       {
//         "i|inputFile",                         // tag
//         "filename",                            // name
//         "name of input file",                  // description
//         balcl::TypeInfo(&optionalFileName),    // linked optional variable
//         balcl::OccurrenceInfo(),               // occurence info
//         ""                                     // env variable name
//       }
//     };
//
//     balcl::CommandLine cmdLine(specTable);
//     if (cmdLine.parse(argc, argv)) {
//         cmdLine.printUsage();
//         return -1;                                                // RETURN
//     }
// ```
// Finally, we test whether `optionalFileName` has been set, and if it has not
// been set, take input from a prepared `stringstream`.
// ```
//     bsl::stringstream ss;
//     bsl::istream *inStream = &ss;
//
//     bsl::ifstream fileStream;
//     if (optionalFileName.has_value()) {
//         fileStream.open(optionalFileName->c_str());
//         inStream = &fileStream;
//     }
//     else {
//         // Prepare the 'stringstream'.
//
//         ss << "The quick brown fox jumps over the lazy dog\n";
//     }
//
//     performTask(*inStream);
// ```

#include <balscm_version.h>

#include <balcl_option.h>
#include <balcl_optioninfo.h>
#include <balcl_optiontype.h>
#include <balcl_optionvalue.h>

#include <bdlb_printmethods.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bsls_assert.h>
#include <bsls_types.h>

#include <bsl_cstddef.h>    // 'bsl::size_t'
#include <bsl_cstring.h>    // 'bsl::strcmp'
#include <bsl_iosfwd.h>
#include <bsl_sstream.h>    // 'bsl::ostringstream'
#include <bsl_string.h>
#include <bsl_string_view.h>
#include <bsl_vector.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_time.h>

namespace BloombergLP {

namespace balcl {

                        // =============================
                        // struct CommandLine_SchemaData
                        // =============================

/// This `struct`, a pure value-semantic type, is used to manage option
/// value type and name information in support of public interfaces to
/// parsed options.  See `CommandLineOptionsHandle`.
struct CommandLine_SchemaData{

    // PUBLIC DATA
    OptionType::Enum  d_type;    // option data type
    const char       *d_name_p;  // option name
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` have the same value, and
/// `false` otherwise.  Two `CommandLine_SchemaData` objects have the same
/// value if their `type` attributes are the same and if their `name`
/// attributes compare equal.
bool operator==(const CommandLine_SchemaData& lhs,
                const CommandLine_SchemaData& rhs);

/// Return `true` if the specified `lhs` and `rhs` do note have the same
/// value, and `false` otherwise.  Two `CommandLine_SchemaData` objects do
/// not have the same value if their `type` attributes are not the same or
/// if their `name` attributes do not compare equal.
bool operator!=(const CommandLine_SchemaData& lhs,
                const CommandLine_SchemaData& rhs);

                        // =================
                        // class CommandLine
                        // =================

class CommandLineOptionsHandle;

/// This value-semantic `class` parses, validates, and provides access to
/// command-line arguments.  The constructor takes the specification
/// describing the command-line arguments.  Once created, `printUsage` can
/// be invoked to print the usage syntax.  The `parse` method takes
/// command-line arguments and validates them against the specification
/// provided at creation, writing a suitable message to an
/// optionally-specified stream in case of a parsing error.  Once parsed,
/// options and values can be accessed using various accessors.  The class
/// has a set of `theType` methods (e.g., `theString`, `theInt`) that
/// provide access, by name, to the value of the indicated option.  It is
/// also possible to link a variable with an option in the specification;
/// doing so will cause the variable to be loaded with the option value once
/// `parse` has been invoked.  The `options` method returns a
/// `balcl::CommandLineOptionsHandle` object referring to the option names
/// and their values.  A similar but different method, `specifiedOptions`,
/// is suitable for overwriting other configuration parameters (possibly
/// obtained from a configuration file).
class CommandLine {

    // PRIVATE TYPES
    typedef bsl::vector<CommandLine_SchemaData> CommandLine_Schema;
    typedef bsl::vector<OptionValue> OptionValueList;

    enum State {
        e_INVALID    = 0,
        e_PARSED     = 1,
        e_NOT_PARSED = 2
    };

    // DATA
    bsl::vector<Option>                 d_options;    // command-line options
                                                      // specified at
                                                      // construction

    bsl::vector<bsl::vector<int> >      d_positions;  // set of positions of
                                                      // each command-line
                                                      // option in the
                                                      // arguments specified to
                                                      // 'parse', indexed by
                                                      // position of option in
                                                      // 'd_options'

    bsl::vector<int>                    d_nonOptionIndices;
                                                      // set of positions of
                                                      // each command-line
                                                      // non-option in the
                                                      // arguments specified to
                                                      // 'parse', indexed by
                                                      // non-option rank in
                                                      // 'd_options'

    State                               d_state;      // indicates whether
                                                      // 'parse' was invoked,
                                                      // and with what result

    bsl::vector<bsl::string>            d_arguments;  // command-line arguments
                                                      // specified to 'parse'

    CommandLine_Schema                  d_schema;     // schema describing the
                                                      // structure of the
                                                      // option values in the
                                                      // returned handle

    OptionValueList                     d_data;       // primary option values,
                                                      // recorded one by one as
                                                      // they are parsed by the
                                                      // 'parse' manipulator

    OptionValueList                     d_dataFinal;  // final option values,
                                                      // copied from 'd_data'
                                                      // and from default
                                                      // values by 'postParse'

    mutable OptionValueList             d_specifiedOptions;
                                                      // specified data, i.e.
                                                      // 'd_dataFinal' with
                                                      // non-specified
                                                      // arguments reset,
                                                      // created by the
                                                      // 'specifiedOptions'
                                                      // manipulator

    mutable bool                        d_isBindin2Valid;
                                                      // records whether
                                                      // 'd_specifiedOptions'
                                                      // was initialized by
                                                      // 'specifiedOptions'

    bool                                d_envVarsPresent;
                                                      // 'true' if any
                                                      // environment variable
                                                      // names are present in
                                                      // 'd_options'

  private:
    // PRIVATE MANIPULATORS

    /// Reset this command-line object to an uninitialized state.  Note that
    /// the state is set to `INVALID` and not `NOT_PARSED`.
    void clear();

    /// Store the specified `argc` entries from the specified `argv` array
    /// of command-line arguments in this object.
    void initArgs(int argc, const char *const argv[]);

    /// Parse the command-line arguments contained in `d_arguments`, and
    /// write a description of any errors that occur to the specified
    /// `errorStream`.  Note that this operation will parse command line
    /// arguments implementing the documented behavior for `parse`, using
    /// `postParseImp` to perform verification of the parsed values.
    int parseImp(bsl::ostream& errorStream);

    /// Verify that all required arguments have been given a value, and in
    /// that case load all the primary option values (if set) or default
    /// option values (if the primary option value is optional and set by no
    /// command-line argument) into the final option values and set the
    /// linked variables, if any.  Return 0 on success, and a non-zero value
    /// if not all required arguments are provided with a value (and in this
    /// case, leave the final option values and linked variables unchanged).
    /// Upon encountering an error, output to the specified `errorStream` a
    /// descriptive error message and abort parsing subsequent arguments.
    int postParseImp(bsl::ostream& errorStream);

    /// Validate the command-line options passed at construction with
    /// respect to the command-line documented invariants (e.g., valid tags,
    /// names, and descriptions, default values satisfying constraints,
    /// uniqueness of tags and names, and miscellaneous constraints), and
    /// initialize all internal state for parsing.  Optionally specify an
    /// output `errorStream`.  If `errorStream` is not specified,
    /// `bsl::cerr` is used.  Upon encountering errors, output descriptive
    /// error messages and abort the execution of this program by invoking
    /// the currently installed assertion handler (unconditionally).  Note
    /// that this method will attempt to report as many errors as possible
    /// before aborting, but that correcting all these errors will not
    /// guarantee that subsequent execution would result in successful
    /// validation.
    void validateAndInitialize();
    void validateAndInitialize(bsl::ostream& errorStream);

    // PRIVATE ACCESSORS

    /// Return the index (in the options table passed at construction) of an
    /// option whose name matches the specified `name` string, or -1 if no
    /// such `name` exists.
    int findName(const bsl::string_view& name) const;

    /// Return the index (in the options table passed at construction) of a
    /// command-line option whose long tag matches the specified `longTag`
    /// string of the specified `tagLength`, or -1 if no such `longTag`
    /// exists.
    int findTag(const bsl::string_view& longTag) const;

    /// Return the index (in the options table passed at construction) of an
    /// option whose short tag matches the specified `shortTag` character,
    /// or -1 if no such `shortTag` exists.
    int findTag(char shortTag) const;

    /// Output to the specified `errorStream` a message describing the location
    /// in the argument at the specified `index` (in the list of command-line
    /// arguments) where an error was found.  Optionally specify a `start`
    /// character position in that argument; if `start` is specified,
    /// optionally specify an `end` character position as well.
    void location(bsl::ostream& errorStream,
                  int           index,
                  int           start = -1,
                  int           end   = -1) const;

    /// Return the length of the longest name in all the possible non-flag
    /// options of this command line.
    int longestNonFlagNameSize() const;

    /// Return the length of the longest long tag in all the possible options
    /// of this command line.  Note that short tags are ignored.
    int longestTagSize() const;

    /// Check that all required options have already been parsed.  Optionally
    /// specify `checkAlsoNonOptions`.  If `checkAlsoNonOptions` is `true` or
    /// not specified, required non-option arguments that have not been parsed
    /// will also be returned; if `false`, they will be ignored.  Return the
    /// index of the first missing option, or -1 if none.
    int missing(bool checkAlsoNonOptions = true) const;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(CommandLine, bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(CommandLine, bdlb::HasPrintMethod);

    // CLASS METHODS

    /// Return `true` if the specified (statically-initialized) `specTable` of
    /// the specified `LENGTH` has a valid set of command-line option
    /// specifications, and `false` otherwise.  Optionally specify
    /// `errorStream` to which error messages are written.  If no `errorStream`
    /// is specified, this method produces no output.  See {Valid
    /// `balcl::OptionInfo` Specifications} for a description of the validity
    /// requirements.
    template <int LENGTH>
    static bool isValidOptionSpecificationTable(
                                        const OptionInfo (&specTable)[LENGTH]);
    template <int LENGTH>
    static bool isValidOptionSpecificationTable(
                                              OptionInfo (&specTable)[LENGTH]);
    template <int LENGTH>
    static bool isValidOptionSpecificationTable(
                                         const OptionInfo (&specTable)[LENGTH],
                                         bsl::ostream&      errorStream);
    template <int LENGTH>
    static bool isValidOptionSpecificationTable(
                                            OptionInfo    (&specTable)[LENGTH],
                                            bsl::ostream&   errorStream);

    /// Return `true` if the specified `specTable` of the specified `length`
    /// has a valid set of command-line option specifications, and `false`
    /// otherwise.  Optionally specify `errorStream` to which error messages
    /// are written.  If no `errorStream` is specified, this method produces
    /// not output.  See {Valid `balcl::OptionInfo` Specifications} for a
    /// description of the validity requirements.  The behavior is undefined
    /// unless `0 <= length`.  Note that `specTable` need not be statically
    /// initialized.
    static bool isValidOptionSpecificationTable(const OptionInfo *specTable,
                                                int               length);
    static bool isValidOptionSpecificationTable(const OptionInfo *specTable,
                                                int               length,
                                                bsl::ostream&     errorStream);

    // CREATORS

    /// Create an object accepting the command-line options described by the
    /// specified (statically-initialized) `specTable`.  Optionally specify a
    /// `basicAllocator` used to supply memory.  If `basicAllocator` is 0, the
    /// currently installed default allocator is used.  The behavior is
    /// undefined unless `specTable` satisfies the
    /// `isValidOptionSpecificationTable` function.  Note that an appropriate
    /// error message is written to the specified `errorStream`.
    template <int LENGTH>
    CommandLine(const OptionInfo (&specTable)[LENGTH],
                bsl::ostream&      errorStream,
                bslma::Allocator  *basicAllocator = 0);
    template <int LENGTH>
    CommandLine(OptionInfo      (&specTable)[LENGTH],
                bsl::ostream&     errorStream,
                bslma::Allocator *basicAllocator = 0);

    /// Create an object accepting the command-line options described by the
    /// specified (statically-initialized) `specTable`.  Optionally specify a
    /// `basicAllocator` used to supply memory.  If `basicAllocator` is 0, the
    /// currently installed default allocator is used.  The behavior is
    /// undefined unless `specTable` satisfies the
    /// `isValidOptionSpecificationTable` function.  Note that an appropriate
    /// error message is written to `bsl::cerr`.
    template <int LENGTH>
    explicit
    CommandLine(const OptionInfo (&specTable)[LENGTH],
                bslma::Allocator  *basicAllocator = 0);
    template <int LENGTH>
    explicit
    CommandLine(OptionInfo       (&specTable)[LENGTH],
                bslma::Allocator  *basicAllocator = 0);

    /// Create an object accepting the command-line options described by the
    /// specified `specTable` of the specified `length`.  Optionally specify a
    /// `basicAllocator` used to supply memory.  If `basicAllocator` is 0, the
    /// currently installed default allocator is used.  The behavior is
    /// undefined unless `specTable` satisfies the
    /// `isValidOptionSpecificationTable` function.  Note that an appropriate
    /// error message is written to `bsl::cerr`.  Also note that `specTable`
    /// need not be statically initialized.
    CommandLine(const OptionInfo *specTable,
                int               length,
                bslma::Allocator *basicAllocator = 0);

    /// Create an object accepting the command-line options described by the
    /// specified `specTable` of the specified `length`.  Optionally specify a
    /// `basicAllocator` used to supply memory.  If `basicAllocator` is 0, the
    /// currently installed default allocator is used.  The behavior is
    /// undefined unless `specTable` satisfies the
    /// `isValidOptionSpecificationTable` function.  Note that an appropriate
    /// error message is written to the specified `errorStream`.  Also note
    /// that `specTable` need not be statically initialized.
    CommandLine(const OptionInfo *specTable,
                int               length,
                bsl::ostream&     errorStream,
                bslma::Allocator *basicAllocator = 0);

    /// Create a command-line object having the value of the specified
    /// `original` command line, if the `original` is parsed, and otherwise
    /// having a state such that parsing command-line arguments results in the
    /// same value as parsing the same command-line arguments with the
    /// `original`.  Optionally specify a `basicAllocator` used to supply
    /// memory.  If `basicAllocator` is 0, the currently installed default
    /// allocator is used.  The behavior is undefined unless `original` is
    /// valid (i.e., `original.isValid()` returns `true`).
    CommandLine(const CommandLine&  original,
                bslma::Allocator   *basicAllocator = 0);

    /// Destroy this command-line object.
    ~CommandLine();

    // MANIPULATORS

    /// Assign to this command-line object the value of the specified `rhs`
    /// command-line object and return a reference providing modifiable access
    /// to this object.  The behavior is undefined unless both `rhs` and this
    /// object are valid (i.e., `isValid()` and `rhs.isValid()` both return
    /// `true`).
    CommandLine& operator=(const CommandLine& rhs);

    /// Parse the command-line arguments contained in the array starting at the
    /// specified `argv` having the specified `argc` length.  Optionally
    /// specify an `errorStream` to which an appropriate error message is
    /// written if parsing fails.  If `errorStream` is not specified,
    /// `bsl::cerr` is used.  Return 0 on success, and a non-zero value
    /// otherwise.  If an argument is not specified on the command-line, then
    /// if the `Option` object configuration for this `CommandLine` defines an
    /// environment variable name and the runtime environment for this
    /// application has set that environment variable, then set the option to
    /// the value of the environment variable.  Otherwise, if an option is not
    /// set by the command line, or the environment, set the option using the
    /// default value if one has been provided.  If the option has not been set
    /// by the command line, environment, or by default, `hasValue` for the
    /// option will return `false`.  After a successful call `isParsed()` and
    /// `isValid()` will both be `true`, and the information provided by `argv`
    /// can be viewed via the accessors.  After an unsuccessful call
    /// `isParsed()` and `isValid()` will both be `false`.  The behavior is
    /// undefined unless `isValid()` is `true` and `isParsed()` is `false`
    /// (i.e. the `CommandLine` was constructed in a valid state, and `parse`
    /// has not previously been called).
    int parse(int argc, const char *const argv[]);
    int parse(int argc, const char *const argv[], bsl::ostream& errorStream);

    // ACCESSORS

    /// Return `true` if this command-line object is configured with an option
    /// having the specified `name`, and `false` otherwise.
    bool hasOption(const bsl::string_view& name) const;

    /// Return `true` if this command-line object's option having the specified
    /// `name` has a defined value, and `false` otherwise.  An option has a
    /// defined value if `isSpecified(name)` or if a default value was
    /// configured for the option (see {Occurrence Information Field}).  The
    /// behavior is undefined unless this command-line object `isParsed()` and
    /// `hasOption(name)`.
    bool hasValue(const bsl::string_view& name) const;

    /// Return `true` if this object was parsed successfully, and `false`
    /// otherwise.  Note that if `parse` was invoked but failed, this method
    /// returns `false`.
    bool isParsed() const;

    /// Return `true` if the option with the specified `name` has been entered
    /// on the command line and, if the optionally specified `count` is not 0,
    /// load into `count` the number of times the option `name` has been
    /// entered on the command line; otherwise, return `false` and leave
    /// `count` unaffected.  Note that, in order to receive the valid value,
    /// the command line must be successfully parsed.
    bool isSpecified(const bsl::string_view& name) const;
    bool isSpecified(const bsl::string_view& name, int *count) const;

    /// Return `true` if this object is in a valid state, and `false`
    /// otherwise.  Objects are in a valid state after construction from a
    /// valid set of option specifications (see the function-level
    /// documentation of the `isValidOptionSpecificationTable` method) and
    /// after a successful invocation of the `parse` method.  Conversely,
    /// construction from invalid option specifications or an unsuccessful
    /// invocation of the `parse` method leaves this object in an invalid
    /// state.  Note that additional object state is available from the
    /// `isParsed` accessor method.
    bool isValid() const;

    /// Return the number of times the option with the specified `name` has
    /// been entered on the command line, or 0 if `name` is not the name of a
    /// field in the command-line specification passed at construction to this
    /// object.  Note that, in order to receive the valid number of
    /// occurrences, the command line must be successfully parsed.
    int numSpecified(const bsl::string_view& name) const;

    /// Return the command-line options and their values.  If an option was not
    /// entered on the command line *and* a default value was provided for that
    /// option, then that default value will be used (note the difference with
    /// the `specifiedOptions` method).  If an option was not entered on the
    /// command line *and* no default value was provided for that option, then
    /// the corresponding option will be in a null state.  The behavior is
    /// undefined unless `isParsed` returns `true`.
    CommandLineOptionsHandle options() const;

    /// Return the position where the option with the specified `name` has been
    /// entered on the command line (i.e., the index in the `argv` argument to
    /// the `parse` method).  If the option was specified multiple times on the
    /// command line, return the position of the first instance.  If the option
    /// was specified as an environment variable but not on the command line,
    /// return -2.  If the option was not specified, return -1.  The behavior
    /// is undefined unless the option is of scalar type.
    int position(const bsl::string_view& name) const;

    /// Return the positions where the option with the specified `name` has
    /// been entered on the command line (i.e., the offset in the `argv`
    /// argument to the `parse` method).  If the option was not specified,
    /// return an empty vector.  Note that, in order to receive the valid
    /// positions, the command line must be successfully parsed.
    const bsl::vector<int>& positions(const bsl::string_view& name) const;

    /// Print usage to the specified output `errorStream`, describing what the
    /// command line should look like.  If `errorStream` is not specified,
    /// print usage to `stderr`.  Optionally specify `programName` to use as
    /// the name of the program in the printed usage.  If `programName` is not
    /// specified and if `parse` has been previously called successfully, use
    /// the first element of the `argv` argument; otherwise, use a default
    /// name.  This method can be invoked at any time, even before `parse` has
    /// been invoked on this object.
    void printUsage() const;
    void printUsage(const bsl::string_view& programName) const;
    void printUsage(bsl::ostream&           errorStream) const;
    void printUsage(bsl::ostream&           errorStream,
                    const bsl::string_view& programName) const;

    /// Return the command-line options and their values.  If an option was not
    /// entered on the command line or set through an environment variable,
    /// then the option will be in a null state (note the difference with the
    /// `options` method).  This method is especially useful for overwriting
    /// some other configuration (potentially obtained from a configuration
    /// file).  The behavior is undefined unless `isParsed` returns `true`.
    CommandLineOptionsHandle specifiedOptions() const;

    /// Return the type of the option having the specified `name`.  The
    /// behavior is undefined unless this command-line object
    /// `hasOption(name)`.
    OptionType::Enum type(const bsl::string_view& name) const;

                        // 'the*' Accessors

// BDE_VERIFY pragma: -FABC01  // not in alphabetic order

    /// Return the value of the option having the specified `name`.  The value
    /// returned matches that returned by `isSpecified(name)`.  The behavior is
    /// undefined unless this command-line object `isParsed()`,
    /// `hasOption(name)`, and `OptionType::e_BOOL == type(name)`.
    bool theBool(const bsl::string_view& name) const;

    /// Return the value of the option having the specified `name`.  The
    /// behavior is undefined unless this command-line object `isParsed()`,
    /// `hasOption(name)`, `OptionType::e_CHAR == type(name)`, and
    /// `hasValue(name)`.
    char theChar(const bsl::string_view& name) const;

    /// Return the value of the option having the specified `name`.  The
    /// behavior is undefined unless this command-line object `isParsed()`,
    /// `hasOption(name)`, `OptionType::e_INT == type(name)`, and
    /// `hasValue(name)`.
    int theInt(const bsl::string_view& name) const;

    /// Return the value of the option having the specified `name`.  The
    /// behavior is undefined unless this command-line object `isParsed()`,
    /// `hasOption(name)`, `OptionType::e_INT64 == type(name)`, and
    /// `hasValue(name)`.
    bsls::Types::Int64 theInt64(const bsl::string_view& name) const;

    /// Return the value of the option having the specified `name`.  The
    /// behavior is undefined unless this command-line object `isParsed()`,
    /// `hasOption(name)`, `OptionType::e_DOUBLE == type(name)`, and
    /// `hasValue(name)`.
    double theDouble(const bsl::string_view& name) const;

    /// Return a `const` reference to the value of the option having the
    /// specified `name`.  The behavior is undefined unless this
    /// command-line object `isParsed()`, `hasOption(name)`,
    /// `OptionType::e_STRING == type(name)`, and `hasValue(name)`.
    const bsl::string& theString(const bsl::string_view& name) const;

    /// Return a `const` reference to the value of the option having the
    /// specified `name`.  The behavior is undefined unless this
    /// command-line object `isParsed()`, `hasOption(name)`,
    /// `OptionType::e_DATETIME == type(name)`, and `hasValue(name)`.
    const bdlt::Datetime& theDatetime(const bsl::string_view& name) const;

    /// Return a `const` reference to the value of the option having the
    /// specified `name`.  The behavior is undefined unless this
    /// command-line object `isParsed()`, `hasOption(name)`,
    /// `OptionType::e_DATE == type(name)`, and `hasValue(name)`.
    const bdlt::Date& theDate(const bsl::string_view& name) const;

    /// Return a `const` reference to the value of the option having the
    /// specified `name`.  The behavior is undefined unless this
    /// command-line object `isParsed()`, `hasOption(name)`,
    /// `OptionType::e_TIME == type(name)`, and `hasValue(name)`.
    const bdlt::Time& theTime(const bsl::string_view& name) const;

    /// Return a `const` reference to the value of the option having the
    /// specified `name`.  The behavior is undefined unless this
    /// command-line object `isParsed()`, `hasOption(name)`,
    /// `OptionType::e_CHAR_ARRAY == type(name)`, and `hasValue(name)`.
    const bsl::vector<char>& theCharArray(const bsl::string_view& name) const;

    /// Return a `const` reference to the value of the option having the
    /// specified `name`.  The behavior is undefined unless this
    /// command-line object `isParsed()`, `hasOption(name)`,
    /// `OptionType::e_INT_ARRAY == type(name)`, and `hasValue(name)`.
    const bsl::vector<int>& theIntArray(const bsl::string_view& name) const;

    /// Return a `const` reference to the value of the option having the
    /// specified `name`.  The behavior is undefined unless this
    /// command-line object `isParsed()`, `hasOption(name)`,
    /// `OptionType::e_INT64_ARRAY == type(name)`, and `hasValue(name)`.
    const bsl::vector<bsls::Types::Int64>& theInt64Array(
                                           const bsl::string_view& name) const;

    /// Return a `const` reference to the value of the option having the
    /// specified `name`.  The behavior is undefined unless this
    /// command-line object `isParsed()`, `hasOption(name)`,
    /// `OptionType::e_DOUBLE_ARRAY == type(name)`, and `hasValue(name)`.
    const bsl::vector<double>& theDoubleArray(
                                           const bsl::string_view& name) const;

    /// Return a `const` reference to the value of the option having the
    /// specified `name`.  The behavior is undefined unless this
    /// command-line object `isParsed()`, `hasOption(name)`,
    /// `OptionType::e_STRING_ARRAY == type(name)`, and `hasValue(name)`.
    const bsl::vector<bsl::string>& theStringArray(
                                           const bsl::string_view& name) const;

    /// Return a `const` reference to the value of the option having the
    /// specified `name`.  The behavior is undefined unless this
    /// command-line object `isParsed()`, `hasOption(name)`,
    /// `OptionType::e_DATETIME_ARRAY == type(name)`, and `hasValue(name)`.
    const bsl::vector<bdlt::Datetime>& theDatetimeArray(
                                           const bsl::string_view& name) const;

    /// Return a `const` reference to the value of the option having the
    /// specified `name`.  The behavior is undefined unless this
    /// command-line object `isParsed()`, `hasOption(name)`,
    /// `OptionType::e_DATE_ARRAY == type(name)`, and `hasValue(name)`.
    const bsl::vector<bdlt::Date>& theDateArray(
                                           const bsl::string_view& name) const;

    /// Return a `const` reference to the value of the option having the
    /// specified `name`.  The behavior is undefined unless this
    /// command-line object `isParsed()`, `hasOption(name)`,
    /// `OptionType::e_TIME_ARRAY == type(name)`, and `hasValue(name)`.
    const bsl::vector<bdlt::Time>& theTimeArray(
                                           const bsl::string_view& name) const;

// BDE_VERIFY pragma: +FABC01  // not in alphabetic order

                                  // Aspects

    /// Return the allocator used by this object to supply memory.  Note
    /// that if no allocator was supplied at construction the currently
    /// installed default allocator at construction is used.
    bslma::Allocator *allocator() const;

    /// Format this command-line object to the specified output `stream` at
    /// the (absolute value of) the optionally specified indentation `level`
    /// and return a reference to `stream`.  If `level` is specified,
    /// optionally specify `spacesPerLevel`, the number of spaces per
    /// indentation level for this object.  If `level` is negative, suppress
    /// indentation of the first line.  The behavior is undefined unless
    /// `0 <= spacesPerLevel`.  If `stream` is not valid on entry, this
    /// operation has no effect.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` have the same value, and
/// `false` otherwise.  Two command-line arguments have the same value if
/// and only if they are both parsed successfully and have the same
/// command-line specifications and the same values for flags, options, and
/// non-option arguments.  Note that two identical copies built with the
/// same option table, but unparsed, are *not* equal.
bool operator==(const CommandLine& lhs, const CommandLine& rhs);

/// Return `false` if the specified `lhs` and `rhs` have the same value, and
/// `true` otherwise.  Two command-line arguments do not have the same value
/// if and only if they have different command-line specifications, or one
/// is parsed successfully but the other is not, or neither is, or else both
/// have the same specification and both are parsed successfully but they
/// have different values for at least one flag, option or non-option
/// argument.  Note that two identical copies built with the same option
/// table, but unparsed, are *not* equal.
bool operator!=(const CommandLine& lhs, const CommandLine& rhs);

/// Write the options and their values in the specified `rhs` to the
/// specified output `stream` in a (multi-line) human readable format and
/// return a reference to `errorStream`.  Note that the last line is *not*
/// terminated by a newline character.
bsl::ostream& operator<<(bsl::ostream& stream, const CommandLine& rhs);

                        // ==============================
                        // class CommandLineOptionsHandle
                        // ==============================

/// This class provides access to the parsed options (if any) of its
/// creating `CommandLine` object.  The behavior is undefined if any of the
/// methods of this class (accessors all) are invoked after a subsequent
/// invocation of the `parse` method of the creating object or after the
/// destruction of the creating object.
class CommandLineOptionsHandle {

    // PRIVATE TYPES
    typedef bsl::vector<CommandLine_SchemaData> CommandLine_Schema;
    typedef bsl::vector<OptionValue>            OptionValueList;

    // DATA
    const OptionValueList    *d_data_p;
    const CommandLine_Schema *d_schema_p;

    // FRIENDS
    friend class CommandLine;
    friend bool operator==(const CommandLineOptionsHandle&,
                           const CommandLineOptionsHandle&);
    friend bool operator!=(const CommandLineOptionsHandle&,
                           const CommandLineOptionsHandle&);

    // PRIVATE CREATORS

    /// Create a `CommandLineOptionsHandle` object referring to the
    /// specified `dataPtr` and `schemaPtr`.  The behavior is undefined
    /// unless `dataPtr->size() == schemaPtr->size()`.
    CommandLineOptionsHandle(const OptionValueList    *dataPtr,
                             const CommandLine_Schema *schemaPtr);

  public:
    // ACCESSORS

    /// Return the position in this handle object of the option having the
    /// specified `name`, and -1 if the handle has no option of that name.
    /// Note that, on success, the return values are in the range
    /// `[0 .. numOptions() - 1]`.
    int index(const bsl::string_view& name) const;

    /// Return the name of the option at the specified `index`.  The
    /// behavior is undefined unless `index < numOptions()`.
    const char *name(bsl::size_t index) const;

    /// Return the number of parsed options.
    bsl::size_t numOptions() const;

    /// Return the type of the option at the specified `index` in this
    /// handle object.  The behavior is undefined unless
    /// `index < numOptions()`.
    OptionType::Enum type(bsl::size_t index) const;

    /// Return the type of the option having the specified `name`.  The
    /// behavior is undefined unless this handle object has a `name` option
    /// (i.e., `0 <= index(name)`).
    OptionType::Enum type(const bsl::string_view& name) const;

    /// Return a `const` reference to the value (possibly in a null state)
    /// of the option at the specified `index` in this handle object.  The
    /// behavior is undefined unless `index < numOptions()`.
    const OptionValue& value(bsl::size_t index) const;

    /// Return a `const` reference to the value (possibly in a null state)
    /// of this handle object's option having the specified `name`.  The
    /// behavior is undefined unless this handle object has a `name` option
    /// (i.e., `0 <= index(name)`).
    const OptionValue& value(const bsl::string_view& name) const;

                        // 'the*' Accessors

// BDE_VERIFY pragma: -FABC01  // not in alphabetic order

    /// Return a `const` reference to the value of the option at the
    /// specified `index`.  Template parameter `TYPE` must be one of the
    /// supported types (see {Supported Types}).  The behavior is undefined
    /// unless `index < numOptions()`, the option has the (template
    /// parameter) `TYPE` (i.e.,
    /// `OptionType::TypeToEnum<TYPE>::value == type(index)`), and the
    /// option has a value (i.e., `false == value(index).isNull()`).
    template <class TYPE>
    const TYPE& the(bsl::size_t index) const;

    /// Return a `const` reference to the value of the option having the
    /// specified `name`.  Template parameter `TYPE` must be one of the
    /// supported types (see {Supported Types}).  The behavior is undefined
    /// unless this handle object has a `name` option (i.e.,
    /// `0 <= index(name)`), the option has (template parameter) `TYPE`
    /// (i.e., `OptionType::TypeToEnum<TYPE>::value == type(name)`), and the
    /// option has a value (i.e., `false == value(name).isNull()`).
    template <class TYPE>
    const TYPE& the(const bsl::string_view& name) const;

    /// Return the value of the option having the specified `name`.  The
    /// behavior is undefined unless this handle object has a `name` option
    /// (i.e., `0 <= index(name)`), the option has type `bool` (i.e.,
    /// `OptionType::e_BOOL == type(name)`), and the option has a value
    /// (i.e., `false == value(name).isNull()`).
    bool theBool(const bsl::string_view& name) const;

    /// Return the value of the option having the specified `name`.  The
    /// behavior is undefined unless this handle object has a `name` option
    /// (i.e., `0 <= index(name)`), the option has type `char` (i.e.,
    /// `OptionType::e_CHAR == type(name)`), and the option has a value
    /// (i.e., `false == value(name).isNull()`).
    char theChar(const bsl::string_view& name) const;

    /// Return the value of the option having the specified `name`.  The
    /// behavior is undefined unless this handle object has a `name` option
    /// (i.e., `0 <= index(name)`), the option has type `int` (i.e.,
    /// `OptionType::e_INT == type(name)`), and the option has a value
    /// (i.e., `false == value(name).isNull()`).
    int theInt(const bsl::string_view& name) const;

    /// Return the value of the option having the specified `name`.  The
    /// behavior is undefined unless this handle object has a `name` option
    /// (i.e., `0 <= index(name)`), the option has type `bsls::Types::Int64`
    /// (i.e., `OptionType::e_INT64 == type(name)`), and the option has a
    /// value (i.e., `false == value(name).isNull()`).
    bsls::Types::Int64 theInt64(const bsl::string_view& name) const;

    /// Return the value of the option having the specified `name`.  The
    /// behavior is undefined unless this handle object has a `name` option
    /// (i.e., `0 <= index(name)`), the option has type `double` (i.e.,
    /// `OptionType::e_DOUBLE == type(name)`), and the option has a value
    /// (i.e., `false == value(name).isNull()`).
    double theDouble(const bsl::string_view& name) const;

    /// Return a `const` reference to the value of the option having the
    /// specified `name`.  The behavior is undefined unless this handle
    /// object has a `name` option (i.e., `0 <= index(name)`), the option
    /// has type `bsl::string` (i.e., `OptionType::e_STRING == type(name)`),
    /// and the option has a value (i.e., `false == value(name).isNull()`).
    const bsl::string& theString(const bsl::string_view& name) const;

    /// Return a `const` reference to the value of the option having the
    /// specified `name`.  The behavior is undefined unless this handle
    /// object has a `name` option (i.e., `0 <= index(name)`), the option
    /// has type `bdlt::Datetime` (i.e.,
    /// `OptionType::e_DATETIME == type(name)`), and the option has a value
    /// (i.e., `false == value(name).isNull()`).
    const bdlt::Datetime& theDatetime(const bsl::string_view& name) const;

    /// Return a `const` reference to the value of the option having the
    /// specified `name`.  The behavior is undefined unless this handle
    /// object has a `name` option (i.e., `0 <= index(name)`), the option
    /// has type `bdlt::Date` (i.e., `OptionType::e_DATE == type(name)`),
    /// and the option has a value (i.e., `false == value(name).isNull()`).
    const bdlt::Date& theDate(const bsl::string_view& name) const;

    /// Return a `const` reference to the value of the option having the
    /// specified `name`.  The behavior is undefined unless this handle
    /// object has a `name` option (i.e., `0 <= index(name)`), the option
    /// has type `bdlt::Time` (i.e., `OptionType::e_TIME == type(name)`),
    /// and the option has a value (i.e., `false == value(name).isNull()`).
    const bdlt::Time& theTime(const bsl::string_view& name) const;

    /// Return a `const` reference to the value of the option having the
    /// specified `name`.  The behavior is undefined unless this handle
    /// object has a `name` option (i.e., `0 <= index(name)`), the option
    /// has type `bsl::vector<char>` (i.e.,
    /// `OptionType::e_CHAR_ARRAY == type(name)`), and the option has a
    /// value (i.e., `false == value(name).isNull()`).
    const bsl::vector<char>& theCharArray(const bsl::string_view& name) const;

    /// Return a `const` reference to the value of the option having the
    /// specified `name`.  The behavior is undefined unless this handle
    /// object has a `name` option (i.e., `0 <= index(name)`), the option
    /// has type `bsl::vector<int>` (i.e.,
    /// `OptionType::e_INT_ARRAY == type(name)`), and the option has a value
    /// (i.e., `false == value(name).isNull()`).
    const bsl::vector<int>& theIntArray(const bsl::string_view& name) const;

    /// Return a `const` reference to the value of the option having the
    /// specified `name`.  The behavior is undefined unless this handle
    /// object has a `name` option (i.e., `0 <= index(name)`), the option
    /// has type `bsl::vector<bsls::Types::Int64>` (i.e.,
    /// `OptionType::e_INT64_ARRAY == type(name)`), and the option has a
    /// value (i.e., `false == value(name).isNull()`).
    const bsl::vector<bsls::Types::Int64>& theInt64Array(
                                           const bsl::string_view& name) const;

    /// Return a `const` reference to the value of the option having the
    /// specified `name`.  The behavior is undefined unless this handle
    /// object has a `name` option (i.e., `0 <= index(name)`), the option
    /// has type `bsl::vector<double>` (i.e.,
    /// `OptionType::e_DOUBLE_ARRAY == type(name)`), and the option has a
    /// value (i.e., `false == value(name).isNull()`).
    const bsl::vector<double>& theDoubleArray(
                                           const bsl::string_view& name) const;

    /// Return a `const` reference to the value of the option having the
    /// specified `name`.  The behavior is undefined unless this handle
    /// object has a `name` option (i.e., `0 <= index(name)`), the option
    /// has type `bsl::vector<bsl::string>` (i.e.,
    /// `OptionType::e_STRING_ARRAY == type(name)`), and the option has a
    /// value (i.e., `false == value(name).isNull()`).
    const bsl::vector<bsl::string>& theStringArray(
                                           const bsl::string_view& name) const;

    /// Return a `const` reference to the value of the option having the
    /// specified `name`.  The behavior is undefined unless this handle
    /// object has a `name` option (i.e., `0 <= index(name)`), the option
    /// has type `bsl::vector<bdlt::Datetime>` (i.e.,
    /// `OptionType::e_DATETIME_ARRAY == type(name)`), and the option has a
    /// value (i.e., `false == value(name).isNull()`).
    const bsl::vector<bdlt::Datetime>& theDatetimeArray(
                                           const bsl::string_view& name) const;

    /// Return a `const` reference to the value of the option having the
    /// specified `name`.  The behavior is undefined unless this handle
    /// object has a `name` option (i.e., `0 <= index(name)`), the option
    /// has type `bsl::vector<bdlt::Date>` (i.e.,
    /// `OptionType::e_DATE_ARRAY == type(name)`), and the option has a
    /// value (i.e., `false == value(name).isNull()`).
    const bsl::vector<bdlt::Date>& theDateArray(
                                           const bsl::string_view& name) const;

    /// Return a `const` reference to the value of the option having the
    /// specified `name`.  The behavior is undefined unless this handle
    /// object has a `name` option (i.e., `0 <= index(name)`), the option
    /// has type `bsl::vector<bdlt::TIME>` (i.e.,
    /// `OptionType::e_TIME_ARRAY == type(name)`), and the option has a
    /// value (i.e., `false == value(name).isNull()`).
    const bsl::vector<bdlt::Time>& theTimeArray(
                                           const bsl::string_view& name) const;

// BDE_VERIFY pragma: +FABC01  // not in alphabetic order
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` have the same value, and
/// `false` otherwise.  Two `CommandLineOptionsHandle` objects have the same
/// value if they have the same `numOptions` and each of those options have
/// the same `name`, `type`, and `value` or are in the null state.
bool operator==(const CommandLineOptionsHandle& lhs,
                const CommandLineOptionsHandle& rhs);

/// Return `true` if the specified `lhs` and `rhs` do not have the same
/// value, and `false` otherwise.  Two `CommandLineOptionsHandle` objects do
/// not have the same value if they do not have the same `numOptions` or if
/// any of their options differ in `name`, `type`, or 'value (or null
/// state).
bool operator!=(const CommandLineOptionsHandle& lhs,
                const CommandLineOptionsHandle& rhs);

// ============================================================================
//                        INLINE DEFINITIONS
// ============================================================================

                        // -----------------------------
                        // struct CommandLine_SchemaData
                        // -----------------------------

}  // close package namespace

// FREE OPERATORS
inline
bool balcl::operator==(const CommandLine_SchemaData& lhs,
                       const CommandLine_SchemaData& rhs)
{
    return lhs.d_type == rhs.d_type
       &&  0 == bsl::strcmp(lhs.d_name_p, rhs.d_name_p);
}

inline
bool balcl::operator!=(const CommandLine_SchemaData& lhs,
                       const CommandLine_SchemaData& rhs)
{
    return lhs.d_type != rhs.d_type
       ||  0 != bsl::strcmp(lhs.d_name_p, rhs.d_name_p);
}

namespace balcl {

                        // -----------------
                        // class CommandLine
                        // -----------------

// CLASS METHODS
template <int LENGTH>
inline
bool CommandLine::isValidOptionSpecificationTable(
                                         const OptionInfo (&specTable)[LENGTH])
{
    return isValidOptionSpecificationTable(specTable, LENGTH);
}

template <int LENGTH>
inline
bool CommandLine::isValidOptionSpecificationTable(
                                               OptionInfo (&specTable)[LENGTH])
{
    return isValidOptionSpecificationTable(specTable, LENGTH);
}

template <int LENGTH>
inline
bool CommandLine::isValidOptionSpecificationTable(
                                         const OptionInfo (&specTable)[LENGTH],
                                         bsl::ostream&      errorStream)
{
    return isValidOptionSpecificationTable(specTable, LENGTH, errorStream);
}

template <int LENGTH>
inline
bool CommandLine::isValidOptionSpecificationTable(
                                            OptionInfo    (&specTable)[LENGTH],
                                            bsl::ostream&   errorStream)
{
    return isValidOptionSpecificationTable(specTable, LENGTH, errorStream);
}

inline
bool CommandLine::isValidOptionSpecificationTable(const OptionInfo *specTable,
                                                  int               length)
{
    BSLS_ASSERT(specTable);
    BSLS_ASSERT(0 <= length);

    bsl::ostringstream oss;
    return isValidOptionSpecificationTable(specTable, length, oss);
}

// CREATORS
template <int LENGTH>
CommandLine::CommandLine(const OptionInfo (&specTable)[LENGTH],
                         bsl::ostream&      errorStream,
                         bslma::Allocator  *basicAllocator)
: d_options(basicAllocator)
, d_positions(basicAllocator)
, d_nonOptionIndices(basicAllocator)
, d_state(e_INVALID)
, d_arguments(basicAllocator)
, d_schema(basicAllocator)
, d_data(basicAllocator)
, d_dataFinal(basicAllocator)
, d_specifiedOptions(basicAllocator)
, d_isBindin2Valid(false)
, d_envVarsPresent(false)
{
    d_options.reserve(LENGTH);
    for (int ii = 0; ii < LENGTH; ++ii) {
        d_options.push_back(Option(specTable[ii], allocator()));
    }
    validateAndInitialize(errorStream);
    d_state = e_NOT_PARSED;
}

template <int LENGTH>
CommandLine::CommandLine(OptionInfo      (&specTable)[LENGTH],
                         bsl::ostream&     errorStream,
                         bslma::Allocator *basicAllocator)
: d_options(basicAllocator)
, d_positions(basicAllocator)
, d_nonOptionIndices(basicAllocator)
, d_state(e_INVALID)
, d_arguments(basicAllocator)
, d_schema(basicAllocator)
, d_data(basicAllocator)
, d_dataFinal(basicAllocator)
, d_specifiedOptions(basicAllocator)
, d_isBindin2Valid(false)
, d_envVarsPresent(false)
{
    d_options.reserve(LENGTH);
    for (int i = 0; i < LENGTH; ++i) {
        d_options.push_back(Option(specTable[i], allocator()));
    }
    validateAndInitialize(errorStream);
    d_state = e_NOT_PARSED;
}

template <int LENGTH>
CommandLine::CommandLine(const OptionInfo (&specTable)[LENGTH],
                         bslma::Allocator  *basicAllocator)
: d_options(basicAllocator)
, d_positions(basicAllocator)
, d_nonOptionIndices(basicAllocator)
, d_state(e_INVALID)
, d_arguments(basicAllocator)
, d_schema(basicAllocator)
, d_data(basicAllocator)
, d_dataFinal(basicAllocator)
, d_specifiedOptions(basicAllocator)
, d_isBindin2Valid(false)
, d_envVarsPresent(false)
{
    d_options.reserve(LENGTH);
    for (int ii = 0; ii < LENGTH; ++ii) {
        d_options.push_back(Option(specTable[ii], allocator()));
    }
    validateAndInitialize();
    d_state = e_NOT_PARSED;
}

template <int LENGTH>
CommandLine::CommandLine(OptionInfo      (&specTable)[LENGTH],
                         bslma::Allocator *basicAllocator)
: d_options(basicAllocator)
, d_positions(basicAllocator)
, d_nonOptionIndices(basicAllocator)
, d_state(e_INVALID)
, d_arguments(basicAllocator)
, d_schema(basicAllocator)
, d_data(basicAllocator)
, d_dataFinal(basicAllocator)
, d_specifiedOptions(basicAllocator)
, d_isBindin2Valid(false)
, d_envVarsPresent(false)
{
    d_options.reserve(LENGTH);
    for (int ii = 0; ii < LENGTH; ++ii) {
        d_options.push_back(Option(specTable[ii], allocator()));
    }
    validateAndInitialize();
    d_state = e_NOT_PARSED;
}

                        // ------------------------------
                        // class CommandLineOptionsHandle
                        // ------------------------------

// PRIVATE CREATORS
inline
CommandLineOptionsHandle::CommandLineOptionsHandle(
                                           const OptionValueList    *dataPtr,
                                           const CommandLine_Schema *schemaPtr)
: d_data_p(dataPtr)
, d_schema_p(schemaPtr)
{
    BSLS_ASSERT(dataPtr);
    BSLS_ASSERT(schemaPtr);
    BSLS_ASSERT(dataPtr->size() == schemaPtr->size());
}

// ACCESSORS
inline
const char *CommandLineOptionsHandle::name(bsl::size_t index) const
{
    BSLS_ASSERT(index < d_schema_p->size());

    return (*d_schema_p)[index].d_name_p;
}

inline
bsl::size_t CommandLineOptionsHandle::numOptions() const
{
    return d_schema_p->size();
}

inline
OptionType::Enum CommandLineOptionsHandle::type(bsl::size_t index) const
{
    BSLS_ASSERT(index < d_schema_p->size());

    return (*d_schema_p)[index].d_type;
}

inline
OptionType::Enum CommandLineOptionsHandle::type(
                                            const bsl::string_view& name) const
{
    int index = this->index(name);

    BSLS_ASSERT(0                               <= index);
    BSLS_ASSERT(static_cast<bsl::size_t>(index) <  d_schema_p->size());

    return (*d_schema_p)[index].d_type;
}

inline
const OptionValue& CommandLineOptionsHandle::value(bsl::size_t index) const
{
    BSLS_ASSERT(index < d_schema_p->size());

    return (*d_data_p)[index];
}

inline
const OptionValue& CommandLineOptionsHandle::value(
                                            const bsl::string_view& name) const
{
    int index = this->index(name);

    BSLS_ASSERT(0                               <= index);
    BSLS_ASSERT(static_cast<bsl::size_t>(index) <  d_schema_p->size());

    return (*d_data_p)[index];
}

                        // 'the*' Accessors

// BDE_VERIFY pragma: -FABC01  // not in alphabetic order

template <class TYPE>
const TYPE& CommandLineOptionsHandle::the(bsl::size_t index) const
{
    BSLS_ASSERT(OptionType::TypeToEnum<TYPE>::value == type(index));
    BSLS_ASSERT(!value(index).isNull());

    return  (*d_data_p)[index].the<TYPE>();
}

template <class TYPE>
const TYPE& CommandLineOptionsHandle::the(const bsl::string_view& name) const
{
    int index = this->index(name);

    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(OptionType::TypeToEnum<TYPE>::value == type(index));
    BSLS_ASSERT(!value(index).isNull());

    return the<TYPE>(index);
}

inline
bool CommandLineOptionsHandle::theBool(const bsl::string_view& name) const
{
    int index = this->index(name);

    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(OptionType::e_BOOL == type(index));
    BSLS_ASSERT(!value(index).isNull());

    return the<OptionType::Bool>(index);
}

inline
char CommandLineOptionsHandle::theChar(const bsl::string_view& name) const
{
    int index = this->index(name);

    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(OptionType::e_CHAR == type(index));
    BSLS_ASSERT(!value(index).isNull());

    return the<OptionType::Char>(index);
}

inline
int CommandLineOptionsHandle::theInt(const bsl::string_view& name) const
{
    int index = this->index(name);

    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(OptionType::e_INT == type(index));
    BSLS_ASSERT(!value(index).isNull());

    return the<OptionType::Int>(index);
}

inline
bsls::Types::Int64 CommandLineOptionsHandle::theInt64(
                                            const bsl::string_view& name) const
{
    int index = this->index(name);

    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(OptionType::e_INT64 == type(index));
    BSLS_ASSERT(!value(index).isNull());

    return the<OptionType::Int64>(index);
}

inline
double CommandLineOptionsHandle::theDouble(const bsl::string_view& name) const
{
    int index = this->index(name);

    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(OptionType::e_DOUBLE == type(index));
    BSLS_ASSERT(!value(index).isNull());

    return the<OptionType::Double>(index);
}

inline
const bsl::string&
CommandLineOptionsHandle::theString(const bsl::string_view& name) const
{
    int index = this->index(name);

    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(OptionType::e_STRING == type(index));
    BSLS_ASSERT(!value(index).isNull());

    return the<OptionType::String>(index);
}

inline
const bdlt::Datetime&
CommandLineOptionsHandle::theDatetime(const bsl::string_view& name) const
{
    int index = this->index(name);

    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(OptionType::e_DATETIME == type(index));
    BSLS_ASSERT(!value(index).isNull());

    return the<OptionType::Datetime>(index);
}

inline
const bdlt::Date&
CommandLineOptionsHandle::theDate(const bsl::string_view& name) const
{
    int index = this->index(name);

    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(OptionType::e_DATE == type(index));
    BSLS_ASSERT(!value(index).isNull());

    return the<OptionType::Date>(index);
}

inline
const bdlt::Time& CommandLineOptionsHandle::theTime(
                                            const bsl::string_view& name) const
{
    int index = this->index(name);

    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(OptionType::e_TIME == type(index));
    BSLS_ASSERT(!value(index).isNull());

    return the<OptionType::Time>(index);
}

inline
const bsl::vector<char>&
CommandLineOptionsHandle::theCharArray(const bsl::string_view& name) const
{
    int index = this->index(name);

    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(OptionType::e_CHAR_ARRAY == type(index));
    BSLS_ASSERT(!value(index).isNull());

    return the<OptionType::CharArray>(index);
}

inline
const bsl::vector<int>&
CommandLineOptionsHandle::theIntArray(const bsl::string_view& name) const
{
    int index = this->index(name);

    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(OptionType::e_INT_ARRAY == type(index));
    BSLS_ASSERT(!value(index).isNull());

    return the<OptionType::IntArray>(index);
}

inline
const bsl::vector<bsls::Types::Int64>&
CommandLineOptionsHandle::theInt64Array(const bsl::string_view& name) const
{
    int index = this->index(name);

    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(OptionType::e_INT64_ARRAY == type(index));
    BSLS_ASSERT(!value(index).isNull());

    return the<OptionType::Int64Array>(index);
}

inline
const bsl::vector<double>&
CommandLineOptionsHandle::theDoubleArray(const bsl::string_view& name) const
{
    int index = this->index(name);

    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(OptionType::e_DOUBLE_ARRAY == type(index));
    BSLS_ASSERT(!value(index).isNull());

    return the<OptionType::DoubleArray>(index);
}

inline
const bsl::vector<bsl::string>&
CommandLineOptionsHandle::theStringArray(const bsl::string_view& name) const
{
    int index = this->index(name);

    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(OptionType::e_STRING_ARRAY == type(index));
    BSLS_ASSERT(!value(index).isNull());

    return the<OptionType::StringArray>(index);
}

inline
const bsl::vector<bdlt::Datetime>&
CommandLineOptionsHandle::theDatetimeArray(const bsl::string_view& name) const
{
    int index = this->index(name);

    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(OptionType::e_DATETIME_ARRAY == type(index));
    BSLS_ASSERT(!value(index).isNull());

    return the<OptionType::DatetimeArray>(index);
}

inline
const bsl::vector<bdlt::Date>&
CommandLineOptionsHandle::theDateArray(const bsl::string_view& name) const
{
    int index = this->index(name);

    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(OptionType::e_DATE_ARRAY == type(index));
    BSLS_ASSERT(!value(index).isNull());

    return the<OptionType::DateArray>(index);
}

inline
const bsl::vector<bdlt::Time>&
CommandLineOptionsHandle::theTimeArray(const bsl::string_view& name) const
{
    int index = this->index(name);

    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(OptionType::e_TIME_ARRAY == type(index));
    BSLS_ASSERT(!value(index).isNull());

    return the<OptionType::TimeArray>(index);
}

// BDE_VERIFY pragma: +FABC01  // not in alphabetic order

}  // close package namespace

// FREE OPERATORS
inline
bool balcl::operator==(const balcl::CommandLineOptionsHandle& lhs,
                       const balcl::CommandLineOptionsHandle& rhs)
{
    BSLS_ASSERT(lhs.d_data_p); BSLS_ASSERT(lhs.d_schema_p);
    BSLS_ASSERT(rhs.d_data_p); BSLS_ASSERT(rhs.d_schema_p);

    return *lhs.d_data_p   == *rhs.d_data_p
        && *lhs.d_schema_p == *rhs.d_schema_p;
}

inline
bool balcl::operator!=(const balcl::CommandLineOptionsHandle& lhs,
                       const balcl::CommandLineOptionsHandle& rhs)
{
    BSLS_ASSERT(lhs.d_data_p); BSLS_ASSERT(lhs.d_schema_p);
    BSLS_ASSERT(rhs.d_data_p); BSLS_ASSERT(rhs.d_schema_p);

    return *lhs.d_data_p   != *rhs.d_data_p
        || *lhs.d_schema_p != *rhs.d_schema_p;
}

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
