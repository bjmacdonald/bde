// bslmt_threadutilimpl_pthread.h                                     -*-C++-*-
#ifndef INCLUDED_BSLMT_THREADUTILIMPL_PTHREAD
#define INCLUDED_BSLMT_THREADUTILIMPL_PTHREAD

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a POSIX implementation of `bslmt::ThreadUtil`.
//
//@CLASSES:
//  bslmt::ThreadUtilImpl<PosixThreads>: POSIX specialization
//
//@SEE_ALSO: bslmt_threadutil
//
//@DESCRIPTION: This component provides an implementation of
// `bslmt::ThreadUtil` for POSIX threads ("pthreads"),
// `bslmt::ThreadUtilImpl<PosixThreads>`, via the template specialization:
// ```
// bslmt::ThreadUtilImpl<Platform::PosixThreads>
// ```
// This template class should not be used (directly) by client code.  Clients
// should instead use `bslmt::ThreadUtil`.
//
///Supported Clock-Types
///---------------------
// `bsls::SystemClockType` supplies the enumeration indicating the system clock
// on which timeouts supplied to other methods should be based.  If the clock
// type indicated at construction is `bsls::SystemClockType::e_REALTIME`, the
// `absTime` argument passed to the `timedWait` method of the various
// synchronization primitives offered in `bslmt` should be expressed as an
// *absolute* offset since 00:00:00 UTC, January 1, 1970 (which matches the
// epoch used in `bsls::SystemTime::now(bsls::SystemClockType::e_REALTIME)`.
// If the clock type indicated at construction is
// `bsls::SystemClockType::e_MONOTONIC`, the `absTime` argument passed to the
// `timedWait` method of the various synchronization primitives offered in
// `bslmt` should be expressed as an *absolute* offset since the epoch of this
// clock (which matches the epoch used in
// `bsls::SystemTime::now(bsls::SystemClockType::e_MONOTONIC)`.
//
///Usage
///-----
// This component is an implementation detail of `bslmt` and is *not* intended
// for direct client use.  It is subject to change without notice.  As such, a
// usage example is not provided.

#include <bslscm_version.h>

#include <bslmt_platform.h>

#ifdef BSLMT_PLATFORM_POSIX_THREADS

// Platform-specific implementation starts here.

#include <bslmt_threadattributes.h>

#include <bsls_platform.h>
#include <bsls_systemclocktype.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>

#include <bsl_string.h>

#include <pthread.h>

namespace BloombergLP {

extern "C" {
    /// `bslmt_ThreadFunction` is an alias for a function type taking a
    /// single `void` pointer argument and returning `void *`.  Such
    /// functions are suitable to be specified as thread entry point
    /// functions to `bslmt::ThreadUtil::create`.
    typedef void *(*bslmt_ThreadFunction)(void *);

    /// `bslmt_KeyDestructorFunction` is an alias for a function type taking
    /// a single `void` pointer argument and returning `void`.  Such
    /// functions are suitable to be specified as thread-specific key
    /// destructor functions to `bslmt::ThreadUtil::createKey`.
    typedef void (*bslmt_KeyDestructorFunction)(void *);
}

namespace bslmt {

template <class THREAD_POLICY>
struct ThreadUtilImpl;

               // ============================================
               // class ThreadUtilImpl<Platform::PosixThreads>
               // ============================================

/// This class provides a full specialization of `ThreadUtilImpl` for
/// pthreads.
template <>
struct ThreadUtilImpl<Platform::PosixThreads> {

    // TYPES
    typedef pthread_t Handle;        // thread handle type
    typedef pthread_t NativeHandle;  // native thread handle type
    typedef pthread_t Id;            // thread Id type
    typedef pthread_key_t Key;       // thread-specific storage key type

    // CLASS DATA
    static const pthread_t INVALID_HANDLE;

    // CLASS METHODS
                          // *** Thread Management ***

