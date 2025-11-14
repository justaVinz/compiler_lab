//
// Created by Vinzenz Brehme on 11.11.25.
//
#pragma once
#ifndef COMPILER_LAB_TOKENIZEATTEMPT_H
#define COMPILER_LAB_TOKENIZEATTEMPT_H
#include <iosfwd>

#include "Token.h"
#include <optional>


class TokenizeAttempt {
private:
    std::optional<Token> token;
    int charsLexed;
public:
    TokenizeAttempt(Token token, int charsLexed);
    TokenizeAttempt();

    std::optional<Token> getToken() const { return token; }
    void setToken(const Token &t) { token = t; }
    int getCharsLexed() const { return charsLexed; }
    void setCharsLexed(int chars_lexed) { charsLexed = chars_lexed; }

    friend std::ostream& operator<<(std::ostream& os, const TokenizeAttempt& obj);
};


#endif //COMPILER_LAB_TOKENIZEATTEMPT_H