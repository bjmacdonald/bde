// bslma_default.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLMA_DEFAULT
#define INCLUDED_BSLMA_DEFAULT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities to set/fetch the default and global allocators.
//
//@CLASSES:
//  bslma::Default: namespace for default/global allocator management utilities
//
//@SEE_ALSO: bslma_allocator, bslma_newdeleteallocator
//
//@DESCRIPTION: This component provides a set of utility functions that manage
// the addresses of two distinguished memory allocators: the *default*
// allocator and the *global* allocator.  Each of these allocators are of type
// derived from `bslma::Allocator`.  Note that for brevity in the following we
// will generally refer to "the address of the default allocator" as simply
// "the default allocator" (and similarly for the global allocator).
//
// The global allocator is intended to be used as the allocator for (global)
// singleton objects.  In general, the default allocator is for all other
// memory allocations in contexts where an alternative allocator is not
// explicitly specified (or *cannot* be specified as, for example, when a
// compiler-generated temporary object of a type that requires an allocator is
// created).
//
// Initially, both the default allocator and global allocator resolve to the
// address of the `bslma::NewDeleteAllocator` singleton, i.e.:
// ```
// &bslma::NewDeleteAllocator::singleton()
// ```
// Methods are provided to retrieve and set the two allocators independently.
// The following two subsections supply further details, in turn, on the
// methods that pertain to the default and global allocators.
//
///Default Allocator
///-----------------
// Two methods provide access to the default allocator,
// `bslma::Default::defaultAllocator` and `bslma::Default::allocator` (the
// latter when called with no argument, or an explicit 0).  When
// `bslma::Default::allocator` is supplied with a non-0 argument, it simply
// returns that argument to the caller (i.e., it acts as a pass-through).  A
// (non-singleton) class that is designed to take advantage of an allocator
// will typically revert to the default allocator whenever a constructor is
// called without an allocator (yielding the default argument value of 0).  The
// `bslma::Default::allocator` method facilitates this behavior.  See the usage
// examples below for an illustration of this technique.
//
// The default allocator can be set *prior* to a call to
// `bslma::Default::defaultAllocator`, to `bslma::Default::allocator` with no
// argument or an explicit 0, or to `bslma::Default::lockDefaultAllocator`, by
// calling `bslma::Default::setDefaultAllocator`.  This method returns 0 on
// success and a non-zero value on failure.  This method fails if the default
// allocator is "locked".  The default allocator is initially unlocked.  It is
// *explicitly* locked by calling `bslma::Default::lockDefaultAllocator`.  In
// addition, the default allocator is *implicitly* locked as a *side-effect* of
// calling `bslma::Default::defaultAllocator`, or `bslma::Default::allocator`
// with no argument or an explicit 0.  Once locked, the default allocator
// cannot be unlocked.  However, the `bslma::Default::setDefaultAllocatorRaw`
// method will unconditionally set the default allocator regardless of whether
// it is locked.  When the C++17 `pmr` library is available, setting the
// default allocator by any of these methods also sets the default memory
// resource to the same value by invoking `std::pmr::set_default_resource`.
//
// A well-behaved program should call `bslma::Default::setDefaultAllocator`
// *once*.  It should be invoked in `main` before starting any threads, and be
// followed immediately by a call to 'bslma::Default::lockDefaultAllocator.
// Note that `bslma::Default::setDefaultAllocatorRaw` is provided for *testing*
// *only*, and should typically *never* be used in a production environment.
//
// When using a platform library that supports the C++17 PMR interface, a
// successful change to the default allocator will also result in changing the
// default *memory* *resource* returned by `std::pmr::get_default_resource` to
// the same value.  This automatic syncronization can be broken by deliberately
// calling `std::pmr::set_default_resource`, which will change the default
// memory resource without also changing the default allocator.  Note that
// `std::pmr::get_default_resource` does not lock the default memory resource
// the way `bslma::Default::defaultAllocator` locks the default allocator.
//
// *WARNING*: Note that the default allocator can become locked prior to
// entering `main` as a side-effect of initializing a file-scope static object.
// For example, the presence of a global `bsl::string` object in an executable
// will have this unintended consequence.  Further note that this phenomenon
// can *vary* *across* *platforms*.  In particular, linkers differ as to the
// aggressiveness with which they pull in file-scope static objects from the
// libraries that are on the link line.  *AVOID* file-scope static objects that
// require runtime initialization, *especially* those that take an allocator.
//
///Global Allocator
///----------------
// The interface pertaining to the global allocator is comparatively much
// simpler, consisting of just two methods.  The
// `bslma::Default::globalAllocator` method, when called with no argument (or
// an explicit 0), returns the global allocator currently in effect at the
// point of call.  It has *no* side-effects.  When supplied with a non-0
// argument, `bslma::Default::globalAllocator` simply returns that argument to
// the caller (i.e., it acts as a pass-through similar to
// `bslma::Default::allocator` when it is supplied with a non-0 argument).  The
// global allocator may be set using the `bslma::Default::setGlobalAllocator`
// method.  This method *always* succeeds.  In that respect, the global
// allocator cannot become locked like the default allocator.
// `bslma::Default::setGlobalAllocator` returns the global allocator that is in
// effect upon entry to the function.
//
// Note that `bslma::Default::setGlobalAllocator` should be used with *extreme*
// *caution*.  In particular, a well-behaved program should call this function
// at most once.  If called, it should be invoked in `main` before starting any
// threads and before initializing singletons.
//
///Usage
///-----
// The following sequence of usage examples illustrate recommended use of the
// default and global allocators.  The examples employ the following simple
// memory allocator, `my_CountingAllocator`, that counts both the number of
// memory blocks that have been allocated, but not yet deallocated, and the
// cumulative number of blocks ever allocated.  The two values are available
// through the accessors `numBlocksInUse` and `numBlocksTotal`, respectively.
// For actual allocations and deallocations, `my_CountingAllocator` uses global
// operators `new` and `delete`:
// ```
// // my_countingallocator.h
// #include <bslma_allocator.h>
//
// /// This concrete allocator maintains: (1) a count of the number of
// /// blocks allocated that have not yet been deallocated, and (2) a count
// /// of the cumulative number of blocks ever allocated.
// class my_CountingAllocator : public bslma::Allocator {
//
//     // DATA
//     int d_numBlocksInUse;  // number of blocks currently allocated
//     int d_numBlocksTotal;  // cumulative blocks ever requested
//
//     // NOT IMPLEMENTED
//     my_CountingAllocator(const my_CountingAllocator&);
//     my_CountingAllocator& operator=(const my_CountingAllocator&);
//
//   public:
//     // CREATORS
//
//     /// Create a counting allocator.
//     my_CountingAllocator();
//
//     /// Destroy this counting allocator.
//     virtual ~my_CountingAllocator();
//
//     // MANIPULATORS
//
//     /// Return a newly allocated block of memory of (at least) the
//     /// specified positive `size` (bytes).  If `size` is 0, a null
//     /// pointer is returned with no effect.  Note that the alignment of
//     /// the address returned is the maximum alignment for any
//     /// fundamental type defined for this platform.
//     virtual void *allocate(size_type size);
//
//     /// Return the memory at the specified `address` back to this
//     /// allocator.  If `address` is 0, this function has no effect.  The
//     /// behavior is undefined if `address` was not allocated using this
//     /// allocator, or has already been deallocated.
//     virtual void deallocate(void *address);
//
//     // ACCESSORS
//
//     /// Return the number of blocks currently in use from this counting
//     /// allocator.
//     int numBlocksInUse() const;
//
//     /// Return the cumulative number of blocks ever allocated using this
//     /// counting allocator.  Note that
//     /// numBlocksTotal() >= numBlocksInUse().
//     int numBlocksTotal() const;
// };
//
// // CREATORS
// inline
// my_CountingAllocator::my_CountingAllocator()
// : d_numBlocksInUse(0)
// , d_numBlocksTotal(0)
// {
// }
//
// // ACCESSORS
// inline
// int my_CountingAllocator::numBlocksInUse() const
// {
//     return d_numBlocksInUse;
// }
//
// inline
// int my_CountingAllocator::numBlocksTotal() const
// {
//     return d_numBlocksTotal;
// }
// ```
// The `virtual` methods of `my_CountingAllocator` are defined in the component
// `.cpp` file:
// ```
// // my_countingallocator.cpp
// #include <my_countingallocator.h>
//
// // CREATORS
// my_CountingAllocator::~my_CountingAllocator()
// {
// }
//
// // MANIPULATORS
// void *my_CountingAllocator::allocate(size_type size)
// {
//     ++d_numBlocksInUse;
//     ++d_numBlocksTotal;
//     return ::operator new(size);
// }
//
// void my_CountingAllocator::deallocate(void *address)
// {
//     --d_numBlocksInUse;
//     ::operator delete(address);
// }
// ```
//
///Example 1: Basic Default Allocator Use
/// - - - - - - - - - - - - - - - - - - -
// This usage example illustrates the basics of class design that relate to
// proper use of the default allocator, and introduces the standard pattern to
// apply when setting (and *locking*) the default allocator.  First we define a
// trivial class, `my_Id`, that uses an allocator.  `my_Id` simply encapsulates
// a C-style (null-terminated) id string that is accessible through the `id`
// method.  Note that each constructor is declared to take an *optional*
// `bslma::Allocator *` as its last argument.  Also note that the expression:
// ```
// bslma::Default::allocator(basicAllocator)
// ```
// is used in applicable member initializers to propagate each constructor's
// allocator argument to the data members that require it (in this case, the
// object allocator that is held by each `my_Id` object).  If `basicAllocator`
// is 0, the object is created using the default allocator.  Otherwise, the
// explicitly supplied allocator is used:
// ```
// // my_id.h
// #include <bslma_allocator.h>
// #include <bslma_default.h>
//
// /// This is a trivial class solely intended to illustrate proper use
// /// of the default allocator.
// class my_Id {
//
//     // DATA
//     char             *d_buffer_p;     // allocated (*owned*)
//     bslma::Allocator *d_allocator_p;  // allocator (held, not owned)
//
//     // NOT IMPLEMENTED (in order to reduce example size)
//     my_Id& operator=(const my_Id&);
//
//   public:
//     // CREATORS
//
//     /// Create an Id object having the specified `id`.  Optionally
//     /// specify a `basicAllocator` used to supply memory.  If
//     /// `basicAllocator` is 0, the currently installed default allocator
//     /// is used.
//     explicit my_Id(const char *id, bslma::Allocator *basicAllocator = 0);
//
//     /// Create an Id object initialized to the value of the specified
//     /// `original` Id object.  Optionally specify a `basicAllocator`
//     /// used to supply memory.  If `basicAllocator` is 0, the currently
//     /// installed default allocator is used.
//     my_Id(const my_Id& original, bslma::Allocator *basicAllocator = 0);
//
//     /// Destroy this Id object.
//     ~my_Id();
//
//     // ACCESSORS
//
//     /// Return the id of this Id object.
//     const char *id() const;
// };
//
// // CREATORS
// inline
// my_Id::my_Id(const char *id, bslma::Allocator *basicAllocator)
// : d_allocator_p(bslma::Default::allocator(basicAllocator))
// {
//     d_buffer_p = (char *)d_allocator_p->allocate(std::strlen(id) + 1);
//     std::strcpy(d_buffer_p, id);
// }
//
// inline
// my_Id::my_Id(const my_Id& original, bslma::Allocator *basicAllocator)
// : d_allocator_p(bslma::Default::allocator(basicAllocator))
// {
//     const char *id = original.id();
//     d_buffer_p = (char *)d_allocator_p->allocate(std::strlen(id) + 1);
//     std::strcpy(d_buffer_p, id);
// }
//
// inline
// my_Id::~my_Id()
// {
//     d_allocator_p->deallocate(d_buffer_p);
// }
//
// // ACCESSORS
// inline
// const char *my_Id::id() const
// {
//     return d_buffer_p;
// }
// ```
// Next we set the default allocator to one of our counting allocator objects.
// Note that immediately after successfully setting it, we lock the default
// allocator, so that subsequent calls to `bslma::Default::setDefaultAllocator`
// fail.  (The default allocator can still be modified by calling
// `bslma::Default::setDefaultAllocatorRaw`, but calling that function in
// production code is anti-social.  Our usage examples expressly do *not* call
// that method.)  With the possible exception of test drivers, the default
// allocator should be set and locked early in `main` before threads are
// started and before objects are initialized:
// ```
// static my_CountingAllocator defaultCountingAllocator;
//
// int status =
//             bslma::Default::setDefaultAllocator(&defaultCountingAllocator);
// assert(0 == status);
// bslma::Default::lockDefaultAllocator();  // subsequent calls to "set" fail
// assert(bslma::Default::defaultAllocator() == &defaultCountingAllocator);
//
// status = bslma::Default::setDefaultAllocator(
//                                   &bslma::NewDeleteAllocator::singleton());
// assert(0 != status);
// assert(bslma::Default::defaultAllocator() == &defaultCountingAllocator);
// ```
// In the following, we instantiate two objects of type `my_Id`.  The first
// object, `idA`, is not supplied with an allocator, so it uses the default
// allocator.  The second object, `idB`, is supplied with an object of type
// `my_CountingAllocator`.  The assertions track the states of the two
// allocators at each point in the code fragment.  In particular, note that the
// state of the default allocator does not change during the lifetime of `idB`:
// ```
// assert(0 == defaultCountingAllocator.numBlocksInUse());
// assert(0 == defaultCountingAllocator.numBlocksTotal());
// {
//     my_Id id("A");
//     assert(1 == defaultCountingAllocator.numBlocksInUse());
//     assert(1 == defaultCountingAllocator.numBlocksTotal());
// }
// assert(0 == defaultCountingAllocator.numBlocksInUse());
// assert(1 == defaultCountingAllocator.numBlocksTotal());
//
// my_CountingAllocator objectCountingAllocator;
// assert(0 == objectCountingAllocator.numBlocksInUse());
// assert(0 == objectCountingAllocator.numBlocksTotal());
// {
//     my_Id idB("B", &objectCountingAllocator);
//     assert(1 == objectCountingAllocator.numBlocksInUse());
//     assert(1 == objectCountingAllocator.numBlocksTotal());
//     assert(0 == defaultCountingAllocator.numBlocksInUse());
//     assert(1 == defaultCountingAllocator.numBlocksTotal());
// }
// assert(0 == objectCountingAllocator.numBlocksInUse());
// assert(1 == objectCountingAllocator.numBlocksTotal());
// assert(0 == defaultCountingAllocator.numBlocksInUse());
// assert(1 == defaultCountingAllocator.numBlocksTotal());
// ```
//
///Example 2: Detecting Allocator Propagation Bugs
///- - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how the default allocator is used to detect a very
// common programming error pertaining to allocator usage.  First we define the
// trivial (but buggy) `my_IdPair` class:
// ```
// // my_idpair.h
// #include <my_id.h>
// #include <bslma_default.h>
//
// /// This is a trivial class solely intended to help illustrate a common
// /// programming error.  This class has two objects of type `my_Id`, only
// /// one of which has the allocator correctly passed to it in the
// /// definition of the constructor.
// class my_IdPair {
//
//     // DATA
//     my_Id d_id;     // primary id (allocating)
//     my_Id d_alias;  // alias (allocating)
//
//     // NOT IMPLEMENTED (in order to reduce example size)
//     my_IdPair(const my_IdPair&);
//     my_IdPair& operator=(const my_IdPair&);
//
//   public:
//     // CREATORS
//
//     /// Create an Id pair having the specified `id` and `alias` ids.
//     /// Optionally specify a `basicAllocator` used to supply memory.  If
//     /// `basicAllocator` is 0, the currently installed default allocator
//     /// is used.
//     my_IdPair(const char       *id,
//               const char       *alias,
//               bslma::Allocator *basicAllocator = 0);
//
//     /// Destroy this Id pair.
//     ~my_IdPair();
//
//     // ACCESSORS
//
//     /// Return the primary id of this Id pair.
//     const char *id() const;
//
//     /// Return the alias of this Id pair.
//     const char *alias() const;
// };
//
// // CREATORS
// inline
// my_IdPair::my_IdPair(const char       *id,
//                      const char       *alias,
//                      bslma::Allocator *basicAllocator)
// : d_id(id, bslma::Default::allocator(basicAllocator))
// , d_alias(alias)  // drat! (forgot to pass along 'basicAllocator')
// {
// }
//
// inline
// my_IdPair::~my_IdPair()
// {
// }
//
// // ACCESSORS
// inline
// const char *my_IdPair::id() const
// {
//     return d_id.id();
// }
//
// inline
// const char *my_IdPair::alias() const
// {
//     return d_alias.id();
// }
// ```
// The definition of the `my_IdPair` constructor above intentionally includes a
// common programming error: The allocator in use by the object is not passed
// to *all* data members that require it.  We will see shortly how this error
// is detected at runtime using the default allocator.
//
// Next, the default allocator is set and locked identically to what was done
// in usage example 1:
// ```
// static my_CountingAllocator defaultCountingAllocator;
//
// int status =
//             bslma::Default::setDefaultAllocator(&defaultCountingAllocator);
// assert(0 == status);
// bslma::Default::lockDefaultAllocator();
// assert(bslma::Default::defaultAllocator() == &defaultCountingAllocator);
// ```
// Now we instantiate an object of type `my_IdPair` without explicitly
// specifying an allocator.  As a result, the object uses the default
// allocator.  The assertions verify the expected changes in the state of the
// default allocator:
// ```
// assert(0 == defaultCountingAllocator.numBlocksInUse());
// assert(0 == defaultCountingAllocator.numBlocksTotal());
// {
//     my_IdPair idPair("A", "B");
//     assert(2 == defaultCountingAllocator.numBlocksInUse());
//     assert(2 == defaultCountingAllocator.numBlocksTotal());
// }
// assert(0 == defaultCountingAllocator.numBlocksInUse());
// assert(2 == defaultCountingAllocator.numBlocksTotal());
// ```
// Next we instantiate a second object of type `my_IdPair`, this time supplying
// it with a counting allocator object that is distinct from the default
// allocator.  The assertions in the following code fragment that are commented
// out indicate the *expected* states of the allocators (i.e., in a bug-free
// implementation of `my_IdPair`) after the object has been constructed and
// again after it has been destroyed.  However, due to the (intentional) bug in
// the constructor, the uncommented assertions reveal the *true* state of
// affairs:
// ```
// my_CountingAllocator objectCountingAllocator;
// assert(0 == objectCountingAllocator.numBlocksInUse());
// assert(0 == objectCountingAllocator.numBlocksTotal());
// {
//     my_IdPair idPair("X", "Y", &objectCountingAllocator);
//     // assert(2 == objectCountingAllocator.numBlocksInUse());
//     // assert(2 == objectCountingAllocator.numBlocksTotal());
//     // assert(0 == defaultCountingAllocator.numBlocksInUse());
//     // assert(2 == defaultCountingAllocator.numBlocksTotal());
//     assert(1 == objectCountingAllocator.numBlocksInUse());
//     assert(1 == objectCountingAllocator.numBlocksTotal());
//     assert(1 == defaultCountingAllocator.numBlocksInUse());
//     assert(3 == defaultCountingAllocator.numBlocksTotal());
// }
// // assert(0 == objectCountingAllocator.numBlocksInUse());
// // assert(2 == objectCountingAllocator.numBlocksTotal());
// // assert(0 == defaultCountingAllocator.numBlocksInUse());
// // assert(2 == defaultCountingAllocator.numBlocksTotal());
// assert(0 == objectCountingAllocator.numBlocksInUse());
// assert(1 == objectCountingAllocator.numBlocksTotal());
// assert(0 == defaultCountingAllocator.numBlocksInUse());
// assert(3 == defaultCountingAllocator.numBlocksTotal());
// ```
// Note that, although not necessary in the case of the simple `my_IdPair`
// class, the default allocator can be used (and typically *should* be used)
// within the body of a constructor, or any other member function, to allocate
// dynamic memory that is *temporarily* needed by the method (and, hence, not
// owned by the object after the method has returned).  Thus, the invariant
// that must hold immediately after a method of an object returns is that the
// value returned by `defaultCountingAllocator.numBlocksInUse()` must be
// *identical* to what it was immediately prior to calling the method.  Of
// course, note that the above invariant pertains to cases in *single*-threaded
// programs where the object allocator in use by the object is *distinct* from
// the default allocator.  Also note that the value returned by
// `defaultCountingAllocator.numBlocksTotal()` *can* differ across function
// invocations (i.e., even in correct code).
//
///Example 3: Basic Global Allocator Use
///- - - - - - - - - - - - - - - - - - -
// Next we define a simple singleton class, `my_Singleton`, that defaults to
// using the global allocator if one is not explicitly specified when the
// singleton object is initialized.  Toward that end, note that in contrast to
// `my_Id`, the constructor for `my_Singleton` uses:
// ```
// bslma::Default::globalAllocator(basicAllocator)
// ```
// in its member initializer:
// ```
// // my_singleton.h
//
// /// This is a trivial singleton class solely intended to illustrate use
// /// of the global allocator.
// class my_Singleton {
//
//     // CLASS DATA
//     static my_Singleton *s_singleton_p;  // pointer to singleton object
//
//     // PRIVATE DATA
//     my_Id d_id;  // allocating
//
//     // NOT IMPLEMENTED
//     my_Singleton(const my_Singleton&  original,
//                  bslma::Allocator    *basicAllocator = 0);
//     my_Singleton& operator=(const my_Singleton& rhs);
//
//   private:
//     // PRIVATE CREATORS
//
//     /// Create a singleton having the specified `id`.  Optionally
//     /// specify a `basicAllocator` used to supply memory.  If
//     /// `basicAllocator` is 0, the currently installed global allocator
//     /// is used.
//     explicit my_Singleton(const char       *id,
//                           bslma::Allocator *basicAllocator = 0);
//
//     /// Destroy this singleton.
//     ~my_Singleton();
//
//   public:
//     // CLASS METHODS
//
//     /// Initialize the singleton with the specified `id`.  Optionally
//     /// specify a `basicAllocator` used to supply memory.  If
//     /// `basicAllocator` is 0, the currently installed global allocator
//     /// is used.
//     static void initSingleton(const char       *id,
//                               bslma::Allocator *basicAllocator = 0);
//
//     /// Return a reference to the non-modifiable singleton of this
//     /// class.  The behavior is undefined unless the singleton has been
//     /// initialized.
//     static const my_Singleton& singleton();
//
//     // ACCESSORS
//
//     /// Return the id of this singleton.
//     const char *id() const;
// };
//
// // CLASS METHODS
// inline
// const my_Singleton& my_Singleton::singleton()
// {
//     return *s_singleton_p;
// }
//
// // CREATORS
// inline
// my_Singleton::my_Singleton(const char *id,
//                            bslma::Allocator *basicAllocator)
// : d_id(id, bslma::Default::globalAllocator(basicAllocator))
// {
// }
//
// inline
// my_Singleton::~my_Singleton()
// {
// }
//
// // ACCESSORS
// inline
// const char *my_Singleton::id() const
// {
//     return d_id.id();
// }
// ```
// The following completes the definition of `my_Singleton` in the component
// `.cpp` file:
// ```
// // my_singleton.cpp
// #include <my_singleton.h>
// #include <bsls_alignedbuffer.h>
//
// my_Singleton *my_Singleton::s_singleton_p;
//
// // CLASS METHODS
// void my_Singleton::initSingleton(const char       *id,
//                                  bslma::Allocator *basicAllocator)
// {
//     static bsls::AlignedBuffer<sizeof(my_Singleton)> singleton;
//     s_singleton_p = new (singleton.buffer()) my_Singleton(id,
//                                                           basicAllocator);
// }
// ```
// In the following, the default and global allocators are set to distinct
// instances of `my_CountingAllocator`.  Note that the default allocator is set
// and locked identically to what was done in the previous two usage examples:
// ```
// static my_CountingAllocator defaultCountingAllocator;
//
// int status = bslma::Default::setDefaultAllocator(
//                                                 &defaultCountingAllocator);
// assert(0 == status);
// bslma::Default::lockDefaultAllocator();
// assert(bslma::Default::defaultAllocator() == &defaultCountingAllocator);
//
// static my_CountingAllocator globalCountingAllocator;
//
// bslma::Default::setGlobalAllocator(&globalCountingAllocator);
// assert(bslma::Default::globalAllocator() == &globalCountingAllocator);
// ```
// Finally, we initialize the singleton object.  We explicitly specify the
// desired allocator in the call to `initSingleton` to make our intentions as
// clear as possible.  Of course, because of the way the `my_Singleton`
// constructor was written, the result would have been the same if no allocator
// had been specified.  As in previous examples, the states of the default and
// global allocators are asserted before and after initializing the singleton:
// ```
// assert(0 == defaultCountingAllocator.numBlocksInUse());
// assert(0 == defaultCountingAllocator.numBlocksTotal());
// assert(0 == globalCountingAllocator.numBlocksInUse());
// assert(0 == globalCountingAllocator.numBlocksTotal());
//
// my_Singleton::initSingleton("S", bslma::Default::globalAllocator());
//
// assert(0 == defaultCountingAllocator.numBlocksInUse());
// assert(0 == defaultCountingAllocator.numBlocksTotal());
// assert(1 == globalCountingAllocator.numBlocksInUse());
// assert(1 == globalCountingAllocator.numBlocksTotal());
// ```

