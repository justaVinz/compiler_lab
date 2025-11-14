#include "TokenizeHelper.h"

#include <iostream>
#include <ostream>

#include "structs/TokenizeAttempt.h"

TokenizeAttempt TokenizeHelper::tokenizeStringIdentifiers(std::string& code) {
    return {};
}

TokenizeAttempt TokenizeHelper::tokenizeKeywordPunctuators(std::string& code) {
    // define punctuators
    std::string punctuator = code.substr(0, 3);
    std::vector<std::string> subString;

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

    for (int i = 0; i < 3; i++) {
        lexedChars++;
        for (std::string s : punctuators) {
            if(punctuator == s) {
                // initialize new token
                Token found = Token();
                found.setTokenType("PUNCTUATOR");
                found.setValue(punctuator);

                // initialize new TokenizeAttempt
                TokenizeAttempt validAttempt = TokenizeAttempt();
                validAttempt.setToken(found);
                validAttempt.setCharsLexed(lexedChars);

                return validAttempt;
            }
        }
        punctuator.pop_back();
    }
    return TokenizeAttempt();
}

