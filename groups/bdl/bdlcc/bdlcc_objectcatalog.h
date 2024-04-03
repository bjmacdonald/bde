// bdlcc_objectcatalog.h                                              -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLCC_OBJECTCATALOG
#define INCLUDED_BDLCC_OBJECTCATALOG

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an efficient indexed, thread-safe object container.
//
//@CLASSES:
//     bdlcc::ObjectCatalog: templatized, thread-safe, indexed object container
// bdlcc::ObjectCatalogIter: thread-safe iterator for 'bdlcc::ObjectCatalog'
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a thread-safe and efficient templatized
// catalog of objects.  A 'bdlcc::ObjectCatalog' supports efficient insertion
// of objects through the 'add' method, which returns a handle that can be used
// for further reference to the newly added element.  An element can be
// accessed by providing its handle to the 'find' function.  Thread-safe design
// implies that the element is returned by value into an object buffer rather
// than by reference (see this package documentation for a discussion of
// thread-safe container design).  Likewise, an element can be modified by
// providing its handle and a new value to the 'replace' method.  Finally, an
// element can be removed by passing its handle to the 'remove' method; the
// handle is then no longer valid and subsequent calls to 'find' or 'remove'
// with this handle will return 0.
//
// 'bdlcc::ObjectCatalogIter' provides thread safe iteration through all the
// objects of an object catalog of parameterized 'TYPE'.  The order of the
// iteration is implementation defined.  Thread safe iteration is provided by
// (read)locking the object catalog during the iterator's construction and
// unlocking it at the iterator's destruction.  This guarantees that during the
// life time of an iterator, the object catalog can't be modified (however
// multiple threads can still concurrently read the object catalog).
//
// Note that an object catalog has a maximum capacity of 2^23 items.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Catalog Usage
/// - - - - - - - - - - - -
// Consider a client sending queries to a server asynchronously.  When the
// response to a query arrives, the client needs to invoke the callback
// associated with that query.  For good performance, the callback should be
// invoked as quickly as possible.  One way to achieve this is as follows.  The
// client creates a catalog for the functors associated with queries.  It sends
// to the server the handle (obtained by passing the callback functor
// associated with the query to the 'add' method of catalog), along with the
// query.  The server does not interpret this handle in any way and sends it
// back to the client along with the computed query result.  The client, upon
// receiving the response, gets the functor (associated with the query) back by
// passing the handle (contained in the response message) to the 'find' method
// of catalog.
//
// Assume the following declarations (we leave the implementations as
// undefined, as the definitions are largely irrelevant to this example):
//..
//  struct Query {
//      // Class simulating the query.
//  };
//
//  class QueryResult {
//      // Class simulating the result of a query.
//  };
//
//  class RequestMsg
//      // Class encapsulating the request message.  It encapsulates the
//      // actual query and the handle associated with the callback for the
//      // query.
//  {
//      Query d_query;
//      int   d_handle;
//
//    public:
//      RequestMsg(Query query, int handle)
//          // Create a request message with the specified 'query' and
//          // 'handle'.
//      : d_query(query)
//      , d_handle(handle)
//      {
//      }
//
//      int handle() const
//          // Return the handle contained in this response message.
//      {
//          return d_handle;
//      }
//  };
//
//  class ResponseMsg
//      // Class encapsulating the response message.  It encapsulates the query
//      // result and the handle associated with the callback for the query.
//  {
//      int d_handle;
//
//    public:
//      void setHandle(int handle)
//          // Set the "handle" contained in this response message to the
//          // specified 'handle'.
//      {
//          d_handle = handle;
//      }
//
//      QueryResult queryResult() const
//          // Return the query result contained in this response message.
//      {
//          return QueryResult();
//      }
//
//      int handle() const
//          // Return the handle contained in this response message.
//      {
//          return d_handle;
//      }
//  };
//
//  void sendMessage(RequestMsg msg, RemoteAddress peer)
//      // Send the specified 'msg' to the specified 'peer'.
//  {
//      serverMutex.lock();
//      peer->push(msg.handle());
//      serverNotEmptyCondition.signal();
//      serverMutex.unlock();
//  }
//
//  void recvMessage(ResponseMsg *msg, RemoteAddress peer)
//      // Get the response from the specified 'peer' into the specified 'msg'.
//  {
//      serverMutex.lock();
//      while (peer->empty()) {
//          serverNotEmptyCondition.wait(&serverMutex);
//      }
//      msg->setHandle(peer->front());
//      peer->pop();
//      serverMutex.unlock();
//  }
//
//  void getQueryAndCallback(Query                            *query,
//                           bsl::function<void(QueryResult)> *callBack)
//      // Set the specified 'query' and 'callBack' to the next 'Query' and its
//      // associated functor (the functor to be called when the response to
//      // this 'Query' comes in).
//  {
//      (void)query;
//      *callBack = &queryCallBack;
//  }
//..
// Furthermore, let also the following variables be declared:
//..
//  RemoteAddress serverAddress;  // address of remote server
//
//  bdlcc::ObjectCatalog<bsl::function<void(QueryResult)> > catalog;
//      // Catalog of query callbacks, used by the client internally to keep
//      // track of callback functions across multiple queries.  The invariant
//      // is that each element corresponds to a pending query (i.e., the
//      // callback function has not yet been or is in the process of being
//      // invoked).
//..
// Now we define functions that will be used in the thread entry functions:
//..
//  void testClientProcessQueryCpp()
//  {
//      int queriesToBeProcessed = NUM_QUERIES_TO_PROCESS;
//      while (queriesToBeProcessed--) {
//          Query query;
//          bsl::function<void(QueryResult)> callBack;
//
//          // The following call blocks until a query becomes available.
//          getQueryAndCallback(&query, &callBack);
//
//          // Register 'callBack' in the object catalog.
//          int handle = catalog.add(callBack);
//          assert(handle);
//
//          // Send query to server in the form of a 'RequestMsg'.
//          RequestMsg msg(query, handle);
//          sendMessage(msg, serverAddress);
//      }
//  }
//
//  void testClientProcessResponseCpp()
//  {
//      int queriesToBeProcessed = NUM_QUERIES_TO_PROCESS;
//      while (queriesToBeProcessed--) {
//          // The following call blocks until some response is available in
//          // the form of a 'ResponseMsg'.
//
//          ResponseMsg msg;
//          recvMessage(&msg, serverAddress);
//          int handle = msg.handle();
//          QueryResult result = msg.queryResult();
//
//          // Process query 'result' by applying registered 'callBack' to it.
//          // The 'callBack' function is retrieved from the 'catalog' using
//          // the given 'handle'.
//
//          bsl::function<void(QueryResult)> callBack;
//          assert(0 == catalog.find(handle, &callBack));
//          callBack(result);
//
//          // Finally, remove the no-longer-needed 'callBack' from the
//          // 'catalog'.  Assert so that 'catalog' may not grow unbounded if
//          // remove fails.
//
//          assert(0 == catalog.remove(handle));
//      }
//  }
//..
// In some thread, the client executes the following code.
//..
//  extern "C" void *testClientProcessQuery(void *)
//  {
//      testClientProcessQueryCpp();
//      return 0;
//  }
//..
// In some other thread, the client executes the following code.
//..
//  extern "C" void *testClientProcessResponse(void *)
//  {
//      testClientProcessResponseCpp();
//      return 0;
//  }
//..
//
///Example 2: Iterator Usage
///- - - - - - - - - - - - -
// The following code fragment shows how to use bdlcc::ObjectCatalogIter to
// iterate through all the objects of 'catalog' (a catalog of objects of type
// 'MyType').
//..
//  void use(bsl::function<void(QueryResult)> object)
//  {
//      (void)object;
//  }
//..
// Now iterate through the 'catalog':
//..
//  for (bdlcc::ObjectCatalogIter<MyType> it(catalog); it; ++it) {
//      bsl::pair<int, MyType> p = it(); // p.first contains the handle and
//                                       // p.second contains the object
//      use(p.second);                   // the function 'use' uses the
//                                       // object in some way
//  }
//  // 'it' is now destroyed out of the scope, releasing the lock.
//..
// Note that the associated catalog is (read)locked when the iterator is
// constructed and is unlocked only when the iterator is destroyed.  This means
// that until the iterator is destroyed, all the threads trying to modify the
// catalog will remain blocked (even though multiple threads can concurrently
// read the object catalog).  So clients must make sure to destroy their
// iterators after they are done using them.  One easy way is to use the
// 'for (bdlcc::ObjectCatalogIter<MyType> it(catalog); ...' as above.

