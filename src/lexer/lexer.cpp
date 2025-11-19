#include <iostream>
#include <fstream>
#include <string>
#include <optional>

#include "TokenizeHelper.h"
#include "../helper/structs/TokenizeAttempt.h"
#include "./../helper/structs/Token.h"

#define TEST_PATH "./test/lexer/testTokenizeKeywordPunctuators.c"


int main() {
    /*
    std::cout << "Start" << std::endl;
    Token token("IDENTIFIER", "var", 3, 3);
    std::cout << "Token created" << std::endl;
    std::cout << token << std::endl;
    TokenizeAttempt attempt(token, 3);
    std::cout << "TokenizeAttempt created" << std::endl;
    std::cout << attempt << std::endl;
    std::cout << "Token output done" << std::endl;
    */
    char testString[] = "->Test";
    char testString1[] = "\"bla\\n\"";

    char testString3[] = "'\\n'";
    char testString4[] = "0";
;

    TokenizeAttempt test = TokenizeAttempt();
    std::cout << test << std::endl;

    TokenizeAttempt test1 = TokenizeHelper::tokenizeKeywordPunctuators(testString);
    TokenizeAttempt test2 = TokenizeHelper::tokenizeStringLiterals(testString1);

    TokenizeAttempt test3 = TokenizeHelper::tokenizeCharacterConstants(testString3);
    TokenizeAttempt test4 = TokenizeHelper::tokenizeDecimalConstants(testString4);

    std::cout << test1 << std::endl;
    std::cout << test2 << std::endl;

    std::cout << test3 << std::endl;
    std::cout << test4 << std::endl;

    return 0;
}
