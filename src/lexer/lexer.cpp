#include <iostream>
#include <fstream>
#include <string>

#include "./../helper/structs/Token.h"

#define TEST_PATH "./test/lexer/test_1.c"


int main() {
    std::string line = "if (x > 0) { for } else";
    // static std::string sourceCode = Utils::readSourceCode(TEST_PATH);
    Token token("IDENTIFIER", "var", 3, 3);
    std::cout << token << std::endl;
    return 0;
}
