// bslmt_fastpostsemaphore.h                                          -*-C++-*-

#ifndef INCLUDED_BSLMT_FASTPOSTSEMAPHORE
#define INCLUDED_BSLMT_FASTPOSTSEMAPHORE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a semaphore class optimizing `post`.
//
//@CLASSES:
//  bslmt::FastPostSemaphore: semaphore class optimizing `post`
//
//@SEE_ALSO: bslmt_semaphore
//
//@DESCRIPTION: This component defines a semaphore, `bslmt::FastPostSemaphore`,
// with the `post` operation being optimized at the potential expense of other
// operations.  In particular, `bslmt::FastPostSemaphore` is an efficient
// synchronization primitive that enables sharing of a counted number of
// resources.  `bslmt::FastPostSemaphore` supports the methods `timedWait`,
// `enable`, and `disable` in addition to the standard semaphore methods.
//
// Commonly, during periods of time when the protected resource is scarce (the
// semaphore count is frequently zero) and threads are frequently blocked on
// wait methods, pessimizing the performance of the threads that block will
// have little effect on overall performance.  In this case, optimizing `post`
// *may* be a performance improvement.  Note that when the resource is
// plentiful, there are no blocked threads and we expect the differences
// between semaphore implementations to be trivial.
//
///Supported Clock-Types
///---------------------
// `bsls::SystemClockType` supplies the enumeration indicating the system clock
// on which timeouts supplied to other methods should be based.  If the clock
// type indicated at construction is `bsls::SystemClockType::e_REALTIME`, the
// `absTime` argument passed to the `timedWait` method should be expressed as
// an *absolute* offset since 00:00:00 UTC, January 1, 1970 (which matches the
// epoch used in `bsls::SystemTime::now(bsls::SystemClockType::e_REALTIME)`.
// If the clock type indicated at construction is
// `bsls::SystemClockType::e_MONOTONIC`, the `absTime` argument passed to the
// `timedWait` method should be expressed as an *absolute* offset since the
// epoch of this clock (which matches the epoch used in
// `bsls::SystemTime::now(bsls::SystemClockType::e_MONOTONIC)`.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: A Simple Queue
///- - - - - - - - - - - - -
// This example illustrates a very simple fixed-size queue where potential
// clients can push integers to a queue, and later retrieve the integer values
// from the queue in FIFO order.  Also, `waitUntilEmpty` is implemented to
// depict the common usage of `getDisabledState`.
//
// First, we define the `IntQueue` class:
// ```
// /// FIFO queue of integer values.
// class IntQueue {
//
//     // DATA
//     bsl::vector<int>         d_data;            // queue values
//
//     bslmt::FastPostSemaphore d_pushSem;         // resource availability
//                                                 // for push
//
//     bslmt::FastPostSemaphore d_popSem;          // resource availability
//                                                 // for pop
//
//     bsls::AtomicUint         d_pushIdx;         // index to push to
//
//     bsls::AtomicUint         d_popIdx;          // index to pop from
//
//     mutable bslmt::Mutex     d_emptyMutex;      // blocking point for
//                                                 // 'waitUntilEmpty'
//
//     mutable bslmt::Condition d_emptyCondition;  // condition variable for
//                                                 // 'waitUntilEmpty'
//
//     // NOT IMPLEMENTED
//     IntQueue(const IntQueue&);
//     IntQueue& operator=(const IntQueue&);
//
//   public:
//     // PUBLIC CONSTANTS
//     enum ReturnValue {
//         e_SUCCESS  = bslmt::FastPostSemaphore::e_SUCCESS,   // indicates
//                                                             // success
//
//         e_DISABLED = bslmt::FastPostSemaphore::e_DISABLED,  // indicates
//                                                             // queue is
//                                                             // disabled
//
//         e_FAILED = bslmt::FastPostSemaphore::e_FAILED       // indicates
//                                                             // failure
//     };
//
//     // CREATORS
//
//     /// Create an `IntQueue` object with the specified `capacity`.
//     /// Optionally specify a `basicAllocator` used to supply memory.  If
//     /// `basicAllocator` is 0, the currently installed default allocator
//     /// is used.
//     explicit
//     IntQueue(bsl::size_t capacity, bslma::Allocator *basicAllocator = 0);
//
//     /// Destroy this object.
//     //! ~IntQueue() = default;
//
//     // MANIPULATORS
//
//     /// Disable dequeueing from this queue.  All subsequent invocations
//     /// of `popFront` and `waitUntilEmpty` will fail immediately.  All
//     /// blocked invocations of `popFront` and `waitUntilEmpty` will fail
//     /// immediately.  If the queue is already dequeue disabled, this
//     /// method has no effect.
//     void disablePopFront();
//
//     /// Enable dequeuing.  If the queue is not dequeue disabled, this method
//     /// has no effect.
//     void enablePopFront();
//
//     /// Remove the element from the front of this queue and load that
//     /// element into the specified `value`.  If the queue is empty,
//     /// block until it is not empty.  Return 0 on success, and a nonzero
//     /// value if the queue is disabled.
//     int popFront(int *value);
//
//     /// Append the specified `value` to the back of this queue, blocking
//     /// until either space is available - if necessary - or the queue is
//     /// disabled.  Return 0 on success, and a nonzero value if the queue
//     /// is disabled.
//     int pushBack(int value);
//
//     // ACCESSORS
//
//     /// Block until all the elements in this queue are removed.  Return
//     /// 0 on success, and a non-zero value if the queue is not empty and
//     /// `isPopFrontDisabled()`.  A blocked thread waiting for the queue
//     /// to empty will return a non-zero value if `disablePopFront` is
//     /// invoked.
//     int waitUntilEmpty() const;
// };
// ```
// Next, implement the queue:
// ```
// // CREATORS
// IntQueue::IntQueue(bsl::size_t capacity, bslma::Allocator *basicAllocator)
// : d_data(capacity, basicAllocator)
// , d_pushSem(static_cast<int>(capacity))
// , d_popSem(0)
// , d_pushIdx(0)
// , d_popIdx(0)
// , d_emptyMutex()
// , d_emptyCondition()
// {
// }
//
// // MANIPULATORS
// void IntQueue::disablePopFront()
// {
//     d_popSem.disable();
// }
//
// void IntQueue::enablePopFront()
// {
//     d_popSem.enable();
// }
//
// int IntQueue::popFront(int *value)
// {
//     // wait for available element
//
//     int rv = d_popSem.wait();
//     if (0 != rv) {
//         return rv;                                                // RETURN
//     }
//
//     *value = d_data[d_popIdx++ % d_data.size()];
//
//     d_pushSem.post();  // signal additional empty element
//
//     if (0 == d_popSem.getValue()) {
//         {
//             bslmt::LockGuard<bslmt::Mutex> guard(&d_emptyMutex);
//         }
//         d_emptyCondition.broadcast();
//     }
//
//     return 0;
// }
//
// int IntQueue::pushBack(int value)
// {
//     // wait for an empty element
//
//     int rv = d_pushSem.wait();
//     if (0 != rv) {
//         return rv;                                                // RETURN
//     }
//
//     d_data[d_pushIdx++ % d_data.size()] = value;
//
//     d_popSem.post();  // signal additional available element
//
//     return 0;
// }
//
// // ACCESSORS
// int IntQueue::waitUntilEmpty() const
// {
//     bslmt::LockGuard<bslmt::Mutex> guard(&d_emptyMutex);
//
//     int state = d_popSem.getDisabledState();
//
//     while (d_popSem.getValue()) {
//         if (state != d_popSem.getDisabledState()) {
//             return e_DISABLED;                                    // RETURN
//         }
//         d_emptyCondition.wait(&d_emptyMutex);
//     }
//
//     return e_SUCCESS;
// }
// ```
// Then, declare an instance of `IntQueue`:
// ```
// IntQueue queue(10);
// ```
// Next, populate some values:
// ```
// assert(0 == queue.pushBack(5));
// assert(0 == queue.pushBack(7));
// assert(0 == queue.pushBack(3));
// ```
// Now, pop and verify the values:
// ```
// int value;
//
// assert(0 == queue.popFront(&value));
// assert(5 == value);
//
// assert(0 == queue.popFront(&value));
// assert(7 == value);
//
// assert(0 == queue.popFront(&value));
// assert(3 == value);
// ```
// Finally, use `waitUntilEmpty` to verify the queue is empty:
// ```
// assert(IntQueue::e_SUCCESS == queue.waitUntilEmpty());
// ```

