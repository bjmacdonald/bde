// bsla_format.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLA_FORMAT
#define INCLUDED_BSLA_FORMAT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a macro to indicate that a return value is a format string.
//
//@MACROS:
//  BSLA_FORMAT(FMT_IDX):  validate 'printf'-style format spec. in 'n'th arg.
//  BSLA_FORMAT_IS_ACTIVE: defined if 'BSLA_FORMAT' is active
//
//@SEE_ALSO: bsla_annotations
//
//@DESCRIPTION: This component provides a preprocessor macro to indicate that
// an indexed argument of a function is a 'printf'-style format specification,
// and that the function will return a 'printf'-style format string with an
// equivalent specification.
//
///Macro Reference
///---------------
//: 'BSLA_FORMAT(FMT_IDX)':
//:     This annotation specifies that the so-annotated function takes an
//:     argument that is a valid format string for a 'printf'-style function
//:     and returns a format string that is consistent with that format.  This
//:     allows format strings manipulated by translation functions to be
//:     checked against arguments.  Without this annotation, attempting to
//:     manipulate the format string via this kind of function might generate
//:     warnings about non-literal formats, or fail to generate warnings about
//:     mismatched arguments.
//:
//: 'BSLA_FORMAT_IS_ACTIVE':
//:     The macro 'BSLA_FORMAT_IS_ACTIVE' is defined if 'BSLA_FORMAT' expands
//:     to something with the desired effect; otherwise 'BSLA_FORMAT_IS_ACTIVE'
//:     is not defined and 'BSLA_FORMAT' expands to nothing.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: A Language Translator Function
///- - - - - - - - - - - - - - - - - - - - -
// First, we define an 'enum', 'Language', to indicate the choice of languages:
//..
//  enum Language {
//      e_ENGLISH,
//      e_SPANISH,
//      e_DUTCH,
//      e_FRENCH };
//..
// Then, we define a function, 'prefixName', which will take a format string
// and prefix it with the word 'name' in the selected language.  The
// 'BSLA_FORMAT' annotation indicates that the result will be a pointer to a
// 'printf'-style format string equivalent to the format string passed to the
// third argument:
//..
//  const char *prefixName(char *buf, Language lang, const char *format)
//                                                              BSLA_FORMAT(3);
//  const char *prefixName(char *buf, Language lang, const char *format)
//      // Create a buffer beginning with the word 'name' translated to the
//      // specified 'lang', followed by the specified format string 'format',
//      // using the specified 'buf' for memory.
//  {
//      const char *name = "";
//      switch (lang) {
//        case e_ENGLISH: name = "Name";   break;
//        case e_SPANISH: name = "Nombre"; break;
//        case e_DUTCH:   name = "Naam";   break;
//        case e_FRENCH:  name = "Nom";    break;
//      }
//      ::strcpy(buf, name);
//      ::strcat(buf, ": ");
//      ::strcat(buf, format);
//
//      return buf;
//  }
//..
// Next, in 'main', we call 'printf' and 'sscanf' using the return value of
// 'prefixName'.  No warnings occur when correct arguments are passed:
//..
//  char buffer[1000];
//  ::printf(prefixName(buffer, e_SPANISH, "%s\n"), "Michael Bloomberg");
//
//  char name[100];
//  ::sscanf("Emmanuel Macron", prefixName(buffer, e_FRENCH, "%s"), name);
//..
// Now, we call 'printf' and 'sscanf' passing arguments that won't match the
// resulting format string:
//..
//  ::printf(prefixName(buffer, e_ENGLISH, "%s\n"), 2.7);
//  int x;
//  ::sscanf("Sharon den Adel", prefixName(buffer, e_DUTCH, "%s"), &x);
//..
// Finally, we observe the following warning messages with g++:
//..
//  .../bsla/bsla_format.t.cpp:306:56: warning: format '%s' expects argument
//   of type 'char*', but argument 2 has type 'double' [-Wformat=]
//       ::printf(prefixName(buffer, e_ENGLISH, "%s\n"), 2.7);
//                                                          ^
//  .../bsla_format.t.cpp:308:70: warning: format '%s' expects argument of
//   type 'char*', but argument 3 has type 'int*' [-Wformat=]
//       ::sscanf("Sharon den Adel", prefixName(buffer, e_DUTCH, "%s"), &x);
//                                                                      ~~^
//..

#include <bsls_platform.h>

                       // =============================
                       // Checks for Pre-Defined macros
                       // =============================

#if defined(BSLA_FORMAT)
#error BSLA_FORMAT is already defined!
#endif

#if defined(BSLA_FORMAT_IS_ACTIVE)
#error BSLA_FORMAT_IS_ACTIVE is already defined!
#endif

                       // =========================
                       // Set macros as appropriate
                       // =========================

#if defined(BSLS_PLATFORM_CMP_GNU)   ||                                      \
    defined(BSLS_PLATFORM_CMP_CLANG) ||                                      \
    defined(BSLS_PLATFORM_CMP_IBM)
    #define BSLA_FORMAT(FMT_IDX) __attribute__((format_arg(FMT_IDX)))

    #define BSLA_FORMAT_IS_ACTIVE 1
#else
    #define BSLA_FORMAT(FMT_IDX)
#endif

#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
