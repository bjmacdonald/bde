// bslstl_istringstream.cpp                                           -*-C++-*-
#include <bslstl_istringstream.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

namespace bsl {

// Force instantiation of stringstream classes.
template class basic_istringstream<
    char, char_traits<char>, allocator<char> >;
template class basic_istringstream<
    wchar_t, char_traits<wchar_t>, allocator<wchar_t> >;

}  // close namespace bsl

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