#include <bslscm_version.h>

#include <bslmt_condition.h>
#include <bslmt_fastpostsemaphoreimpl.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_threadutil.h>

#include <bsls_atomicoperations.h>
#include <bsls_libraryfeatures.h>
#include <bsls_systemclocktype.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
#include <bslmt_chronoutil.h>

#include <bsl_chrono.h>
#endif

namespace BloombergLP {
namespace bslmt {

                         // =======================
                         // class FastPostSemaphore
                         // =======================

/// This class implements a semaphore type, optimized for `post`, for thread
/// synchronization.
class FastPostSemaphore {

    // PRIVATE TYPES
    typedef FastPostSemaphoreImpl<bsls::AtomicOperations,
                                  bslmt::Mutex,
                                  bslmt::Condition,
                                  bslmt::ThreadUtil> Impl;

    // DATA
    Impl d_impl;

    // NOT IMPLEMENTED
    FastPostSemaphore(const FastPostSemaphore&);
    FastPostSemaphore& operator=(const FastPostSemaphore&);

  public:
    // PUBLIC CONSTANTS
    enum ReturnValue {
        e_SUCCESS     = Impl::e_SUCCESS,      // indicates success

        e_DISABLED    = Impl::e_DISABLED,     // indicates semaphore is
                                              // disabled

