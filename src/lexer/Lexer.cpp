#include <iostream>
#include <fstream>
#include <string>

#include "Tokenizer.h"
#include "./../helper/structs/Token.h"
#include "./../helper/Utils.h"

#define TEST_PATH "./test/lexer/"

void printTokens(std::vector<Token> tokens, std::string fileName) {
    for (const Token& token : tokens) {
        if(token.getTokenType() == "EOF") {
            continue;
        }
        std::cout << fileName << ":"
            << token.getSourceLine()+1 << ":"
            << token.getSourceIndex()+1 << ": "
            << token.getTokenType() << " "
            << token.getValue() << std::endl;
        }
}

void run(const std::string& fileName, const std::string& path, bool isVerbose) {
    std::string sourceCode = Utils::readSourceCode(path);
    sourceCode += '\0';
    auto sequence = Tokenizer::tokenizeSeq(sourceCode, isVerbose);
    // error appeared
    if (sequence.second.has_value()) {
        int a = sequence.second->first;
        int b = sequence.second->second;
        printTokens(sequence.first, fileName);
        std::cerr << "Lexer Error at line:" << a+1 << ":" << b+1 << std::endl;
    }
    // print tokens
    else {
        printTokens(sequence.first, fileName);
    }
}
