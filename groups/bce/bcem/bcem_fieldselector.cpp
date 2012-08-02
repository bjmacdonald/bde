// bcem_fieldselector.cpp                                             -*-C++-*-
#include <bcem_fieldselector.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcem_fieldselector_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {

                     // ------------------------
                     // class bcem_FieldSelector
                     // ------------------------

// ACCESSORS
bsl::ostream& bcem_FieldSelector::print(bsl::ostream& stream,
                                        int           level,
                                        int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    printer.printAttribute("state", (int) d_state);
    if (isName()) {
        printer.printAttribute("name", d_name_p);
    }
    else if (isIndex()) {
        printer.printAttribute("index", d_index);
    }

    printer.end();

    return stream;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------

