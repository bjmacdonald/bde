// bslstl_priorityqueue.h                                             -*-C++-*-
#ifndef INCLUDED_BSLSTL_PRIORITYQUEUE
#define INCLUDED_BSLSTL_PRIORITYQUEUE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")


//@PURPOSE: Provide container adapter class template 'priority_queue'.
//
//@CLASSES:
//   bslstl::priority_queue: template of highest-priority-first data structure
//
//@SEE_ALSO: bslstl_queue, bslstl_stack
//
//@AUTHOR: Shijin Kong (skong25)
//
//@DESCRIPTION: This component defines a class template, 'bsl::priority_queue',
// holding a container (of a parameterized type 'CONTAINER' containing elements
// of another parameterized type 'VALUE'), and adapting it to provide
// highest-priority-first priority queue data structure.  The component takes a
// third parameterized type 'COMPARATOR' for customized priorities comparison
// between two elements.
//
// An instantiation of 'priority_queue' is an allocator-aware, value-semantic
// type whose salient attributes are its size (number of held elements) and the
// sorted sequence of values (of held elements).  If 'priority_queue' is
// instantiated with a parameterized type 'VALUE' that is not itself
// value-semantic, then it will not retain all of its value-semantic qualities.
// A 'priority_queue' cannot be tested for equality, but its parameterized type
// 'VALUE' must be able to be tested for comparing less by its parameterized
// type 'COMPARATOR'.
//
// 'priority_queue' meets the requirements of a container adapter in the C++
// standard [23.6].  The 'priority_queue' implemented here adheres to the C++11
// standard, except that it does not have methods that take rvalue references
// and 'initializer_lists'.  Note that excluded C++11 features are those that
// require C++11 compiler support.
//
///Memory Allocation
///-----------------
// The type supplied as 'ALLOCATOR' template parameter in some of
// 'priority_queue' constructors determines how the held container (of
// parameterized 'CONTAINER') will allocate memory.  A 'priority_queue'
// supports allocators meeting the requirements of the C++11 standard
// [17.6.3.5] as long as the held container does.  In addition it supports
// scoped-allocators derived from the 'bslma_Allocator' memory allocation
// protocol.  Clients intending to use 'bslma' style allocators should use
// 'bsl::allocator' as the 'ALLOCATOR' template parameter,  providing a C++11
// standard-compatible adapter for a 'bslma_Allocator' object.
//
///Operations
///----------
// The C++11 standard [23.6.4] declares any container type supporting
// operations 'front', 'push_back', and 'pop_back' can be used to instantiate
// the parameterized type 'CONTAINER'.  Below is a list of public methods of
// 'priority_queue' class that are effectively implementationed based on
// corresponding operations in the held container (referenced as 'c').
//  +--------------------------------------+---------------------------+
//  | Public methods in 'priority_queue'   | Operation in 'CONTAINER'  |
//  +======================================+===========================+
//  | void push(const value_type& value);  | c.push_back(value);       |
//  | void pop();                          | c.pop_back();             |
//  +--------------------------------------+---------------------------+
//  | bool empty() const;                  | c.empty();                |
//  | size_type size() const;              | c.size();                 |
//  | const_reference top() const;         | c.front();                |
//  +--------------------------------------+---------------------------+
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Pushing and Popping Tasks from a Priority Queue
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example, we will define an element class 'Task', push a group of
// 'Task' objects into a priority queue, and then pop them out according to
// their priorities.  The parameterized type 'VALUE' is 'Task' in this example;
// the parameterized type 'CONTAINER' to be adapted is 'bsl::vector<Task>'; the
// parameterized type 'COMPARATOR' is a user-defined functor 'TaskComparator'.
//
// First, we define a 'Task' class:
//..
//  class Task
//  {
//    private:
//      // DATA
//      int d_priority;  // priority of the task
//
//    public:
//      // CREATORS
//      Task(int priority);
//         // Construct a 'Task' object having the specified 'priority'.
//
//      // ACCESSORS
//      int priority() const;
//  };
//
//  // CREATORS
//  Task::Task(int priority)
//  : d_priority(priority)
//  {
//  }
//
//  // ACCESSORS
//  int Task::priority() const
//  {
//      return d_priority;
//  }
//..
// Then, we define a comparison functor for 'Task' objects:
//..
//  struct TaskComparator {
//      // This 'struct' defines an ordering on 'Task' objects,
//      // allowing them to be included in sorted data structures such as
//      // 'bsl::priority_queue'.
//
//      bool operator()(const Task& lhs, const Task& rhs) const
//          // Return 'true' if the priority of the specified task 'lhs' is
//          // less than the priority of the specified 'rhs', and 'false'
//          // otherwise.  Notice that the smaller the value returned by the
//          // 'Task::priority' method, the higher the priority.
//      {
//          return lhs.priority() > rhs.priority();
//      }
//  };
//..
// Notice that we can not use the default 'std::less' as comparator here due to
// two reasons: the 'Task' class does not explicitly define how two 'Task'
// objects are compared; and the smaller the value returned by
// 'Task::priority()', the higher the priority the task has.
//
// Next, we create a 'bsl::priority_queue' object via its default constructor
// to adapt the 'bsl::vector<Task>' type, setting the parameterized
// 'COMPARATOR' to 'TaskComparator':
//..
//  bsl::priority_queue<Task, vector<Task>, TaskComparator> taskPrQueue;
//..
// Now, we push a series of 'Task' objects having different priorities
// into the priority queue:
//..
//  taskPrQueue.push(Task(         1));
//  taskPrQueue.push(Task(     65535));
//  taskPrQueue.push(Task(       530));
//  taskPrQueue.push(Task(      -200));
//  taskPrQueue.push(Task(         0));
//  taskPrQueue.push(Task(   INT_MAX));
//  taskPrQueue.push(Task(     10005));
//  taskPrQueue.push(Task(      1366));
//  taskPrQueue.push(Task( 999999999));
//  taskPrQueue.push(Task(   INT_MIN));
//  taskPrQueue.push(Task(-123456789));
//..
// Notice that every time a new 'Task' element is pushed in, the priority queue
// adjusts its internal data structure to ensure the 'top' method always
// returns the highest priority (the smallest integer value) of elements held
// in the priority queue.
//
// Finally, using a 'for' loop, we pop tasks from the priority queue one by
// one:
//..
//  for (int i = 0; !taskPrQueue.empty(); ++i) {
//      printf("    %d: %d\n", i, taskPrQueue.top().priority());
//      taskPrQueue.pop();
//  }
//  assert(taskPrQueue.empty());
//..
// Notice that every time a 'Task' element is popped out, the priority queue
// adjusts its internal data structure to ensure the 'top' method always
// returns the highest priority (the smallest integer value) of remaining
// elements in the priority queue.
//
// The tasks are popped out on a highest-priority-first basis.  The screen
// output will be as follows:
//
//    0: -2147483648
//    1: -123456789
//    2: -200
//    3: 0
//    4: 1
//    5: 530
//    6: 1366
//    7: 10005
//    8: 65535
//    9: 999999999
//    10: 2147483647

