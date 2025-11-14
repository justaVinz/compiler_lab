
#ifndef COMPILER_LAB_TOKENIZEHELPER_H
#define COMPILER_LAB_TOKENIZEHELPER_H
#include <string>

#include "../helper/structs/TokenizeAttempt.h"


class TokenizeHelper {
public:
    static TokenizeAttempt tokenizeStringIdentifiers(char* identString);
    static TokenizeAttempt tokenizeKeywordPunctuators(char* punctString);
};


#endif //COMPILER_LAB_TOKENIZEHELPER_H