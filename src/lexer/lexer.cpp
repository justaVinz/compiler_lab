#include <iostream>
#include <fstream>
#include <string>

#include "../helper/structs/TokenizeAttempt.h"
#include "./../helper/structs/Token.h"

#define TEST_PATH "./test/lexer/test_1.c"


int main() {
    std::cout << "Start" << std::endl;
    Token token("IDENTIFIER", "var", 3, 3);
    std::cout << "Token created" << std::endl;
    std::cout << token << std::endl;
    TokenizeAttempt attempt(token, 3);
    std::cout << "TokenizeAttempt created" << std::endl;
    std::cout << attempt << std::endl;
    std::cout << "Token output done" << std::endl;
}