#ifndef INCLUDED_BSLSTL_ALLOCATOR
#include <bslstl_allocator.h>
#endif

#ifndef INCLUDED_BSLSTL_VECTOR
#include <bslstl_vector.h>
#endif

#ifndef INCLUDED_BSLALG_SWAPUTIL
#include <bslalg_swaputil.h>
#endif

#ifndef INCLUDED_BSLMF_ENABLEIF
#include <bslmf_enableif.h>
#endif

#ifndef INCLUDED_BSLMF_ISCONVERTIBLE
#include <bslmf_isconvertible.h>
#endif

#ifndef INCLUDED_FUNCTIONAL
#include <functional>
#define INCLUDED_FUNCTIONAL
#endif

namespace bsl {

                         // ====================
                         // class priority_queue
                         // ====================

template <class VALUE,
          class CONTAINER  = vector<VALUE>,
          class COMPARATOR = native_std::less<typename CONTAINER::value_type> >
class priority_queue
    // This class is a value-semantic class template, adapting a container of
    // the parameterized 'CONTAINER' type that holds elements of the
    // parameterized 'VALUE' type, to provides a highest-priority-first
    // priority queue data structure, where the priorities of elements are
    // compared by a comparator of the parameterized 'COMPARATOR' type.  The
    // container object held by a 'priority_queue' class object is referenced
    // as 'c' in the following documentation.
{
  protected:
    // DATA
    CONTAINER c;        // container for elements in the 'priority_queue',
                        // protected as required by the C++11 Standard
    COMPARATOR comp;    // comparator that defines the priority order of
                        // elements in the 'priority_queue'

  public:
    // PUBLIC TYPES
    typedef typename CONTAINER::value_type      value_type;
    typedef typename CONTAINER::reference       reference;
    typedef typename CONTAINER::const_reference const_reference;
    typedef typename CONTAINER::size_type       size_type;
    typedef          CONTAINER                  container_type;

    // CREATORS
    explicit priority_queue();
        // Construct an empty priority, adapting a default-constructed
        // container of the parameterized 'CONTAINER' type.  Use a
        // default-constructed comparator of the parameterized 'COMPARATOR'
        // type to order priorities of elements.

    priority_queue(const COMPARATOR& comparator,
                   const CONTAINER&  container);
        // Construct a priority queue, adapting the specified 'container',
        // having the specified 'comparator' to order priorities of elements
        // held in 'container'.

    template <class INPUT_ITERATOR>
    priority_queue(INPUT_ITERATOR    first,
                   INPUT_ITERATOR    last);
        // Construct an empty priority queue, adapting a default-constructed
        // container of the parameterized 'CONTAINER' type, and insert into the
        // container a sequence of 'value_type' elements starting at the
        // specified 'first', and ending immediately before the specified
        // 'last'.  Use a default-constructed comparator of the parameterized
        // 'COMPARATOR' type to order the priorities of elements.

    template <class INPUT_ITERATOR>
    priority_queue(INPUT_ITERATOR    first,
                   INPUT_ITERATOR    last,
                   const COMPARATOR& comparator,
                   const CONTAINER&  container);
        // Construct a priority queue, adapting the specified 'container',
        // having the specified 'comparator' to order the priorities of
        // elements, including those originally existed in 'container', and
        // those inserted into the 'container' from a sequence of 'value_type'
        // elements starting at the specified 'first', and ending immediately
        // before the specified 'last'.

    priority_queue(const priority_queue& original);
        // Construct a priority queue having the same value as the specified
        // 'original'.  Use the comparator from 'original' to order the
        // priorities of elements.

    template <class ALLOCATOR>
    explicit priority_queue(const ALLOCATOR& allocator,
                            typename BloombergLP::bslmf_EnableIf<
                                BloombergLP::bslmf_IsConvertible<
                                    ALLOCATOR,
                                    typename CONTAINER::allocator_type>::VALUE>
                                ::type * = 0);
        // Construct an empty priority queue that adapts a default-constructed
        // container of the parameterized 'CONTAINER' type and will use the
        // specified 'allocator' to supply memory.  Use a default-constructed
        // comparator of the parameterized 'COMPARATOR' type to order the
        // priorities of elements.  Note that the 'ALLOCATOR' parameter type
        // has to be convertible to the allocator of the 'CONTAINER' parameter
        // type, 'CONTAINER::allocator_type'.  Otherwise this constructor is
        // disabled.

    template <class ALLOCATOR>
    priority_queue(const COMPARATOR& comparator,
                   const ALLOCATOR& allocator,
                   typename BloombergLP::bslmf_EnableIf<
                       BloombergLP::bslmf_IsConvertible<
                                    ALLOCATOR,
                                    typename CONTAINER::allocator_type>::VALUE>
                       ::type * = 0);
        // Construct an empty priority queue that adapts a default-constructed
        // container of the parameterized 'CONTAINER'type and will use the
        // specified 'allocator' to supply memory, and the specified
        // 'comparator' to order priorities of elements in a
        // default-constructed container of the parameterized 'CONTAINER' type.
        // Note that the 'ALLOCATOR' parameter type has to be convertible to
        // the allocator of the 'CONTAINER' parameter type,
        // 'CONTAINER::allocator_type'.  Otherwise this constructor is
        // disabled.

    template <class ALLOCATOR>
    priority_queue(const COMPARATOR& comparator,
                   const CONTAINER&  container,
                   const ALLOCATOR&  allocator,
                   typename BloombergLP::bslmf_EnableIf<
                       BloombergLP::bslmf_IsConvertible<
                                    ALLOCATOR,
                                    typename CONTAINER::allocator_type>::VALUE>
                   ::type * = 0);
        // Construct a priority queue that will use the specified 'allocator'
        // to supply memory, and the specified 'comparator' to order priorities
        // of elements in the specified 'container'.  Note that the 'ALLOCATOR'
        // parameter type has to be convertible to the allocator of the
        // 'CONTAINER' parameter type, 'CONTAINER::allocator_type'.  Otherwise
        // this constructor is disabled.

    template <class ALLOCATOR>
    priority_queue(const priority_queue& original,
                   const ALLOCATOR& allocator,
                   typename BloombergLP::bslmf_EnableIf<
                       BloombergLP::bslmf_IsConvertible<
                                    ALLOCATOR,
                                    typename CONTAINER::allocator_type>::VALUE>
                   ::type * = 0);
        // Construct a priority queue having the same value as the specified
        // 'original' that will use the specified 'allocator' to supply memory.
        // Use the comparator from 'original' to order the priorities of
        // elements.  Note that the 'ALLOCATOR' parameter type has to be
        // convertible to the allocator of the 'CONTAINER' parameter type,
        // 'CONTAINER::allocator_type'.  Otherwise this constructor is
        // disabled.

    // MANIPULATORS
    void push(const value_type& value);
        // Insert a new element having the specified priority 'value' into this
        // 'priority_queue' object.  In effect, performs 'c.push_back(value);'.

    void pop();
        // Remove the top element from this 'priority_queue' object that has
        // the highest priority.  In effect, performs 'c.pop();'.  The behavior
        // is undefined if there is currently no elements in this object.

    void swap(priority_queue& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  In effect, performs
        // 'using bsl::swap; swap(c, other.c);'.

    // ACCESSORS
    bool empty() const;
        // Return 'true' if this 'priority_queue' object contains no elements,
        // and 'false' otherwise.  In effect, performs 'return c.empty();'.

    size_type size() const;
        // Return the number of elements in this 'priority_queue' object.  In
        // effect, performs 'return c.size()'.

    const_reference top() const;
        // Return the immutable element having the highest priority in this
        // 'priority_queue' object.  In effect, performs 'return c.front()'.
        // The behavior is undefined if there is currently no elements in this
        // object.
};

// FREE FUNCTIONS

template <class VALUE, class CONTAINER, class COMPARATOR>
void swap(priority_queue<VALUE, CONTAINER, COMPARATOR>& lhs,
          priority_queue<VALUE, CONTAINER, COMPARATOR>& rhs);

// ===========================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ===========================================================================

                         // --------------------
                         // class priority_queue
                         // --------------------

// CREATORS
template <class VALUE, class CONTAINER, class COMPARATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue()
{
}

template <class VALUE, class CONTAINER, class COMPARATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                                                  const COMPARATOR& comparator,
                                                  const CONTAINER&  container)
: c(container)
, comp(comparator)
{
    native_std::make_heap(c.begin(), c.end(), comp);
}

template <class VALUE, class CONTAINER, class COMPARATOR>
template <class INPUT_ITERATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                                                       INPUT_ITERATOR    first,
                                                       INPUT_ITERATOR    last)
{
    c.insert(c.end(), first, last);
    native_std::make_heap(c.begin(), c.end(), comp);
}

