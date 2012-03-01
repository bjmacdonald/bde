// bael_multiplexobserver.cpp     -*-C++-*-
#include <bael_multiplexobserver.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_multiplexobserver_cpp,"$Id$ $CSID$")

#include <bael_countingallocator.h>      // for testing only
#include <bael_defaultobserver.h>        // for testing only
#include <bael_testobserver.h>           // for testing only
#include <bsls_assert.h>
#include <bsl_iostream.h>                // for warning print only

namespace BloombergLP {

                         // ----------------------------
                         // class bael_MultiplexObserver
                         // ----------------------------

// CREATORS
bael_MultiplexObserver::~bael_MultiplexObserver()
{
    BSLS_ASSERT(0 <= numRegisteredObservers());

    // TBD: Remove this test once the observer changes in BDE 2.12 have
    // stabilized.

    bsl::set<bael_Observer *>::const_iterator it = d_observerSet.begin();
    for (; it != d_observerSet.end(); ++it) {
        if (0xdeadbeef == *((unsigned int*)(*it))){
            bsl::cerr << "ERROR: bael_MultiplexObserver: Registered observer "
                      << "is destroyed before being deregistered."
                      << " [~bael_MultiplexObserver]" << bsl::endl;
        }
    }

}

// MANIPULATORS
void bael_MultiplexObserver::publish(const bael_Record&  record,
                                     const bael_Context& context)
{
    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_rwMutex);

    // Print warning once that this publish method is deprecated.

    static bool needWarning = true;
    if (needWarning) {
        bsl::cerr << "WARNING: bael_MultiplexObserver: this publish method is "
                  << "deprecated, please use the alternative publish overload."
                  << bsl::endl;
        needWarning = false;
    }

    bsl::set<bael_Observer *>::const_iterator it = d_observerSet.begin();
    for (; it != d_observerSet.end(); ++it) {
        (*it)->publish(record, context);
    }
}

void bael_MultiplexObserver::publish(
                            const bcema_SharedPtr<const bael_Record>&  record,
                            const bael_Context&                        context)
{
    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_rwMutex);

    bsl::set<bael_Observer *>::const_iterator it = d_observerSet.begin();
    for (; it != d_observerSet.end(); ++it) {
        (*it)->publish(record, context);
    }
}

void bael_MultiplexObserver::releaseRecords()
{
    bcemt_ReadLockGuard<bcemt_RWMutex> guard(&d_rwMutex);

    bsl::set<bael_Observer *>::const_iterator it = d_observerSet.begin();
    for (; it != d_observerSet.end(); ++it) {
        // TBD: Remove this test once the observer changes in BDE 2.12 have
        // stabilized.

        if (0xdeadbeef == *((unsigned int*)(*it))) {
            bsl::cerr << "ERROR: bael_MultiplexObserver: Registered observer "
                      << "is destroyed before being deregistered."
                      << " [releaseRecords]" << bsl::endl;
        }
        else {
            (*it)->releaseRecords();
        }
    }
}

int bael_MultiplexObserver::registerObserver(bael_Observer *observer)
{
    if (0 == observer) {
        return 1;                                                     // RETURN
    }

    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_rwMutex);
    return !d_observerSet.insert(observer).second;
}

int bael_MultiplexObserver::deregisterObserver(bael_Observer *observer)
{
    if (0 == observer) {
        return 1;                                                     // RETURN
    }

    bcemt_WriteLockGuard<bcemt_RWMutex> guard(&d_rwMutex);
    const bool isRegistered =
                           d_observerSet.find(observer) != d_observerSet.end();

    if (isRegistered) {
        d_observerSet.erase(observer);
        observer->releaseRecords();
    }

    return !isRegistered;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