#include <bdlscm_version.h>

#include <bslmt_rwmutex.h>
#include <bslmt_readlockguard.h>
#include <bslmt_writelockguard.h>

#include <bdlma_pool.h>

#include <bslalg_scalarprimitives.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_deprecatefeature.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_objectbuffer.h>
#include <bsls_platform.h>
#include <bsls_review.h>

#include <bsl_utility.h>
#include <bsl_vector.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslalg_typetraits.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#include <vector>

namespace BloombergLP {
namespace bdlcc {

template <class TYPE>
class ObjectCatalog_AutoCleanup;
template <class TYPE>
class ObjectCatalogIter;
template <class TYPE>
class ObjectCatalog;

                   // =====================================
                   // local class ObjectCatalog_AutoCleanup
                   // =====================================

template <class TYPE>
class ObjectCatalog_AutoCleanup {
    // This class provides a specialized proctor object that, upon destruction
    // and unless the 'release' method is called (1) removes a managed node
    // from the 'ObjectCatalog', and (2) deallocates all associated memory as
    // necessary.

    ObjectCatalog<TYPE> *d_catalog_p;       // temporarily managed catalog
    typename ObjectCatalog<TYPE>::Node
                        *d_node_p;          // temporarily managed node
    bool                 d_deallocateFlag;  // how to return the managed node