        e_TIMED_OUT   = Impl::e_TIMED_OUT,    // indicates operation timed out

        e_WOULD_BLOCK = Impl::e_WOULD_BLOCK,  // indicates operation would
                                              // block ('tryWait')

        e_FAILED      = Impl::e_FAILED        // indicates failure reported
                                              // from 'd_condition'
    };

    // CREATORS

    /// Create a `FastPostSemaphore` object initially having a count of 0.
    /// Optionally specify a `clockType` indicating the type of the system
    /// clock against which the `bsls::TimeInterval` `absTime` timeouts
    /// passed to the `timedWait` method are to be interpreted (see
    /// {Supported Clock-Types} in the component-level documentation).  If
    /// `clockType` is not specified then the realtime system clock is used.
    explicit
    FastPostSemaphore(
    bsls::SystemClockType::Enum clockType = bsls::SystemClockType::e_REALTIME);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    /// Create a `FastPostSemaphore` object initially having a count of 0.
    /// Use the realtime system clock as the clock against which the
    /// `absTime` timeouts passed to the `timedWait` methods are interpreted
    /// (see {Supported Clock-Types} in the component-level documentation).
    explicit
    FastPostSemaphore(const bsl::chrono::system_clock&);

    /// Create a `FastPostSemaphore` object initially having a count of 0.
    /// Use the monotonic system clock as the clock against which the
    /// `absTime` timeouts passed to the `timedWait` methods are interpreted
    /// (see {Supported Clock-Types} in the component-level documentation).
    explicit
    FastPostSemaphore(const bsl::chrono::steady_clock&);
#endif

    /// Create a `FastPostSemaphore` object initially having the specified
    /// `count`.  Optionally specify a `clockType` indicating the type of
    /// the system clock against which the `bsls::TimeInterval` `absTime`
    /// timeouts passed to the `timedWait` method are to be interpreted (see
    /// {Supported Clock-Types} in the component-level documentation).  If
    /// `clockType` is not specified then the realtime system clock is used.
    explicit
    FastPostSemaphore(
    int                         count,
    bsls::SystemClockType::Enum clockType = bsls::SystemClockType::e_REALTIME);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    /// Create a `FastPostSemaphore` object initially having the specified
    /// `count`.  Use the realtime system clock as the clock against which
    /// the `absTime` timeouts passed to the `timedWait` methods are
    /// interpreted (see {Supported Clock-Types} in the component-level
    /// documentation).
    FastPostSemaphore(int count, const bsl::chrono::system_clock&);

