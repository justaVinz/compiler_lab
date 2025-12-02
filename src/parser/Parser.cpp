#include "Parser.h"

#include <stdexcept>
#include <unordered_set>

Parser::Parser(std::vector<Token> tokens)
    : tokens(std::move(tokens)) {}

const Token& Parser::current() const {
    if (position >= tokens.size()) {
        static Token eof("EOF", "EOF", 0, 0);
        return eof;
    }
    return tokens[position];
}

bool Parser::parse() {
    bool ok = translationUnit();
    if (ok && !atEnd()) {
        setError("Extra tokens after end of translation unit");
        return false;
    }
    return ok && atEnd();
}

bool Parser::atEnd() const {
    return current().getTokenType() == "EOF";
}

bool Parser::matchPunctuator(const std::string& value) {
    if (current().getTokenType() == "punctuator" && current().getValue() == value) {
        ++position;
        return true;
    }
    return false;
}

bool Parser::matchIdentifier(std::optional<std::string> expected) {
    if (current().getTokenType() == "identifier") {
        if (!expected || current().getValue() == *expected) {
            ++position;
            return true;
        }
    }
    return false;
}

bool Parser::matchKeyword(const std::string& keyword) {
    if (current().getTokenType() == "identifier" && current().getValue() == keyword) {
        ++position;
        return true;
    }
    return false;
}

bool Parser::matchTokenType(const std::string& type) {
    if (current().getTokenType() == type) {
        ++position;
        return true;
    }
    return false;
}

void Parser::setError(const std::string& message) {
    if (errorMessage.empty()) {
        errorMessage = message;
        if (!atEnd()) {
            errorToken = current();
        }
    }
}

bool Parser::parseTranslationUnit() {
    if (tokens.empty()) {
        setError("No tokens to parse");
        return false;
    }
    while (!atEnd()) {
        if (!parseExternalDeclaration()) {
            return false;
        }
    }
    return true;
}

bool Parser::parseExternalDeclaration() {
    std::vector<std::string> declSpecifiers;
    if (!parseDeclarationSpecifiers(declSpecifiers)) {
        setError("Expected declaration specifiers");
        return false;
    }

    // If the next token is ';', this is a pure type/struct declaration
    if (matchPunctuator(";")) {
        return true;
    }

    // Attempt to parse declarator to determine if function-definition
    size_t backup = position;
    bool isFunction = false;
    if (!parseDeclarator(isFunction)) {
        setError("Expected declarator");
        return false;
    }

    if (isFunction && matchPunctuator("{")) {
        --position; // rewind '{' for compound-statement parsing
        position = backup;
        return parseFunctionDefinition(declSpecifiers);
    }

    position = backup;
    return parseDeclaration(declSpecifiers);
}


bool Parser::parseFunctionDefinition(const std::vector<std::string>& declSpecifiers) {
    (void)declSpecifiers;
    bool isFunction = false;
    if (!parseDeclarator(isFunction) || !isFunction) {
        setError("Expected function declarator");
        return false;
    }

    // skip optional declaration-list (not supported in subset)
    if (!parseCompoundStatement()) {
        setError("Invalid function body");
        return false;
    }
    return true;
}

bool Parser::parseDeclaration(const std::vector<std::string>& declSpecifiers) {
    (void)declSpecifiers;

    // optional init-declarator-list
    if (matchPunctuator(";")) {
        // declaration-specifiers ;
        return true;
    }

    if (!parseInitDeclaratorList()) {
        setError("Invalid init-declarator list");
        return false;
    }
    if (!matchPunctuator(";")) {
        setError("Expected ';' after declaration");
        return false;
    }
    return true;
}


bool Parser::parseDeclarationSpecifiers(std::vector<std::string>& outSpecifiers) {
    bool parsedAny = false;
    while (!atEnd()) {
        const Token& tok = current();
        // struct-or-union-specifier (restricted to struct)
        if (tok.getTokenType() == "identifier" && tok.getValue() == "struct") {
            if (!parseStructSpecifier()) {
                return false;
            }
            parsedAny = true;
            outSpecifiers.push_back("struct");
            continue;
        }

        // simple type specifiers
        if (isTypeSpecifier(tok)) {
            parsedAny = true;
            outSpecifiers.push_back(tok.getValue());
            ++position;
            continue;
        }
        break;
    }
    return parsedAny;
}

