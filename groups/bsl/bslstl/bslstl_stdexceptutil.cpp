// bslstl_stdexceptutil.cpp                                           -*-C++-*-
#include <bslstl_stdexceptutil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_atomicoperations.h>
#include <bsls_exceptionutil.h>
#include <bsls_log.h>
#include <bsls_platform.h>
#include <bsls_pointercastutil.h>
#include <bsls_stackaddressutil.h>

#include <stdexcept>
#include <string>

namespace {
namespace u {

using namespace BloombergLP;
using namespace bslstl;

// TYPES
typedef bsls::AtomicOperations          Atomics;
typedef Atomics::AtomicTypes::Pointer   AtomicPtr;
typedef bsls::PointerCastUtil           CastUtil;

template <class t_EXCEPTION>
class ExceptionSource {
    // This template 'class' enables us to store the exception name and the pre
    // throw hook for each supported exception type, and have a template
    // function 'doThrow' which does the work of examining the pre throw hook,
    // calling it if set, and throwing the exception.

    // DATA
    static const char   *s_exceptionName;
    static u::AtomicPtr  s_preThrowHook;

  public:
    // CLASS METHODS
    static void doThrow(const char *message);
        // Throw the template parameter 't_EXCEPTION' with the specified
        // 'message'.  If the pre throw hook has been set, call it with the
        // exception name and 'message'.

    static void setPreThrowHook(StdExceptUtil::PreThrowHook hook);
        // Set the pre throw hook for the template parameter 't_EXCEPTION' to
        // the specified 'hook'.
};

template <class t_EXCEPTION>
inline
void ExceptionSource<t_EXCEPTION>::doThrow(const char *message)
{
    BSLS_ASSERT(message);

    typedef StdExceptUtil::PreThrowHook PreThrowHook;

    PreThrowHook preThrowHook = u::CastUtil::cast<PreThrowHook>(
                                   u::Atomics::getPtrAcquire(&s_preThrowHook));
    if (preThrowHook) {
        (*preThrowHook)(s_exceptionName, message);
    }

    BSLS_THROW(t_EXCEPTION(message));
}

template <class t_EXCEPTION>
inline
void ExceptionSource<t_EXCEPTION>::setPreThrowHook(
                                              StdExceptUtil::PreThrowHook hook)
{
    u::Atomics::setPtrRelease(
                             &s_preThrowHook, u::CastUtil::cast<void *>(hook));
}

template <class t_EXCEPTION>
u::AtomicPtr ExceptionSource<t_EXCEPTION>::s_preThrowHook = {
                                                      static_cast<void *>(0) };

#define U_INIT_EXCEPTION_NAME(exception)                                      \
template <>                                                                   \
const char *ExceptionSource<std::exception>::s_exceptionName =                \
                                                             "std::" #exception

U_INIT_EXCEPTION_NAME(runtime_error);
U_INIT_EXCEPTION_NAME(logic_error);
U_INIT_EXCEPTION_NAME(domain_error);
U_INIT_EXCEPTION_NAME(invalid_argument);
U_INIT_EXCEPTION_NAME(length_error);
U_INIT_EXCEPTION_NAME(out_of_range);
U_INIT_EXCEPTION_NAME(range_error);
U_INIT_EXCEPTION_NAME(overflow_error);
U_INIT_EXCEPTION_NAME(underflow_error);

#undef U_INIT_EXCEPTION_NAME

}  // close namespace u
}  // close unnamed namespace

namespace BloombergLP {
namespace bslstl {

