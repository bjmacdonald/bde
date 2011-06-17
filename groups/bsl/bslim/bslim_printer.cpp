// bslim_printer.cpp                                                  -*-C++-*-
#include <bslim_printer.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_assert.h>

#include <bsl_iomanip.h>

namespace BloombergLP {
namespace bslim {

                        // -------------
                        // class Printer
                        // -------------

// PRIVATE ACCESSORS
void Printer::printEndIndentation() const
{
    if (d_spacesPerLevel < 0) {
        *d_stream_p << ' ';
    }
    else {
        *d_stream_p << bsl::setw(d_spacesPerLevel * d_level) << "";
    }
}

void Printer::printIndentation() const
{
    if (d_spacesPerLevel < 0) {
        *d_stream_p << ' ';
    }
    else {
        *d_stream_p << bsl::setw(d_spacesPerLevel * d_levelPlusOne) << "";
    }
}

// CREATORS
Printer::Printer(bsl::ostream *stream, int level, int spacesPerLevel)
: d_stream_p(stream)
, d_spacesPerLevel(spacesPerLevel)
{
    BSLS_ASSERT(stream);

    d_suppressInitialIndentFlag = level < 0;
    d_level                     = level < 0 ? -level : level;
    d_levelPlusOne              = d_level + 1;
}

Printer::~Printer()
{
}

// ACCESSORS
int Printer::absLevel() const
{
    return d_level;
}

void Printer::end(bool suppressBracket) const
{
    if (!suppressBracket) {
        printEndIndentation();
        *d_stream_p << ']';
    }

    if (d_spacesPerLevel >= 0) {
        *d_stream_p << '\n';
    }
}

void Printer::printHexAddr(const void *address, const char *name) const
{
    printIndentation();

    if (name != NULL) {
        *d_stream_p << name << " = ";
    }

    Printer_Helper::print(*d_stream_p,
                          address,
                          -d_levelPlusOne,
                          d_spacesPerLevel);
}

int Printer::spacesPerLevel() const
{
    return d_spacesPerLevel;
}

void Printer::start(bool suppressBracket) const
{
    if (!suppressInitialIndentFlag()) {
        const int absSpacesPerLevel = d_spacesPerLevel < 0
                                      ? -d_spacesPerLevel
                                      :  d_spacesPerLevel;
        *d_stream_p << bsl::setw(absSpacesPerLevel * d_level) << "";
    }

    if (!suppressBracket) {
        *d_stream_p << '[';
        if (d_spacesPerLevel >= 0) {
            *d_stream_p << '\n';
        }
    }
}

bool Printer::suppressInitialIndentFlag() const
{
    return d_suppressInitialIndentFlag;
}

}  // close namespace bslim
}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