    /// Create a new thread of program control having the specified
    /// `attributes` that invokes the specified `function` with a single
    /// argument specified by `userData`, and load into the specified
    /// `threadHandle` an identifier that may be used to refer to the thread
    /// in future calls to this utility.  Return 0 on success, and a
    /// non-zero value otherwise.  The behavior is undefined if the
    /// specified `thread` is 0 or if `attributes.stackSize()` has been set
    /// to a negative value other than the unset value.  Note that unless
    /// explicitly "detached" (by `detach`), or unless the
    /// `BSLMT_CREATE_DETACHED` attribute is specified, a call to `join`
    /// must be made once the thread terminates to reclaim any system
    /// resources associated with the newly created identifier.
    static int create(Handle                  *threadHandle,
                      const ThreadAttributes&  attributes,
                      bslmt_ThreadFunction     function,
                      void                    *userData);

    /// Create a new thread of program control having platform specific
    /// default attributes (i.e., "stack size", "scheduling priority"), that
    /// invokes the specified `function` with a single argument specified by
    /// `userData`, and load into the specified `threadHandle`, an
    /// identifier that may be used to refer to the thread in future calls
    /// to this utility.  Return 0 on success, and a non-zero value
    /// otherwise.  The behavior is undefined if the `threadHandle` is 0.
    /// Note that unless explicitly "detached" (`detach`), a call to `join`
    /// must be made once the thread terminates to reclaim any system
    /// resources associated with the newly created identifier.
    static int create(Handle               *threadHandle,
                      bslmt_ThreadFunction  function,
                      void                 *userData);

    /// "Detach" the thread identified by the specified `threadHandle`, such
    /// that when it terminates, the resources associated the thread will
    /// automatically be reclaimed.  Note that once a thread is "detached",
    /// it is no longer possible to `join` the thread to retrieve the its
    /// exit status.
    static int detach(Handle& threadHandle);

    /// Exit the current thread and return the specified `status`.  If the
    /// current thread is not "detached", then a call to `join` must be made
    /// to reclaim any resources used by the thread, and to retrieve the
    /// exit status.  Note that generally, the preferred method of exiting a
    /// thread is to return form the entry point function.
    static void exit(void *status);

    /// Return the maximum available priority for the specified `policy`,
    /// where `policy` is of type `ThreadAttributes::SchedulingPolicy`.
    /// Return `ThreadAttributes::BSLMT_UNSET_PRIORITY` if the maximum
    /// scheduling priority cannot be determined.  Note that, for some
    /// platform / policy combinations, `getMinSchedulingPriority(policy)`
    /// and `getMaxSchedulingPriority(policy)` return the same value.
    static int getMaxSchedulingPriority(
                                    ThreadAttributes::SchedulingPolicy policy);

    /// Return the minimum available priority for the specified `policy`,
    /// where `policy` is of type `ThreadAttributes::SchedulingPolicy`.
    /// Return `ThreadAttributes::BSLMT_UNSET_PRIORITY` if the minimum
    /// scheduling priority cannot be determined.  Note that, for some
    /// platform / policy combinations, `getMinSchedulingPriority(policy)`
    /// and `getMaxSchedulingPriority(policy)` return the same value.
    static int getMinSchedulingPriority(
                                    ThreadAttributes::SchedulingPolicy policy);

    /// Load the name of the current thread into the specified `threadName`.
    /// Note that this method clears `*threadName` on all platforms other
    /// than Linux and Darwin.
    static void getThreadName(bsl::string *threadName);

    /// Suspend execution of the current thread until the thread specified
    /// by `threadHandle` terminates, and reclaim any system resources
    /// associated with the `threadHandle`.  Return 0 on success, and a
    /// non-zero value otherwise.  If the optionally specified `status` is
    /// not 0, load into the `status` the value returned by the specified
    /// `thread`.
    static int join(Handle& threadHandle, void **status = (void**)0);

