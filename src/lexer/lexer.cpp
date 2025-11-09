#include <iostream>
#include <fstream>
#include <string>

#include "LexerHelper.h"

#define TEST_PATH "./test/lexer/test_1.c"

int main() {
    std::string line = "if (x > 0) { for } else";
    // static std::string sourceCode = Utils::readSourceCode(TEST_PATH);
    static std::vector<std::string> tokens = LexerHelper::getTokens(line);

    for (std::string token : tokens) {
        std::cout << token << " ";
    }

    return 0;
}