  private:
    // NOT IMPLEMENTED
    ObjectCatalog_AutoCleanup(const ObjectCatalog_AutoCleanup&)
                                                          BSLS_KEYWORD_DELETED;
    ObjectCatalog_AutoCleanup& operator=(const ObjectCatalog_AutoCleanup&)
                                                          BSLS_KEYWORD_DELETED;

  public:
    // CREATORS
    explicit ObjectCatalog_AutoCleanup(ObjectCatalog<TYPE> *catalog);
        // Create a proctor to manage the specified 'catalog'.

    ~ObjectCatalog_AutoCleanup();
        // Remove a managed node from the 'ObjectCatalog' (by returning it to
        // the catalog's free list or node pool, as specified in 'manageNode'),
        // deallocate all associated memory, and destroy this object.

    // MANIPULATORS
    void manageNode(typename ObjectCatalog<TYPE>::Node *node,
                    bool                                deallocateFlag);
        // Release from management the catalog node, if any, currently managed
        // by this object and begin managing the specified catalog 'node'.  The
        // specified 'deallocateFlag' tells the destructor how to dispose of
        // 'node' if 'node' is managed during the destruction of this object.

    void releaseNode();
        // Release from management the catalog node, if any, currently managed
        // by this object, if any.

    void release();
        // Release from management all resources currently managed by this
        // object, if any.
};

                            // ===================
                            // class ObjectCatalog
                            // ===================

template <class TYPE>
class ObjectCatalog {
    // This class defines an efficient indexed object catalog of 'TYPE'
    // objects.  This container is *exception* *neutral* with no guarantee of
    // rollback: if an exception is thrown during the invocation of a method on
    // a pre-existing instance, the object is left in a valid but undefined
    // state.  In no event is memory leaked or a mutex left in a locked state.

    // PRIVATE TYPES
    typedef ObjectCatalogIter<TYPE> Iter;

    enum {
        // Masks used for breaking up a handle.  Note: a handle (of type int)
        // is always 4 bytes, even on 64 bit modes.

        k_INDEX_MASK      = 0x007fffff,
        k_BUSY_INDICATOR  = 0x00800000,
        k_GENERATION_INC  = 0x01000000,
        k_GENERATION_MASK = 0xff000000
    };

    struct Node {
        // PUBLIC DATA
        typedef union {
            // PUBLIC DATA
            bsls::ObjectBuffer<TYPE>            d_value;

            Node                               *d_next_p; // when free, pointer
                                                          // to next free node
        } Payload;
        Payload d_payload;
        int     d_handle;
    };

    // DATA
    bsl::vector<Node *>     d_nodes;
    bdlma::Pool             d_nodePool;
    Node                   *d_nextFreeNode_p;
    bsls::AtomicInt         d_length;
    mutable bslmt::RWMutex  d_lock;

  private:
    // NOT IMPLEMENTED
    ObjectCatalog(const ObjectCatalog&) BSLS_KEYWORD_DELETED;
    ObjectCatalog& operator=(const ObjectCatalog&) BSLS_KEYWORD_DELETED;

    // FRIENDS
    friend class ObjectCatalog_AutoCleanup<TYPE>;
    friend class ObjectCatalogIter<TYPE>;

  private:
    // PRIVATE CLASS METHODS
    static TYPE *getNodeValue(Node *node);
        // Return a pointer to the 'd_value' field of the specified 'node'.
        // The behavior is undefined unless '0 != node' and
        // 'node->d_payload.d_value' is initialized to a 'TYPE' object.

    // PRIVATE MANIPULATORS
    void freeNode(Node *node);
        // Add the specified 'node' to the free node list.  Destruction of the
        // object held in the node must be handled by the 'remove' function
        // directly.  (This is because 'freeNode' is also used in the
        // 'ObjectCatalog_AutoCleanup' guard, but there it should not invoke
        // the object's destructor.)