#include <bslscm_version.h>

#include <bsls_atomicoperations.h>

#include <bslma_allocator.h>
#include <bslma_newdeleteallocator.h>

namespace BloombergLP {

namespace bslma {

                        // ==============
                        // struct Default
                        // ==============

/// This struct is a mechanism with global state, i.e., all state is held in
/// global variables and all functions are class methods.  The state
/// consists of two distinct parts that don't influence one another: the
/// default allocator and the global allocator.  All addresses are stored
/// without assuming ownership.
///
/// The `setDefaultAllocator` method will only modify the default allocator
/// prior to the default allocator being accessed (by either `allocator`,
/// `defaultAllocator`, or `lockDefaultAllocator`). The global allocator may
/// be freely modified.
///
/// Note that *only* the *owner* of `main` (or in *testing*), where the
/// caller affirmatively takes responsibility for the behavior of all
/// clients of the global allocator, is intended to change the global
/// allocator.
struct Default {

  private:
    // CLASS DATA

                        // *** default allocator ***

    /// The default allocator that is requested by the user and will be
    /// installed as the default allocator on the first attempt to access
    /// the default allocator (via `allocator`, `defaultAllocator`, or
    /// `lockDefaultAllocator`).  This value is initialized to `0`,
    /// indicating the user hasn't made a choice yet.  In that case, when
    /// needed, the new-delete allocator will be used.  This value can be
    /// set (multiple times) by calling to `setDefaultAllocator`.  Note that
    /// once changed this variable will not become `0` again.
    static bsls::AtomicOperations::AtomicTypes::Pointer
                                                   s_requestedDefaultAllocator;

