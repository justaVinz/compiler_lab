//
// Created by Vinzenz Brehme on 10.11.25.
//

#include "Token.h"

#include <iostream>
#include <string>


Token::Token(std::string type, std::string val, int line, int index)
    : tokenType(std::move(type)), value(std::move(val)), sourceLine(line), sourceIndex(index) {}

Token::Token()
    : tokenType(""), value(""), sourceLine(0), sourceIndex(0) {}


std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << "Token("
       << token.getTokenType() << ", "
       << token.getValue() << ", "
       << token.getSourceLine() << ", "
       << token.getSourceIndex() << ")";
    return os;
}



