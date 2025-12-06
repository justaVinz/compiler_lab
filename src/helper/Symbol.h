#pragma once
#ifndef COMPILER_LAB_SYMBOL_H
#define COMPILER_LAB_SYMBOL_H

#include <ostream>
#include <string_view>
#include <array>

//X-macro symbol list
#define SYMBOL_LIST \
    X(terminal) \
    X(id) \
    X(literal) /* distinction into different literals may be needed. Or not. Who knows? */ \
    X(start) \
    X(transunit) \
    X(transunit_) \
    X(extdec) \
    X(extdec_) \
    X(extdec__) \
    X(decEnd) /* captialized E as we are not decending */ \
    X(declarator) \
    X(pointer) \
    X(pointer_) \
    X(type) \
    X(structtype) \
    X(structdeclist) \
    X(structdeclist_) \
    X(dec) \
    X(dec_) \
    X(directdec) \
    X(directdec_) \
    X(paramlist) \
    X(paramlist_) \
    X(paramdec) \
    X(paramdec_) \
    X(abstractdeclarator) \
    X(abstractdeclarator_) \
    X(directabstractdeclarator) \
    X(directabstractdeclarator_) \
    X(funcdef_) /* underscore as the rest of the functiondef symbols have already been parsed by extdec, extdec_ and extdec__ for LL(1) purposes. */ \


//actual enum definition
enum Symbol {
#define X(name) name,
    SYMBOL_LIST
#undef X
    COUNT  // last variant so gets the int value n where n is the amount of symbols. Used for sizing the array down below
};


//array of names
constexpr std::array<std::string_view,
    static_cast<size_t>(Symbol::COUNT)>
SymbolNames = {
#define X(name) #name,
    SYMBOL_LIST
#undef X
};


//print
inline std::ostream& operator<<(std::ostream& os, Symbol s) {
    size_t idx = static_cast<size_t>(s);
    if (idx < SymbolNames.size())
        return os << SymbolNames[idx];

    return os << "UNKNOWN_SYMBOL(" << idx << ")";
}

#endif // COMPILER_LAB_SYMBOL_H