# include "Tokenizer.h"
#include <cstddef>
#include <iostream>
#include <ostream>

# include "TokenizeHelper.h"
#include <utility>


void updateCandidateTokenizeAttempt(TokenizeAttempt* dest, TokenizeAttempt src) {
    if(src.getToken() && !dest->getToken()){ //First time lexing a token, we replace
        *dest = src;
        return;
    }
    if(!src.getToken() && dest->getToken()) { //We had a token, new function doesn't produce one, abort
        return;
    }
    if(src.getCharsLexed() > dest->getCharsLexed()) { //both or neither have a token, charsLexed determines what happens next.
        *dest = src;
        return;
    }
}
TokenizeAttempt Tokenizer::tokenize(const char* src, bool isVerbose) {
    TokenizeAttempt bestTokenizeAttempt = TokenizeAttempt(); //start with no tokens, then go through the functions
    updateCandidateTokenizeAttempt(&bestTokenizeAttempt, TokenizeHelper::tokenizeKeywordPunctuators(src)); //keywords first
    updateCandidateTokenizeAttempt(&bestTokenizeAttempt, TokenizeHelper::tokenizeCharacterConstants(src));
    updateCandidateTokenizeAttempt(&bestTokenizeAttempt, TokenizeHelper::tokenizeDecimalConstants(src));
    updateCandidateTokenizeAttempt(&bestTokenizeAttempt, TokenizeHelper::tokenizeIdentifier(src));
    updateCandidateTokenizeAttempt(&bestTokenizeAttempt, TokenizeHelper::tokenizeStringLiterals(src));
    if(isVerbose)
        std::cout << bestTokenizeAttempt << std::endl;
    return bestTokenizeAttempt;
}


std::pair<std::vector<Token>, std::optional<std::pair<int, int>>> Tokenizer::tokenizeSeq(std::string source, bool isVerbose) {
    const char* sourceP = source.c_str();
    std::vector<Token> tokens;
    int line = 0;
    int pos = 0;

    while(true) {
        char c = *sourceP;

        if(c == '\0') {
            tokens.push_back(Token("EOF", "EOF", line, pos));
            return {tokens, std::optional<std::pair<int,int>>{}};
        }

        if(c == '\n') {
            line++;
            pos = 0;
            sourceP++;
            continue;
        }
        else if(std::isspace(c)) { // whitespace außer '\n'
            pos++;
            sourceP++;
            continue;
        }

        TokenizeAttempt attempt = tokenize(sourceP, isVerbose);

        if(!attempt.getToken()) {
            int lexed = attempt.getCharsLexed();
            pos += lexed;
            sourceP += lexed;
            return {tokens, std::optional<std::pair<int,int>>{ {line, pos} }};
        }

        int lexed = attempt.getCharsLexed();
        Token token = *attempt.getToken();
        token.setSourceLine(line);
        token.setSourceIndex(pos);

        tokens.push_back(token);
        pos += lexed;
        sourceP += lexed;
    }
}
