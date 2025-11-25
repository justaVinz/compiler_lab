#include <iostream>
#include <stack>
#include "../helper/structs/Node.h"
#include "Parser.h"
#include "../helper/structs/Token.h"

int parser_test() {
    Token t1("IDENT", "x", 1, 1);
    Token t2("PLUS", "+", 1, 2);
    Token t3("IDENT", "y", 1, 3);

    std::stack<Token> tokenStack;
    tokenStack.push(t3);
    tokenStack.push(t2);
    tokenStack.push(t1);

    std::stack<Node> nodeStack;

    Parser parser(tokenStack, nodeStack);

    std::cout << "Lookahead 1: " << parser.lookahead(1).getValue() << std::endl;
    std::cout << "Lookahead 2: " << parser.lookahead(2).getValue() << std::endl;

    Token tok = parser.consume();
    Node leaf1(DUMMY, std::vector<Node>());
    leaf1.setToken(tok);
    nodeStack.push(leaf1);

    tok = parser.consume();
    Node leaf2(DUMMY, std::vector<Node>());
    leaf2.setToken(tok);
    nodeStack.push(leaf2);

    tok = parser.consume();
    Node leaf3(DUMMY, std::vector<Node>());
    leaf3.setToken(tok);
    nodeStack.push(leaf3);

    std::cout << "Node-Stack Size: " << nodeStack.size() << std::endl;
    while (!nodeStack.empty()) {
        Node n = nodeStack.top();
        nodeStack.pop();
        std::cout << "Node with Token: " << n.getToken().getValue() << std::endl;
    }

    return 0;
}