template <class VALUE, class CONTAINER, class COMPARATOR>
template <class INPUT_ITERATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                                                  INPUT_ITERATOR    first,
                                                  INPUT_ITERATOR    last,
                                                  const COMPARATOR& comparator,
                                                  const CONTAINER&  container)
: c(container)
, comp(comparator)
{
    c.insert(c.end(), first, last);
    native_std::make_heap(c.begin(), c.end(), comp);
}

template <class VALUE, class CONTAINER, class COMPARATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                                                const priority_queue& original)
: c(original.c)
, comp(original.comp)
{
}

template <class VALUE, class CONTAINER, class COMPARATOR>
template <class ALLOCATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                                    const ALLOCATOR& allocator,
                                    typename BloombergLP::bslmf_EnableIf<
                                        BloombergLP::bslmf_IsConvertible<
                                            ALLOCATOR,
                                            typename CONTAINER::allocator_type>
                                            ::VALUE>
                                        ::type *)
: c(allocator)
, comp(COMPARATOR())
{
}

template <class VALUE, class CONTAINER, class COMPARATOR>
template <class ALLOCATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                                    const COMPARATOR& comparator,
                                    const ALLOCATOR&  allocator,
                                    typename BloombergLP::bslmf_EnableIf<
                                        BloombergLP::bslmf_IsConvertible<
                                            ALLOCATOR,
                                            typename CONTAINER::allocator_type>
                                            ::VALUE>
                                        ::type *)
