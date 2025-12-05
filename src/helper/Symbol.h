#pragma once
#include <memory>
#ifndef COMPILER_LAB_SYMBOL_H
#define COMPILER_LAB_SYMBOL_H

enum Symbol {
    terminal,
    id,
    literal, //distinction into different literals may be needed. Or not. Who knows?
    start,
    transunit,
    transunit_,
    extdec,
    extdec_,
    extdec__,
    decEnd, //captialized E as we are not decending
    declarator,
    pointer,
    pointer_,
    type,
    structdeclist,
    structdeclist_,
    directdec,
    directdecA,
    directdecB,
    paramlist,
    paramlist_,
    paramdec,
    paramdec_,
    abstractdec,
    abstractdec_,
    directabstractdec,
    funcdef_, // underscore as the rest of the functiondef symbols have already been parsed by extdec, extdec_ and extdec__ for LL(1) purposes.
};

#endif //COMPILER_LAB_SYMBOL_H