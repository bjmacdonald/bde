// s_baltst_enumerated.cpp          *DO NOT EDIT*          @generated -*-C++-*-

#include <bsls_ident.h>
BSLS_IDENT_RCSID(s_baltst_enumerated_cpp, "$Id$ $CSID$")

#include <s_baltst_enumerated.h>

#include <bdlat_formattingmode.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlb_print.h>
#include <bdlb_printmethods.h>
#include <bdlb_string.h>

#include <bslim_printer.h>
#include <bsls_assert.h>

#include <bsl_cstring.h>
#include <bsl_iomanip.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>
#include <bsl_utility.h>

namespace BloombergLP {
namespace s_baltst {

                              // ----------------
                              // class Enumerated
                              // ----------------

// CONSTANTS

const char Enumerated::CLASS_NAME[] = "Enumerated";

const bdlat_EnumeratorInfo Enumerated::ENUMERATOR_INFO_ARRAY[] = {
    {
        Enumerated::NEW_YORK,
        "NEW_YORK",
        sizeof("NEW_YORK") - 1,
        ""
    },
    {
        Enumerated::NEW_JERSEY,
        "NEW_JERSEY",
        sizeof("NEW_JERSEY") - 1,
        ""
    },
    {
        Enumerated::LONDON,
        "LONDON",
        sizeof("LONDON") - 1,
        ""
    }
};

// CLASS METHODS

int Enumerated::fromInt(Enumerated::Value *result, int number)
{
    switch (number) {
      case Enumerated::NEW_YORK:
      case Enumerated::NEW_JERSEY:
      case Enumerated::LONDON:
        *result = static_cast<Enumerated::Value>(number);
        return 0;
      default:
        return -1;
    }
}

int Enumerated::fromString(
        Enumerated::Value *result,
        const char         *string,
        int                 stringLength)
{
    for (int i = 0; i < 3; ++i) {
        const bdlat_EnumeratorInfo& enumeratorInfo =
                    Enumerated::ENUMERATOR_INFO_ARRAY[i];

        if (stringLength == enumeratorInfo.d_nameLength
        &&  0 == bsl::memcmp(enumeratorInfo.d_name_p, string, stringLength))
        {
            *result = static_cast<Enumerated::Value>(enumeratorInfo.d_value);
            return 0;
        }
    }

    return -1;
}

const char *Enumerated::toString(Enumerated::Value value)
{
    switch (value) {
      case NEW_YORK: {
        return "NEW_YORK";
      }
      case NEW_JERSEY: {
        return "NEW_JERSEY";
      }
      case LONDON: {
        return "LONDON";
      }
    }

    BSLS_ASSERT(0 == "invalid enumerator");
    return 0;
}

}  // close package namespace
}  // close enterprise namespace

// GENERATED BY @BLP_BAS_CODEGEN_VERSION@
// USING bas_codegen.pl s_baltst_enumerated.xsd --mode msg --includedir . --msgComponent enumerated --noRecurse --noExternalization --noHashSupport --noAggregateConversion
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright 2022 Bloomberg Finance L.P. All rights reserved.
//      Property of Bloomberg Finance L.P. (BFLP)
//      This software is made available solely pursuant to the
//      terms of a BFLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
