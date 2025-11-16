#include <iostream>
#include <fstream>
#include <string>
#include <optional>

#include "./TokenizeHelper.h"
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
    std::string testString = "->Test";
    std::optional<TokenizeAttempt> test = TokenizeHelper::tokenizeKeywordPunctuators(testString);
    if (test) {
        std::cout << *test << std::endl; // Dereferenzieren
    } else {
        std::cout << "Kein Token gefunden" << std::endl;
    }
    return 0;
}