bool Parser::parseInitDeclaratorList() {
    bool isFunctionDecl = false;
    if (!parseInitDeclarator(isFunctionDecl)) {
        return false;
    }
    if (matchPunctuator(",")) {
        setError("Only one init-declarator is allowed");
        return false;
    }
    return true;
}

bool Parser::parseInitDeclarator(bool& isFunctionDecl) {
    if (!parseDeclarator(isFunctionDecl)) {
        return false;
    }
    if (isFunctionDecl) {
        return true;
    }
    if (matchPunctuator("=")) {
        setError("initializer is not allowed");
        return false;
    }
    return true;
}

bool Parser::parseDeclarator(bool& isFunction) {
    isFunction = false;
    parsePointer();
    return parseDirectDeclarator(isFunction);
}

bool Parser::parsePointer() {
    while (matchPunctuator("*")) {
    }
    return true;
}

bool Parser::parseDirectDeclarator(bool& isFunction) {
    isFunction = false;
    if (matchPunctuator("(")) {
        if (!parseDeclarator(isFunction)) {
            return false;
        }
        if (!matchPunctuator(")")) {
            setError("Expected ')' after declarator");
            return false;
        }
    } else if (matchIdentifier()) {
        // identifier consumed
    } else {
        setError("Expected identifier in declarator");
        return false;
    }

    while (true) {
        if (matchPunctuator("(")) {
            if (!parseParameterTypeList()) {
                return false;
            }
            if (!matchPunctuator(")")) {
                setError("Expected ')' after parameter list");
                return false;
            }
            isFunction = true;
        } else {
            break;
        }
    }
    return true;
}

bool Parser::parseParameterTypeList() {
    if (matchPunctuator(")")) {
        --position;
        return true; // empty parameter list
    }
    if (!parseParameterDeclaration()) {
        return false;
    }
    while (matchPunctuator(",")) {
        if (!parseParameterDeclaration()) {
            return false;
        }
    }
    return true;
}

bool Parser::parseParameterDeclaration() {
    std::vector<std::string> specs;
    if (!parseDeclarationSpecifiers(specs)) {
        setError("Expected declaration specifiers in parameter");
        return false;
    }
    bool isFunction = false;
    if (!parseDeclarator(isFunction)) {
        setError("Expected declarator in parameter");
        return false;
    }
    return true;
}

bool Parser::parseStatement() {
    if (atEnd()) {
        setError("Unexpected end of input in statement");
        return false;
    }

    const Token& tok = current();
    if (tok.getTokenType() == "identifier" && tok.getValue() == "if") {
        return parseSelectionStatement();
    }
    if (tok.getTokenType() == "identifier" && tok.getValue() == "while") {
        return parseIterationStatement();
    }
    if (tok.getTokenType() == "identifier" && tok.getValue() == "return") {
        return parseJumpStatement();
    }
    if (tok.getTokenType() == "identifier" && tok.getValue() == "goto") {
        return parseJumpStatement();
    }
    if (tok.getTokenType() == "identifier" && tok.getValue() == "break") {
        return parseJumpStatement();
    }
    if (tok.getTokenType() == "identifier" && tok.getValue() == "continue") {
        return parseJumpStatement();
    }
    if (matchPunctuator("{")) {
        --position;
        return parseCompoundStatement();
    }
    if (matchIdentifier()) {
        if (matchPunctuator(":")) {
            --position; // rewind to start of label handling
            --position;
            return parseLabeledStatement();
        }
        --position;
    }
    return parseExpressionStatement();
}

bool Parser::parseLabeledStatement() {
    if (!matchIdentifier()) {
        setError("Expected identifier for label");
        return false;
    }
    if (!matchPunctuator(":")) {
        setError("Expected ':' after label");
        return false;
    }
    return parseStatement();
}

bool Parser::parseCompoundStatement() {
    if (!matchPunctuator("{")) {
        setError("Expected '{'");
        return false;
    }
    while (!atEnd() && !matchPunctuator("}")) {
        if (!parseBlockItem()) {
            return false;
        }
    }
    return true;
}

bool Parser::parseBlockItem() {
    if (isTypeSpecifier(current())) {
        std::vector<std::string> specs;
        if (!parseDeclarationSpecifiers(specs)) {
            return false;
        }
        return parseDeclaration(specs);
    }
    return parseStatement();
}

bool Parser::parseExpressionStatement() {
    if (matchPunctuator(";")) {
        return true;
    }
    if (!parseExpression()) {
        return false;
    }
    if (!matchPunctuator(";")) {
        setError("Expected ';' after expression");
        return false;
    }
    return true;
}