    /// Create a `FastPostSemaphore` object initially having the specified
    /// `count`.  Use the monotonic system clock as the clock against which
    /// the `absTime` timeouts passed to the `timedWait` methods are
    /// interpreted (see {Supported Clock-Types} in the component-level
    /// documentation).
    FastPostSemaphore(int count, const bsl::chrono::steady_clock&);
#endif

    //! ~FastPostSemaphore() = default;
        // Destroy this object.

    // MANIPULATORS

    /// Enable waiting on this semaphore.  If the semaphore is not disabled,
    /// this call has no effect.
    void enable();

    /// Disable waiting on this semaphore.  All subsequent invocations of
    /// `wait`, `tryWait`, and `timedWait` will fail immediately.  All
    /// blocked invocations of `wait` and `timedWait` will fail immediately.
    /// If the semaphore is already disabled, this method will have no
    /// effect.
    void disable();

    /// Atomically increment the count of this semaphore.
    void post();

    /// Atomically increase the count of this semaphore by the specified
    /// `value`.  The behavior is undefined unless `value > 0`.
    void post(int value);

    /// Atomically increase the count of this semaphore by the specified
    /// `value`.  If the resources available to this semaphore is greater
    /// than or equal to the specified `available` and the number of threads
    /// blocked in this semaphore is greater than or equal to the specified
    /// `blocked`, always send a signal to potentially wake a waiting thread
    /// (even if the signal should not be needed).  The behavior is
    /// undefined unless `value > 0`.  Note that this method is provided to
    /// help mitigate issues in the implementation of underlying
    /// synchronization primitives.
    void postWithRedundantSignal(int value, int available, int blocked);

    /// If the count of this semaphore is positive, reduce the count by the
    /// lesser of the count and the specified `maximumToTake` and return the
    /// magnitude of the change to the count.  Otherwise, do nothing and
    /// return 0.
    int take(int maximumToTake);

    /// If the count of this semaphore is positive, reduce the count to 0
    /// and return the original value of the count.  Otherwise, do nothing
    /// and return 0.
    int takeAll();

    /// If this semaphore is initially disabled, or becomes disabled while
    /// blocking, return `e_DISABLED` with no effect on the count.
    /// Otherwise, block until the count of this semaphore is a positive
    /// value or the specified `absTime` timeout expires.  If the count of
    /// this semaphore is a positive value, return 0 and atomically
    /// decrement the count.  If the `absTime` timeout expires, return
    /// `e_TIMED_OUT` with no effect on the count.  Return `e_FAILED` if an
    /// error occurs.  Errors are unrecoverable.  After an error, the
    /// semaphore may be destroyed, but any other use has undefined
    /// behavior.  `absTime` is an *absolute* time represented as an
    /// interval from some epoch, which is determined by the clock indicated
    /// at construction (see {Supported Clock-Types} in the component
    /// documentation).
    int timedWait(const bsls::TimeInterval& absTime);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    /// If this semaphore is initially disabled, or becomes disabled while
    /// blocking, return `e_DISABLED` with no effect on the count.
    /// Otherwise, block until the count of this semaphore is a positive
    /// value or the specified `absTime` timeout expires.  If the count of
    /// this semaphore is a positive value, return 0 and atomically
    /// decrement the count.  If the `absTime` timeout expires, return
    /// `e_TIMEDOUT` with no effect on the count.  Return `e_FAILED` if an
    /// error occurs.  Errors are unrecoverable.  After an error, the
    /// semaphore may be destroyed, but any other use has undefined
    /// behavior.  `absTime` is an *absolute* time represented as an
    /// interval from some epoch, which is determined by the clock indicated
    /// at construction (see {Supported Clock-Types} in the component
    /// documentation).
    template <class CLOCK, class DURATION>
    int timedWait(const bsl::chrono::time_point<CLOCK, DURATION>& absTime);
#endif

    /// If this semaphore is initially disabled, return `e_DISABLED` with no
    /// effect on the count.  Otherwise, if the count of this semaphore is a
    /// positive value, return 0 and atomically decrement the count.  If
    /// this semaphore is not disabled and the count of this semaphore is
    /// not a positive value, return `e_WOULD_BLOCK` with no effect on the
    /// count.
    int tryWait();

