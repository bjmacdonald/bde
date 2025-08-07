// ball_multiplexobserver.h                                           -*-C++-*-
#ifndef INCLUDED_BALL_MULTIPLEXOBSERVER
#define INCLUDED_BALL_MULTIPLEXOBSERVER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a multiplexing observer that forwards to other observers.
//
//@DEPRECATED: Use `ball_broadcastobserver` instead.
//
//@CLASSES:
//  ball::MultiplexObserver: multiplexing observer that forwards log records
//
//@SEE_ALSO: ball_record, ball_context, ball_streamobserver, ball_loggermanager
//
//@DESCRIPTION: This component provides a concrete implementation of the
// `ball::Observer` protocol for receiving and processing log records:
// ```
//              ( ball::MultiplexObserver )
//                           |             ctor
//                           |             registerObserver
//                           |             deregisterObserver
//                           |             numRegisteredObservers
//                           V
//                   ( ball::Observer )
//                                         dtor
//                                         publish
//                                         releaseRecords
// ```
// `ball::MultiplexObserver` is a concrete class derived from `ball::Observer`
// that processes the log records it receives through its `publish` method by
// forwarding them to other concrete observers.  `ball::MultiplexObserver`
// maintains a registry of observers to which it forwards log records.  Clients
// of `ball::MultiplexObserver` register observers using the `registerObserver`
// method and unregister observers with the `deregisterObserver` method.  Once
// registered, an observer receives all log records that its associated
// multiplexing observer receives.
//
///Thread Safety
///-------------
// `ball::MultiplexObserver` is thread-safe and thread-enabled, meaning that
// multiple threads may share the same instance, or may have their own
// instances.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example: Basic Usage
/// - - - - - - - - - -
// Note: This usage example retained here for reference purposes for legacy
// code that still uses multiplex observers.  The use of this component is
// strongly discouraged.
//
// Multiplexing observers are used to interface a `ball` logging system, which
// generates log records, with the multiplicity of observers that are to
// receive the generated records that are published.  Establishing this
// interface proceeds in three logical steps:
// ```
//   (1) Create a distinguished `ball::MultiplexObserver` that will be the
//       unique observer to receive log records directly from the logging
//       system.
//   (2) Create the other observers required by the application and register
//       each of these observers with some `ball::MultiplexObserver`.  (Note
//       that a `ball::MultiplexObserver` may be registered with another
//       `ball::MultiplexObserver`.)
//   (3) Install the distinguished multiplexor from step (1) within the
//       `ball` logging system.
// ```
// This example demonstrates the use of a multiplexing observer to forward log
// records from a `ball` logging system to three registered observers.  Each of
// the three registered observers performs distinct actions upon receipt of log
// records:
// ```
//   (1) `defaultObserver`, an instance of `ball::StreamObserver`, formats
//       the records it receives and outputs them to `stdout`.
//   (2) `logfileObserver`, an instance of `my_LogfileObserver` (assumed to
//       be a concrete class derived from `ball::Observer`) writes selected
//       records to a log file.
//   (3) `encryptingObserver`, an instance of `my_EncryptingObserver` (also
//       assumed to be a concrete class derived from `ball::Observer`) creates
//       a compact, encrypted representation of each record, suitable for
//       sending over an unsecure network.
// ```
// First, we create the three downstream observers that will be registered with
// multiplexor observer:
// ```
//    ball::StreamObserver   defaultObserver(&bsl::cout);
//    my_LogfileObserver     logfileObserver(&bsl::cout);
//    my_EncryptingObserver  encryptingObserver(&bsl::cout);
// ```
// Next, we create an initially empty multiplexing observer `multiplexor` and
// register the three downstream observers `multiplexor`:
// ```
//    ball::MultiplexObserver multiplexor;
//    assert(0 == multiplexor.numRegisteredObservers());
//
//    multiplexor.registerObserver(&defaultObserver);
//    multiplexor.registerObserver(&logfileObserver);
//    multiplexor.registerObserver(&encryptingObserver);
//    assert(3 == multiplexor.numRegisteredObservers());
// ```
// Then, `multiplexor` is installed within a `ball` logging system to be the
// direct recipient of published log records.  The code uses deprecated
// `ball::LoggerManager` API and is elided.
//
// Henceforth, all log records that are published by the logging system will be
// transmitted to the `publish` method of `multiplexor` which, in turn,
// forwards them to `defaultObserver`, `logfileObserver`, and
// `encryptingObserver` by calling their respective `publish` methods.
//
// Finally, deregister the three observers when the logs have been all
// forwarded:
// ```
//    multiplexor.deregisterObserver(&defaultObserver);
//    multiplexor.deregisterObserver(&logfileObserver);
//    multiplexor.deregisterObserver(&encryptingObserver);
// ```
// Note that any observer must exist before registering with multiplexor.  Any
// observer already registered must deregister before its destruction.
// Additional observers may be registered with `multiplexor` at any time.
// Similarly, observers may be unregistered at any time.  This capability
// allows for extremely flexible observation scenarios.