: c(allocator)
, comp(comparator)
{
}

template <class VALUE, class CONTAINER, class COMPARATOR>
template <class ALLOCATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                                    const COMPARATOR& comparator,
                                    const CONTAINER&  container,
                                    const ALLOCATOR&  allocator,
                                    typename BloombergLP::bslmf_EnableIf<
                                        BloombergLP::bslmf_IsConvertible<
                                            ALLOCATOR,
                                            typename CONTAINER::allocator_type>
                                            ::VALUE>
                                        ::type *)
: c(container, allocator)
, comp(comparator)
{
    native_std::make_heap(c.begin(), c.end(), comp);
}


template <class VALUE, class CONTAINER, class COMPARATOR>
template <class ALLOCATOR>
inline
priority_queue<VALUE, CONTAINER, COMPARATOR>::priority_queue(
                                    const priority_queue& original,
                                    const ALLOCATOR&      allocator,
                                    typename BloombergLP::bslmf_EnableIf<
                                        BloombergLP::bslmf_IsConvertible<
                                            ALLOCATOR,
                                            typename CONTAINER::allocator_type>
                                            ::VALUE>
                                        ::type *)
: c(original.c, allocator)
, comp(original.comp)
{
}


// MANIPULATORS
template <class VALUE, class CONTAINER, class COMPARATOR>
inline
void priority_queue<VALUE, CONTAINER, COMPARATOR>::push(
                                                       const value_type& value)
{
    c.push_back(value);
    native_std::push_heap(c.begin(), c.end(), comp);
}