    /// The currently installed default allocator.  This variable must be
    /// set only once, when the default allocator is first accessed (via
    /// `allocator`, `defaultAllocator`, or `lockDefaultAllocator`) but may
    /// be `0` before it is accessed unless *for* *testing* *only*
    /// `setDefaultAllocatorRaw` is used.
    static bsls::AtomicOperations::AtomicTypes::Pointer s_defaultAllocator;

                        // *** global allocator ***

    /// The address of the global allocator to use.
    static bsls::AtomicOperations::AtomicTypes::Pointer s_globalAllocator;

    // PRIVATE CLASS METHODS

    /// Return the address of the default allocator and disable all
    /// subsequent calls to the `setDefaultAllocator` method.  If
    /// `s_defaultAllocator` is 0 (meaning the default allocator has not yet
    /// been accessed), set it to the last value supplied to
    /// `setDefaultAllocator` (stored in `s_requestedDefaultAllocator`).  If
    /// `s_requestedDefaultAllocator` is 0 (meaning `setDefaultAllocator`
    /// has not been called) set `s_defaultAllocator` and
    /// `s_requestedDefaultAllocator` to be the new-delete allocator.  Note
    /// that this operation performs the one and only assigment to
    /// `s_defaultAllocator` (unless `setDefaultAllocatorRaw` is called).
    /// In C++, this function also ensures that the default
    /// `std::pmr::memory_resource` is the same as the default `Allocator`.
    /// Note also that this function has the same externally visible
    /// behavior as `defaultAllocator`, but forms the (thread-safe)
    /// "slow-path" for that function's implementation.
    static Allocator *determineAndReturnDefaultAllocator();

