#ifndef COMPILER_LAB_PARSER_H
#define COMPILER_LAB_PARSER_H
#include "../helper/structs/Token.h"
#include "../helper/structs/Node.h"
#include <vector>

class Parser {
public:
    Parser(std::vector<Token> remSymbols); //remaining symbols
    Parser();

    Node lookahead(int k);
    void consume(); // for terminals

    std::vector<Node> getRemTokens() const { return remSymbols;}
    std::vector<Node> getParseTree() const { return parseTree;}

private:
    std::vector<Node> remSymbols;
    std::vector<Node> parseTree;
};
#endif //COMPILER_LAB_PARSER_H
