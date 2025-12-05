#include "Parser.h"
#include <vector>
#include "../helper/structs/Node.h"
#include "../helper/structs/Token.h"
#include "../helper/Symbol.h"
#include <assert.h>


// Constructors
Parser::Parser(std::vector<Token> symbols) {
    for(Token symbol:symbols) {
        Symbol typ = Symbol::TODO;
        remSymbols.push_back(Node(typ, std::vector<Node>(), symbol));
    }
}

Parser::Parser() : remSymbols(), parseTree() {}

// Lookahead k
Node Parser::lookahead(int k) {
    return remSymbols.at(remSymbols.size()-k-1);
}

// Consume top token
void Parser::consume() {
    assert(!remSymbols.empty()); // Error Token
    remSymbols.pop_back();
}
