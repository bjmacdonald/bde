// bslalg_bidirectionallinklistutil.cpp                               -*-C++-*-
#include <bslalg_bidirectionallinklistutil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslalg_bidirectionallink.h>

#include <bsls_assert.h>
#include <bsls_types.h>

namespace BloombergLP
{

namespace bslalg
{

void BidirectionalLinkListUtil::insertLinkAfterTarget(
                                                    BidirectionalLink *newNode,
                                                    BidirectionalLink *target)
{
    BSLS_ASSERT_SAFE(newNode);
    BSLS_ASSERT_SAFE(target);

    BidirectionalLink *next = target->nextLink();

    target->setNextLink(newNode);
    if (next) {
        next->setPreviousLink(newNode);
    }

    newNode->setPreviousLink(target);
    newNode->setNextLink(next);
}

void BidirectionalLinkListUtil::insertLinkBeforeTarget(
                                                   BidirectionalLink  *newNode,
                                                   BidirectionalLink  *target)
{
    BSLS_ASSERT(newNode);


    if (!target) {  // Prepending before an empty list is *explicitly* *allowed*
        newNode->reset();
    }
    else if (BidirectionalLink *prev = target->previousLink()) {
        newNode->setPreviousLink(prev);
        prev->setNextLink(newNode);
        newNode->setNextLink(target);
        target->setPreviousLink(newNode);
    }
    else {
        newNode->setPreviousLink(0);  // asserted precondition
        newNode->setNextLink(target);
        target->setPreviousLink(newNode);
    }
}

void BidirectionalLinkListUtil::spliceListBeforeTarget
                                                   (BidirectionalLink *first,
                                                    BidirectionalLink *last,
                                                    BidirectionalLink *target)
{
    BSLS_ASSERT(first);
    BSLS_ASSERT(last);

    // unlink from existing list
    if (BidirectionalLink* prev = first->previousLink()) {
        prev->setNextLink(last->nextLink());
    }
    if (BidirectionalLink* next = last->nextLink()) {
        next->setPreviousLink(first->previousLink());
    }

    // update into spliced location:
    if (!target) {
        // Prepending target an empty list is *explicitly* *allowed*
        // The "spliced" segment is still extracted from the original list
        first->setPreviousLink(0);  // redundant with pre-condition
        last->setNextLink(0);
    }
    else {
        if (BidirectionalLink *prev = target->previousLink()) {
            first->setPreviousLink(prev);
            prev->setNextLink(first);
        }
        else {
            first->setPreviousLink(0);
        }

        last->setNextLink(target);
        target->setPreviousLink(last);
    }
}


void BidirectionalLinkListUtil::unlink(BidirectionalLink *node)
{
    BSLS_ASSERT_SAFE(node);

    if (BidirectionalLink *prev = node->previousLink()) {
        if (BidirectionalLink *next = node->nextLink()) {
             next->setPreviousLink(prev);
            prev->setNextLink(next);
        }
        else {
            prev->setNextLink(0);
        }
    }
    else if(BidirectionalLink *next = node->nextLink()) {
        next->setPreviousLink(0);
    }
}

}  // close namespace BloobmergLP::bslalg
}  // close namespace BloombergLP
// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