    template <class VECTOR>
    void removeAllImp(VECTOR *buffer);
        // Remove all objects that are currently held in this catalog and
        // optionally load into the optionally specified 'buffer' the removed
        // objects.

    // PRIVATE ACCESSORS
    Node *findNode(int handle) const;
        // Return a pointer to the node with the specified 'handle', or 0 if
        // not found.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(ObjectCatalog, bslma::UsesBslmaAllocator);

    // CREATORS
    explicit
    ObjectCatalog(bslma::Allocator *allocator = 0);
        // Create an empty object catalog, using the optionally specified
        // 'allocator' to supply any memory.

    ~ObjectCatalog();
        // Destroy this object catalog.

    // MANIPULATORS
    int add(TYPE const& object);
        // Add the value of the specified 'object' to this catalog and return a
        // non-zero integer handle that may be used to refer to the object in
        // future calls to this catalog.  The behavior is undefined if the
        // catalog was full.

    int add(bslmf::MovableRef<TYPE> object);
        // Add the value of the specified 'object' to this catalog and return a
        // non-zero integer handle that may be used to refer to the object in
        // future calls to this catalog, leaving 'object' in an unspecified but
        // valid state.  The behavior is undefined if the catalog was full.

    int remove(int handle, TYPE *valueBuffer = 0);
        // Optionally load into the optionally specified 'valueBuffer' the
        // value of the object having the specified 'handle' and remove it from
        // this catalog.  Return zero on success, and a non-zero value if the
        // 'handle' is not contained in this catalog.  Note that 'valueBuffer'
        // is assigned into, and thus must point to a valid 'TYPE' instance.

    void removeAll();
    void removeAll(bsl::vector<TYPE>      *buffer);
    void removeAll(std::vector<TYPE>      *buffer);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
    void removeAll(std::pmr::vector<TYPE> *buffer);
#endif
        // Remove all objects that are currently held in this catalog and
        // optionally load into the optionally specified 'buffer' the removed
        // objects.

    int replace(int handle, const TYPE& newObject);
        // Replace the object having the specified 'handle' with the specified
        // 'newObject'.  Return 0 on success, and a non-zero value if the
        // handle is not contained in this catalog.

    int replace(int handle, bslmf::MovableRef<TYPE> newObject);
        // Replace the object having the specified 'handle' with the specified
        // 'newObject', leaving 'newObject' in an unspecified but valid state.
        // Return 0 on success, and a non-zero value if the handle is not
        // contained in this catalog.

    // ACCESSORS
    bslma::Allocator *allocator() const;
        // Return the allocator used by this object.

    int find(int handle) const;
    int find(int handle, TYPE *valueBuffer) const;
        // Locate the object having the specified 'handle' and optionally load
        // its value into the optionally specified 'valueBuffer'.  Return zero
        // on success, and a non-zero value if the 'handle' is not contained in
        // this catalog.  Note that 'valueBuffer' is assigned into, and thus
        // must point to a valid 'TYPE' instance.  Note that the overload with
        // 'valueBuffer' passed is not supported unless 'TYPE' has a copy
        // constructor.

    bool isMember(const TYPE& object) const;
        // Return 'true' if the catalog contains an item that compares equal to
        // the specified 'object' and 'false' otherwise.

    int length() const;
        // Return a "snapshot" of the number of items currently contained in
        // this catalog.

    BSLS_DEPRECATE_FEATURE("bde", "ObjectCataloog::value(handle)",
                                    "use 'ObjectCatalogIter::value()' instead")
    const TYPE& value(int handle) const;
        // Return a 'const' reference to the object having the specified
        // 'handle'.  The behavior is undefined unless 'handle' is contained in
        // this catalog.
        //
        // This method is *DEPRECATED* because it is not thread-safe.  Use
        // 'find', 'isMember', or access the object through an iterator.

    // FOR TESTING PURPOSES ONLY

    void verifyState() const;
        // Verify that this catalog is in a consistent state.  This function is
        // introduced for testing purposes only.
};

                          // =======================
                          // class ObjectCatalogIter
                          // =======================

template <class TYPE>
class ObjectCatalogIter {
    // Provide thread safe iteration through all the objects of an object
    // catalog of parameterized 'TYPE'.  The order of the iteration is
    // implementation defined.  An iterator is *valid* if it is associated with
    // an object in the catalog, otherwise it is *invalid*.  Thread-safe
    // iteration is provided by (read)locking the object catalog during the
    // iterator's construction and unlocking it at the iterator's destruction.
    // This guarantees that during the life time of an iterator, the object
    // catalog can't be modified (nevertheless, multiple threads can
    // concurrently read the object catalog).

