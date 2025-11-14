# include "Tokenizer.h"
#include <cstddef>
#include <utility>
TokenizeAttempt Tokenizer::tokenize(const char* source) {

}

std::pair<std::vector<Token>, std::optional<std::pair<int, int>>> Tokenizer::tokenizeSeq(std::string source) {
    const char* sourceP = source.c_str();
    std::vector<Token> tokens;
    int line = 0;
    int pos = 0;
    while(true) {
        char c = *sourceP;
        if(c == '\0') { //source has been fully lexed
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
        if(attempt.getToken())
    }
}