    /// If this semaphore is initially disabled, or becomes disabled while
    /// blocking, return `e_DISABLED` with no effect on the count.
    /// Otherwise, block until the count of this semaphore is a positive
    /// value, return 0 and atomically decrement the count.  Return
    /// `e_FAILED` if an error occurs.
    int wait();

    // ACCESSORS

    /// Return the clock type used for timeouts.
    bsls::SystemClockType::Enum clockType() const;

    /// Return an odd value if this semaphore is wait disabled, and an even
    /// value otherwise.  The returned value can be used to detect a rapid
    /// short sequence of `disable` and `enable` invocations by comparing
    /// the value returned by `getDisabledState` before and after the
    /// sequence.  For example, for any initial state of a semaphore
    /// instance `obj`:
    /// ```
    /// int state = obj.getDisabledState();
    /// obj.disable();
    /// obj.enable();
    /// ASSERT(state != obj.getDisabledState());
    /// ```
    /// This functionality is useful in higher-level components to determine
    /// if this semaphore was disabled during an operation.
    int getDisabledState() const;

    /// Return the current value (`count > 0 ? count : 0`) of this
    /// semaphore.
    int getValue() const;

    /// Return `true` if this semaphore is wait disabled, and `false`
    /// otherwise.  Note that the semaphore is created in the "wait enabled"
    /// state.
    bool isDisabled() const;
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                         // -----------------------
                         // class FastPostSemaphore
                         // -----------------------

// CREATORS
inline
FastPostSemaphore::FastPostSemaphore(bsls::SystemClockType::Enum clockType)
: d_impl(clockType)
{
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
inline
FastPostSemaphore::FastPostSemaphore(const bsl::chrono::system_clock&)
: d_impl(bsls::SystemClockType::e_REALTIME)
{
}

inline
FastPostSemaphore::FastPostSemaphore(const bsl::chrono::steady_clock&)
: d_impl(bsls::SystemClockType::e_MONOTONIC)
{
}
#endif

inline
FastPostSemaphore::FastPostSemaphore(int                         count,
                                     bsls::SystemClockType::Enum clockType)
: d_impl(count, clockType)
{
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
inline
FastPostSemaphore::FastPostSemaphore(int                              count,
                                     const bsl::chrono::system_clock&)
: d_impl(count, bsls::SystemClockType::e_REALTIME)
{
}

inline
FastPostSemaphore::FastPostSemaphore(int                              count,
                                     const bsl::chrono::steady_clock&)
: d_impl(count, bsls::SystemClockType::e_MONOTONIC)
{
}
#endif

// MANIPULATORS
inline
void FastPostSemaphore::disable()
{
    d_impl.disable();
}

inline
void FastPostSemaphore::enable()
{
    d_impl.enable();
}

inline
void FastPostSemaphore::post()
{
    d_impl.post();
}

inline
void FastPostSemaphore::post(int value)
{
    d_impl.post(value);
}

inline
void FastPostSemaphore::postWithRedundantSignal(int value,
                                                int available,
                                                int blocked)
{
    d_impl.postWithRedundantSignal(value, available, blocked);
}

inline
int FastPostSemaphore::take(int maximumToTake)
{
    return d_impl.take(maximumToTake);
}

inline
int FastPostSemaphore::takeAll()
{
    return d_impl.takeAll();
}

inline
int FastPostSemaphore::timedWait(const bsls::TimeInterval& absTime)
{
    return d_impl.timedWait(absTime);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
template <class CLOCK, class DURATION>
inline
int FastPostSemaphore::timedWait(
                       const bsl::chrono::time_point<CLOCK, DURATION>& absTime)
{
    return bslmt::ChronoUtil::timedWait(this, absTime);
}
#endif

inline
int FastPostSemaphore::tryWait()
{
    return d_impl.tryWait();
}

inline
int FastPostSemaphore::wait()
{
    return d_impl.wait();
}

// ACCESSORS
inline
bsls::SystemClockType::Enum FastPostSemaphore::clockType() const
{
    return d_impl.clockType();
}

inline
int FastPostSemaphore::getDisabledState() const
{
    return d_impl.getDisabledState();
}

inline
int FastPostSemaphore::getValue() const
{
    return d_impl.getValue();
}

inline
bool FastPostSemaphore::isDisabled() const
{
    return d_impl.isDisabled();
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
