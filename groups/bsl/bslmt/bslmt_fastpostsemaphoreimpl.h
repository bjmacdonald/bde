// bslmt_fastpostsemaphoreimpl.h                                      -*-C++-*-

#ifndef INCLUDED_BSLMT_FASTPOSTSEMAPHOREIMPL
#define INCLUDED_BSLMT_FASTPOSTSEMAPHOREIMPL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a testable semaphore class optimizing 'post'.
//
//@CLASSES:
//  bslmt::FastPostSemaphoreImpl: testable semaphore class optimizing 'post'
//
//@SEE_ALSO: bslmt_fastpostsemaphore, bslmt_semaphore
//
//@DESCRIPTION: This component defines a testable semaphore,
// 'bslmt::FastPostSemaphoreImpl', with the 'post' operation being optimized at
// the potential expense of other operations.  In particular,
// 'bslmt::FastPostSemaphoreImpl' is an efficient synchronization primitive
// that enables sharing of a counted number of resources.
// 'bslmt::FastPostSemaphoreImpl' supports the methods 'timedWait', 'enable',
// and 'disable' in addition to the standard semaphore methods.
//
// Commonly, during periods of time when the protected resource is scarce (the
// semaphore count is frequently zero) and threads are frequently blocked on
// wait methods, pessimizing the performance of the threads that block will
// have little effect on overall performance.  In this case, optimizing 'post'
// *may* be a performance improvement.  Note that when the resource is
// plentiful, there are no blocked threads and we expect the differences
// between semaphore implementations to be trivial.
//
///Supported Clock-Types
///---------------------
// 'bsls::SystemClockType' supplies the enumeration indicating the system clock
// on which timeouts supplied to other methods should be based.  If the clock
// type indicated at construction is 'bsls::SystemClockType::e_REALTIME', the
// 'absTime' argument passed to the 'timedWait' method should be expressed as
// an *absolute* offset since 00:00:00 UTC, January 1, 1970 (which matches the
// epoch used in 'bsls::SystemTime::now(bsls::SystemClockType::e_REALTIME)'.
// If the clock type indicated at construction is
// 'bsls::SystemClockType::e_MONOTONIC', the 'absTime' argument passed to the
// 'timedWait' method should be expressed as an *absolute* offset since the
// epoch of this clock (which matches the epoch used in
// 'bsls::SystemTime::now(bsls::SystemClockType::e_MONOTONIC)'.
//
///Usage
///-----
// There is no usage example for this component since it is not meant for
// direct client use.

#include <bslscm_version.h>

#include <bslmt_lockguard.h>

#include <bsls_review.h>
#include <bsls_systemclocktype.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>

#include <bsl_climits.h>

namespace BloombergLP {
namespace bslmt {

                // =========================================
                // class FastPostSemaphoreImplWorkaroundUtil
                // =========================================

class FastPostSemaphoreImplWorkaroundUtil {
    // This class provides utility functions for workarounds to system level
    // issues for 'FastPostSemaphoreImpl'.

  private:
    // CLASS DATA
    static bool s_postAlwaysSignals;

  public:
    // PUBLIC CLASS METHODS
    static void removePostAlwaysSignalsMitigation();
        // Remove the mitigation of 'post' always signalling the condition
        // variable.  Note this mitigation was introduced as a work around for
        // a lost signal bug in the underlying implementation of condition
        // variable (e.g.,
        // https://sourceware.org/bugzilla/show_bug.cgi?id=25847).  The
        // availability of the pthread correction is tracked in DRQS 172614796,
        // and the reversion of this workaround in DRQS 174079882.

    static bool usePostAlwaysSignalsMitigation();
        // Return 'true' if  the mitigation of 'post' always signalling the
        // condition variable should be used, and 'false' otherwise.
};

                       // ===========================
                       // class FastPostSemaphoreImpl
                       // ===========================

template <class ATOMIC_OP, class MUTEX, class CONDITION, class THREADUTIL>
class FastPostSemaphoreImpl {
    // This class implements a semaphore type, optimized for 'post', for thread
    // synchronization.

    // PRIVATE TYPES
    typedef typename ATOMIC_OP::AtomicTypes::Int64 AtomicInt64;
    typedef          bsls::Types::Int64            Int64;

    // DATA
    AtomicInt64 d_state;          // bit pattern representing the state of the
                                  // semaphore (see *Implementation* *Note*)

