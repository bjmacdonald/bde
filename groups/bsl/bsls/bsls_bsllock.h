// bsls_bsllock.h                                                     -*-C++-*-
#ifndef INCLUDED_BSLS_BSLLOCK
#define INCLUDED_BSLS_BSLLOCK

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a platform-independent mutex for use below `bslmt`.
//
//@CLASSES:
//  bsls::BslLock: platform-independent mutex
//  bsls::BslLockGuard: RAII mechanism for locking/unlocking a `BslLock`
//
//@SEE_ALSO: bslmt_mutex
//
//@DESCRIPTION: This component provides a mutually exclusive lock primitive
// ("mutex") by wrapping a suitable platform-specific mechanism.  The
// `bsls::BslLock` class provides `lock` and `unlock` operations.  Note that
// `bsls::BslLock` is not intended for direct client use; see `bslmt_mutex`
// instead.  Also note that `bsls::BslLock` is not recursive.
//
// This component also provides the `bsls::BslLockGuard` class, a mechanism
// that follows the RAII idiom for automatically acquiring and releasing the
// lock on an associated `bsls::BslLock` object.  To ensure exception safety,
// client code should make use of a `bsls::BslLockGuard` object wherever
// appropriate rather than calling the methods on the associated
// `bsls::BslLock` object directly.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Using `bsls::BslLock` to Make a `class` Thread-Safe
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we illustrate the use of `bsls::BslLock` and
// `bsls::BslLockGuard` to write a thread-safe class.
//
// First, we provide an elided definition of the `my_Account` class.  Note the
// `d_lock` data member of type `bsls::BslLock`:
// ```
// class my_Account {
//     // This 'class' implements a very simplistic bank account.  It is meant
//     // for illustrative purposes only.
//
//     // DATA
//     double                d_money;  // amount of money in the account
//     mutable bsls::BslLock d_lock;   // ensure exclusive access to 'd_money'
//
//   // ...
//
//   public:
//
//     // ...
//
//     // MANIPULATORS
//     void deposit(double amount);
//         // Atomically deposit the specified 'amount' of money into this
//         // account.  The behavior is undefined unless 'amount >= 0.0'.
//
//     int withdraw(double amount);
//         // Atomically withdraw the specified 'amount' of money from this
//         // account.  Return 0 on success, and a non-zero value otherwise.
//         // The behavior is undefined unless 'amount >= 0.0'.
//
//     // ...
// };
// ```
// Next, we show the implementation of the two `my_Account` manipulators
// show-casing the use of `bsls::BslLock` and `bsls::BslLockGuard`:
// ```
// // MANIPULATORS
// void my_Account::deposit(double amount)
// {
// ```
// Here, we use the interface of `bsls::BslLock` directly.  However, wherever
// appropriate, a `bsls::BslLockGuard` object should be used instead to ensure
// that an acquired lock is always properly released, even if an exception is
// thrown:
// ```
//     d_lock.lock();  // consider using 'bsls::BslLockGuard' (see 'withdraw')
//     d_money += amount;
//     d_lock.unlock();
// }
// ```
// In contrast, `withdraw` uses a `bsls::BslLockGuard` to automatically acquire
// and release the lock.  The lock is acquired as a side-effect of the
// construction of `guard`, and released when `guard` is destroyed upon
// returning from the function:
// ```
// int my_Account::withdraw(double amount)
// {
//     bsls::BslLockGuard guard(&d_lock);  // a very good practice
//
//     if (amount <= d_money) {
//         d_money -= amount;
//         return 0;
//     }
//     else {
//         return -1;
//     }
// }
// ```

#include <bsls_platform.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <bsls_bsllockimpl_win32.h>
#else
#include <bsls_bsllockimpl_pthread.h>
#endif

#ifdef BDE_BUILD_TARGET_SAFE
// This component needs to be below bsls_assert in the physical hierarchy, so
// 'BSLS_ASSERT' macros can't be used here.  To workaround this issue, we use
// the C 'assert' instead.
#include <assert.h>
#define BSLS_BSLLOCK_ASSERT_SAFE(x) assert((x))
#else
#define BSLS_BSLLOCK_ASSERT_SAFE(x)
#endif