template <class VALUE, class CONTAINER, class COMPARATOR>
inline
void priority_queue<VALUE, CONTAINER, COMPARATOR>::pop()
{
    native_std::pop_heap(c.begin(), c.end(), comp);
    c.pop_back();
}

template <class VALUE, class CONTAINER, class COMPARATOR>
inline
void priority_queue<VALUE, CONTAINER, COMPARATOR>::swap(priority_queue& other)
{
    BloombergLP::bslalg_SwapUtil::swap(&c   , &other.c   );
    BloombergLP::bslalg_SwapUtil::swap(&comp, &other.comp);
}

// ACCESSORS
template <class VALUE, class CONTAINER, class COMPARATOR>
inline
bool priority_queue<VALUE, CONTAINER, COMPARATOR>::empty() const
{
    return c.empty();
}

template <class VALUE, class CONTAINER, class COMPARATOR>
inline
typename priority_queue<VALUE, CONTAINER, COMPARATOR>::size_type
priority_queue<VALUE, CONTAINER, COMPARATOR>::size() const
{
    return c.size();
}

template <class VALUE, class CONTAINER, class COMPARATOR>
inline
typename priority_queue<VALUE, CONTAINER, COMPARATOR>::const_reference
priority_queue<VALUE, CONTAINER, COMPARATOR>::top() const
{
    return c.front();
}

// FREE FUNCTIONS
template <class VALUE, class CONTAINER, class COMPARATOR>
void swap(priority_queue<VALUE, CONTAINER, COMPARATOR>& lhs,
          priority_queue<VALUE, CONTAINER, COMPARATOR>& rhs)
{
    lhs.swap(rhs);
}

}  // close namespace bsl

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
