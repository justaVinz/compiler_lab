#ifndef COMPILER_LAB_PARSER_H
#define COMPILER_LAB_PARSER_H
#include "../helper/structs/Token.h"
#include "../helper/structs/Node.h"
#include <stack>

class Parser {
public:
    Parser(std::stack<Token> tokensStack, std::stack<Node> nodesStack);
    Parser();

    Token lookahead(int k);
    Token consume(); // for terminals

    std::stack<Token> getTokens() const { return tokens; }
    void setTokens(const std::stack<Token> &tokens) { this->tokens = tokens; }
    std::stack<Node> getNodes() const { return nodes; }
    void setNodes(const std::stack<Node> &nodes) { this->nodes = nodes; }

private:
    std::stack<Token> tokens;
    std::stack<Node> nodes;
};
#endif //COMPILER_LAB_PARSER_H
