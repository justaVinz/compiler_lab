#include "TokenizeHelper.h"

#include <iostream>
#include <ostream>

#include "../helper/structs/TokenizeAttempt.h"

TokenizeAttempt TokenizeHelper::tokenizeStringLiterals(std::string& code) {
    std::cout << code[0] << std::endl;

    if (code.size() < 2 || code[0] != '"') {
        return TokenizeAttempt();
    }

    std::string current;
    int charsLexed = 0;
    bool escaped = false;

    current += code[0];
    charsLexed++;

    for (size_t i = 1; i < code.size(); ++i) {
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

bool isIdentifierNonDigit(char c) {
    return (
        c == '_'
        || ('a' <= c && c <= 'z')
        || ('A' <= c && c <= 'Z')
    );
}

bool isDigit_orIdentifierNonDigit(char c) {
    return ('0'<= c && c <= '9') || isIdentifierNonDigit(c);
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


TokenizeAttempt TokenizeHelper::decimalConstants(char* code) {

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