    /// Suspend execution of the current thread for a period of at least the
    /// optionally specified `seconds` and `microseconds` (relative time),
    /// and optionally load into the optionally specified `unsleptTime` the
    /// amount of time that was not slept by this function if the operation
    /// was interrupted by a signal.  Return 0 on success, and non-zero if
    /// the operation was interrupted by a signal.  Note that the actual
    /// time suspended depends on many factors including system scheduling,
    /// and system timer resolution.  Note that the actual time suspended
    /// depends on many factors including system scheduling, and system
    /// timer resolution.
    static int microSleep(int                 microseconds,
                          int                 seconds = 0,
                          bsls::TimeInterval *unsleptTime = 0);

    /// Set the name of the current thread to the specified `threadName`.
    /// On all platforms other than Linux and Darwin this method has no
    /// effect.  Note that on those two platforms `threadName` will be
    /// truncated to a length of 15 bytes, not including the terminating
    /// '\0'.
    static void setThreadName(const bslstl::StringRef& threadName);

    /// Suspend execution of the current thread for a period of at least the
    /// specified `sleepTime` (relative time), and optionally load into the
    /// optionally specified `unsleptTime` the amount of time that was not
    /// slept by this function if the operation was interrupted by a signal.
    /// Return 0 on success, and a non-zero value if the operation was
    /// interrupted by a signal.  Note that the actual time suspended
    /// depends on many factors including system scheduling, and system
    /// timer resolution.
    static int sleep(const bsls::TimeInterval&  sleepTime,
                     bsls::TimeInterval        *unsleptTime = 0);

    /// Suspend execution of the current thread until the specified
    /// `absoluteTime`.  Optionally specify `clockType` which determines the
    /// epoch from which the interval `absoluteTime` is measured (see
    /// {Supported Clock-Types} in the component documentation).  Return 0
    /// on success, and a non-zero value otherwise.  The behavior is
    /// undefined unless `absoluteTime` represents a time after January 1,
    /// 1970 and before the end of December 31, 9999 (i.e., a time interval
    /// greater than or equal to 0, and less than 253,402,300,800 seconds).
    /// Note that the actual time suspended depends on many factors
    /// including system scheduling and system timer resolution.
    static int sleepUntil(const bsls::TimeInterval&   absoluteTime,
                          bsls::SystemClockType::Enum clockType
                                          = bsls::SystemClockType::e_REALTIME);

    /// Suspend execution of the current thread until the specified
    /// `absoluteTime`.  Optionally specify `retryOnSignalInterrupt`
    /// indicating whether to put this thread to sleep again if the
    /// operating system interrupts the sleep because of a signal.
    /// Optionally specify `clockType` which determines the epoch from which
    /// the interval `absoluteTime` is measured (see {Supported
    /// Clock-Types} in the component documentation).  Return 0 on success,
    /// and a non-zero value otherwise.  If `retryOnSignalInterrupt` is
    /// `true`, an interrupt from a signal will be ignored and the current
    /// the thread will be put back to sleep until `absoluteTime`, otherwise
    /// this call will return 0 to the calling thread immediately.  The
    /// behavior is undefined unless `absoluteTime` represents a time after
    /// January 1, 1970 and before the end of December 31, 9999 (i.e., a
    /// time interval greater than or equal to 0, and less than
    /// 253,402,300,800 seconds).  Note that the actual time suspended
    /// depends on many factors including system scheduling and system timer
    /// resolution.
    static int sleepUntil(const bsls::TimeInterval&   absoluteTime,
                          bool                        retryOnSignalInterrupt
                                                                       = false,
                          bsls::SystemClockType::Enum clockType
                                          = bsls::SystemClockType::e_REALTIME);

    /// Put the current thread to the end of the scheduler's queue and
    /// schedule another thread to run.  This allows cooperating threads of
    /// the same priority to share CPU resources equally.
    static void yield();

                       // *** Thread Identification ***

    /// Return `true` if the specified `a` and `b` thread handles, identify
    /// the same thread, and `false` otherwise.
    static bool areEqual(const Handle& a, const Handle& b);

    /// Return `true` if the specified `a` and `b` thread id identify the
    /// same thread, and `false` otherwise.
    static bool areEqualId(const Id& a, const Id& b);