    MUTEX       d_waitMutex;      // mutex used with 'd_waitCondition', does
                                  // not protect any values

    CONDITION   d_waitCondition;  // condition variable for blocking/signalling
                                  // threads in the wait methods

    // PRIVATE CLASS METHODS
    static bsls::Types::Int64 disabledGeneration(Int64 state);
        // Return a value suitable for detecting a rapid short sequence of
        // 'disable' and 'enable' invocations, by comparing the value returned
        // by 'disabledGeneration' before and after the sequence, for the
        // specified 'state'.

    static bsls::Types::Int64 getValueRaw(Int64 state);
        // Return the semaphore value, without enforcing a zero minimum value,
        // implied by the specified 'state'.

    static bool hasAvailable(Int64 state);
        // Return 'true' if the specified 'state' implies the associated
        // semaphore has available count.

    static bool hasBlockedThread(Int64 state);
        // Return 'true' if the specified 'state' implies the associated
        // semaphore has one or more threads blocked in a wait operation.

    static bool isDisabled(Int64 state);
        // Return 'true' if the specified 'state' implies the associated
        // semaphore is "wait disabled".

    static bool willHaveBlockedThread(Int64 state);
        // Return 'true' if the specified 'state' implies the associated
        // semaphore does not have sufficient resources to meet the demand upon
        // the resources, implying there will be one or more threads blocked in
        // wait operations (without further 'post' invocations).

    // PRIVATE MANIPULATORS
    int timedWaitSlowPath(const bsls::TimeInterval& absTime,
                          const bsls::Types::Int64  initialState);
        // If this semaphore becomes disabled as detected from the disabled
        // generation encoded in the specified 'initialState' (see
        // *Implementation* *Note*), return 'e_DISABLED' with no effect on the
        // count.  Otherwise, block until the count of this semaphore is a
        // positive value or the specified 'absTime' timeout expires.  If the
        // count of this timed semaphore is a positive value, return 0 and
        // atomically decrement the count.  If the 'absTime' timeout expires,
        // return 'e_TIMED_OUT' with no effect on the count.  'absTime' is an
        // absolute time represented as an interval from some epoch, which is
        // determined by the clock indicated at construction (see {Supported
        // Clock-Types} in the component-level documentation).  This method is
        // invoked from 'timedWait' when the invoking thread may have to be
        // blocked.

    int waitSlowPath(const bsls::Types::Int64 initialState);
        // If this semaphore becomes disabled as detected from the disabled
        // generation encoded in the specified 'initialState' (see
        // *Implementation* *Note*), return 'e_DISABLED' with no effect on the
        // count.  Otherwise, block until the count of this semaphore is a
        // positive value, return 0 and atomically decrement the count.  This
        // method is invoked from 'wait' when the invoking thread may have to
        // be blocked.

    // NOT IMPLEMENTED
    FastPostSemaphoreImpl(const FastPostSemaphoreImpl&);
    FastPostSemaphoreImpl& operator=(const FastPostSemaphoreImpl&);

  public:
    // PUBLIC CONSTANTS
    enum ReturnValue {
        e_SUCCESS     =  0,  // indicates success
        e_DISABLED    = -1,  // indicates semaphore is disabled
        e_TIMED_OUT   = -2,  // indicates operation timed out
        e_WOULD_BLOCK = -3,  // indicates operation would block ('tryWait')
        e_FAILED      = -4   // indicates failure reported from condition
    };

    // The following constants are used to maintain the semaphore's 'd_state'
    // attribute values for:
    //: o number of threads blocked,
    //: o generation count for tracking enabled/disabled,
    //: o and available count.
    //
    // The 'k_*_MASK' constants define the layout of the attributes, the
    // 'k_*_INC' constants are used to modify the 'd_state' attributes, and the
    // 'k_*_SHIFT' constants allow recovery of the stored value.
    //
    // See *Implementation* *Note* for further details.  These are 'public' to
    // ease testing.

