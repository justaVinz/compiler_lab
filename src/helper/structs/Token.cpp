//
// Created by Vinzenz Brehme on 10.11.25.
//

#include "Token.h"

#include <iostream>
#include <string>


Token::Token(std::string type, std::string val, int line, int index) {
    tokenType = type;
    value = val;
    sourceLine = line;
    sourceIndex = index;
}

std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << "Token(" << token.tokenType << ", " << token.value << ", " << token.sourceLine << ", " << token.sourceIndex << ")";
    return os;
}


