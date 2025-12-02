#include "ParserRunner.h"

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "Parser.h"
#include "../helper/Utils.h"
#include "../helper/structs/Token.h"
#include "../lexer/Tokenizer.h"

namespace {
void printTokens(const std::vector<Token>& tokens, const std::string& fileName) {
    for (const auto& token : tokens) {
        if (token.getTokenType() == "EOF") continue;
        std::cout << fileName << ":" << token.getSourceLine() + 1 << ":" << token.getSourceIndex() + 1
                  << ": " << token.getTokenType() << " " << token.getValue() << std::endl;
    }
}
}

bool runParser(const std::string& fileName, const std::string& path, bool isVerbose) {
    std::string sourceCode = Utils::readSourceCode(path);
    auto sequence = Tokenizer::tokenizeSeq(sourceCode, false);

    if (sequence.second.has_value()) {
        if (isVerbose) {
            printTokens(sequence.first, fileName);
        }
        std::cerr << "Lexer Error at line:" << sequence.second->first + 1 << ":" << sequence.second->second + 1
                  << std::endl;
        return false;
    }

    Parser parser(sequence.first);
    bool ok = parser.parse();
    if (!ok) {
        std::cerr << "Parse error: " << parser.getErrorMessage();
        auto location = parser.getErrorToken();
        int line = 1;
        int column = 1;
        if (location) {
            line = location->getSourceLine() + 1;
            column = location->getSourceIndex() + 1;
        }
        if (isVerbose) {
            printTokens(sequence.first, fileName);
        }
        std::cerr << "Parser Error at line:" << line << ":" << column << std::endl;
        return false;
    }

    if (isVerbose) {
        std::cout << "Parsing successful" << std::endl;
    }
    return true;
}