namespace BloombergLP {
namespace bsls {

                              // =============
                              // class BslLock
                              // =============

/// This `class` implements a light-weight, portable wrapper of an OS-level
/// mutex to support intra-process synchronization.  The mutex implemented
/// by this class is *non*-recursive.  Note that `BslLock` is *not* intended
/// for direct use by client code; it is meant for internal use only.
class BslLock {

    // DATA
#ifdef BSLS_PLATFORM_OS_WINDOWS
    BslLockImpl_win32   d_lock;  // Windows critical section
#else
    BslLockImpl_pthread d_lock;  // 'pthreads' mutex object
#endif

  private:
    // NOT IMPLEMENTED
    BslLock(const BslLock&);             // = delete
    BslLock& operator=(const BslLock&);  // = delete

  public:
    // CREATORS

    /// Create a lock object initialized to the unlocked state.
    BslLock();

    /// Destroy this lock object.  The behavior is undefined unless this
    /// object is in the unlocked state.
    ~BslLock();

    // MANIPULATORS

    /// Acquire the lock on this object.  If the lock on this object is
    /// currently held by another thread, then suspend execution of the
    /// calling thread until the lock can be acquired.  The behavior is
    /// undefined unless the calling thread does not already hold the lock
    /// on this object.  Note that deadlock may result if this method is
    /// invoked while the calling thread holds the lock on the object.
    void lock();

    /// Release the lock on this object that was previously acquired through
    /// a call to `lock`, enabling another thread to acquire the lock.  The
    /// behavior is undefined unless the calling thread holds the lock on
    /// this object.
    void unlock();
};

                           // ==================
                           // class BslLockGuard
                           // ==================

/// This `class` implements a guard for automatically acquiring and
/// releasing the lock on an associated `bsls::BslLock` object.  This
/// mechanism follows the RAII idiom whereby the lock on the `BslLock`
/// associated with a guard object is acquired upon construction and
/// released upon destruction.
class BslLockGuard {

    // DATA
    BslLock *d_lock_p;  // lock guarded by this object (held, not owned)

  private:
    // NOT IMPLEMENTED
    BslLockGuard(const BslLockGuard&);             // = delete
    BslLockGuard& operator=(const BslLockGuard&);  // = delete

  public:
    // CREATORS

    /// Create a guard object that conditionally manages the specified
    /// `lock`, and acquires the lock on `lock` by invoking its `lock`
    /// method.  The behavior is undefined unless the calling thread does
    /// not already hold the lock on `lock`.  Note that deadlock may result
    /// if a guard is created for `lock` while the calling thread holds the
    /// lock on `lock`.  Also note that `lock` must remain valid throughout
    /// the lifetime of this guard, or until `release` is called.
    explicit BslLockGuard(BslLock *lock);

    /// Destroy this guard object and release the lock on the object it
    /// manages (if any) by invoking the `unlock` method of the object that
    /// was supplied at construction of this guard.  If no lock is currently
    /// being managed, this method has no effect.  Note that if this guard
    /// object currently manages a lock, this method assumes the behavior of
    /// `BslLock::unlock`.
    ~BslLockGuard();

    // MANIPULATORS

    /// Release from management, with no effect, the object currently
    /// managed by this guard, if any.  Note that `unlock` is *not* called
    /// on the managed object upon its release.
    void release();
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                              // -------------
                              // class BslLock
                              // -------------

// CREATORS
inline
BslLock::BslLock()
{
}

inline
BslLock::~BslLock()
{
}

// MANIPULATORS
inline
void BslLock::lock()
{
    d_lock.lock();
}

inline
void BslLock::unlock()
{
    d_lock.unlock();
}

                           // ------------------
                           // class BslLockGuard
                           // ------------------

// CREATORS
inline
BslLockGuard::BslLockGuard(BslLock *lock)
: d_lock_p(lock)
{
    BSLS_BSLLOCK_ASSERT_SAFE(lock);
    d_lock_p->lock();
}

inline
BslLockGuard::~BslLockGuard()
{
    if (d_lock_p) {
        d_lock_p->unlock();
    }
}

// MANIPULATORS
inline
void BslLockGuard::release()
{
    d_lock_p = 0;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