  public:
    // CLASS METHODS

                        // *** default allocator ***

    /// Set the address of the default allocator to the specified
    /// `basicAllocator` unless calls to this method have been disabled.
    /// Return 0 on success and a non-zero value otherwise.  This method
    /// will fail if either `defaultAllocator`, `lockDefaultAllocator`, or
    /// `allocator` with argument 0 has been called previously in this
    /// process.  In C++17 and later, a successful call to this method will
    /// also set the default `std::pmr::memory_resource`.  The behavior is
    /// undefined unless `basicAllocator` is the address of an allocator
    /// with sufficient lifetime to satisfy all allocation requests within
    /// this process, and unless there is only one thread started within
    /// this process.  Note that this method is intended for use *only* by
    /// the *owner* of `main` (or for use in *testing*) where the caller
    /// affirmatively takes responsibility for the behavior of all clients
    /// of the default allocator, and should *not* be used for any other
    /// purpose.
    static int setDefaultAllocator(Allocator *basicAllocator);

    /// Unconditionally set the address of the default allocator to the
    /// specified `basicAllocator`.  In C++17 and later, a call to this
    /// method will also set the default `std::pmr::memory_resource`.  The
    /// behavior is undefined unless `basicAllocator` is the address of an
    /// allocator with sufficient lifetime to satisfy all allocation
    /// requests within this process, and unless there is only one thread
    /// started within this process.  Note that this method is intended for
    /// use *only* in *testing* where the caller affirmatively takes
    /// responsibility for the behavior of all clients of the default
    /// allocator, and should *not* be used for any other purpose.
    static void setDefaultAllocatorRaw(Allocator *basicAllocator);