    const ObjectCatalog<TYPE> *d_catalog_p;
    bsl::ptrdiff_t             d_index;

  private:
    // NOT IMPLEMENTED
    ObjectCatalogIter(const ObjectCatalogIter&) BSLS_KEYWORD_DELETED;
    ObjectCatalogIter& operator=(const ObjectCatalogIter&)
                                                          BSLS_KEYWORD_DELETED;
    bool operator==(const ObjectCatalogIter&) const BSLS_KEYWORD_DELETED;
    bool operator!=(const ObjectCatalogIter&) const BSLS_KEYWORD_DELETED;
    template <class BDE_OTHER_TYPE>
    bool operator==(
          const ObjectCatalogIter<BDE_OTHER_TYPE>&) const BSLS_KEYWORD_DELETED;
    template <class BDE_OTHER_TYPE>
    bool operator!=(
          const ObjectCatalogIter<BDE_OTHER_TYPE>&) const BSLS_KEYWORD_DELETED;

  public:
    // CREATORS
    explicit ObjectCatalogIter(const ObjectCatalog<TYPE>& catalog);
        // Create an iterator for the specified 'catalog' and associate it with
        // the first member of the 'catalog'.  If the 'catalog' is empty then
        // the iterator is initialized to be invalid.  The 'catalog' is locked
        // for read for the duration of iterator's life.

    ObjectCatalogIter(const ObjectCatalog<TYPE>& catalog, int handle);
        // Create an iterator for the specified 'catalog' and associate it with
        // the member of the 'catalog' associated with the specified 'handle'.
        // If the 'catalog' is empty or if 'handle' is invalid, then the
        // iterator is initialized to be invalid.  The 'catalog' is locked for
        // read for the duration of iterator's life.

    ~ObjectCatalogIter();
        // Destroy this iterator and unlock the catalog associated with it.

    // MANIPULATORS
    void operator++();
        // Advance this iterator to refer to the next object of the associated
        // catalog; if there is no next object in the associated catalog, then
        // this iterator becomes *invalid*.  The behavior is undefined unless
        // this iterator is valid.  Note that the order of the iteration is not
        // specified.

    // ACCESSORS
    operator const void *() const;
        // Return non-zero if the iterator is *valid*, and 0 otherwise.

    bsl::pair<int, TYPE> operator()() const;
        // Return a pair containing the handle (as the first element of the
        // pair) and the object (as the second element of the pair) associated
        // with this iterator.  The behavior is undefined unless the iterator
        // is *valid*.

    int handle() const;
        // Return the handle referred to by the iterator.  The behavior is
        // undefined unless the iterator is *valid*.

    const TYPE& value() const;
        // Return a 'const' reference to the value referred to by the iterator.
        // The behavior is undefined unless the iterator is *valid*.
};

// ----------------------------------------------------------------------------
//                            INLINE DEFINITIONS
// ----------------------------------------------------------------------------

                   // -------------------------------------
                   // local class ObjectCatalog_AutoCleanup
                   // -------------------------------------

// CREATORS
template <class TYPE>
ObjectCatalog_AutoCleanup<TYPE>::ObjectCatalog_AutoCleanup(
                                                  ObjectCatalog<TYPE> *catalog)
: d_catalog_p(catalog)
, d_node_p(0)
, d_deallocateFlag(false)
{
}

template <class TYPE>
ObjectCatalog_AutoCleanup<TYPE>::~ObjectCatalog_AutoCleanup()
{
    if (d_catalog_p && d_node_p) {
        if (d_deallocateFlag) {
            // Return node to the pool.

            d_catalog_p->d_nodePool.deallocate(d_node_p);
        } else {
            // Return node to the catalog's free list.

            d_catalog_p->freeNode(d_node_p);
        }
    }
}

// MANIPULATORS
template <class TYPE>
void ObjectCatalog_AutoCleanup<TYPE>::manageNode(
                            typename ObjectCatalog<TYPE>::Node *node,
                            bool                                deallocateFlag)
{
    d_node_p = node;
    d_deallocateFlag = deallocateFlag;
}

template <class TYPE>
void ObjectCatalog_AutoCleanup<TYPE>::releaseNode()
{
    d_node_p = 0;
}

template <class TYPE>
void ObjectCatalog_AutoCleanup<TYPE>::release()
{
    d_catalog_p = 0;
    d_node_p = 0;
}

