// bslmt_rwmutex.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLMT_RWMUTEX
#define INCLUDED_BSLMT_RWMUTEX

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a platform-independent RW mutex class.
//
//@DEPRECATED: Use `bslmt_readerwritermutex` instead.
//
//@CLASSES:
//   bslmt::RWMutex: platform-independent wrapper of an RW mutex
//
//@SEE_ALSO: bslmt_readerwritermutex, bslmt_readerwriterlock,
//           bslmt_readlockguard, bslmt_writelockguard
//
//@DESCRIPTION: This component provides a class, `bslmt::RWMutex`, that defines
// a platform-independent RW mutex.  An RW mutex provides for a shared "read"
// lock that may be held simultaneously by any number of threads, and a "write"
// lock that is exclusive (i.e., it may be held by only one thread at a time).
// The "write" lock is also exclusive with the "read" lock, so that no threads
// may hold a "read" lock while the "write" lock is held, and vice versa.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// TBD

#include <bslscm_version.h>

#include <bslmt_platform.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#if defined(BSLMT_PLATFORM_WIN32_THREADS) || defined(BSLS_PLATFORM_OS_AIX)
#include <bslmt_readerwriterlock.h>
#endif

#ifdef BSLMT_PLATFORM_POSIX_THREADS
#include <pthread.h>
#endif
#include <bsl_cstddef.h>

namespace BloombergLP {
namespace bslmt {

template <class THREAD_POLICY>
struct RWMutexImpl;

}  // close package namespace

#ifdef BSLMT_PLATFORM_POSIX_THREADS

namespace bslmt {

                     // ================================
                     // struct RWMutexImpl<PosixThreads>
                     // ================================

/// This is a platform-specific implementation detail that is not intended
/// for use outside of this component.  Use the `RWMutex` class instead.
/// This structure is a wrapper around a POSIX RW lock on Sun (on AIX the
/// POSIX RW lock has poor performance and no writer guarantees).
template <>
struct RWMutexImpl<Platform::PosixThreads> {

  private:
    // DATA
    pthread_rwlock_t d_lock;

  public:
    // CREATORS
    RWMutexImpl();
    ~RWMutexImpl();

    // MANIPULATORS
    void lockRead();
    void lockWrite();
    int tryLockRead();
    int tryLockWrite();
    void unlock();
};

}  // close package namespace

#endif  // BSLMT_PLATFORM_POSIX_THREADS

namespace bslmt {

                              // =============
                              // class RWMutex
                              // =============

/// This class is a platform-independent interface to a reader-writer lock
/// ("RW mutex").  Multiple readers can safely hold the lock simultaneously,
/// whereas only one writer is allowed to hold the lock at a time.  This
/// class uses the most efficient RW mutex implementation available for the
/// current platform.  Note that the implementation may allow readers to
/// starve writers.
class RWMutex {

    // DATA
#if defined(BSLS_PLATFORM_OS_AIX) || defined(BSLMT_PLATFORM_WIN32_THREADS)
    ReaderWriterLock d_impl;
#else
    RWMutexImpl<Platform::ThreadPolicy> d_impl;
#endif

    // NOT IMPLEMENTED
    RWMutex(const RWMutex&);
    RWMutex& operator=(const RWMutex&);

  public:
    // CREATORS

    /// Create an RW mutex initialized to an unlocked state.
    RWMutex();

    /// Destroy this RW mutex.  The behavior is undefined if the mutex
    /// is in a locked state.
    ~RWMutex();

    // MANIPULATORS

    /// Lock this reader-writer mutex for reading.  If there are no active
    /// or pending write locks, lock this mutex for reading and return
    /// immediately.  Otherwise, block until the read lock on this mutex is
    /// acquired.  Use `unlock` to release the lock on this mutex.  The
    /// behavior is undefined if this method is called from a thread that
    /// already has a lock on this mutex.
    void lockRead();