    /// Disable all subsequent calls to the `setDefaultAllocator` method.
    /// Subsequent calls to this method have no effect.  Note that
    /// subsequent calls to the `setDefaultAllocatorRaw` method are *not*
    /// disabled by this method.
    static void lockDefaultAllocator();

    /// Return the address of the default allocator and disable all
    /// subsequent calls to the `setDefaultAllocator` method.  Note that
    /// prior to the first call to `setDefaultAllocator` or
    /// `setDefaultAllocatorRaw` methods, the address of the default
    /// allocator is that of the `NewDeleteAllocator` singleton.  Also note
    /// that subsequent calls to `setDefaultAllocatorRaw` method are *not*
    /// disabled by this method.
    static Allocator *defaultAllocator();

    /// Return the allocator returned by `defaultAllocator` and disable all
    /// subsequent calls to the `setDefaultAllocator` method if the
    /// optionally-specified `basicAllocator` is 0; return `basicAllocator`
    /// otherwise.
    static Allocator *allocator(Allocator *basicAllocator = 0);

                        // *** global allocator ***

    /// Return the address of the global allocator if the optionally-
    /// specified `basicAllocator` is 0, and `basicAllocator` otherwise.
    /// Note that prior to the first call to the `setGlobalAllocator`
    /// method, the address of the global allocator is that of the
    /// `NewDeleteAllocator` singleton.
    static Allocator *globalAllocator(Allocator *basicAllocator = 0);