    /// Return the unique integral identifier of a thread uniquely
    /// identified by the specified `threadId` within the current process.
    /// Note that this representation is particularly useful for logging
    /// purposes.  Also note that this value is only valid until the thread
    /// terminates and may be reused thereafter.
    static bsls::Types::Uint64 idAsUint64(const Id& threadId);

    /// Return the unique integral identifier of a thread uniquely
    /// identified by the specified `threadId` within the current process.
    /// Note that this representation is particularly useful for logging
    /// purposes.  Also note that this value is only valid until the thread
    /// terminates and may be reused thereafter.
    ///
    /// DEPRECATED: use `idAsUint64`.
    static int idAsInt(const Id& threadId);

    /// Return the platform specific identifier associated with the thread
    /// specified by `threadHandle`.  Note that the returned native handle
    /// may not be a globally unique identifier for the thread (see
    /// `selfIdAsUint`).
    static NativeHandle nativeHandle(const Handle& threadHandle);

    /// Return an identifier that can be used to refer to the current thread
    /// in future calls to this utility.
    static Handle self();

    /// Return an identifier that can be used to uniquely identify the
    /// current thread within the current process.  Note that the id is only
    /// valid until the thread terminates and may be reused thereafter.
    static Id selfId();

    /// Return an integer of the unique identifier of the current thread
    /// within the current process.  This representation is particularly
    /// useful for logging purposes.  Note that this value is only valid
    /// until the thread terminates and may be reused thereafter.
    ///
    /// DEPRECATED: Use `selfIdAsUint64` instead.
    static bsls::Types::Uint64 selfIdAsInt();

    /// Return an integer of the unique identifier of the current thread
    /// within the current process.  This representation is particularly
    /// useful for logging purposes.  Note that this value is only valid
    /// until the thread terminates and may be reused thereafter.
    static bsls::Types::Uint64 selfIdAsUint64();

    /// Return the unique identifier of the thread having the specified
    /// `threadHandle` within the current process.  Note that this value is
    /// only valid until the thread terminates and may be reused thereafter.
    static Id handleToId(const Handle& threadHandle);

                // *** Thread-Specific (Local) Storage (TSS or TLS) ***

    /// Load, into the specified `key`, an identifier that can be used to
    /// store (`setSpecific`) and retrieve (`getSpecific`) a single
    /// thread-specific pointer value.  Associate with the identifier, the
    /// specified `destructor` if a non-zero value is specified.  Return 0
    /// on success, and a non-zero value otherwise.
    static int createKey(Key *key, bslmt_KeyDestructorFunction destructor);

    /// Delete the specified thread-specific `key`.  Return 0 on success,
    /// and a non-zero value otherwise.  Note that deleting a key does not
    /// delete any data that is currently associated with the key in the
    /// calling thread or any other thread.
    static int deleteKey(Key& key);

    /// Return the value associated with the specified thread-specific
    /// `key`.  Note that if the key is not valid, a value of zero is
    /// returned, which is indistinguishable from a valid key with a 0
    /// value.
    static void *getSpecific(const Key& key);

    /// Associate the specified `value` with the specified thread-specific
    /// `key`.  Return 0 on success, and a non-zero value otherwise.  TBD
    /// elaborate on what `value` represents
    static int setSpecific(const Key& key, const void *value);

    /// Return the number of concurrent threads supported by the
    /// implementation on success, and 0 otherwise.
    static unsigned int hardwareConcurrency();
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

