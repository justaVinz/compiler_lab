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


void print(std::vector<Token> tokens);
void print_shallow(Node node) {
    //std::cout << "Node$";
    std::cout << node.getType();
    if(node.getToken().has_value()) {
        std::cout << "-" << node.getToken()->getValue();
    }
}
void print(Node node) {
    print_shallow(node);
    if(node.getChildren().empty()) {
        return;
    }
    std::cout << " $";
    bool first = true;
    for(Node child:node.getChildren()) {
        if(!first)
            std::cout << " ";
        first = false;
        print_shallow(child);
    }
    std::cout << "$";

}

// Constructors
Parser::Parser(std::vector<Token> tokens, bool verbose) {
    while (!tokens.empty()) { //This switches the order s.t. the token accessible by back is the first token that needs parsing
        remTokens.push_back(tokens.back());
        tokens.pop_back();
    }
    remSymbols.push_back(Symbol::start);
    isVerbose=verbose;
}

Parser::Parser() : remSymbols(), parseTree() {
    isVerbose=false;
}

// Lookahead k
Node Parser::peekSymbol(int k) { //This isn't a standard thing of ll(k) parsers, i hope i don't need to use it.
    return remSymbols.at(remSymbols.size()-k-1);
}
Token Parser::peek(int k) {
    if(isVerbose)
        std::cout << "Peeked at " << remTokens.at(remTokens.size()-k-1) << "using k=" << k << '\n'; //TODO remove
    return remTokens.at(remTokens.size()-k-1);
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
        Parser parser(tokens, isVerbose);
        if(!parser.parse()) {
            std::cout << "Successfully parsed " << fileName << '\n';
        }
        return;
}

void print(std::vector<Token> tokens) {
    std::cout<<"Debug print of tokens\n";
    for(Token token:tokens)
        std::cout << token << '\n';
    std::cout <<"Debug print over\n";
}

void Parser::dump_state() {
    std::cout << "remTokens" << '\n';
    for(int i = remTokens.size()-1; i>=0; i--) {
        std::cout << remTokens.at(i) << '\n';
    }
    std::cout << "remSymbols" << '\n';
    for(int i = remSymbols.size()-1; i>=0; i--) {
        Node node = remSymbols.at(i);
        print_shallow(node);
        std::cout << '\n';
    }
}

int Parser::parse() {
    while(!remSymbols.empty() && !remTokens.empty()) {
        std::optional<Node> changedNode = parseSymbol();
        if(!changedNode.has_value()) {
            std::cout << "Parsing Error at Token " << remTokens.back() << '\n';
            dump_state();
            return 1;
        }
        if(isVerbose) {
            std::cout << '\t';
            print(changedNode.value());
            std::cout << '\n'; //this should always show the production used.
        }
        parseTree.push_back(changedNode.value());
        remSymbols.pop_back();
        for(int i = changedNode->getChildren().size()-1; i>=0; i--) {
            remSymbols.push_back(changedNode->getChildren().at(i));
        }
    }
    if(!remTokens.empty() && remTokens.back().getTokenType()=="EOF") {
        remTokens.pop_back();
    }
    if(remSymbols.empty() && remTokens.empty()) {
        //Success!
        return 0;
    } else {
        std::cout << "Failure. Leftover Tokens or Symbols. The otherone is empty" << '\n';
        dump_state();
        return 1;
    }
}
std::optional<Node> Parser::parseSymbol() {
    Node symbol = remSymbols.back();
    Token next = peek(0); //Slightly easier access to next token for LL(1) purposes.
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
                return symbol;
            }
        case extdec__:
            if(next.getValue()==";") {
                symbol.addChild(decEnd);
                return symbol;
            } else {
                symbol.addChild(funcdef_);
                return symbol;
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
                    return symbol;
                } else {
                    symbol.addChild(std::string("struct"));
                    symbol.addChild(id);
                    return symbol;
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
                return symbol;
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
            if(next.getValue()==")") {
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
                return symbol;
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
        case funcdef_:
            symbol.addChild(compoundstatement);
            return symbol;
        case compoundstatement:
            symbol.addChild(std::string("{"));
            symbol.addChild(blockitemlist);
            symbol.addChild(std::string("}"));
            return symbol;
        case blockitemlist:
            if(next.getValue() == "}") {
                return symbol;
            } else {
                symbol.addChild(blockitem);
                symbol.addChild(blockitemlist);
                return symbol;
            }
        case blockitem:
            if((next.getValue()=="void" || next.getValue()=="char" || next.getValue()=="int" || next.getValue()=="struct")) {
                symbol.addChild(dec);
                return symbol;
            } else {
                symbol.addChild(statement);
                return symbol;
            }
        case statement:
            if(next.getTokenType() == "identifier" && peek(1).getValue()==":") {
                symbol.addChild(labelstatement);
                return symbol;
            } else if(next.getValue() == "if") {
                symbol.addChild(selectstatement);
                return symbol;
            } else if(next.getValue() == "while") {
                symbol.addChild(iterstatement);
                return symbol;
            } else if(next.getValue() == "goto" || next.getValue() == "continue" || next.getValue() == "break" || next.getValue() == "return") {
                symbol.addChild(jumpstatement);
                return symbol;
            } else {
                symbol.addChild(exprstatement); // :(
                return symbol;
            }
        case labelstatement:
            symbol.addChild(id);
            symbol.addChild(std::string(":"));
            symbol.addChild(statement);
            return symbol;
        case selectstatement:
            symbol.addChild(std::string("if"));
            symbol.addChild(std::string("("));
            symbol.addChild(expr);
            symbol.addChild(std::string(")"));
            symbol.addChild(statement);
            symbol.addChild(selectstatement_);
            return symbol;
        case selectstatement_:
            if(next.getValue() == "else") {
                symbol.addChild(std::string("else"));
                symbol.addChild(statement);
                return symbol;
            } else {
                return symbol;
            }
        case iterstatement:
            symbol.addChild(std::string("while"));
            symbol.addChild(std::string("("));
            symbol.addChild(expr);
            symbol.addChild(std::string(")"));
            symbol.addChild(statement);
            return symbol;
        case jumpstatement:
            if(next.getValue() == "goto") {
                symbol.addChild(std::string("goto"));
                symbol.addChild(id);
                symbol.addChild(std::string(";"));
                return symbol;
            } else if(next.getValue() == "continue") {
                symbol.addChild(std::string("continue"));
                symbol.addChild(std::string(";"));
                return symbol;
            } else if(next.getValue() == "break") {
                symbol.addChild(std::string("break"));
                symbol.addChild(std::string(";"));
                return symbol;
            } else if(next.getValue() == "return" && peek(1).getValue() == ";") {
                symbol.addChild(std::string("return"));
                symbol.addChild(std::string(";"));
                return symbol;
            } else if(next.getValue() == "return") {
                symbol.addChild(std::string("return"));
                symbol.addChild(expr);
                symbol.addChild(std::string(";"));
                return symbol;
            } else { //no need to try anything here if it doesn't work, keeps unsuccessful parsing shorter ig.
                return std::nullopt;
            }
        case exprstatement:
            if(next.getValue()==";") {
                symbol.addChild(std::string(";"));
                return symbol;
            } else {
                symbol.addChild(expr);
                symbol.addChild(std::string(";"));
                return symbol;
            }





    }
    std::cout << "Unhandled case found: " << symbol.getType();
    return std::nullopt;
}