    static const Int64 k_BLOCKED_INC        = 0x0000000000000001LL;
    static const Int64 k_BLOCKED_MASK       = 0x0000000000ffffffLL;
    static const Int64 k_DISABLED_GEN_INC   = 0x0000000001000000LL;
    static const Int64 k_DISABLED_GEN_MASK  = 0x000000000f000000LL;
    static const int   k_DISABLED_GEN_SHIFT = 24;
    static const Int64 k_AVAILABLE_INC      = 0x0000000010000000LL;
    static const Int64 k_AVAILABLE_MASK     = 0xfffffffff0000000LL;
    static const int   k_AVAILABLE_SHIFT    = 28;

    // CREATORS
    explicit
    FastPostSemaphoreImpl(
    bsls::SystemClockType::Enum clockType = bsls::SystemClockType::e_REALTIME);
        // Create a 'FastPostSemaphoreImpl' object initially having a count of
        // 0.  Optionally specify a 'clockType' indicating the type of the
        // system clock against which the 'bsls::TimeInterval' 'absTime'
        // timeouts passed to the 'timedWait' method are to be interpreted (see
        // {Supported Clock-Types} in the component-level documentation).  If
        // 'clockType' is not specified then the realtime system clock is used.

    explicit
    FastPostSemaphoreImpl(
    int                         count,
    bsls::SystemClockType::Enum clockType = bsls::SystemClockType::e_REALTIME);
        // Create a 'FastPostSemaphoreImpl' object initially having the
        // specified 'count'.  Optionally specify a 'clockType' indicating the
        // type of the system clock against which the 'bsls::TimeInterval'
        // 'absTime' timeouts passed to the 'timedWait' method are to be
        // interpreted (see {Supported Clock-Types} in the component
        // documentation).  If 'clockType' is not specified then the realtime
        // system clock is used.

    // ~FastPostSemaphoreImpl() = default;
        // Destroy this object.

    // MANIPULATORS
    void disable();
        // Disable waiting on this semaphore.  All subsequent invocations of
        // 'wait', 'tryWait', and 'timedWait' will fail immediately.  All
        // blocked invocations of 'wait' and 'timedWait' will fail immediately.
        // If the semaphore is initially disabled, this call has no effect.

    void enable();
        // Enable waiting on this semaphore.  If the semaphore is initially
        // enabled, this call has no effect.

    void post();
        // Atomically increment the count of this semaphore.

    void post(int value);
        // Atomically increase the count of this semaphore by the specified
        // 'value'.  The behavior is undefined unless 'value > 0'.

    void postWithRedundantSignal(int value, int available, int blocked);
        // Atomically increase the count of this semaphore by the specified
        // 'value'.  If the resources available to this semaphore is greater
        // than or equal to the specified 'available' and the number of threads
        // blocked in this semaphore is greater than or equal to the specified
        // 'blocked', always send a signal to potentially wake a waiting thread
        // (even if the signal should not be needed).  The behavior is
        // undefined unless 'value > 0'.  Note that this method is provided to
        // help mitigate issues in the implementation of underlying
        // synchronization primitives.

    int take(int maximumToTake);
        // If the count of this semaphore is positive, reduce the count by the
        // lesser of the count and the specified 'maximumToTake' and return the
        // magnitude of the change to the count.  Otherwise, do nothing and
        // return 0.

    int takeAll();
        // If the count of this semaphore is positive, reduce the count to 0
        // and return the original value of the count.  Otherwise, do nothing
        // and return 0.

    int timedWait(const bsls::TimeInterval& absTime);
        // If this semaphore is initially disabled, or becomes disabled while
        // blocking, return 'e_DISABLED' with no effect on the count.
        // Otherwise, block until the count of this semaphore is a positive
        // value or the specified 'absTime' timeout expires.  If the count of
        // this semaphore is a positive value, return 0 and atomically
        // decrement the count.  If the 'absTime' timeout expires, return
        // 'e_TIMED_OUT' with no effect on the count.  Return 'e_FAILED' if an
        // error occurs.  'absTime' is an *absolute* time represented as an
        // interval from some epoch, which is determined by the clock indicated
        // at construction (see {Supported Clock-Types} in the component
        // documentation).

    int tryWait();
        // If this semaphore is initially disabled, return 'e_DISABLED' with no
        // effect on the count.  Otherwise, if the count of this semaphore is a
        // positive value, return 0 and atomically decrement the count.  If
        // this semaphore is not disabled and the count of this semaphore is
        // not a positive value, return 'e_WOULD_BLOCK' with no effect on the
        // count.

