
#ifndef COMPILER_LAB_TOKENIZEHELPER_H
#define COMPILER_LAB_TOKENIZEHELPER_H
#include <string>

#include "../helper/structs/TokenizeAttempt.h"


class TokenizeHelper {
public:
    static TokenizeAttempt tokenizeStringLiterals(std::string& identString);
    static TokenizeAttempt tokenizeKeywordPunctuators(std::string& punctString);
};


#endif //COMPILER_LAB_TOKENIZEHELPER_H