#include <balscm_version.h>

#include <ball_observer.h>

#include <bslma_allocator.h>

#include <bslmt_readlockguard.h>
#include <bslmt_rwmutex.h>
#include <bslmt_writelockguard.h>

#include <bsls_keyword.h>

#include <bsl_memory.h>
#include <bsl_set.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace ball {

class Record;
class Context;

                         // =======================
                         // class MultiplexObserver
                         // =======================

/// This class provides a multiplexing implementation of the `Observer`
/// protocol.  Other concrete observers may be registered with a
/// multiplexing observer (`registerObserver` method) and later unregistered
/// (`deregisterObserver` method).  The `publish` method of this class
/// forwards the log records that it receives to the `publish` method of
/// each registered observer.
class MultiplexObserver : public Observer {

    // DATA
    bsl::set<Observer *>   d_observerSet;  // observer registry

    mutable bslmt::RWMutex d_rwMutex;      // protects concurrent access to
                                           // 'd_observerSet'

    // NOT IMPLEMENTED
    MultiplexObserver(const MultiplexObserver&);
    MultiplexObserver& operator=(const MultiplexObserver&);

  public:
    // CREATORS

    /// Create a multiplexing observer having no registered observers.
    /// Optionally specify a `basicAllocator` used to supply memory.  If
    /// `basicAllocator` is 0, the currently installed default allocator is
    /// used.
    explicit MultiplexObserver(bslma::Allocator *basicAllocator = 0);

    /// Destroy this multiplexing observer.  Note that this method has no
    /// effect on the lifetime of observers registered with this observer, if
    /// any.
    ~MultiplexObserver() BSLS_KEYWORD_OVERRIDE;

    // MANIPULATORS

    /// Process the specified log `record` having the specified publishing
    /// `context` by forwarding `record` and `context` to each of the
    /// observers registered with this multiplexing observer.
    ///
    /// @DEPRECATED: Use the alternative `publish` overload instead.
    void publish(const Record&  record,
                 const Context& context) BSLS_KEYWORD_OVERRIDE;

    /// Process the specified log `record` having the specified publishing
    /// `context`.  This concrete publish implementations processes the
    /// `record` by forwarding `record` and `context` to each of the
    /// observers registered with this multiplexing observer.
    void publish(const bsl::shared_ptr<const Record>& record,
                 const Context&                       context)
                                                         BSLS_KEYWORD_OVERRIDE;

    /// Discard any shared reference to a `Record` object that was supplied
    /// to the `publish` method, and is held by this observer.  This
    /// implementation processes `releaseRecords` by calling
    /// `releaseRecords` on each of the registered observers.  Note that
    /// this operation should be called if resources underlying the
    /// previously provided shared pointers must be released.
    void releaseRecords() BSLS_KEYWORD_OVERRIDE;

    /// Add the specified `observer` to the registry of this multiplexing
    /// observer.  Return 0 if `observer` is non-null and was not already
    /// registered with this multiplexing observer, and a non-zero value
    /// (with no effect) otherwise.  Henceforth, this multiplexing observer
    /// will forward each record it receives through its `publish` method,
    /// including the record's context, to the `publish` method of
    /// `observer`, until `observer` is deregistered.  The behavior is
    /// undefined unless `observer` remains valid until it is deregistered
    /// from this multiplexing observer or until this observer is destroyed.
    int registerObserver(Observer *observer);

    /// Remove the specified `observer` from the registry of this
    /// multiplexing observer.  Return 0 if `observer` is non-null and was
    /// registered with this multiplexing observer, and a non-zero value
    /// (with no effect) otherwise.  Henceforth, `observer` will no longer
    /// receive log records from this multiplexing observer.
    int deregisterObserver(Observer *observer);

    // ACCESSORS

    /// Return the number of observers registered with this multiplexing
    /// observer.
    int numRegisteredObservers() const;
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                         // -----------------------
                         // class MultiplexObserver
                         // -----------------------

// CREATORS
inline
MultiplexObserver::MultiplexObserver(bslma::Allocator *basicAllocator)
: d_observerSet(basicAllocator)
{
}

// ACCESSORS
inline
int MultiplexObserver::numRegisteredObservers() const
{
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_rwMutex);
    return static_cast<int>(d_observerSet.size());
}

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
