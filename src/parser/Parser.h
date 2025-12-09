#ifndef COMPILER_LAB_PARSER_H
#define COMPILER_LAB_PARSER_H
#include "../helper/structs/Token.h"
#include "../helper/structs/Node.h"
#include <vector>

class Parser {
public:
    Parser(std::vector<Token> tokens, bool isVerbose); //remaining symbols
    Parser();
    static void run(const std::string& fileName, const std::string& path, bool isVerbose);

    Node peekSymbol(int k);
    Token peek(int k);
    void dump_state();
    int parse();
    std::optional<Node> parseSymbol();

    std::vector<Token> getRemTokens() const { return remTokens;}
    std::vector<Node> getRemSymbols() const {return remSymbols;}
    std::vector<Node> getParseTree() const { return parseTree;}

private:
    std::vector<Token> remTokens;
    std::vector<Node> remSymbols;
    std::vector<Node> parseTree;
    bool isVerbose;
};
#endif //COMPILER_LAB_PARSER_H
