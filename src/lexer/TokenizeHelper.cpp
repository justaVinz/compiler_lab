#include "TokenizeHelper.h"

#include <iostream>
#include <ostream>

#include "../helper/structs/TokenizeAttempt.h"

TokenizeAttempt TokenizeHelper::tokenizeStringLiterals(char* code) {
    if (!code || *code != '"') return TokenizeAttempt();

    char* start = code;
    char* ptr = code + 1;
    bool escaped = false;

    while (*ptr) {
        if (escaped) {
            escaped = false;
        } else if (*ptr == '\\') {
            escaped = true;
        } else if (*ptr == '"') {
            std::string value(start, ptr - start + 1);
            Token token;
            token.setTokenType("string-literal");
            token.setValue(value);

            TokenizeAttempt attempt;
            attempt.setToken(token);
            attempt.setCharsLexed(ptr - start + 1);
            return attempt;
        }
        ++ptr;
    }
    return TokenizeAttempt();
}

TokenizeAttempt TokenizeHelper::tokenizeKeywordPunctuators(char* code) {
    if (code == nullptr || code[0] == '\0') {
        TokenizeAttempt attempt;
        attempt.setCharsLexed(0);
        return attempt;
    }

    std::string toCheck;
    int max_len = 3;
    for (int i = 0; i < max_len; i++) {
        toCheck += code[i];
    }

    std::vector<std::string> punctuators = {
        "[", "]", "(", ")", "{", "}", ".", "->",
        "++", "--", "&", "*", "+", "-", "~", "!",
        "/", "%", "<<", ">>",
        "<", ">", "<=", ">=", "==", "!=",
        "^", "|", "&&", "||",
        "?", ":", ";", "...",
        "=", "*=", "/=", "%=", "+=", "-=", "<<=", ">>=", "&=", "^=", "|=",
        ","
    };

    int lexedChars = 0;

    for (int i = max_len; i > 0; i++) {
        lexedChars++;
        for (const std::string& s : punctuators) {
            if (toCheck == s) {
                // initialize new token
                Token found;
                found.setTokenType("punctuator");
                found.setValue(toCheck);

                // initialize new TokenizeAttempt
                TokenizeAttempt validAttempt;
                validAttempt.setToken(found);
                validAttempt.setCharsLexed(lexedChars);

                return validAttempt;
            }
        }
        toCheck.pop_back();
    }
    return TokenizeAttempt();
}


