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

bool isDigit_orIdentifierNonDigit(char c) {
    return ('0'<= c && c <= '9') || isIdentifierNonDigit(c);
}
bool isIdentifierNonDigit(char c) {
    return (
        c == '_'
        || ('a' <= c && c <= 'z')
        || ('A' <= c && c <= 'Z')
    );
}

TokenizeAttempt TokenizeHelper::tokenizeCharacterConstants(std::string& code) {
    if (code.size() < 3 || code[0] != '\'') {
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
        } else if (c == '\'') {
            current += c;

            Token token;
            token.setTokenType("character-constant");
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

TokenizeAttempt TokenizeHelper::decimalConstants(std::string& code) {
    if (code.empty() || !('0' <= code[0] && code[0] <= '9')) {
        return TokenizeAttempt();
    }

    if (code[0] == '0' && code.size() > 1 && ('0' <= code[1] && code[1] <= '9')) {
        return TokenizeAttempt();
    }

    std::string current;
    int charsLexed = 0;
    for (size_t i = 0; i < code.size(); ++i) {
        char c = code[i];
        if ('0' <= c && c <= '9') {
            current += c;
            charsLexed++;
        } else {
            break;
        }
    }

    Token token;
    token.setTokenType("decimal-constant");
    token.setValue(current);

    TokenizeAttempt attempt;
    attempt.setToken(token);
    attempt.setCharsLexed(charsLexed);

    return attempt;
}