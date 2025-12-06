#include "Parser.h"
#include <iostream>
#include <ostream>
#include <vector>
#include "../helper/structs/Node.h"
#include "../helper/structs/Token.h"
#include "../helper/Symbol.h"
#include <assert.h>
#include "../helper/Utils.h"
#include "../lexer/Tokenizer.h"


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

void Parser::run(const std::string& fileName, const std::string& path, bool isVerbose) { //This is a static method, just to be clear
        std::string fullPath = "test/lexer/" + fileName;
        std::string sourceCode = Utils::readSourceCode(fullPath);
        sourceCode += '\0';
        auto sequence = Tokenizer::tokenizeSeq(sourceCode, false);
        
        if (sequence.second.has_value()) {
            int a = sequence.second->first;
            int b = sequence.second->second;
            std::cerr << "Lexer Error at line:" << a+1 << ":" << b+1 << std::endl;
            return;
        }

        std::vector<Token> tokens = sequence.first;
        Parser parser(tokens);
        parser.parse(isVerbose);
        return;
}

void Parser::parse(bool isVerbose) {
    while(!remSymbols.empty() && !remTokens.empty()) {
        std::optional<Node> changedNode = parseSymbol();
        if(!changedNode.has_value()) {
            std::cout << "Parsing Error at Token " << remTokens.back();
            return;
        }
        if(isVerbose) {
            std::cout << changedNode.value() << '\n'; //this should always show the production used.
        }
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
        std::cout << "Failure. Leftover Tokens or Symbols. The otherone is empty" << '\n';
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
        case declarator:
            if(next.getValue()=="*") {
                symbol.addChild(pointer);
                symbol.addChild(directdec);
                return symbol;
            } else {
                symbol.addChild(directdec);
                return symbol;
            }
        case pointer:
            symbol.addChild(std::string("*"));
            symbol.addChild(pointer_);
            return symbol;
        case pointer_:
            if(next.getValue()=="*") {
                symbol.addChild(std::string("*"));
                symbol.addChild(pointer_);
                return symbol;
            } else {
                return symbol;
            }
        case type:
            if(next.getValue()=="void") {
                symbol.addChild(std::string("void"));
                return symbol;
            } else if(next.getValue()=="char") {
                symbol.addChild(std::string("char"));
                return symbol;
            } else if(next.getValue()=="int") {
                symbol.addChild(std::string("int"));
                return symbol;
            } else if(next.getValue()=="struct") {
                symbol.addChild(structtype);
                return symbol;
            } else {
                return std::nullopt;
            }
        case structtype:
            if(peek(1).getValue()=="{") {
                symbol.addChild(std::string("struct"));
                symbol.addChild(std::string("{"));
                symbol.addChild(structdeclist);
                symbol.addChild(std::string("}"));
                return symbol;
            } else if(peek(1).getTokenType()=="identifier") {
                if(peek(2).getValue()=="{") {
                    symbol.addChild(std::string("struct"));
                    symbol.addChild(id);
                    symbol.addChild(std::string("{"));
                    symbol.addChild(structdeclist);
                    symbol.addChild(std::string("}"));
                } else {
                    symbol.addChild(std::string("struct"));
                    symbol.addChild(id);
                }
            } else {
                return std::nullopt;
            }
        case structdeclist:
            symbol.addChild(dec);
            symbol.addChild(structdeclist_);
            return symbol;
        case structdeclist_:
            if(next.getValue()=="}") {
                return symbol;
            } else {
                symbol.addChild(dec);
                symbol.addChild(structdeclist_);
                return symbol;
            }
        case dec:
            symbol.addChild(type);
            symbol.addChild(dec_);
            return symbol;
        case dec_:
            if(next.getValue()==";") {
                symbol.addChild(decEnd);
                return symbol;
            } else {
                symbol.addChild(declarator);
                symbol.addChild(decEnd);
            }
        case directdec:
            if(next.getValue()=="(") {
                symbol.addChild(std::string("("));
                symbol.addChild(declarator);
                symbol.addChild(std::string(")"));
                symbol.addChild(directdec_);
                return symbol;
            } else {
                symbol.addChild(id);
                symbol.addChild(directdec_);
                return symbol;
            }
        case directdec_:
            if(next.getValue()=="(") {
                symbol.addChild(std::string("("));
                symbol.addChild(paramlist);
                symbol.addChild(std::string(")"));
                symbol.addChild(directdec_);
                return symbol;
            } else {
                return symbol;
            }
        case paramlist:
            symbol.addChild(paramdec);
            symbol.addChild(paramlist_);
            return symbol;
        case paramlist_:
            if(next.getValue()=="(") {
                return symbol;
            } else {
                symbol.addChild(std::string(","));
                symbol.addChild(paramdec);
                symbol.addChild(paramlist_);
                return symbol;
            }
        case paramdec:
            symbol.addChild(type);
            symbol.addChild(paramdec_);
            return symbol;
        case paramdec_: // I can't do this in LL(k) easily, declarator will always yield id before ')' and vice versa for abstract-dec
            if(peek(0).getValue() == "," || peek(0).getValue() == ")") {
                return symbol;
            }
            { //namespace for k
            int k = 0;
            while(peek(k).getValue() != "EOF") {
                if(peek(k).getTokenType() == "identifier") {
                    symbol.addChild(declarator);
                    return symbol;
                }
                if(peek(k).getValue() == ")") {
                    symbol.addChild(abstractdeclarator);
                    return symbol;
                }
                k++;
            }
            }
            return std::nullopt;
        case abstractdeclarator:
            if(next.getValue() == "*") {
                symbol.addChild(pointer);
                symbol.addChild(abstractdeclarator_);
                return symbol;
            } else {
                symbol.addChild(directabstractdeclarator);
            }
        case abstractdeclarator_:
            if(next.getValue() == "(") {
                symbol.addChild(directabstractdeclarator);
                return symbol;
            } else {
                return symbol;
            }
        case directabstractdeclarator:
            if(next.getValue() == "(" && (peek(1).getValue()=="void" || peek(1).getValue()=="char" || peek(1).getValue()=="int" || peek(1).getValue()=="struct")) {
                symbol.addChild(std::string("("));
                symbol.addChild(paramlist);
                symbol.addChild(std::string(")"));
                symbol.addChild(directabstractdeclarator_);
                return symbol;
            } else {
                symbol.addChild(std::string("("));
                symbol.addChild(abstractdeclarator);
                symbol.addChild(std::string(")"));
                symbol.addChild(directabstractdeclarator_);
                return symbol;
            }
        case directabstractdeclarator_: //follow is ')' and ',' first is '(' which makes for easier seperation
            if(next.getValue()=="(") {
                symbol.addChild(std::string("("));
                symbol.addChild(paramlist);
                symbol.addChild(std::string(")"));
                symbol.addChild(directabstractdeclarator_);
                return symbol;
            } else {
                return symbol;
            }




    }
    std::cout << "Unhandled case found: " << symbol.getType();
    return std::nullopt;
}