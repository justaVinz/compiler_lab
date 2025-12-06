#ifndef COMPILER_LAB_TOKENIZEHELPER_H
#define COMPILER_LAB_TOKENIZEHELPER_H

#include "../helper/structs/TokenizeAttempt.h"


class TokenizeHelper {
public:
    static TokenizeAttempt tokenizeStringLiterals(const char* codePointer);
    static TokenizeAttempt tokenizeKeywordPunctuators(const char* codePointer);
    static TokenizeAttempt tokenizeCharacterConstants(const char* punctString);
    static TokenizeAttempt tokenizeDecimalConstants(const char* punctString);
    static TokenizeAttempt tokenizeIdentifier(const char* code);
};


#endif //COMPILER_LAB_TOKENIZEHELPER_H