               // --------------------------------------------
               // class ThreadUtilImpl<Platform::PosixThreads>
               // --------------------------------------------

// CLASS METHODS
                          // *** Thread Management ***
inline
int ThreadUtilImpl<bslmt::Platform::PosixThreads>::create(
                                            Handle               *threadHandle,
                                            bslmt_ThreadFunction  function,
                                            void                 *userData)
{
    ThreadAttributes attr;
    return create(threadHandle, attr, function, userData);
}

inline
int ThreadUtilImpl<bslmt::Platform::PosixThreads>::detach(Handle& threadHandle)
{
    return pthread_detach(threadHandle);
}

inline
void ThreadUtilImpl<bslmt::Platform::PosixThreads>::exit(void *status)
{
    pthread_exit(status);
}

inline
int ThreadUtilImpl<bslmt::Platform::PosixThreads>::join(Handle&   threadHandle,
                                                        void    **status)
{
    return pthread_join(threadHandle, status);
}

inline
int ThreadUtilImpl<bslmt::Platform::PosixThreads>::sleepUntil(
                                      const bsls::TimeInterval&   absoluteTime,
                                      bsls::SystemClockType::Enum clockType)
{
    return sleepUntil(absoluteTime, false, clockType);
}

inline
void ThreadUtilImpl<bslmt::Platform::PosixThreads>::yield()
{
    sched_yield();
}

                       // *** Thread Identification ***

inline
bool ThreadUtilImpl<bslmt::Platform::PosixThreads>::areEqual(const Handle& a,
                                                             const Handle& b)
{
    return pthread_equal(a, b);
}

inline
bool ThreadUtilImpl<bslmt::Platform::PosixThreads>::areEqualId(const Id& a,
                                                               const Id& b)
{
    return pthread_equal(a, b);
}

inline
ThreadUtilImpl<bslmt::Platform::PosixThreads>::Id
ThreadUtilImpl<bslmt::Platform::PosixThreads>::handleToId(
                                                    const Handle& threadHandle)
{
    return threadHandle;
}

inline
bsls::Types::Uint64
ThreadUtilImpl<bslmt::Platform::PosixThreads>::idAsUint64(const Id& threadId)
{
#ifdef BSLS_PLATFORM_OS_DARWIN
    return reinterpret_cast<bsls::Types::Uint64>(threadId);
#else
    return static_cast<bsls::Types::Uint64>(threadId);
#endif
}

inline
int
ThreadUtilImpl<bslmt::Platform::PosixThreads>::idAsInt(const Id& threadId)
{
    // Our interface is not good if the id is a pointer.  The two casts will
    // avoid a compilation error though.  TBD

    return static_cast<int>(idAsUint64(threadId));
}

inline
ThreadUtilImpl<bslmt::Platform::PosixThreads>::NativeHandle
ThreadUtilImpl<bslmt::Platform::PosixThreads>::nativeHandle(
                                                    const Handle& threadHandle)
{
    return threadHandle;
}

inline
ThreadUtilImpl<bslmt::Platform::PosixThreads>::Handle
ThreadUtilImpl<bslmt::Platform::PosixThreads>::self()
{
    return pthread_self();
}

inline
ThreadUtilImpl<bslmt::Platform::PosixThreads>::Id
ThreadUtilImpl<bslmt::Platform::PosixThreads>::selfId()
{
    return pthread_self();
}

inline
bsls::Types::Uint64
ThreadUtilImpl<bslmt::Platform::PosixThreads>::selfIdAsInt()
{
    return idAsInt(selfId());
}

inline
bsls::Types::Uint64
ThreadUtilImpl<bslmt::Platform::PosixThreads>::selfIdAsUint64()
{
    return idAsUint64(selfId());
}

                // *** Thread-Specific (Local) Storage (TSS or TLS) ***

inline
int ThreadUtilImpl<bslmt::Platform::PosixThreads>::createKey(
                                       Key                         *key,
                                       bslmt_KeyDestructorFunction  destructor)
{
    return pthread_key_create(key,destructor);
}

inline
int ThreadUtilImpl<bslmt::Platform::PosixThreads>::deleteKey(Key& key)
{
    return pthread_key_delete(key);
}

inline
void *ThreadUtilImpl<bslmt::Platform::PosixThreads>::getSpecific(
                                                                const Key& key)
{
    return pthread_getspecific(key);
}

inline
int ThreadUtilImpl<bslmt::Platform::PosixThreads>::setSpecific(
                                                             const Key&  key,
                                                             const void *value)
{
    return pthread_setspecific(key, value);
}

}  // close package namespace
}  // close enterprise namespace

#endif  // BSLMT_PLATFORM_POSIX_THREADS

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
