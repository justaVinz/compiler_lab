# include "Tokenizer.h"
#include <cstddef>
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
TokenizeAttempt Tokenizer::tokenize(const char* src) {
    TokenizeAttempt bestTokenizeAttempt = TokenizeAttempt(); //start with no tokens, then go through the functions
    updateCandidateTokenizeAttempt(&bestTokenizeAttempt, TokenizeHelper::tokenizeKeywordPunctuators(src)); //keywords first
    updateCandidateTokenizeAttempt(&bestTokenizeAttempt, TokenizeHelper::tokenizeCharacterConstants(src));
    updateCandidateTokenizeAttempt(&bestTokenizeAttempt, TokenizeHelper::tokenizeDecimalConstants(src));
    updateCandidateTokenizeAttempt(&bestTokenizeAttempt, TokenizeHelper::tokenizeIdentifier(src));
    updateCandidateTokenizeAttempt(&bestTokenizeAttempt, TokenizeHelper::tokenizeStringLiterals(src));
    return bestTokenizeAttempt;
}

std::pair<std::vector<Token>, std::optional<std::pair<int, int>>> Tokenizer::tokenizeSeq(std::string source) {
    const char* sourceP = source.c_str();
    std::vector<Token> tokens;
    int line = 0;
    int pos = 0;
    while(true) {
        char c = *sourceP;
        if(c == '\0') {                                                             //success, source has been fully lexed
            tokens.push_back(Token("EOF", "EOF",line,pos));
            return std::pair(tokens, std::nullopt);
        }
        if(c == '\n') { //new line in source
            line++;
            pos = 0;
            sourceP++;
            continue;
        }
        if(std::isspace(c)) { //checks whitespace
            pos++;
            sourceP++;
            continue;
        }
        TokenizeAttempt attempt = tokenize(sourceP);
        if(!attempt.getToken()) {
            pos += attempt.getCharsLexed();
            return std::pair(tokens, std::pair(line, pos)); //failure, contains first unlexable char position.
        }
        Token token = *attempt.getToken();
        token.setSourceLine(line);
        token.setSourceIndex(pos);

        tokens.push_back(*attempt.getToken());
        pos += attempt.getCharsLexed();
    }
}