    /// Unconditionally set the address of the global allocator to the
    /// specified `basicAllocator`, or to the address of the
    /// `NewDeleteAllocator` singleton if `basicAllocator` is 0.  Return the
    /// address of the global allocator in effect immediately before calling
    /// this method.  The behavior is undefined unless `basicAllocator` is 0
    /// or is the address of an allocator with sufficient lifetime to
    /// satisfy all global allocation requests within this process, and
    /// unless there is only one thread started within this process.  Note
    /// that prior to the first call to this method, the address of the
    /// global allocator is that of the `NewDeleteAllocator` singleton.
    /// Also note that this method is intended for use *only* by the *owner*
    /// of `main` (or for use in *testing*) where the caller affirmatively
    /// takes responsibility for the behavior of all clients of the global
    /// allocator, and should *not* be used for any other purpose.
    static Allocator *setGlobalAllocator(Allocator *basicAllocator);
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // --------------
                        // struct Default
                        // --------------

// CLASS METHODS

                        // *** default allocator ***

inline
void Default::lockDefaultAllocator()
{
    determineAndReturnDefaultAllocator();
}

inline
Allocator *Default::defaultAllocator()
{
    void *alloc = bsls::AtomicOperations::getPtrRelaxed(&s_defaultAllocator);
    return alloc ? static_cast<Allocator *>(alloc)
                 : determineAndReturnDefaultAllocator();
}

inline
Allocator *Default::allocator(Allocator *basicAllocator)
{
    return basicAllocator ? basicAllocator : defaultAllocator();
}

