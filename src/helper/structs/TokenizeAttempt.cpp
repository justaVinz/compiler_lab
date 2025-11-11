#include "TokenizeAttempt.h"
#include "Token.h"

#include <iostream>


TokenizeAttempt::TokenizeAttempt(Token t, int lexed)
    : token(std::move(t)), charsLexed(lexed) {}


TokenizeAttempt::TokenizeAttempt()
    : charsLexed(0) {}

std::ostream& operator<<(std::ostream& os, const TokenizeAttempt& attempt) {
    os << "TokenizeAttempt("
       << attempt.getToken() << ", "
       << attempt.getCharsLexed() << ")";
    return os;
}