                            // -------------------
                            // class ObjectCatalog
                            // -------------------

// PRIVATE CLASS METHODS
template <class TYPE>
inline
TYPE *ObjectCatalog<TYPE>::getNodeValue(
                                      typename ObjectCatalog<TYPE>::Node *node)
{
    BSLS_ASSERT(node);

    return node->d_payload.d_value.address();
}

// PRIVATE MANIPULATORS
template <class TYPE>
inline
void ObjectCatalog<TYPE>::freeNode(typename ObjectCatalog<TYPE>::Node *node)
{
    BSLS_ASSERT(node->d_handle & k_BUSY_INDICATOR);

    node->d_handle += k_GENERATION_INC;
    node->d_handle &= ~k_BUSY_INDICATOR;

    node->d_payload.d_next_p = d_nextFreeNode_p;
    d_nextFreeNode_p = node;
}


template <class TYPE>
template <class VECTOR>
void ObjectCatalog<TYPE>::removeAllImp(VECTOR *buffer)
{
    static const bool isVector =
                            bsl::is_same<bsl::vector<TYPE>, VECTOR>::value
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                         || bsl::is_same<std::pmr::vector<TYPE>, VECTOR>::value
#endif
                         || bsl::is_same<std::vector<TYPE>, VECTOR>::value;
    BSLMF_ASSERT(isVector);

    typedef typename bsl::vector<Node *>::iterator VIt;

    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_lock);

    for (VIt it = d_nodes.begin(); it != d_nodes.end(); ++it) {
        if ((*it)->d_handle & k_BUSY_INDICATOR) {
            TYPE *value = getNodeValue(*it);

            if (buffer) {
                buffer->push_back(bslmf::MovableRefUtil::move(*value));
            }
            value->~TYPE();
        }
    }

    // Even though we get rid of the container of 'Node *' without returning
    // the nodes to the pool prior, the release of the pool immediately after
    // will properly (and efficiently) dispose of those nodes without leaking
    // memory.

    d_nodes.clear();
    d_nodePool.release();
    d_nextFreeNode_p = 0;
    d_length = 0;
}

// PRIVATE ACCESSORS
template <class TYPE>
inline
typename ObjectCatalog<TYPE>::Node *
ObjectCatalog<TYPE>::findNode(int handle) const
{
    int index = handle & k_INDEX_MASK;
    // if (d_nodes.size() < index || !(handle & k_BUSY_INDICATOR)) return 0;

    if (0 > index                    ||
        index >= (int)d_nodes.size() ||
        !(handle & k_BUSY_INDICATOR)) {
        return 0;                                                     // RETURN
    }

    Node *node = d_nodes[index];

    return node->d_handle == handle ? node : 0;
}

// CREATORS
template <class TYPE>
inline
ObjectCatalog<TYPE>::ObjectCatalog(bslma::Allocator *allocator)
: d_nodes(allocator)
, d_nodePool(sizeof(Node), allocator)
, d_nextFreeNode_p(0)
, d_length(0)
{
}

template <class TYPE>
inline
ObjectCatalog<TYPE>::~ObjectCatalog()
{
    removeAll();
}

// MANIPULATORS
template <class TYPE>
int ObjectCatalog<TYPE>::add(const TYPE& object)
{
    int handle;
    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_lock);
    ObjectCatalog_AutoCleanup<TYPE> proctor(this);
    Node *node;

    if (d_nextFreeNode_p) {
        node = d_nextFreeNode_p;
        d_nextFreeNode_p = node->d_payload.d_next_p;

        proctor.manageNode(node, false);
        // Destruction of this proctor will put node back onto the free list.
    } else {
        // If 'd_nodes' grows as big as the flags used to indicate BUSY and
        // generations, then the handle will be all mixed up!

        BSLS_REVIEW_OPT(d_nodes.size() < k_BUSY_INDICATOR);

        node = static_cast<Node *>(d_nodePool.allocate());
        proctor.manageNode(node, true);
        // Destruction of this proctor will deallocate node.

        d_nodes.push_back(node);
        node->d_handle = static_cast<int>(d_nodes.size()) - 1;
        proctor.manageNode(node, false);
        // Destruction of this proctor will put node back onto the free list,
        // which is now OK since the 'push_back' succeeded without throwing.
    }

    node->d_handle |= k_BUSY_INDICATOR;
    handle = node->d_handle;

    // We need to use the copyConstruct logic to pass the allocator through.

    bslalg::ScalarPrimitives::copyConstruct(
              getNodeValue(node), object, d_nodes.get_allocator().mechanism());

    // If the copy constructor throws, the proctor will properly put the node
    // back onto the free list.  Otherwise, the proctor should do nothing.

    proctor.release();

    ++d_length;
    return handle;
}

