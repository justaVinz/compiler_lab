#include "TokenizeHelper.h"

#include <iostream>
#include <ostream>

#include "../helper/structs/TokenizeAttempt.h"

TokenizeAttempt TokenizeHelper::tokenizeStringLiterals(std::string& code) {
    if (code.size() < 2 || code[0] != '"') {
        std::cout << "test: " << code[0] << std::endl;
        std::cout << "reached here 1" << std::endl;
        return TokenizeAttempt();
    }

    std::string current;
    int charsLexed = 1;
    bool escaped = false;

    for (int i = 1; i < code.length(); i++) {
        char c = code[i];

        // ignore possible string endings
        if (escaped) {
            current += c;
            escaped = false;
        // setup for ignoring potential string endings
        } else if (c == '\\') {
            escaped = true;
        } else if (c == '"') {
            // Unescaped, end of string
            Token token;
            token.setTokenType("string-literal");
            token.setValue(current);

            TokenizeAttempt attempt;
            attempt.setToken(token);
            attempt.setCharsLexed(i + 1);

            std::cout << "reached here 2" << std::endl;
            return attempt;
        } else {
            current += c;
        }
        charsLexed++;
    }
    std::cout << "reached here" << std::endl;
    return TokenizeAttempt();
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
                found.setTokenType("punctuator");
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

