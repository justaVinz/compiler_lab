#include "Parser.h"
#include <iostream>
#include <ostream>
#include <vector>
#include "../helper/structs/Node.h"
#include "../helper/structs/Token.h"
#include "../helper/Symbol.h"
#include <assert.h>


// Constructors
Parser::Parser(std::vector<Token> tokens) {
    while (!tokens.empty()) { //This switches the order s.t. the token accessible by back is the first token that needs parsing
        remTokens.push_back(tokens.back());
        tokens.pop_back();
    }
    remSymbols.push_back(Symbol::start);
}

Parser::Parser() : remSymbols(), parseTree() {}

// Lookahead k
Node Parser::peekSymbol(int k) { //This isn't a standard thing of ll(k) parsers, i hope i don't need to use it.
    return remSymbols.at(remSymbols.size()-k-1);
}
Token Parser::peek(int k) {
    return remTokens.at(remSymbols.size()-k-1);
}

void Parser::parse() {
    while(!remSymbols.empty() && !remTokens.empty()) {
        std::optional<Node> changedNode = parseSymbol();
        if(!changedNode.has_value()) {
            std::cout << "Parsing Error at Token " << remTokens.back();
        }
        std::cout << changedNode.value() << '\n'; //this should always show the production used.
        parseTree.push_back(changedNode.value());
        remSymbols.pop_back();
        for(int i = changedNode->getChildren().size()-1; i>=0; i--) {
            remSymbols.push_back(changedNode->getChildren().at(i));
        }
    }
    if(remSymbols.empty() && remTokens.empty()) {
        //Success!
        return;
    } else {
        std::cout << "Failure. Leftover Tokens or Symbols." << '\n';
        return;
    }
}
std::optional<Node> Parser::parseSymbol() {
    Node symbol = remSymbols.back();
    Token next = remTokens.back(); //Slightly easier access to next token for LL(1) purposes.
    switch(symbol.getType()) {
        case terminal:
            assert(symbol.getToken()); //A terminal symbol (node object) needs to contain a token.
            if(symbol.getToken()->getValue()==next.getValue()) {
                symbol.setToken(next);
                remTokens.pop_back();
                return symbol;
            } else {
                return std::nullopt;
            }
        case literal:
            if(next.getTokenType()=="string-literal") {
                symbol.setToken(next);
                remTokens.pop_back();
                return symbol;
            }
            if(next.getTokenType()=="character-constant") {
                symbol.setToken(next);
                remTokens.pop_back();
                return symbol;
            }
            if(next.getTokenType()=="decimal-constant") {
                symbol.setToken(next);
                remTokens.pop_back();
                return symbol;
            }
            return std::nullopt;
        case id:
            if(next.getTokenType()=="identifier") {
                symbol.setToken(next);
                remTokens.pop_back();
                return symbol;
            }
            return std::nullopt;
        case start:
            symbol.addChild(transunit);
            return symbol;
        case transunit:
            symbol.addChild(extdec);
            symbol.addChild(transunit_);
            return symbol;
        case transunit_:
            if(next.getValue() == "EOF") {
                return symbol;
            } else {
                symbol.addChild(extdec);
                symbol.addChild(transunit_);
                return symbol;
            }
        case extdec:
            symbol.addChild(type);
            symbol.addChild(extdec_);
            return symbol;
        case extdec_:
            if(next.getValue()==";") {
                symbol.addChild(decEnd);
                return symbol;
            } else {
                symbol.addChild(declarator);
                symbol.addChild(extdec__);
            }
        case extdec__:
            if(next.getValue()==";") {
                symbol.addChild(decEnd);
                return symbol;
            } else {
                symbol.addChild(funcdef_);
            }
        case decEnd:
            symbol.addChild(std::string(";"));
            return symbol;
    }
    std::cout << "Unhandled case found: " << symbol.getType();
    return std::nullopt;
}