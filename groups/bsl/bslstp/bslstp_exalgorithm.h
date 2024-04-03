// bslstp_exalgorithm.h                                               -*-C++-*-
#ifndef INCLUDED_BSLSTP_EXALGORITHM
#define INCLUDED_BSLSTP_EXALGORITHM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace for algorithm extensions.
//
//@INTERNAL_DEPRECATED: Do not use directly.
//
//@CLASSES:
//  bslstp::exalgorithm::copy_n: copy 'n' elements
//
//@CANONICAL_HEADER: bsl_algorithm.h
//
//@SEE_ALSO: bsl_algorithm
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<bsl_algorithm>' instead.  This component provides a namespace for
// algorithm extensions that are not specified in the C++ standard.  This
// provides backward compatibility to code using extensions that STLPort
// provides.
//
// Note that the functions in this component are based on STLPort's
// implementation, with copyright notice as follows:
//..
//-----------------------------------------------------------------------------
// Copyright (c) 1994
// Hewlett-Packard Company
//
// Copyright (c) 1996,1997
// Silicon Graphics Computer Systems, Inc.
//
// Copyright (c) 1997
// Moscow Center for SPARC Technology
//
// Copyright (c) 1999
// Boris Fomitchev
//
// This material is provided "as is", with absolutely no warranty expressed
// or implied.  Any use is at your own risk.
//
// Permission to use or copy this software for any purpose is hereby granted
// without fee, provided the above notices are retained on all copies.
// Permission to modify the code and to distribute modified code is granted,
// provided the above notices are retained, and a notice that the code was
// modified is included with the above copyright notice.
//-----------------------------------------------------------------------------
//..
//
///Usage
///-----
// This component is for internal use only.

#ifdef BDE_OPENSOURCE_PUBLICATION // STP
#error "bslstp_exalgorithm is not for publication"
#endif

#include <bslscm_version.h>

#include <bslstl_iterator.h>  // iterator tags
#include <bslstl_pair.h>

#include <bsls_platform.h>

#include <algorithm>          // 'copy'

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace bsl {
                         // ==============
                         // class CopyNRet
                         // ==============

template <typename t_INPUT_IT, typename t_OUTPUT_IT>
class CopyNRet {
    // This class is an adapter, allowing users of the 'bsl::copy_n' function
    // to seamlessly switch from one return type of that function to another.
    // This class is created for a strictly defined task and is not intended
    // for use outside the BDE library.  Do not use it under any circumstances.

  private:
    // DATA
    pair<t_INPUT_IT, t_OUTPUT_IT> d_data;  // value returned from 'bsl::copy_n'

  public:
    // PUBLIC DATA
    t_OUTPUT_IT&                  second;  // alias for output iterator in
                                           // returned value

    // CREATORS
    CopyNRet(t_INPUT_IT iit, t_OUTPUT_IT oit)
       // Create a 'StringRefData' object having the pair of specified 'iit'
       // and 'oit' itarators as a value.
    : d_data(iit, oit)
    , second(d_data.second)
    {
    }

    // ACCESSORS
    template <typename T1, typename T2>
    operator pair<T1, T2>() const
        // Return a pair of iterators having the value returned from
        // 'bsl::copy_n'.
    {
        return d_data;
    }

    operator t_OUTPUT_IT() const
        // Return an output iterator having the value of output iterator
        // returned from 'bsl::copy_n'.
    {
        return second;
    }
};

// PRIVATE FREE FUNCTIONS
template <class InputIter, class Size, class OutputIter>
CopyNRet<InputIter, OutputIter>
bslstp_ExAlgorithm_CopyNImp(InputIter  first,
                            Size       count,
                            OutputIter result,
                            const      input_iterator_tag&)
{
    for ( ; count > 0; --count) {
        *result = *first;
        ++first;
        ++result;
    }
    return CopyNRet<InputIter, OutputIter>(first, result);
}

template <class RAIter, class Size, class OutputIter>
inline
CopyNRet<RAIter, OutputIter>
bslstp_ExAlgorithm_CopyNImp(RAIter     first,
                            Size       count,
                            OutputIter result,
                            const      random_access_iterator_tag&)
{
    RAIter last = first + count;
    return CopyNRet<RAIter, OutputIter>(last, std::copy(first, last, result));
}

// FREE FUNCTIONS
template <class InputIter, class Size, class OutputIter>
inline
CopyNRet<InputIter, OutputIter> copy_n(InputIter  first,
                                       Size       count,
                                       OutputIter result)
    // Copy the specified 'count' elements from the specified 'first' address
    // to the specified 'result' address.
{
    typedef typename iterator_traits<InputIter>::iterator_category tag;
    return bslstp_ExAlgorithm_CopyNImp(first, count, result, tag());
}

}  // close namespace bsl

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