    int wait();
        // If this semaphore is initially disabled, or becomes disabled while
        // blocking, return 'e_DISABLED' with no effect on the count.
        // Otherwise, block until the count of this semaphore is a positive
        // value, return 0 and atomically decrement the count.  Return
        // 'e_FAILED' if an error occurs.

    // ACCESSORS
    bsls::SystemClockType::Enum clockType() const;
        // Return the clock type used for timeouts.

    int getDisabledState() const;
        // Return an odd value if this semaphore is wait disabled, and an even
        // value otherwise.  The returned value can be used to detect a rapid
        // short sequence of 'disable' and 'enable' invocations by comparing
        // the value returned by 'getDisabledState' before and after the
        // sequence.  For example, for any initial state of a semphore instance
        // 'obj':
        //..
        //  int state = obj.getDisabledState();
        //  obj.disable();
        //  obj.enable();
        //  ASSERT(state != obj.getDisabledState());
        //..
        // This functionality is useful in higher-level components to determine
        // if this semaphore was disabled during an operation.

    int getValue() const;
        // Return the current value ('count > 0 ? count : 0') of this
        // semaphore.

    bool isDisabled() const;
        // Return 'true' if this semaphore is wait disabled, and 'false'
        // otherwise.  Note that the semaphore is created in the "wait enabled"
        // state.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                // -----------------------------------------
                // class FastPostSemaphoreImplWorkaroundUtil
                // -----------------------------------------

// PUBLIC CLASS METHODS
inline
void FastPostSemaphoreImplWorkaroundUtil::removePostAlwaysSignalsMitigation()
{
    s_postAlwaysSignals = false;
}

inline
bool FastPostSemaphoreImplWorkaroundUtil::usePostAlwaysSignalsMitigation()
{
    return s_postAlwaysSignals;
}