                        // *** global allocator ***

inline
Allocator *Default::globalAllocator(Allocator *basicAllocator)
{
    Allocator *globalAllocator = static_cast<Allocator *>(const_cast<void *>(
                   bsls::AtomicOperations::getPtrAcquire(&s_globalAllocator)));

    return basicAllocator ? basicAllocator
                          : globalAllocator
                                      ? globalAllocator
                                      : &NewDeleteAllocator::singleton();
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR

/// Stateless class to set the initial default resource to
/// `bslma::NewDeleteAllocator` automatically when using this component
/// with a platform that supports C++17 `std::pmr::memory_resource`.
struct Default_NewDeleteSetter {

    /// If `std::pmr::set_default_resource` has not yet been invoked, call
    /// it with the address of the `bslma::NewDeleteAllocator` singleton,
    /// thus ensuring that `bslma::Default` and `get_default_resource` are
    /// in sync.
    Default_NewDeleteSetter();

    Default_NewDeleteSetter(const Default_NewDeleteSetter&) = delete;
};

/// Static initialization of this variable ensures that the default resource
/// is set to `NewDeleteAllocator` before it is read by other code.
static Default_NewDeleteSetter Default_NewDeleteSetterSingleton;

#endif // BSLS_LIBRARYFEATURES_HAS_CPP17_PMR

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY

// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
                        // ====================
                        // struct bdema_Default
                        // ====================

/// This `struct` is a namespace for functions that manipulate and access
/// the default and global allocator pointers.  This alias is defined for
/// backward compatibility.
typedef bslma::Default bdema_Default;

#endif // BDE_OMIT_INTERNAL_DEPRECATED

/// This alias is defined for backward compatibility.
typedef bslma::Default bslma_Default;

#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

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
