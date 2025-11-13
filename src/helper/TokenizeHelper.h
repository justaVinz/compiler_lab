
#ifndef COMPILER_LAB_TOKENIZEHELPER_H
#define COMPILER_LAB_TOKENIZEHELPER_H
#include <string>

#include "structs/TokenizeAttempt.h"


class TokenizeHelper {
public:
    static std::optional<TokenizeAttempt> tokenizeStringIdentifiers(std::string& identString);
    static std::optional<TokenizeAttempt> tokenizeKeywordPunctuators(std::string& punctString);
};


#endif //COMPILER_LAB_TOKENIZEHELPER_H