bool Parser::parseSelectionStatement() {
    if (!matchKeyword("if")) {
        return false;
    }
    if (!matchPunctuator("(")) {
        setError("Expected '(' after if");
        return false;
    }
    if (!parseExpression()) {
        return false;
    }
    if (!matchPunctuator(")")) {
        setError("Expected ')' after if condition");
        return false;
    }
    if (!parseStatement()) {
        return false;
    }
    if (matchKeyword("else")) {
        if (!parseStatement()) {
            return false;
        }
    }
    return true;
}

bool Parser::parseIterationStatement() {
    if (!matchKeyword("while")) {
        return false;
    }
    if (!matchPunctuator("(")) {
        setError("Expected '(' after while");
        return false;
    }
    if (!parseExpression()) {
        return false;
    }
    if (!matchPunctuator(")")) {
        setError("Expected ')' after while condition");
        return false;
    }
    return parseStatement();
}

bool Parser::parseJumpStatement() {
    if (matchKeyword("return")) {
        if (!matchPunctuator(";")) {
            if (!parseExpression()) {
                return false;
            }
            if (!matchPunctuator(";")) {
                setError("Expected ';' after return expression");
                return false;
            }
        }
        return true;
    }
    if (matchKeyword("break")) {
        if (!matchPunctuator(";")) {
            setError("Expected ';' after break");
            return false;
        }
        return true;
    }

    if (matchKeyword("continue")) {
        if (!matchPunctuator(";")) {
            setError("Expected ';' after continue");
            return false;
        }
        return true;
    }
    if (matchKeyword("goto")) {
        if (!matchIdentifier()) {
            setError("Expected identifier after goto");
            return false;
        }
        if (!matchPunctuator(";")) {
            setError("Expected ';' after goto label");
            return false;
        }
        return true;
    }
    setError("Unknown jump statement");
    return false;
}

bool Parser::parseExpression() {
    return parseAssignmentExpression();
}

bool Parser::parseAssignmentExpression() {
    if (!parseConditionalExpression()) {
        return false;
    }
    while (true) {
        const Token& tok = current();
        if (tok.getTokenType() == "punctuator" && isAssignmentOperator(tok.getValue())) {
            ++position;
            if (!parseAssignmentExpression()) {
                return false;
            }
        } else {
            break;
        }
    }
    return true;
}

bool Parser::parseConditionalExpression() {
    if (!parseBinaryExpression(1)) {
        return false;
    }
    if (matchPunctuator("?")) {
        if (!parseExpression()) {
            return false;
        }
        if (!matchPunctuator(":")) {
            setError("Expected ':' in conditional expression");
            return false;
        }
        if (!parseConditionalExpression()) {
            return false;
        }
    }
    return true;
}

int Parser::getBinaryPrecedence(const std::string& op) const {
    if (op == "||") return 1;
    if (op == "&&") return 2;
    if (op == "==" || op == "!=") return 3;
    if (op == "<") return 4;  
    if (op == "+" || op == "-") return 5;
    if (op == "*") return 6;
    return -1;
}

bool Parser::isAssignmentOperator(const std::string& op) const {
    static std::unordered_set<std::string> ops = {
        "="
    };
    return ops.count(op) > 0;
}

bool Parser::parseBinaryExpression(int minPrecedence) {
    if (!parseUnaryExpression()) {
        return false;
    }

    while (true) {
        const Token& tok = current();
        if (tok.getTokenType() != "punctuator") {
            break;
        }
        int precedence = getBinaryPrecedence(tok.getValue());
        if (precedence < minPrecedence) {
            break;
        }
        ++position;
        int nextMinPrec = precedence + 1;
        if (!parseBinaryExpression(nextMinPrec)) {
            return false;
        }
    }
    return true;
}

bool Parser::parseUnaryExpression() {
    const Token& tok = current();
    if (tok.getTokenType() == "punctuator") {
        if (tok.getValue() == "&" || tok.getValue() == "*" ||
             tok.getValue() == "-" || tok.getValue() == "!") {
            ++position;
            return parseUnaryExpression();
        }
    }
    if (tok.getTokenType() == "identifier" && tok.getValue() == "sizeof") {
        ++position;
        if (matchPunctuator("(")) {
            if (startsTypeName()) {
                if (!parseTypeName()) {
                    return false;
                }
            } else {
                if (!parseExpression()) {
                    return false;
                }
            }
            if (!matchPunctuator(")")) {
                setError("Expected ')' after sizeof");
                return false;
            }
            return true;
        }
        return parseUnaryExpression();
    }
    return parsePostfixExpression();
}

