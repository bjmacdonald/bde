// bsls_asserttest_macroreset.h                                       -*-C++-*-
#ifndef INCLUDED_BSLS_ASSERTTEST_MACRORESET
#define INCLUDED_BSLS_ASSERTTEST_MACRORESET

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Undefine macros related to `bsls_asserttest.h`.
//
//@DESCRIPTION: This component provides a header that is *included* to undefine
// all macros defined in `bsls_asserttest.h`, `bsls_assert.h`, and
// `bsls_review.h`; it is designed to be *re*-includable.  This is for the use
// of the `bsls_assertest` test driver to enable it to re-include the
// `bsls_asserttest.h` header with different input macros and verify that they
// output the correct resulting macro definitions.
//
///Usage
///-----
// To undefine all macros defined in `bsls_asserttest.h`, `bsls_assert.h` and
// `bsls_review.h`, first disable the header include guard for this header
// (`bsls_asserttest_macroreset.h`):
// ```
// #undef INCLUDED_BSLS_ASSERTTEST_MACRORESET
// ```
// Then re-include this header, which will undefine the macros that were
// defined by `bsls_asserttest.h`, `bsls_assert.h` and `bsls_review.h`:
// ```
// #include <bsls_asserttest_macroreset.h>
// ```
// Then define any macros that control the behaviors of `bsls_asserttest.h`,
// `bsls_assert.h` and `bsls_review.h` for this particular test, then
// re-include `bsls_asserttest.h`:
// ```
// #include <bsls_asserttest.h>
// ```

// Undefine the external header guards so that the component headers can be
// re-included and re-evaluated.
#undef INCLUDED_BSLS_REVIEW
#undef INCLUDED_BSLS_ASSERT
#undef INCLUDED_BSLS_ASSERTTEST

// Undefine the build mode so it can be set to whatever the test wants to
// verify next.
#undef BDE_BUILD_TARGET_DBG
#undef BDE_BUILD_TARGET_OPT
#undef BDE_BUILD_TARGET_SAFE
#undef BDE_BUILD_TARGET_SAFE_2

// Undefine the explicit review level and assert level macros.
#undef BSLS_REVIEW_LEVEL_NONE
#undef BSLS_REVIEW_LEVEL_REVIEW
#undef BSLS_REVIEW_LEVEL_REVIEW_OPT
#undef BSLS_REVIEW_LEVEL_REVIEW_SAFE

#undef BSLS_ASSERT_LEVEL_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSERT_OPT
#undef BSLS_ASSERT_LEVEL_ASSERT_SAFE
#undef BSLS_ASSERT_LEVEL_ASSUME_ASSERT
#undef BSLS_ASSERT_LEVEL_ASSUME_OPT
#undef BSLS_ASSERT_LEVEL_ASSUME_SAFE
#undef BSLS_ASSERT_LEVEL_NONE

// Undefine the macros actually defined by 'bsls_review.h'.
#undef BSLS_REVIEW
#undef BSLS_REVIEW_REVIEW_IMP
#undef BSLS_REVIEW_REVIEW_COUNT_IMP
#undef BSLS_REVIEW_DISABLED_IMP
#undef BSLS_REVIEW_INVOKE
#undef BSLS_REVIEW_IS_ACTIVE
#undef BSLS_REVIEW_IS_USED
#undef BSLS_REVIEW_OPT
#undef BSLS_REVIEW_OPT_IS_ACTIVE
#undef BSLS_REVIEW_OPT_IS_USED
#undef BSLS_REVIEW_SAFE
#undef BSLS_REVIEW_SAFE_IS_ACTIVE
#undef BSLS_REVIEW_SAFE_IS_USED

// Undefine other control macros
#undef BSLS_REVIEW_VALIDATE_DISABLED_MACROS
#undef BSLS_ASSERT_VALIDATE_DISABLED_MACROS
#undef BSLS_ASSERTTEST_VALIDATE_DISABLED_MACROS

// Undefine the macros actually defined by 'bsls_assert.h'.
#undef BSLS_ASSERT
#undef BSLS_ASSERT_ASSERT
#undef BSLS_ASSERT_ASSUME_IMP
#undef BSLS_ASSERT_ASSERT_IMP
#undef BSLS_ASSERT_DISABLED_IMP
#undef BSLS_ASSERT_INVOKE
#undef BSLS_ASSERT_INVOKE_NORETURN
#undef BSLS_ASSERT_IS_ACTIVE
#undef BSLS_ASSERT_IS_ASSUMED
#undef BSLS_ASSERT_IS_REVIEW
#undef BSLS_ASSERT_IS_USED
#undef BSLS_ASSERT_OPT
#undef BSLS_ASSERT_OPT_IS_ACTIVE
#undef BSLS_ASSERT_OPT_IS_ASSUMED
#undef BSLS_ASSERT_OPT_IS_REVIEW
#undef BSLS_ASSERT_OPT_IS_USED
#undef BSLS_ASSERT_SAFE
#undef BSLS_ASSERT_SAFE_IS_ACTIVE
#undef BSLS_ASSERT_SAFE_IS_ASSUMED
#undef BSLS_ASSERT_SAFE_IS_REVIEW
#undef BSLS_ASSERT_SAFE_IS_USED

// Undefine the macros actually defined by 'bsls_asserttest.h'.
#undef BSLS_ASSERTTEST_SAFE_2_BUILD_FLAG
#undef BSLS_ASSERTTEST_ASSERT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_OPT_ACTIVE_FLAG
#undef BSLS_ASSERTTEST_ASSERT_SAFE_ACTIVE_FLAG

#undef BSLS_ASSERTTEST_ASSERT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL
#undef BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL
#undef BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW
#undef BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL
#undef BSLS_ASSERTTEST_ASSERT_INVOKE_FAIL_RAW

#undef BSLS_ASSERTTEST_ASSERT_PASS
#undef BSLS_ASSERTTEST_ASSERT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS
#undef BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS
#undef BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW
#undef BSLS_ASSERTTEST_ASSERT_INVOKE_PASS
#undef BSLS_ASSERTTEST_ASSERT_INVOKE_PASS_RAW

#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP
#undef BSLS_ASSERTTEST_BRUTE_FORCE_IMP_RAW
#undef BSLS_ASSERTTEST_DISABLED_IMP
#undef BSLS_ASSERTTEST_IS_ACTIVE

#undef BSLS_ASSERTTEST_CAN_CHECK_LEVELS
#undef BSLS_ASSERTTEST_CHECK_LEVEL
#undef BSLS_ASSERTTEST_CHECK_LEVEL_ARG

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
