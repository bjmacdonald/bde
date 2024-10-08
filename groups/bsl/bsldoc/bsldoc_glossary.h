// bsldoc_glossary.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLDOC_GLOSSARY
#define INCLUDED_BSLDOC_GLOSSARY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide definitions for terms used throughout BDE documentation.
//
//@CLASSES:
//
//@DESCRIPTION: This component documents common terminology that is used
// throughout the rest of BDE component documentation.  The terminology is
// broken into the following sections:
// ```
// ,-------------------------------------------------------------------------.
// | Topic               |  Term                                   |   Key   |
// |-------------------------------------------------------------------------|
// | Exception Safety    |                                         |         |
// |                     | Exception-Neutral                       | [ES.1 ] |
// |                     | Basic Guarantee                         | [ES.2 ] |
// |                     | Strong Guarantee                        | [ES.3 ] |
// |                     | No-Throw Guarantee                      | [ES.4 ] |
// |                     | Injected Exception                      | [ES.5 ] |
// |---------------------+-----------------------------------------+---------|
// | Thread Safety       |                                         |         |
// |                     | Minimally Thread-Safe                   | [TS.1 ] |
// |                     | 'const' Thread-Safe                     | [TS.2 ] |
// |                     | Fully Thread-Safe                       | [TS.3 ] |
// |                     | Thread-Aware                            | [TS.4 ] |
// |                     | Thread-Enabled                          | [TS.5 ] |
// |---------------------+-----------------------------------------+---------|
// | Type Classification |                                         |         |
// |                     | Value-Semantic Type                     | [TC.1 ] |
// |                     | In-Core Value-Semantic Type             | [TC.2 ] |
// |                     | Attribute Type                          | [TC.3 ] |
// |                     | Unconstrained Attribute Type            | [TC.4 ] |
// |                     | Simply-Constrained Attribute Type       | [TC.5 ] |
// |                     | Complex-Constrained Attribute Type      | [TC.6 ] |
// |                     | Protocol                                | [TC.7 ] |
// |                     | Mechanism                               | [TC.8 ] |
// |                     | Utility Type                            | [TC.9 ] |
// |                     | Vocabulary Type                         | [TC.10] |
// |---------------------+-----------------------------------------+---------|
// | Value Semantics     |                                         |         |
// |                     | Value-Semantic Operations               | [VS.1 ] |
// |---------------------+-----------------------------------------+---------|
// | Uncategorized       |                                         |         |
// |                     | Alias-Safe                              | [UC.1 ] |
// |                     | Guard                                   | [UC.2 ] |
// |                     | Proctor                                 | [UC.3 ] |
// `-------------------------------------------------------------------------'
// ```
//
///Terminology (in Alphabetic Order)
///---------------------------------
//
///Alias-Safe [UC.1]
///- - - - - - - - -
// * A *function* is *alias-safe* if it behaves as advertized in its contract,
//   even when multiple arguments are aliases for the same object or
//   overlapping regions of memory.
// * A *class* is alias-safe if all of its methods are alias-safe.
//
///Attribute Type [TC.3]
///- - - - - - - - - - -
// * A value-semantic type (see "Value-Semantic Type") is an *attribute*
//   *type* if the type provides no primitive functionality other than
//   standard value-semantic operations (e.g., equality comparison) along with
//   manipulators and accessors to get and set the contained attribute fields.
//   - Attribute types are a subset of value-semantic types.  Therefore, an
//     attribute type must provide a notion of value (i.e., provide
//     `operator==`).
//   - See "Value-Semantic Operations" for a list of standard value-semantic
//     operations.
//
///Basic Guarantee [ES.2]
/// - - - - - - - - - - -
// * A *class* provides the *basic* *guarantee* if objects of that type emerge
//   in some valid, but otherwise unspecified, state should one of its methods
//   terminate as the result of an injected exception (irrespective of the
//   exception source).
//
///Complex-Constrained Attribute Type [TC.6]
///- - - - - - - - - - - - - - - - - - - - -
// * An attribute type (see "Attribute Type") is a *complex-constrained*
//   *attribute* *type* if there are constraints on the values of one or more
//   attributes, and the value of one attribute affects what values are valid
//   for one or more other attributes.
//   - Complex-constrained attribute types are a subset of attribute types,
//     which, in turn, are a subset of value-semantic types.  Therefore, a
//     complex-constrained attribute type must have a notion of value (i.e.,
//     define `operator==`) and provide no primitive functionality aside from
//     the standard value-semantic operations with manipulators and accessors
//     to get and set the contained attribute fields.
//
///`const` Thread-Safe [TS.2]
/// - - - - - - - - - - - - -
// * A *class* is *`const`* *thread-safe* if its accessors may be invoked
//   concurrently from different threads, but it is not safe to access or
//   modify an object of that class in one thread while another thread
//   modifies the same object.
//   - This is the default, and is generally true of classes without either
//     static or mutable data members.
//   - BDE components are assumed to be `const` thread-safe unless documented
//     otherwise.
//   - A class that is `const` thread-safe is also minimally thread-safe.
//
///Exception-Neutral [ES.1]
/// - - - - - - - - - - - -
// * A *function* is *exception-neutral* if it safely propagates to its caller
//   externally injected exceptions (e.g., via lower-level code, template
//   parameters, virtual functions, or callbacks).
// * A *class* is exception-neutral if all of its methods are
//   exception-neutral.
//   - A class is exception-neutral if it provides at least the *basic*
//     *guarantee*, and propagates injected exceptions.
//   - BDE components use RAII to provide exception neutrality, not
//     `try`/`catch`.
//
///Fully Thread-Safe [TS.3]
/// - - - - - - - - - - - -
// * A *class* is *fully* *thread-safe* if all non-creators (and any `friend`
//   functions) can safely execute concurrently.
//   - A class that is fully thread-safe and has at least one manipulator is
//     also thread-aware.
//   - A `const` thread-safe class that has no manipulators is fully
//     thread-safe.
//   - Notably it is not safe to destroy an object before before all functions
//     called on the object have completed.  Typically some form of
//     synchronization (external to the object itself) is need to ensure that
//     the object can be safely destroyed.
//
///Guard [UC.2]
/// - - - - - -
// * A (scoped) guard is an object that maintains control over an associated
//   resource (often acquired on construction) and releases that control when
//   the guard is destroyed (typically at scope exit -- either normally or
//   because an exception was thrown).  The guard *may* provide an explicit
//   release method, but -- unlike a Proctor (see [UC.3]) -- such a release
//   method is not commonly used.
//   - A canonical example is a lock guard (e.g., `std::lock_guard`), which
//     acquires a lock (typically on a mutex) at construction, and releases
//     that lock upon the guard's destruction.
//
///In-Core Value-Semantic Type [TC.2]
/// - - - - - - - - - - - - - - - - -
// * A value-semantic type (see "Value-Semantic Type") is an *in-core*
//   *value-semantic* *type* if the type's value is somehow tied to the
//   current process, for example, if a *salient* *attribute* of the value is
//   a reference to (i.e., the address of) some other autonomous (un-owned)
//   object.
//   - In-core value-semantic types are a subset of value-semantic types.
//     Therefore, an in-core value-semantic type must have a notion of value
//     (i.e., provide `operator==`).
//   - Because an in-core value-semantic type's notion of value is tied to the
//     current process, that "value" (by itself) is not externalizable.
//
///Injected Exception [ES.5]
///- - - - - - - - - - - - -
// * An *injected* *exception* is any exception generated within a method of
//   an object without the explicit use of `throw`.  For example, an exception
//   may be injected by calling a method on a contained object (which, in
//   turn, throws), or via a template parameter, virtual function call, or
//   callback.
//
///Mechanism [TC.8]
/// - - - - - - - -
// * A *class* is a *mechanism* if objects of the class maintain state, but
//   have no sensible notion of value.  Therefore, mechanisms do not provide
//   standard value-semantic operations such as equality comparison,
//   copy construction, or copy assignment.
//   - For example, memory allocators and thread pools are mechanisms.
//
///Minimally Thread-Safe [TS.1]
/// - - - - - - - - - - - - - -
// * A *class* is *minimally* *thread-safe* if two threads can safely operate
//   on two distinct objects of the class.
//
///No-Throw Guarantee [ES.4]
///- - - - - - - - - - - - -
// * A *function* provides the *no-throw* *guarantee* if it will not throw, or
//   in any way propagate, an exception to the caller under any circumstances.
//   - The no-throw guarantee is exceedingly rare in BDE libraries.  Any
//     memory allocation performed using a type derived from
//     `bslma::Allocator` may throw a memory-allocation exception.
//     Additionally, a function may test its input to verify that it has been
//     called in-contract (e.g., using `bsls_assert`) and may throw an
//     exception in cases where it is called out-of-contract (functions with
//     any undefined behavior, by definition, cannot provide the no-throw
//     guarantee).
//
///Proctor [UC.3]
/// - - - - - - -
// * A proctor is a special kind of guard intended to restore a valid state
//   under abnormal circumstances (e.g., a thrown exception), until a valid
//   state is restored normally, after which the proctor's responsibility is
//   explicitly released by its client.  The proctor *must* provide a
//   mechanism to release the resource from management, and -- unlike a
//   standard Guard (see [UC.2]) -- its management responsibility is
//   typically released prior to its destruction.
//   - A canonical example is `bslalg::AutoArrayDestructor`, which is used to
//     implement exception safety while moving elements within an array,
//     restoring a valid state in case of an exception, or doing nothing once
//     its `release` method is invoked (after a valid state has been
//     reestablished).
//
///Protocol [TC.7]
///- - - - - - - -
// * A *protocol* is an abstract base class defining only pure virtual
//   functions, aside from the (non-pure virtual) destructor.
//   - Protocol is a more precisely defined term for what is frequently
//     referred to as an "interface".
//
///Simply-Constrained Attribute Type [TC.5]
/// - - - - - - - - - - - - - - - - - - - -
// * An attribute type (see "Attribute Type") is a *simply-constrained*
//   *attribute* *type* if there are constraints on the values of one or more
//   individual attributes, but the constraints are independent.
//   - Simply-constrained attribute types are a subset of attribute types,
//     which, in turn, are a subset of value-semantic types.  Therefore, a
//     simply-constrained attribute type must have a notion of value (i.e.,
//     define `operator==`), and provide no primitive functionality aside from
//     the standard value-semantic operations with manipulators and accessors
//     to get and set the contained attribute fields.
//
///Strong Guarantee [ES.3]
///- - - - - - - - - - - -
// * A *class* provides the *strong* *guarantee* if objects of that type
//   emerge with all of their (observable) state unchanged (e.g., all
//   iterators on the object will remain valid), and none of its output
//   parameters modified, should one of its methods terminate as the result of
//   an injected exception (irrespective of the exception source).  Note that
//   it is not generally possible to ensure that all global state, or optional
//   modifiable parameters (e.g., allocators), will be entirely unaffected.
//
///Thread-Aware [TS.4]
///- - - - - - - - - -
// * A *class* that is *thread-aware* is designed with multithreading in mind
//   and provides at least one manipulator (or `friend` function) that can
//   safely execute concurrently on a single object by multiple threads.
//
///Thread-Enabled [TS.5]
///- - - - - - - - - - -
// * A *class* that is *thread-enabled* is designed to directly interact with
//   threads, and cannot function correctly in a non-multithreading
//   environment (e.g., a thread pool is thread-enabled, so *must* be used in
//   a multithreading environment).  Typically a *thread-enabled* class
//   creates or joins threads.
//
///Unconstrained Attribute Type [TC.4]
///- - - - - - - - - - - - - - - - - -
// * An attribute type (see "Attribute Type") is an *unconstrained*
//   *attribute* *type* if every combination of individual attribute values is
//   valid for the overall type.
//   - Unconstrained attribute types are a subset of attribute types, which,
//     in turn, are a subset of value-semantic types.  Therefore, an
//     unconstrained attribute type must have a notion of value (i.e., define
//     `operator==`) and provide no primitive functionality aside from the
//     standard value-semantic operations with manipulators and accessors to
//     get and set the contained attribute fields.
//
///Utility Type [TC.9]
///- - - - - - - - - -
// * A *utility* *type* is a `struct` (or `class`) serving as a namespace for
//   a suite of C-style (static) functions.  These types have no instance data
//   members or instance methods.
//
///Value-Semantic Operations [VS.1]
/// - - - - - - - - - - - - - - - -
// * A standard set of operations provided by all value-semantic types (see
//   "Value-Semantic Type").  A value-semantic type provides all of these
//   operators unless otherwise documented:
//   - Equality and Non-Equality Comparisons
//   - Copy Construction
//   - Copy Assignment
//   - Default Construction
//   - `ostream` Printing
//   - `bslx` Serialization (optional)
//
///Value-Semantic Type [TC.1]
/// - - - - - - - - - - - - -
// * A *class* is a *value-semantic* *type* if objects of that type have a
//   value.  A type provides a value if and only if the class defines
//   `operator==`, and the value of an object of that class is defined in
//   terms of `operator==`: Two objects of a type *have* *the* *same* *value*
//   if and only if `operator==`, when applied to these objects, returns
//   `true`.  In addition, types that define a value (i.e., provide
//   `operator==`) *must* have the following properties:
//   1. `operator==` is commutative (i.e., `A == B` if and only if `B == A`).
//   2. `operator!=` is the inverse of `operator==` (i.e., `A != B` if and only
//      if `!(A == B)`).
//   3. Two object of the type have the same value if and only if the values of
//      each of the *salient* *attributes* -- i.e., those attributes (each a
//      value-semantic type) that comprise the value -- respectively compare
//      equal.
//   4. There is *no* sequence of operations that, when separately applied to
//      two objects of the type having the same value, cause them to assume
//      different values.
//   5. The value of an object of the type is independent of any modifiable
//      state that is not owned exclusively by that object.
//   6. There exists a programmatic means, independent of the equality
//      operator, to determine if two objects of the type have the same value.
// * A value-semantic class is exception-neutral (see "Exception-Neutral") and
//   provides a standard set of operations (see "Value-Semantic Operations")
//   unless otherwise documented.
//
///Vocabulary Type [TC.10]
///- - - - - - - - - - - -
// * A *vocabulary* *type* is a type that holds a value or performs a service
//   that is used widely *in* *the* *interface* of classes and/or free
//   function (hence, it is part of the "vocabulary" used to communicate
//   between types in the system).
//   - `bsl::string` is a vocabulary type because it is used in the interface
//     of many types to communicate the value of a character string.

#include <bslscm_version.h>

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