bool Parser::parsePostfixExpression() {
    // Start with a primary-expression:
    // identifier, literal, or ( expression )
    if (!parsePrimaryExpression()) {
        return false;
    }

    // Handle any number of postfix operators:
    //  ()   function call
    //  []   array subscript
    //  .id  struct/union member access
    //  ->id pointer-to-struct/union member access
    while (true) {
        // Function call: postfix-expression "(" argument-expression-list_opt ")"
        if (matchPunctuator("(")) {
            // Handle empty argument list: f()
            if (!matchPunctuator(")")) {
                if (!parseArgumentExpressionList()) {
                    return false;
                }
                if (!matchPunctuator(")")) {
                    setError("Expected ')' after argument list");
                    return false;
                }
            }
        }
        // Array subscript: postfix-expression "[" expression "]"
        else if (matchPunctuator("[")) {
            if (!parseExpression()) {
                return false;
            }
            if (!matchPunctuator("]")) {
                setError("Expected ']' after array subscript");
                return false;
            }
        }
        // Member access: postfix-expression "." identifier
        else if (matchPunctuator(".")) {
            if (!matchIdentifier()) {
                setError("Expected identifier after '.'");
                return false;
            }
        }
        // Pointer member access: postfix-expression "->" identifier
        else if (matchPunctuator("->")) {
            if (!matchIdentifier()) {
                setError("Expected identifier after '->'");
                return false;
            }
        }
        else {
            break; // no more postfix ops
        }
    }

    return true;
}

bool Parser::parsePrimaryExpression() {
    if (matchIdentifier()) {
        return true;
    }
    if (matchTokenType("decimal-constant") || matchTokenType("string-literal") || matchTokenType("character-constant")) {
        return true;
    }
    if (matchPunctuator("(")) {
        if (!parseExpression()) {
            return false;
        }
        if (!matchPunctuator(")")) {
            setError("Expected ')' after expression");
            return false;
        }
        return true;
    }
    setError("Expected primary expression");
    return false;
}

bool Parser::parseArgumentExpressionList() {
    if (!parseAssignmentExpression()) {
        return false;
    }
    while (matchPunctuator(",")) {
        if (!parseAssignmentExpression()) {
            return false;
        }
    }
    return true;
}

bool Parser::isTypeSpecifier(const Token& token) const {
    static std::unordered_set<std::string> specifiers = {
        "void", "char", "int", "struct"
    };
    return token.getTokenType() == "identifier" && specifiers.count(token.getValue()) > 0;
}

bool Parser::startsTypeName() {
    // type-name in c4 = declaration-specifiers abstract-declarator_opt
    // simplest lookahead: check if next token is type-specifier or 'struct'
    const Token& tok = current();

    if (tok.getTokenType() == "identifier") {
        if (tok.getValue() == "void" ||
            tok.getValue() == "char" ||
            tok.getValue() == "int"  ||
            tok.getValue() == "struct") {
            return true;
        }
    }
    return false;
}

bool Parser::parseTypeName() {
    std::vector<std::string> specs;

    if (!parseDeclarationSpecifiers(specs)) {
        setError("Expected type-name");
        return false;
    }

    // optional abstract-declarator: we only support pointer-stars in c4
    while (matchPunctuator("*")) {}

    return true;
}

bool Parser::parseStructSpecifier() {
    if (!matchKeyword("struct")) {
        return false;
    }

    bool haveTag = false;

    // optional tag
    if (current().getTokenType() == "identifier" &&
        current().getValue() != "struct" &&  // not a keyword again
        current().getValue() != "void" &&
        current().getValue() != "char" &&
        current().getValue() != "int") {
        matchIdentifier();
        haveTag = true;
    }

    // struct { ... } or struct Tag { ... }
    if (matchPunctuator("{")) {
        while (!atEnd() && !matchPunctuator("}")) {
            std::vector<std::string> specs;
            if (!parseDeclarationSpecifiers(specs)) {
                setError("Expected declaration-specifiers in struct");
                return false;
            }
            if (!parseDeclaration(specs)) {
                return false;
            }
        }
        return true;
    }

    // struct Tag (no body) is allowed, but struct alone is not
    if (!haveTag) {
        setError("Expected identifier or '{' after 'struct'");
        return false;
    }

    return true;
}
