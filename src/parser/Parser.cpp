#include "Parser.h"

#include <stack>
#include "../helper/structs/Node.h"
#include "../helper/structs/Token.h"


// Constructors
Parser::Parser(std::stack<Token> tokensStack, std::stack<Node> nodesStack)
: tokens(std::move(tokensStack)), nodes(std::move(nodesStack)) {}

Parser::Parser() : tokens(), nodes() {}

// Lookahead k
Token Parser::lookahead(int k) {
    std::stack<Token> temp = tokens;

    Token t;

    for (int i = 0; i < k; ++i) {
        if (temp.empty()) return Token(); // Error Token
        t = temp.top();
        temp.pop();
    }

    return t;
}

// Consume top token
Token Parser::consume() {
    if (tokens.empty()) return Token(); // Error Token

    Token t = tokens.top();
    tokens.pop();
    return t;
}