                       // ---------------------------
                       // class FastPostSemaphoreImpl
                       // ---------------------------

// PRIVATE CLASS METHODS
template <class ATOMIC_OP, class MUTEX, class CONDITION, class THREADUTIL>
inline
bsls::Types::Int64
                 FastPostSemaphoreImpl<ATOMIC_OP, MUTEX, CONDITION, THREADUTIL>
                                              ::disabledGeneration(Int64 state)
{
    return state & k_DISABLED_GEN_MASK;
}

template <class ATOMIC_OP, class MUTEX, class CONDITION, class THREADUTIL>
inline
bsls::Types::Int64
                 FastPostSemaphoreImpl<ATOMIC_OP, MUTEX, CONDITION, THREADUTIL>
                                                     ::getValueRaw(Int64 state)
{
    return (state >> k_AVAILABLE_SHIFT) - (state & k_BLOCKED_MASK);
}

template <class ATOMIC_OP, class MUTEX, class CONDITION, class THREADUTIL>
inline
bool FastPostSemaphoreImpl<ATOMIC_OP, MUTEX, CONDITION, THREADUTIL>
                                                    ::hasAvailable(Int64 state)
{
    return k_AVAILABLE_INC <= state;
}

template <class ATOMIC_OP, class MUTEX, class CONDITION, class THREADUTIL>
inline
bool FastPostSemaphoreImpl<ATOMIC_OP, MUTEX, CONDITION, THREADUTIL>
                                                ::hasBlockedThread(Int64 state)
{
    return 0 != (state & k_BLOCKED_MASK);
}

template <class ATOMIC_OP, class MUTEX, class CONDITION, class THREADUTIL>
inline
bool FastPostSemaphoreImpl<ATOMIC_OP, MUTEX, CONDITION, THREADUTIL>
                                                      ::isDisabled(Int64 state)
{
    return 0 != (state & k_DISABLED_GEN_INC);
}

template <class ATOMIC_OP, class MUTEX, class CONDITION, class THREADUTIL>
inline
bool FastPostSemaphoreImpl<ATOMIC_OP, MUTEX, CONDITION, THREADUTIL>
                                           ::willHaveBlockedThread(Int64 state)
{
    return (state >> k_AVAILABLE_SHIFT) < (state & k_BLOCKED_MASK);
}

// PRIVATE MANIPULATORS
template <class ATOMIC_OP, class MUTEX, class CONDITION, class THREADUTIL>
int FastPostSemaphoreImpl<ATOMIC_OP, MUTEX, CONDITION, THREADUTIL>
                    ::timedWaitSlowPath(const bsls::TimeInterval& absTime,
                                        const bsls::Types::Int64  initialState)
{
    int rv = e_SUCCESS;

    const Int64 disabledGen = disabledGeneration(initialState);

    // 'state' currently indicates the thread should block, yield and retest
    // instead

    THREADUTIL::yield();

    Int64 state = ATOMIC_OP::getInt64Acquire(&d_state);

    if (willHaveBlockedThread(state)) {
        {
            LockGuard<MUTEX> guard(&d_waitMutex);

            // note that the following operation indicates the thread is
            // blocked (see *Implementation* *Notes*) and does not affect the
            // decision for other threads to block

            state = ATOMIC_OP::addInt64NvAcqRel(
                                              &d_state,
                                              k_AVAILABLE_INC + k_BLOCKED_INC);

            // wait until there is an available resource or this semaphore is
            // disabled

            while (   !hasAvailable(state)
                   && disabledGen == disabledGeneration(state)) {
                int rv = d_waitCondition.timedWait(&d_waitMutex, absTime);
                if (rv) {
                    ATOMIC_OP::addInt64AcqRel(&d_state, -k_BLOCKED_INC);
                    if (-1 == rv) {
                        return e_TIMED_OUT;                           // RETURN
                    }
                    return e_FAILED;                                  // RETURN
                }
                state = ATOMIC_OP::getInt64Acquire(&d_state);
            }

            if (hasAvailable(state)) {
                state = ATOMIC_OP::addInt64NvAcqRel(
                                           &d_state,
                                           -(k_AVAILABLE_INC + k_BLOCKED_INC));
            }
            else {
                ATOMIC_OP::addInt64AcqRel(&d_state, -k_BLOCKED_INC);
                rv = e_DISABLED;
            }
        }

        // signal when 'state' indicates there is an available resource, the
        // semaphore is not disabled, and there are blocked threads

        if (   hasAvailable(state)
            && !isDisabled(state)
            && hasBlockedThread(state)) {
            d_waitCondition.signal();
        }
    }
    else {
        // signal when 'state' indicates there is an available resource, the
        // semaphore is not disabled, and there are blocked threads

        if (   hasAvailable(state)
            && !isDisabled(state)
            && hasBlockedThread(state)) {
            {
                LockGuard<MUTEX> guard(&d_waitMutex);
            }
            d_waitCondition.signal();
        }
    }

    return rv;
}

template <class ATOMIC_OP, class MUTEX, class CONDITION, class THREADUTIL>
int FastPostSemaphoreImpl<ATOMIC_OP, MUTEX, CONDITION, THREADUTIL>
                          ::waitSlowPath(const bsls::Types::Int64 initialState)
{
    int rv = e_SUCCESS;

    const Int64 disabledGen = disabledGeneration(initialState);

    // 'state' currently indicates the thread should block, yield and retest
    // instead

    THREADUTIL::yield();

    Int64 state = ATOMIC_OP::getInt64Acquire(&d_state);

    if (willHaveBlockedThread(state)) {
        {
            LockGuard<MUTEX> guard(&d_waitMutex);

            // note that the following operation indicates the thread is
            // blocked (see *Implementation* *Notes*) and does not affect the
            // decision for other threads to block

            state = ATOMIC_OP::addInt64NvAcqRel(
                                              &d_state,
                                              k_AVAILABLE_INC + k_BLOCKED_INC);

            // wait until there is an available resource or this semaphore is
            // disabled

            while (   !hasAvailable(state)
                   && disabledGen == disabledGeneration(state)) {
                int rv = d_waitCondition.wait(&d_waitMutex);
                if (rv) {
                    ATOMIC_OP::addInt64AcqRel(&d_state, -k_BLOCKED_INC);
                    return e_FAILED;                                  // RETURN
                }
                state = ATOMIC_OP::getInt64Acquire(&d_state);
            }

            if (hasAvailable(state)) {
                state = ATOMIC_OP::addInt64NvAcqRel(
                                           &d_state,
                                           -(k_AVAILABLE_INC + k_BLOCKED_INC));
            }
            else {
                ATOMIC_OP::addInt64AcqRel(&d_state, -k_BLOCKED_INC);
                rv = e_DISABLED;
            }
        }

        // signal when 'state' indicates there is an available resource, the
        // semaphore is not disabled, and there are blocked threads

        if (   hasAvailable(state)
            && !isDisabled(state)
            && hasBlockedThread(state)) {
            d_waitCondition.signal();
        }
    }
    else {
        // signal when 'state' indicates there is an available resource, the
        // semaphore is not disabled, and there are blocked threads

        if (   hasAvailable(state)
            && !isDisabled(state)
            && hasBlockedThread(state)) {
            {
                LockGuard<MUTEX> guard(&d_waitMutex);
            }
            d_waitCondition.signal();
        }
    }

    return rv;
}

// CREATORS
template <class ATOMIC_OP, class MUTEX, class CONDITION, class THREADUTIL>
inline
FastPostSemaphoreImpl<ATOMIC_OP, MUTEX, CONDITION, THREADUTIL>
                 ::FastPostSemaphoreImpl(bsls::SystemClockType::Enum clockType)
: d_waitMutex()
, d_waitCondition(clockType)
{
    ATOMIC_OP::initInt64(&d_state, 0);
}

template <class ATOMIC_OP, class MUTEX, class CONDITION, class THREADUTIL>
inline
FastPostSemaphoreImpl<ATOMIC_OP, MUTEX, CONDITION, THREADUTIL>
                 ::FastPostSemaphoreImpl(int                         count,
                                         bsls::SystemClockType::Enum clockType)
: d_waitMutex()
, d_waitCondition(clockType)
{
    ATOMIC_OP::initInt64(&d_state, k_AVAILABLE_INC * count);
}

// MANIPULATORS
template <class ATOMIC_OP, class MUTEX, class CONDITION, class THREADUTIL>
void FastPostSemaphoreImpl<ATOMIC_OP, MUTEX, CONDITION, THREADUTIL>::disable()
{
    Int64 state = ATOMIC_OP::getInt64Acquire(&d_state);

    while (false == isDisabled(state)) {
        const Int64 expState = state;

        // increment, without overflowing, the disabled attribute

        Int64 newState = (state & ~k_DISABLED_GEN_MASK) |
                          ((state + k_DISABLED_GEN_INC) & k_DISABLED_GEN_MASK);

        state = ATOMIC_OP::testAndSwapInt64AcqRel(&d_state,
                                                  state,
                                                  newState);

        if (expState == state) {
            state = newState;

            // note that 'd_waitMutex' must be acquired to ensure a thread in a
            // wait operation either "sees" the change in state before
            // determining whether to block using 'd_waitCondition', or has
            // blocked and will receive a signal sent to 'd_waitCondition'

            {
                LockGuard<MUTEX> guard(&d_waitMutex);
            }
            d_waitCondition.broadcast();
        }
    }

    // When threads blocked on 'd_waitCondition' are signalled, they must
    // prefer consuming available resources and returning success over
    // returning 'e_DISABLED'.  This ensures the signalled thread obtains the
    // resource when another thread issues a post followed by a disablement
    // (see DRQS 153332608).  Similarly, to ensure a disablement followed by a
    // post method does not provide a resource to the signalled thread,
    // 'disable' must wait until no threads will block before returning.  Note
    // that the semaphore may be re-enabled (and re-disabled) during this wait.

    while (isDisabled(state) && willHaveBlockedThread(state)) {
        THREADUTIL::yield();

        state = ATOMIC_OP::getInt64Acquire(&d_state);
    }
}

template <class ATOMIC_OP, class MUTEX, class CONDITION, class THREADUTIL>
void FastPostSemaphoreImpl<ATOMIC_OP, MUTEX, CONDITION, THREADUTIL>::enable()
{
    Int64 state = ATOMIC_OP::getInt64Acquire(&d_state);

    while (isDisabled(state)) {
        // When this semaphore is disabled, an 'enable' followed by a post
        // method must not provide a resource to a signalled thread that should
        // return 'e_DISABLED' (see note in 'disable').  Hence, wait until no
        // threads will block before performing the enablement.

        if (willHaveBlockedThread(state)) {
            THREADUTIL::yield();

            state = ATOMIC_OP::getInt64Acquire(&d_state);
        }
        else {
            const Int64 expState = state;

            // increment, without overflowing, the disabled attribute

            Int64 newState = (state & ~k_DISABLED_GEN_MASK) |
                          ((state + k_DISABLED_GEN_INC) & k_DISABLED_GEN_MASK);

            state = ATOMIC_OP::testAndSwapInt64AcqRel(&d_state,
                                                      state,
                                                      newState);

            if (expState == state) {
                state = newState;
            }
        }
    }
}

template <class ATOMIC_OP, class MUTEX, class CONDITION, class THREADUTIL>
inline
void FastPostSemaphoreImpl<ATOMIC_OP, MUTEX, CONDITION, THREADUTIL>::post()
{
    Int64 state = ATOMIC_OP::addInt64NvAcqRel(&d_state, k_AVAILABLE_INC);

    // signal only when 'state' indicates there are no other threads that can
    // unblock blocked threads

    if (   (   FastPostSemaphoreImplWorkaroundUtil::
                                               usePostAlwaysSignalsMitigation()
            || k_AVAILABLE_INC == (state & k_AVAILABLE_MASK))
        && !isDisabled(state)
        && hasBlockedThread(state)) {

        // note that 'd_waitMutex' must be acquired to ensure a thread in a
        // wait operation either "sees" the change in state before determining
        // whether to block using 'd_waitCondition', or has blocked and will
        // receive a signal sent to 'd_waitCondition'

        {
            LockGuard<MUTEX> guard(&d_waitMutex);
        }
        d_waitCondition.signal();
    }
}

template <class ATOMIC_OP, class MUTEX, class CONDITION, class THREADUTIL>
inline
void FastPostSemaphoreImpl<ATOMIC_OP, MUTEX, CONDITION, THREADUTIL>
                                                              ::post(int value)
{
    Int64 v     = k_AVAILABLE_INC * value;
    Int64 state = ATOMIC_OP::addInt64NvAcqRel(&d_state, v);

    // signal only when 'state' indicates there are no other threads that can
    // unblock blocked threads

    if (   (   FastPostSemaphoreImplWorkaroundUtil::
                                               usePostAlwaysSignalsMitigation()
            || v == (state & k_AVAILABLE_MASK))
        && !isDisabled(state)
        && hasBlockedThread(state)) {

        // note that 'd_waitMutex' must be acquired to ensure a thread in a
        // wait operation either "sees" the change in state before determining
        // whether to block using 'd_waitCondition', or has blocked and will
        // receive a signal sent to 'd_waitCondition'

        {
            LockGuard<MUTEX> guard(&d_waitMutex);
        }
        d_waitCondition.signal();
    }
}

template <class ATOMIC_OP, class MUTEX, class CONDITION, class THREADUTIL>
inline
void FastPostSemaphoreImpl<ATOMIC_OP, MUTEX, CONDITION, THREADUTIL>
                                       ::postWithRedundantSignal(int value,
                                                                 int available,
                                                                 int blocked)
{
    Int64 v     = k_AVAILABLE_INC * value;
    Int64 state = ATOMIC_OP::addInt64NvAcqRel(&d_state, v);

    // signal only when 'state' indicates there are no other threads that can
    // unblock blocked threads, or there are 'available' or more resources and
    // 'blocked' or more threads

    if (   (   FastPostSemaphoreImplWorkaroundUtil::
                                               usePostAlwaysSignalsMitigation()
            || v == (state & k_AVAILABLE_MASK)
            || (   k_AVAILABLE_INC * available <= (state & k_AVAILABLE_MASK)
                && blocked <= (state & k_BLOCKED_MASK)))
        && !isDisabled(state)
        && hasBlockedThread(state)) {

        // note that 'd_waitMutex' must be acquired to ensure a thread in a
        // wait operation either "sees" the change in state before determining
        // whether to block using 'd_waitCondition', or has blocked and will
        // receive a signal sent to 'd_waitCondition'

        {
            LockGuard<MUTEX> guard(&d_waitMutex);
        }
        d_waitCondition.signal();

        BSLS_REVIEW_OPT(   (   FastPostSemaphoreImplWorkaroundUtil::
                                               usePostAlwaysSignalsMitigation()
                            || v == (state & k_AVAILABLE_MASK))
                        && "redundant signal sent");
    }
}

template <class ATOMIC_OP, class MUTEX, class CONDITION, class THREADUTIL>
inline
int FastPostSemaphoreImpl<ATOMIC_OP, MUTEX, CONDITION, THREADUTIL>
                                                      ::take(int maximumToTake)
{
    Int64 state = ATOMIC_OP::getInt64Acquire(&d_state);
    Int64 expState;
    Int64 count;

    do {
        // remove all available up to 'maximumToTake'

        expState = state;

        count = getValueRaw(state);

        if (0 >= count) {
            return 0;                                                 // RETURN
        }

        if (maximumToTake < count) {
            count = maximumToTake;
        }

        state = ATOMIC_OP::testAndSwapInt64AcqRel(
                                              &d_state,
                                              state,
                                              state - k_AVAILABLE_INC * count);
    } while (state != expState);

    return static_cast<int>(count);
}

template <class ATOMIC_OP, class MUTEX, class CONDITION, class THREADUTIL>
inline
int FastPostSemaphoreImpl<ATOMIC_OP, MUTEX, CONDITION, THREADUTIL>::takeAll()
{
    return take(INT_MAX);
}

template <class ATOMIC_OP, class MUTEX, class CONDITION, class THREADUTIL>
inline
int FastPostSemaphoreImpl<ATOMIC_OP, MUTEX, CONDITION, THREADUTIL>
                                 ::timedWait(const bsls::TimeInterval& absTime)
{
    Int64 state = ATOMIC_OP::addInt64NvAcqRel(&d_state, -k_AVAILABLE_INC);

    if (isDisabled(state)) {
        post();
        return e_DISABLED;                                            // RETURN
    }

    if (willHaveBlockedThread(state)) {
        return timedWaitSlowPath(absTime, state);                     // RETURN
    }

    return 0;
}

template <class ATOMIC_OP, class MUTEX, class CONDITION, class THREADUTIL>
inline
int FastPostSemaphoreImpl<ATOMIC_OP, MUTEX, CONDITION, THREADUTIL>::tryWait()
{
    Int64 state = ATOMIC_OP::addInt64NvAcqRel(&d_state, -k_AVAILABLE_INC);

    if (isDisabled(state)) {
        post();
        return e_DISABLED;                                            // RETURN
    }

    if (willHaveBlockedThread(state)) {
        post();
        return e_WOULD_BLOCK;                                         // RETURN
    }

    return 0;
}

template <class ATOMIC_OP, class MUTEX, class CONDITION, class THREADUTIL>
inline
int FastPostSemaphoreImpl<ATOMIC_OP, MUTEX, CONDITION, THREADUTIL>::wait()
{
    Int64 state = ATOMIC_OP::addInt64NvAcqRel(&d_state, -k_AVAILABLE_INC);

    if (isDisabled(state)) {
        post();
        return e_DISABLED;                                            // RETURN
    }

    if (willHaveBlockedThread(state)) {
        return waitSlowPath(state);                                   // RETURN
    }

    return 0;
}

// ACCESSORS
template <class ATOMIC_OP, class MUTEX, class CONDITION, class THREADUTIL>
inline
bsls::SystemClockType::Enum
FastPostSemaphoreImpl<ATOMIC_OP, MUTEX, CONDITION, THREADUTIL>
                                                            ::clockType() const
{
    return d_waitCondition.clockType();
}

template <class ATOMIC_OP, class MUTEX, class CONDITION, class THREADUTIL>
inline
int FastPostSemaphoreImpl<ATOMIC_OP, MUTEX, CONDITION, THREADUTIL>
                                                     ::getDisabledState() const
{
    Int64 state = ATOMIC_OP::getInt64Acquire(&d_state);

    return static_cast<int>((state & k_DISABLED_GEN_MASK)
                                                      >> k_DISABLED_GEN_SHIFT);
}

template <class ATOMIC_OP, class MUTEX, class CONDITION, class THREADUTIL>
inline
int FastPostSemaphoreImpl<ATOMIC_OP, MUTEX, CONDITION, THREADUTIL>
                                                             ::getValue() const
{
    Int64 count = getValueRaw(ATOMIC_OP::getInt64Acquire(&d_state));

    return static_cast<int>(count > 0 ? count : 0);
}

template <class ATOMIC_OP, class MUTEX, class CONDITION, class THREADUTIL>
inline
bool FastPostSemaphoreImpl<ATOMIC_OP, MUTEX, CONDITION, THREADUTIL>
                                                           ::isDisabled() const
{
    Int64 state = ATOMIC_OP::getInt64Acquire(&d_state);

    return isDisabled(state);
}

}  // close package namespace
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
