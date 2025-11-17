#include "TokenizeHelper.h"

#include <iostream>
#include <ostream>

#include "../helper/structs/TokenizeAttempt.h"

TokenizeAttempt TokenizeHelper::tokenizeStringLiterals(char* code) {
    if (code == nullptr || code[0] != '"') {
        return TokenizeAttempt();
    }

    std::string current;
    int charsLexed = 0;
    bool escaped = false;

    current += code[0];
    charsLexed++;

    size_t len = strlen(code);

    for (size_t i = 1; i < len; ++i) {
        char c = code[i];

        if (escaped) {
            current += c;
            escaped = false;
        } else if (c == '\\') {
            escaped = true;
            current += c;
        } else if (c == '"') {
            current += c;

            Token token;
            token.setTokenType("string-literal");
            token.setValue(current);

            TokenizeAttempt attempt;
            attempt.setToken(token);
            attempt.setCharsLexed(i + 1);

            return attempt;
        } else {
            current += c;
        }

        charsLexed++;
    }
    return TokenizeAttempt();
}


TokenizeAttempt TokenizeHelper::tokenizeKeywordPunctuators(char* code) {
    if (code == nullptr || code[0] == '\0') {
        return TokenizeAttempt();
    }

    std::string punctuator;
    int code_len = strlen(code);
    int max_len = (code_len < 3) ? code_len : 3;
    for (int i = 0; i < max_len; i++) {
        punctuator += code[i];
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

    for (int i = 0; i < max_len; i++) {
        lexedChars++;
        for (const std::string& s : punctuators) {
            if (punctuator == s) {
                // initialize new token
                Token found;
                found.setTokenType("punctuator");
                found.setValue(punctuator);

                // initialize new TokenizeAttempt
                TokenizeAttempt validAttempt;
                validAttempt.setToken(found);
                validAttempt.setCharsLexed(lexedChars);

                return validAttempt;
            }
        }
        punctuator.pop_back();
    }

    return TokenizeAttempt();
}