template <class TYPE>
int ObjectCatalog<TYPE>::add(bslmf::MovableRef<TYPE> object)
{
    TYPE& local = object;

    int handle;
    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_lock);
    ObjectCatalog_AutoCleanup<TYPE> proctor(this);
    Node *node;

    if (d_nextFreeNode_p) {
        node = d_nextFreeNode_p;
        d_nextFreeNode_p = node->d_payload.d_next_p;

        proctor.manageNode(node, false);
        // Destruction of this proctor will put node back onto the free list.
    } else {
        // If 'd_nodes' grows as big as the flags used to indicate BUSY and
        // generations, then the handle will be all mixed up!

        BSLS_REVIEW_OPT(d_nodes.size() < k_BUSY_INDICATOR);

        node = static_cast<Node *>(d_nodePool.allocate());
        proctor.manageNode(node, true);
        // Destruction of this proctor will deallocate node.

        d_nodes.push_back(node);
        node->d_handle = static_cast<int>(d_nodes.size()) - 1;
        proctor.manageNode(node, false);
        // Destruction of this proctor will put node back onto the free list,
        // which is now OK since the 'push_back' succeeded without throwing.
    }

    node->d_handle |= k_BUSY_INDICATOR;
    handle = node->d_handle;

    // We need to use the moveConstruct logic to pass the allocator through.

    bslalg::ScalarPrimitives::moveConstruct(
               getNodeValue(node), local, d_nodes.get_allocator().mechanism());

    // If the copy constructor throws, the proctor will properly put the node
    // back onto the free list.  Otherwise, the proctor should do nothing.

    proctor.release();

    ++d_length;
    return handle;
}

template <class TYPE>
inline
int ObjectCatalog<TYPE>::remove(int handle, TYPE *valueBuffer)
{
    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_lock);

    Node *node = findNode(handle);

    if (!node) {
        return -1;                                                    // RETURN
    }

    TYPE *value = getNodeValue(node);

    if (valueBuffer) {
        *valueBuffer = bslmf::MovableRefUtil::move(*value);
    }

    value->~TYPE();
    freeNode(node);

    --d_length;
    return 0;
}

template <class TYPE>
inline
void ObjectCatalog<TYPE>::removeAll()
{
    removeAllImp(static_cast<bsl::vector<TYPE> *>(0));
}

template <class TYPE>
inline
void ObjectCatalog<TYPE>::removeAll(bsl::vector<TYPE> *buffer)
{
    removeAllImp(buffer);
}

template <class TYPE>
inline
void ObjectCatalog<TYPE>::removeAll(std::vector<TYPE> *buffer)
{
    removeAllImp(buffer);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
template <class TYPE>
inline
void ObjectCatalog<TYPE>::removeAll(std::pmr::vector<TYPE> *buffer)
{
    removeAllImp(buffer);
}
#endif

template <class TYPE>
int ObjectCatalog<TYPE>::replace(int handle, const TYPE& newObject)
{
    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_lock);

    Node *node = findNode(handle);

    if (!node) {
        return -1;                                                    // RETURN
    }

    TYPE *value = getNodeValue(node);

    value->~TYPE();

    // We need to use the copyConstruct logic to pass the allocator through.

    bslalg::ScalarPrimitives::copyConstruct(
                        value, newObject, d_nodes.get_allocator().mechanism());

    return 0;
}

template <class TYPE>
int ObjectCatalog<TYPE>::replace(int handle, bslmf::MovableRef<TYPE> newObject)
{
    TYPE& local = newObject;

    bslmt::WriteLockGuard<bslmt::RWMutex> guard(&d_lock);

    Node *node = findNode(handle);

    if (!node) {
        return -1;                                                    // RETURN
    }

    TYPE *value = getNodeValue(node);

    value->~TYPE();

    // We need to use the moveConstruct logic to pass the allocator through.

    bslalg::ScalarPrimitives::moveConstruct(
                            value, local, d_nodes.get_allocator().mechanism());

    return 0;
}

// ACCESSORS
template <class TYPE>
inline
bslma::Allocator *ObjectCatalog<TYPE>::allocator() const
{
    return d_nodePool.allocator();
}

template <class TYPE>
inline
int ObjectCatalog<TYPE>::find(int handle) const
{
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_lock);

    return 0 == findNode(handle) ? -1 : 0;
}

template <class TYPE>
inline
int ObjectCatalog<TYPE>::find(int handle, TYPE *valueBuffer) const
{
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_lock);

    Node *node = findNode(handle);

    if (!node) {
        return -1;                                                    // RETURN
    }

    *valueBuffer = *getNodeValue(node);

    return 0;
}

