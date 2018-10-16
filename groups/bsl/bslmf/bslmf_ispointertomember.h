// bslmf_ispointertomember.h                                          -*-C++-*-
#ifndef INCLUDED_BSLMF_ISPOINTERTOMEMBER
#define INCLUDED_BSLMF_ISPOINTERTOMEMBER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for pointer-to-member types.
//
//@CLASSES:
//  bslmf::IsPointerToMember: check for pointer-to-member types
//  bslmf::IsPointerToMemberData: check for pointer-to-member data types
//  bslmf::IsPointerToMemberFunction: check for pointer-to-member function types
//
//@AUTHOR: Clay Wilson (cwilson9)
//
//@DESCRIPTION: This component defines a simple template structure used to
// evaluate whether it's parameter is a pointer to member, optionally qualified
// with 'const' or 'volatile'.  'bslmf::IsPointerToMember' defines a 'value'
// member that is initialized (at compile-time) to 1 if the parameter is a
// pointer to member (or a reference to such a type), and to 0 otherwise.  The
// meta-functions 'bslmf::IsPointerToMemberData' and
// 'bslmf::IsPointerToMemberFunction' are also provided to test specifically
// for pointers to (non-'static') data members and pointers to (non-'static')
// function members, respectively.  Note that, consistent with TR1, these
// metafunctions evaluate to 0 (false) for reference types (i.e.,
// reference-to-pointer-to-member types).
//
///Usage
///-----
// For example:
//..
//  struct MyStruct {};
//  enum   MyEnum {};
//  class  MyClass {};
//
//  typedef int (MyClass::* PMFdRi)(double, int&);
//  typedef int (MyClass::* PMFCe)(MyEnum) const;
//
//  assert(0 == bslmf::IsPointerToMember<int             *>::value);
//  assert(0 == bslmf::IsPointerToMember<    MyStruct    *>::value);
//  assert(1 == bslmf::IsPointerToMember<int MyStruct::*  >::value);
//  assert(0 == bslmf::IsPointerToMember<int MyStruct::*& >::value);
//  assert(0 == bslmf::IsPointerToMember<int MyStruct::* *>::value);
//
//  assert(1 == bslmf::IsPointerToMemberData<int MyStruct::*>::value);
//  assert(0 == bslmf::IsPointerToMemberData<PMFdRi         >::value);
//
//  assert(1 == bslmf::IsPointerToMember<PMFdRi >::value);
//  assert(0 == bslmf::IsPointerToMember<PMFdRi&>::value);
//  assert(1 == bslmf::IsPointerToMember<PMFCe  >::value);
//  assert(0 == bslmf::IsPointerToMember<PMFCe& >::value);
//
//  assert(1 == bslmf::IsPointerToMemberFunction<PMFdRi        >::value);
//  assert(0 == bslmf::IsPointerToMemberFunction<int MyClass::*>::value);
//..

#include <bslscm_version.h>

#include <bslmf_ismemberfunctionpointer.h>
#include <bslmf_ismemberobjectpointer.h>
#include <bslmf_ismemberpointer.h>

namespace BloombergLP {
namespace bslmf {

                      // ================================
                      // struct IsPointerToMemberFunction
                      // ================================

template <class TYPE>
struct IsPointerToMemberFunction : bsl::is_member_function_pointer<TYPE>::type
{
    // Metafunction to test if 'TYPE' is a pointer to a member function.  Note
    // that the result is false if 'TYPE' is a reference.
};

                      // ============================
                      // struct IsPointerToMemberData
                      // ============================

template <class TYPE>
struct IsPointerToMemberData : bsl::is_member_object_pointer<TYPE>::type
{
    // Metafunction to test if 'TYPE' is a pointer to a member object.  Note
    // that the result is false if 'TYPE' is a reference.
};

                      // ========================
                      // struct IsPointerToMember
                      // ========================

template <class TYPE>
struct IsPointerToMember : bsl::is_member_pointer<TYPE>::type
{
    // Metafunction to test if 'TYPE' is a pointer to member (function or
    // object).  The result is false if 'TYPE' is a reference.
};

}  // close package namespace
}  // close enterprise namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

#ifdef bslmf_IsPointerToMember
#undef bslmf_IsPointerToMember
#endif
#define bslmf_IsPointerToMember bslmf::IsPointerToMember
    // This alias is defined for backward compatibility.

#ifdef bslmf_IsPointerToMemberFunction
#undef bslmf_IsPointerToMemberFunction
#endif
#define bslmf_IsPointerToMemberFunction bslmf::IsPointerToMemberFunction
    // This alias is defined for backward compatibility.

#ifdef bslmf_IsPointerToMemberData
#undef bslmf_IsPointerToMemberData
#endif
#define bslmf_IsPointerToMemberData bslmf::IsPointerToMemberData
    // This alias is defined for backward compatibility.
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

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
