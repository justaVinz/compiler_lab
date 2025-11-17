
#ifndef COMPILER_LAB_TOKENIZEHELPER_H
#define COMPILER_LAB_TOKENIZEHELPER_H
#include <string>

#include "../helper/structs/TokenizeAttempt.h"


class TokenizeHelper {
public:
    static TokenizeAttempt tokenizeStringLiterals(char* codePointer);
    static TokenizeAttempt tokenizeKeywordPunctuators(char* codePointer);
};


#endif //COMPILER_LAB_TOKENIZEHELPER_H