                        //====================
                        // class StdExceptUtil
                        //====================

// CLASS METHODS
void StdExceptUtil::logCheapStackTrace(const char *exceptionName,
                                       const char *message)
{
    enum { k_MAX_FRAMES                = 128,
           k_MAX_PRINTED_BYTES_PER_PTR = sizeof(void *) * 2 + sizeof(char),
           k_CHEAP_STACK_BUF_SIZE      = 1024 + k_MAX_FRAMES *
                                                 k_MAX_PRINTED_BYTES_PER_PTR };

    char cheapStackBuf[k_CHEAP_STACK_BUF_SIZE];
    bsls::StackAddressUtil::formatCheapStack(cheapStackBuf,
                                             k_CHEAP_STACK_BUF_SIZE,
                                             0);

    BSLS_LOG_WARN("About to throw %s, %s %s",
                  exceptionName,
                  message,
                  cheapStackBuf);
}

void StdExceptUtil::setRuntimeErrorHook(PreThrowHook hook)
{
    u::ExceptionSource<std::runtime_error>::setPreThrowHook(hook);
}

void StdExceptUtil::setLogicErrorHook(PreThrowHook hook)
{
    u::ExceptionSource<std::logic_error>::setPreThrowHook(hook);
}

void StdExceptUtil::setDomainErrorHook(PreThrowHook hook)
{
    u::ExceptionSource<std::domain_error>::setPreThrowHook(hook);
}

void StdExceptUtil::setInvalidArgumentHook(PreThrowHook hook)
{
    u::ExceptionSource<std::invalid_argument>::setPreThrowHook(hook);
}

void StdExceptUtil::setLengthErrorHook(PreThrowHook hook)
{
    u::ExceptionSource<std::length_error>::setPreThrowHook(hook);
}

void StdExceptUtil::setOutOfRangeHook(PreThrowHook hook)
{
    u::ExceptionSource<std::out_of_range>::setPreThrowHook(hook);
}

void StdExceptUtil::setRangeErrorHook(PreThrowHook hook)
{
    u::ExceptionSource<std::range_error>::setPreThrowHook(hook);
}

void StdExceptUtil::setOverflowErrorHook(PreThrowHook hook)
{
    u::ExceptionSource<std::overflow_error>::setPreThrowHook(hook);
}

void StdExceptUtil::setUnderflowErrorHook(PreThrowHook hook)
{
    u::ExceptionSource<std::underflow_error>::setPreThrowHook(hook);
}

// Implementation note: the calls to 'BSLS_ASSERT_INVOKE_NORETURN' in the
// following 'throw' methods should be unreachable, the reason they are there
// is to prevent the optimizer from chaining the call to 'doThrow' (as the
// Solaris CC compiler did) rather than inlining it, resulting in 'doThrow'
// rather than 'StdExceptUtil::throw...' showing up in the stack trace, which
// is less user-friendly.

void StdExceptUtil::throwRuntimeError(const char *message)
{
    u::ExceptionSource<std::runtime_error>::doThrow(message);

    BSLS_ASSERT_INVOKE_NORETURN("throw runtime_error shouldn't get here");
}

void StdExceptUtil::throwLogicError(const char *message)
{
    u::ExceptionSource<std::logic_error>::doThrow(message);

    BSLS_ASSERT_INVOKE_NORETURN("throw logic_error shouldn't get here");
}

void StdExceptUtil::throwDomainError(const char *message)
{
    u::ExceptionSource<std::domain_error>::doThrow(message);

    BSLS_ASSERT_INVOKE_NORETURN("throw domain_error shouldn't get here");
}

void StdExceptUtil::throwInvalidArgument(const char *message)
{
    u::ExceptionSource<std::invalid_argument>::doThrow(message);

    BSLS_ASSERT_INVOKE_NORETURN("throw invalid_argument shouldn't get here");
}

void StdExceptUtil::throwLengthError(const char *message)
{
    u::ExceptionSource<std::length_error>::doThrow(message);

    BSLS_ASSERT_INVOKE_NORETURN("throw length_error shouldn't get here");
}

void StdExceptUtil::throwOutOfRange(const char *message)
{
    u::ExceptionSource<std::out_of_range>::doThrow(message);

    BSLS_ASSERT_INVOKE_NORETURN("throw out_of_range shouldn't get here");
}

void StdExceptUtil::throwRangeError(const char *message)
{
    u::ExceptionSource<std::range_error>::doThrow(message);

    BSLS_ASSERT_INVOKE_NORETURN("throw range_error shouldn't get here");
}

void StdExceptUtil::throwOverflowError(const char *message)
{
    u::ExceptionSource<std::overflow_error>::doThrow(message);

    BSLS_ASSERT_INVOKE_NORETURN("throw overflow_error shouldn't get here");
}

void StdExceptUtil::throwUnderflowError(const char *message)
{
    u::ExceptionSource<std::underflow_error>::doThrow(message);

    BSLS_ASSERT_INVOKE_NORETURN("throw underflow_error shouldn't get here");
}

}  // close package namespace
}  // close enterprise namespace

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
