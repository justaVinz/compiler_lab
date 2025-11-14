#include "TokenizeAttempt.h"
#include "Token.h"

#include <iostream>


TokenizeAttempt::TokenizeAttempt(Token t, int lexed)
    : token(std::move(t)), charsLexed(lexed) {}


TokenizeAttempt::TokenizeAttempt()
    : charsLexed(0) {}

std::ostream& operator<<(std::ostream& os, const TokenizeAttempt& attempt) {
    os << "TokenizeAttempt(";
    if (attempt.getToken())     //you can't print optionals so we need to check
        os << *(attempt.getToken());
    else
        os << "None";
    os << ", " << attempt.getCharsLexed() << ")";
    return os;
}