template <class TYPE>
bool ObjectCatalog<TYPE>::isMember(const TYPE& object) const
{
    for (Iter it(*this); it; ++it) {
        if (it.value() == object) {
            return true;                                              // RETURN
        }
    }

    return false;
}

template <class TYPE>
inline
int ObjectCatalog<TYPE>::length() const
{
    return d_length;
}

template <class TYPE>
inline
const TYPE& ObjectCatalog<TYPE>::value(int handle) const
{
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_lock);

    Node *node = findNode(handle);

    BSLS_ASSERT(node);

    return *getNodeValue(node);
}

template <class TYPE>
void ObjectCatalog<TYPE>::verifyState() const
{
    bslmt::ReadLockGuard<bslmt::RWMutex> guard(&d_lock);

    BSLS_ASSERT(             0 <= d_length);
    BSLS_ASSERT(d_nodes.size() >= static_cast<unsigned>(d_length));

    unsigned numBusy = 0, numFree = 0;
    for (unsigned ii = 0; ii < d_nodes.size(); ++ii) {
        const int handle = d_nodes[ii]->d_handle;
        BSLS_ASSERT((handle & k_INDEX_MASK) == ii);
        handle & k_BUSY_INDICATOR ? ++numBusy
                                  : ++numFree;
    }
    BSLS_ASSERT(          numBusy == static_cast<unsigned>(d_length));
    BSLS_ASSERT(numFree + numBusy == d_nodes.size());

    for (const Node *p = d_nextFreeNode_p; p; p = p->d_payload.d_next_p) {
        BSLS_ASSERT(!(p->d_handle & k_BUSY_INDICATOR));
        --numFree;
    }
    BSLS_ASSERT(0 == numFree);
}

                            // -----------------
                            // ObjectCatalogIter
                            // -----------------

// CREATORS
template <class TYPE>
inline
ObjectCatalogIter<TYPE>::ObjectCatalogIter(const ObjectCatalog<TYPE>& catalog)
: d_catalog_p(&catalog)
, d_index(-1)
{
    d_catalog_p->d_lock.lockRead();
    operator++();
}

template <class TYPE>
inline
ObjectCatalogIter<TYPE>::ObjectCatalogIter(const ObjectCatalog<TYPE>& catalog,
                                           int                        handle)
: d_catalog_p(&catalog)
{
    typedef ObjectCatalog<TYPE>     Catalog;
    typedef typename Catalog::Node  Node;

    d_catalog_p->d_lock.lockRead();

    Node *node = d_catalog_p->findNode(handle);
    d_index = node ? node->d_handle & Catalog::k_INDEX_MASK
                   : bsl::ssize(d_catalog_p->d_nodes);
}

template <class TYPE>
inline
ObjectCatalogIter<TYPE>::~ObjectCatalogIter()
{
    d_catalog_p->d_lock.unlock();
}

// MANIPULATORS
template <class TYPE>
void ObjectCatalogIter<TYPE>::operator++()
{
    ++d_index;
    while ((unsigned)d_index < d_catalog_p->d_nodes.size() &&
          !(d_catalog_p->d_nodes[d_index]->d_handle &
              ObjectCatalog<TYPE>::k_BUSY_INDICATOR)) {
        ++d_index;
    }
}

template <class TYPE>
inline
int ObjectCatalogIter<TYPE>::handle() const
{
    BSLS_ASSERT(static_cast<unsigned>(d_index) < d_catalog_p->d_nodes.size());

    return d_catalog_p->d_nodes[d_index]->d_handle;
}

template <class TYPE>
inline
const TYPE& ObjectCatalogIter<TYPE>::value() const
{
    BSLS_ASSERT(static_cast<unsigned>(d_index) < d_catalog_p->d_nodes.size());

    return *ObjectCatalog<TYPE>::getNodeValue(d_catalog_p->d_nodes[d_index]);
}

}  // close package namespace

// ACCESSORS
template <class TYPE>
inline
bdlcc::ObjectCatalogIter<TYPE>::operator const void *() const
{
    return static_cast<unsigned>(d_index) < d_catalog_p->d_nodes.size()
         ? this
         : 0;
}

namespace bdlcc {

template <class TYPE>
inline
bsl::pair<int, TYPE> ObjectCatalogIter<TYPE>::operator()() const
{
    typedef ObjectCatalog<TYPE> Catalog;

    typename Catalog::Node *node = d_catalog_p->d_nodes[d_index];

    return bsl::pair<int, TYPE>(node->d_handle, *Catalog::getNodeValue(node));
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