    /// Lock this reader-writer mutex for writing.  If there are no active
    /// or pending locks on this mutex, lock this mutex for writing and
    /// return immediately.  Otherwise, block until the write lock on this
    /// mutex is acquired.  Use `unlock` to release the lock on this mutex.
    /// The behavior is undefined if this method is called from a thread
    /// that already has a lock on this mutex.
    void lockWrite();

    /// Attempt to lock this reader-writer mutex for reading.  Immediately
    /// return 0 on success, and a non-zero value if there are active or
    /// pending writers.  If successful, `unlock` must be used to release
    /// the lock on this mutex.  The behavior is undefined if this method is
    /// called from a thread that already has a lock on this mutex.
    int tryLockRead();

    /// Attempt to lock this reader-writer mutex for writing.  Immediately
    /// return 0 on success, and a non-zero value if there are active or
    /// pending locks on this mutex.  If successful, `unlock` must be used
    /// to release the lock on this mutex.  The behavior is undefined if
    /// this method is called from a thread that already has a lock on this
    /// mutex.
    int tryLockWrite();

    /// Release the lock that the calling thread holds on this reader-writer
    /// mutex.  The behavior is undefined unless the calling thread
    /// currently has a lock on this mutex.
    void unlock();
};


// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

#ifdef BSLMT_PLATFORM_POSIX_THREADS

                            // ------------------
                            // struct RWMutexImpl
                            // ------------------

// CREATORS
inline
RWMutexImpl<bslmt::Platform::PosixThreads>::RWMutexImpl()
{
    const int rc = pthread_rwlock_init(&d_lock, NULL);

    // pthread_rwlock_init should not return a failure code.

    if (rc) {
        BSLS_ASSERT_INVOKE_NORETURN("'pthread_rwlock_init' failed");
    }
}

inline
RWMutexImpl<bslmt::Platform::PosixThreads>::~RWMutexImpl()
{
    const int rc = pthread_rwlock_destroy(&d_lock);

    // pthread_rwlock_destroy should not return a failure code.

    BSLS_ASSERT_SAFE(0 == rc);
    (void) rc;    // suppress 'unused variable' warnings
}

// MANIPULATORS
inline
void
RWMutexImpl<bslmt::Platform::PosixThreads>::lockRead()
{
    const int rc = pthread_rwlock_rdlock(&d_lock);

    // pthread_rwlock_rdlock should not return a failure code.

    BSLS_ASSERT_SAFE(0 == rc);
    (void) rc;    // suppress 'unused variable' warnings
}

inline
void
RWMutexImpl<bslmt::Platform::PosixThreads>::lockWrite()
{
    const int rc = pthread_rwlock_wrlock(&d_lock);

    // pthread_rwlock_wrlock should not return a failure code.

    BSLS_ASSERT_SAFE(0 == rc);
    (void) rc;    // suppress 'unused variable' warnings
}

inline
int
RWMutexImpl<bslmt::Platform::PosixThreads>::tryLockRead()
{
    return pthread_rwlock_tryrdlock(&d_lock) ? 1 : 0;
}

inline
int
RWMutexImpl<bslmt::Platform::PosixThreads>::tryLockWrite()
{
    return pthread_rwlock_trywrlock(&d_lock) ? 1 : 0;
}

inline
void
RWMutexImpl<bslmt::Platform::PosixThreads>::unlock()
{
    pthread_rwlock_unlock(&d_lock);
}

#endif  // BSLMT_PLATFORM_POSIX_THREADS

                              // -------------
                              // class RWMutex
                              // -------------

// CREATORS
inline
RWMutex::RWMutex()
{
}

inline
RWMutex::~RWMutex()
{
}

// MANIPULATORS
inline
void RWMutex::lockRead()
{
    d_impl.lockRead();
}

inline
void RWMutex::lockWrite()
{
    d_impl.lockWrite();
}

inline
int RWMutex::tryLockRead()
{
    return d_impl.tryLockRead();
}

inline
int RWMutex::tryLockWrite()
{
    return d_impl.tryLockWrite();
}

inline
void RWMutex::unlock()
{
    d_impl.unlock();
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
