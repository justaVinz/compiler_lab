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

TokenizeAttempt TokenizeHelper::tokenizeCharacterConstants(char* code) {
    if (code == nullptr) {
        return TokenizeAttempt();
    }

    if (code[0] != '\'') {
        return TokenizeAttempt();
    }

    std::string current;
    bool escaped = false;

    current += code[0];
    int charsLexed = 1;

    size_t i = 1;
    while (code[i] != '\0') {
        char c = code[i];

        if (escaped) {
            if (!(c == 'n' || c == 't' || c == 'r' || c == 'b' ||
            c == 'f' || c == 'v' || c == 'a' ||
            c == '\\' || c == '\'' || c == '\"' || c == '?')) {
                // invalid escape sequence
                break;
            }
            current += c;
            escaped = false;
        } else if (c == '\\') {
            if(charsLexed != 1){
                // escape must be at set position
                break;
            }
            escaped = true;
            current += c;
        } else if (c == '\'') {
            if(charsLexed != 2 && charsLexed != 3){
                // closing quotes must be at set position
                break;
            }
            current += c;
            ++i;
            ++charsLexed;

            Token token;
            token.setTokenType("character-constant");
            token.setValue(current);

            TokenizeAttempt attempt;
            attempt.setToken(token);
            attempt.setCharsLexed(charsLexed);
            return attempt;
        } else {
            if(charsLexed != 1 || c == '\n'){
                // normal characters should be at fixed position
                break;
            }
            current += c;
        }

        ++i;
        ++charsLexed;
    }

    TokenizeAttempt attempt;
    attempt.setCharsLexed(charsLexed);
    return attempt;
}


TokenizeAttempt TokenizeHelper::tokenizeDecimalConstants(char* code) {

    if (code == nullptr) {
        return TokenizeAttempt();
    }

    std::string current;
    int charsLexed = 0;

    size_t i = 0;
    while (code[i] != '\0') {
        char c = code[i];
        if(i == 1){
            if (code[0] == '0' && ('0' <= c && c <= '9')) {
                TokenizeAttempt attempt;
                attempt.setCharsLexed(charsLexed);

                return attempt;
            }
        }
        if ('0' <= c && c <= '9') {
            current += c;
            charsLexed++;
        } else {
            break;
        }
        ++i;
    }

    if (charsLexed == 0) {
        return TokenizeAttempt();
    }

    Token token;
    token.setTokenType("decimal-constant");
    token.setValue(current);

    TokenizeAttempt attempt;
    attempt.setToken(token);
    attempt.setCharsLexed(charsLexed);

    return attempt;
}


