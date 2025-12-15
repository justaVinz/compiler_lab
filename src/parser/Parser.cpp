#include "Parser.h"

#include <iostream>
#include <stdexcept>
#include <unordered_set>

// Helper pretty-printer for the parse tree (optional utility)
void printNode(const ParseNode* node, int indent) {
    if (!node) return;
    for (int i = 0; i < indent; ++i) std::cout << "  ";

    std::cout << node->name;
    if (node->token) {
        std::cout << "  [token: " << node->token->getTokenType()
                  << " '" << node->token->getValue() << "']";
    }
    std::cout << "\n";

    for (const auto& child : node->children) {
        printNode(child.get(), indent + 1);
    }
}

// --- Parser core helpers -----------------------------------------------------

Parser::Parser(std::vector<Token> tokens)
    : tokens(std::move(tokens)) {}

const Token& Parser::current() const {
    if (position >= tokens.size()) {
        static Token eof("EOF", "EOF", 0, 0);
        return eof;
    }
    return tokens[position];
}

std::unique_ptr<ParseNode> Parser::makeNode(const std::string& name) {
    auto node = std::make_unique<ParseNode>();
    node->name = name;
    return node;
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

// --- Top-level parsing -------------------------------------------------------

bool Parser::parseTranslationUnit() {
    if (tokens.empty()) {
        setError("No tokens to parse");
        return false;
    }

    auto root = makeNode("translation-unit");

    while (!atEnd()) {
        std::unique_ptr<ParseNode> extDeclNode;
        if (!parseExternalDeclaration(extDeclNode)) {
            return false;
        }
        root->children.push_back(std::move(extDeclNode));
    }
    parseTreeRoot = std::move(root);
    return true;
}

bool Parser::parse() {
    bool ok = parseTranslationUnit();
    if (ok && !atEnd()) {
        setError("Extra tokens after end of translation unit");
        return false;
    }
    return ok && atEnd();
}

// external-declaration:
//   function-definition
// | declaration
bool Parser::parseExternalDeclaration(std::unique_ptr<ParseNode>& outNode) {
    std::vector<std::string> declSpecifiers;
    std::unique_ptr<ParseNode> specNode;

    if (!parseDeclarationSpecifiers(declSpecifiers, specNode)) {
        setError("Expected declaration specifiers");
        return false;
    }

    // Case 1: declaration-specifiers ';'   (e.g., "int;")
    if (matchPunctuator(";")) {
        auto external = makeNode("external-declaration");

        // Build a simple declaration node for this form
        auto declNode = makeNode("declaration");
        declNode->children.push_back(std::move(specNode));
        external->children.push_back(std::move(declNode));

        outNode = std::move(external);
        return true;
    }

    // Lookahead: is this a function-definition or just a declaration?
    size_t backup = position;
    bool isFunction = false;
    std::unique_ptr<ParseNode> dummyDecl;

    if (!parseDeclarator(isFunction, dummyDecl)) {
        setError("Expected declarator");
        return false;
    }

    bool hasBody = isFunction &&
        current().getTokenType() == "punctuator" &&
        current().getValue() == "{";

    // Rewind to just after declaration-specifiers
    position = backup;

    auto external = makeNode("external-declaration");
    external->children.push_back(std::move(specNode));

    if (hasBody) {
        // function-definition
        std::unique_ptr<ParseNode> funcNode;
        if (!parseFunctionDefinition(declSpecifiers, funcNode)) {
            return false;
        }
        external->children.push_back(std::move(funcNode));
    } else {
        // declaration
        std::unique_ptr<ParseNode> declNode;
        if (!parseDeclaration(declSpecifiers, declNode)) {
            return false;
        }
        external->children.push_back(std::move(declNode));
    }

    outNode = std::move(external);
    return true;
}

// function-definition:
//   declaration-specifiers_opt declarator compound-statement
bool Parser::parseFunctionDefinition(
    const std::vector<std::string>& declSpecifiers,
    std::unique_ptr<ParseNode>& outNode
) {
    auto node = makeNode("function-definition");

    if (!declSpecifiers.empty()) {
        auto specsNode = makeNode("declaration-specifiers");
        for (const auto& spec : declSpecifiers) {
            auto ts = makeNode("type-specifier");
            ts->name = "type-specifier(" + spec + ")";
            specsNode->children.push_back(std::move(ts));
        }
        node->children.push_back(std::move(specsNode));
    }

    bool isFunction = false;
    std::unique_ptr<ParseNode> declNode;
    if (!parseDeclarator(isFunction, declNode) || !isFunction) {
        setError("Expected function declarator");
        return false;
    }
    node->children.push_back(std::move(declNode));

    std::unique_ptr<ParseNode> bodyNode;
    if (!parseCompoundStatement(bodyNode)) {
        setError("Invalid function body");
        return false;
    }
    node->children.push_back(std::move(bodyNode));

    outNode = std::move(node);
    return true;
}

// declaration:
//   declaration-specifiers ';'
// | declaration-specifiers init-declarator-list ';'
bool Parser::parseDeclaration(const std::vector<std::string>& declSpecifiers,
                              std::unique_ptr<ParseNode>& outNode) {
    auto node = makeNode("declaration");

    if (!declSpecifiers.empty()) {
        auto specsNode = makeNode("declaration-specifiers");
        for (const auto& spec : declSpecifiers) {
            auto specNode = makeNode("type-specifier");
            specNode->name = "type-specifier(" + spec + ")";
            specsNode->children.push_back(std::move(specNode));
        }
        node->children.push_back(std::move(specsNode));
    }

    // declaration-specifiers ';'
    if (matchPunctuator(";")) {
        outNode = std::move(node);
        return true;
    }

    // declaration-specifiers init-declarator-list ';'
    std::unique_ptr<ParseNode> initListNode;
    if (!parseInitDeclaratorList(initListNode)) {
        setError("Invalid init-declarator list");
        return false;
    }
    if (!matchPunctuator(";")) {
        setError("Expected ';' after declaration");
        return false;
    }
    node->children.push_back(std::move(initListNode));

    outNode = std::move(node);
    return true;
}

// init-declarator-list:
//   init-declarator
// (C4: only one declarator is allowed)
bool Parser::parseInitDeclaratorList(std::unique_ptr<ParseNode>& outNode) {
    auto node = makeNode("init-declarator-list");

    bool isFunctionDecl = false;
    std::unique_ptr<ParseNode> initDeclNode;
    if (!parseInitDeclarator(isFunctionDecl, initDeclNode)) {
        return false;
    }
    node->children.push_back(std::move(initDeclNode));

    if (matchPunctuator(",")) {
        setError("Only one init-declarator is allowed");
        return false;
    }

    outNode = std::move(node);
    return true;
}

// declaration-specifiers:
//   (type-specifier | struct-specifier)+
// For C4: { void, char, int, struct }
bool Parser::parseDeclarationSpecifiers(std::vector<std::string>& outSpecifiers,
                                        std::unique_ptr<ParseNode>& outNode) {
    auto node = makeNode("declaration-specifiers");
    bool parsedAny = false;

    while (!atEnd()) {
        const Token& tok = current();

        // struct-specifier
        if (tok.getTokenType() == "identifier" && tok.getValue() == "struct") {
            if (!parseStructSpecifier()) {
                return false;
            }
            parsedAny = true;
            outSpecifiers.push_back("struct");

            auto structNode = makeNode("struct-specifier");
            node->children.push_back(std::move(structNode));
            continue;
        }

        // simple type-specifier: void/char/int
        if (isTypeSpecifier(tok)) {
            parsedAny = true;
            outSpecifiers.push_back(tok.getValue());

            auto specNode = makeNode("type-specifier");
            specNode->token = &tok;
            node->children.push_back(std::move(specNode));

            ++position;
            continue;
        }

        break;
    }

    if (!parsedAny) {
        return false;
    }

    outNode = std::move(node);
    return true;
}

// init-declarator:
//   declarator
// | declarator '=' <not allowed in C4 for now>
bool Parser::parseInitDeclarator(bool& isFunctionDecl,
                                 std::unique_ptr<ParseNode>& outNode) {
    auto node = makeNode("init-declarator");

    isFunctionDecl = false;

    std::unique_ptr<ParseNode> declaratorNode;
    if (!parseDeclarator(isFunctionDecl, declaratorNode)) {
        return false;
    }
    node->children.push_back(std::move(declaratorNode));

    if (isFunctionDecl) {
        outNode = std::move(node);
        return true;
    }

    if (matchPunctuator("=")) {
        setError("initializer is not allowed");
        return false;
    }

    outNode = std::move(node);
    return true;
}

// declarator:
//   pointer_opt direct-declarator
bool Parser::parseDeclarator(bool& isFunction, std::unique_ptr<ParseNode>& outNode) {
    auto node = makeNode("declarator");
    isFunction = false;

    std::unique_ptr<ParseNode> ptrNode;
    if (!parsePointer(ptrNode)) {
        return false;
    }
    if (ptrNode) {
        node->children.push_back(std::move(ptrNode));
    }

    std::unique_ptr<ParseNode> directNode;
    if (!parseDirectDeclarator(isFunction, directNode)) {
        return false;
    }
    node->children.push_back(std::move(directNode));

    outNode = std::move(node);
    return true;
}

// pointer:
//   '*'*
// (chain of stars)
bool Parser::parsePointer(std::unique_ptr<ParseNode>& outNode) {
    auto node = makeNode("pointer");
    bool sawStar = false;

    while (matchPunctuator("*")) {
        sawStar = true;
        auto starNode = makeNode("punctuator(*)");
        starNode->token = &tokens[position - 1];
        node->children.push_back(std::move(starNode));
    }

    if (!sawStar) {
        outNode = nullptr;
        return true;
    }

    outNode = std::move(node);
    return true;
}

// direct-declarator:
//   identifier
// | '(' declarator ')'
//   ( '(' parameter-type-list_opt ')' )*
bool Parser::parseDirectDeclarator(bool& isFunction,
                                   std::unique_ptr<ParseNode>& outNode) {
    auto node = makeNode("direct-declarator");
    isFunction = false;

    // Base: ( declarator )  OR  identifier
    if (matchPunctuator("(")) {
        auto lpar = makeNode("(");
        lpar->token = &tokens[position - 1];
        node->children.push_back(std::move(lpar));

        std::unique_ptr<ParseNode> innerDecl;
        if (!parseDeclarator(isFunction, innerDecl)) {
            return false;
        }
        node->children.push_back(std::move(innerDecl));

        if (!matchPunctuator(")")) {
            setError("Expected ')' after declarator");
            return false;
        }
        auto rpar = makeNode(")");
        rpar->token = &tokens[position - 1];
        node->children.push_back(std::move(rpar));
    }
    else if (matchIdentifier()) {
        auto idNode = makeNode("identifier");
        idNode->token = &tokens[position - 1];
        node->children.push_back(std::move(idNode));
    }
    else {
        setError("Expected identifier in declarator");
        return false;
    }

    // suffix: function parameter list(s)
    while (true) {
        if (matchPunctuator("(")) {
            auto paramsWrapper = makeNode("parameter-list-suffix");

            // f() → empty parameter list
            if (current().getTokenType() == "punctuator" &&
                current().getValue() == ")") {
                auto emptyParams = makeNode("parameter-type-list");
                paramsWrapper->children.push_back(std::move(emptyParams));
                ++position; // consume ')'
            } else {
                std::unique_ptr<ParseNode> paramsNode;
                if (!parseParameterTypeList(paramsNode)) {
                    return false;
                }
                if (!matchPunctuator(")")) {
                    setError("Expected ')' after parameter list");
                    return false;
                }
                paramsWrapper->children.push_back(std::move(paramsNode));
            }

            node->children.push_back(std::move(paramsWrapper));
            isFunction = true;
        } else {
            break;
        }
    }

    outNode = std::move(node);
    return true;
}

// parameter-type-list:
//   parameter-declaration (',' parameter-declaration)*
bool Parser::parseParameterTypeList(std::unique_ptr<ParseNode>& outNode) {
    auto node = makeNode("parameter-type-list");

    std::unique_ptr<ParseNode> firstParam;
    if (!parseParameterDeclaration(firstParam)) {
        return false;
    }
    node->children.push_back(std::move(firstParam));

    while (matchPunctuator(",")) {
        std::unique_ptr<ParseNode> param;
        if (!parseParameterDeclaration(param)) {
            return false;
        }
        node->children.push_back(std::move(param));
    }

    outNode = std::move(node);
    return true;
}

// parameter-declaration:
//   declaration-specifiers declarator
bool Parser::parseParameterDeclaration(std::unique_ptr<ParseNode>& outNode) {
    auto node = makeNode("parameter-declaration");

    // 1. declaration-specifiers
    std::vector<std::string> specs;
    std::unique_ptr<ParseNode> specNode;
    if (!parseDeclarationSpecifiers(specs, specNode)) {
        setError("Expected declaration specifiers in parameter");
        return false;
    }
    node->children.push_back(std::move(specNode));

    // Look-ahead: if the next token ends the parameter, we have the
    //   declaration-specifiers abstract-declarator_opt
    // case where abstract-declarator is *empty* (e.g. plain "int").
    if (current().getTokenType() == "punctuator" &&
        (current().getValue() == ")" || current().getValue() == ",")) {
        // unnamed parameter like "int"
        outNode = std::move(node);
        return true;
    }

    // 2. Try the "declarator" branch first (named parameter).
    size_t backup = position;
    bool isFunction = false;
    std::unique_ptr<ParseNode> declNode;
    if (parseDeclarator(isFunction, declNode)) {
        node->children.push_back(std::move(declNode));
        outNode = std::move(node);
        return true;
    }

    // 3. If that failed, roll back and try "abstract-declarator".
    position = backup;

    std::unique_ptr<ParseNode> absNode;
    if (parseAbstractDeclarator(absNode)) {
        node->children.push_back(std::move(absNode));
        outNode = std::move(node);
        return true;
    }

    // 4. Neither declarator nor abstract-declarator worked → real error.
    setError("Expected declarator or abstract-declarator in parameter");
    return false;
}


// --- Statements --------------------------------------------------------------

bool Parser::parseStatement(std::unique_ptr<ParseNode>& outNode) {
    if (atEnd()) {
        setError("Unexpected end of input in statement");
        return false;
    }

    auto node = makeNode("statement");
    const Token& tok = current();

    // if-statement
    if (tok.getTokenType() == "identifier" && tok.getValue() == "if") {
        std::unique_ptr<ParseNode> selNode;
        if (!parseSelectionStatement(selNode)) {
            return false;
        }
        node->children.push_back(std::move(selNode));
        outNode = std::move(node);
        return true;
    }

    // while-statement
    if (tok.getTokenType() == "identifier" && tok.getValue() == "while") {
        std::unique_ptr<ParseNode> iterNode;
        if (!parseIterationStatement(iterNode)) {
            return false;
        }
        node->children.push_back(std::move(iterNode));
        outNode = std::move(node);
        return true;
    }

    // jump statements
    if (tok.getTokenType() == "identifier" &&
        (tok.getValue() == "return" ||
         tok.getValue() == "goto"   ||
         tok.getValue() == "break"  ||
         tok.getValue() == "continue")) {

        std::unique_ptr<ParseNode> jumpNode;
        if (!parseJumpStatement(jumpNode)) {
            return false;
        }
        node->children.push_back(std::move(jumpNode));
        outNode = std::move(node);
        return true;
    }

    // compound-statement
    if (matchPunctuator("{")) {
        --position; // give it back to parseCompoundStatement
        std::unique_ptr<ParseNode> compNode;
        if (!parseCompoundStatement(compNode)) {
            return false;
        }
        node->children.push_back(std::move(compNode));
        outNode = std::move(node);
        return true;
    }

    // labeled-statement
    {
        size_t backup = position;
        if (matchIdentifier()) {
            if (matchPunctuator(":")) {
                position = backup;
                std::unique_ptr<ParseNode> labNode;
                if (!parseLabeledStatement(labNode)) {
                    return false;
                }
                node->children.push_back(std::move(labNode));
                outNode = std::move(node);
                return true;
            }
            position = backup;
        }
    }

    // expression-statement
    std::unique_ptr<ParseNode> exprStmtNode;
    if (!parseExpressionStatement(exprStmtNode)) {
        return false;
    }
    node->children.push_back(std::move(exprStmtNode));
    outNode = std::move(node);
    return true;
}

bool Parser::parseLabeledStatement(std::unique_ptr<ParseNode>& outNode) {
    auto node = makeNode("labeled-statement");

    if (!matchIdentifier()) {
        setError("Expected identifier for label");
        return false;
    }
    auto labelNode = makeNode("label");
    labelNode->token = &tokens[position - 1];
    node->children.push_back(std::move(labelNode));

    if (!matchPunctuator(":")) {
        setError("Expected ':' after label");
        return false;
    }

    std::unique_ptr<ParseNode> stmtNode;
    if (!parseStatement(stmtNode)) {
        return false;
    }
    node->children.push_back(std::move(stmtNode));

    outNode = std::move(node);
    return true;
}

bool Parser::parseCompoundStatement(std::unique_ptr<ParseNode>& outNode) {
    auto node = makeNode("compound-statement");

    if (!matchPunctuator("{")) {
        setError("Expected '{'");
        return false;
    }

    while (!atEnd() &&
           !(current().getTokenType() == "punctuator" &&
             current().getValue() == "}")) {

        std::unique_ptr<ParseNode> itemNode;
        if (!parseBlockItem(itemNode)) {
            return false;
        }
        node->children.push_back(std::move(itemNode));
    }

    if (!matchPunctuator("}")) {
        setError("Expected '}' at end of compound-statement");
        return false;
    }

    outNode = std::move(node);
    return true;
}

bool Parser::parseBlockItem(std::unique_ptr<ParseNode>& outNode) {
    auto node = makeNode("block-item");

    if (isTypeSpecifier(current())) {
        std::vector<std::string> specs;
        std::unique_ptr<ParseNode> specNode;
        if (!parseDeclarationSpecifiers(specs, specNode)) {
            return false;
        }

        std::unique_ptr<ParseNode> declNode;
        if (!parseDeclaration(specs, declNode)) {
            return false;
        }

        node->children.push_back(std::move(specNode));
        node->children.push_back(std::move(declNode));
    } else {
        std::unique_ptr<ParseNode> stmtNode;
        if (!parseStatement(stmtNode)) {
            return false;
        }
        node->children.push_back(std::move(stmtNode));
    }

    outNode = std::move(node);
    return true;
}

bool Parser::parseExpressionStatement(std::unique_ptr<ParseNode>& outNode) {
    auto node = makeNode("expression-statement");

    if (matchPunctuator(";")) {
        auto emptyNode = makeNode("empty");
        node->children.push_back(std::move(emptyNode));
        outNode = std::move(node);
        return true;
    }

    std::unique_ptr<ParseNode> exprNode;
    if (!parseExpression(exprNode)) {
        return false;
    }
    if (!matchPunctuator(";")) {
        setError("Expected ';' after expression");
        return false;
    }

    node->children.push_back(std::move(exprNode));
    outNode = std::move(node);
    return true;
}

bool Parser::parseSelectionStatement(std::unique_ptr<ParseNode>& outNode) {
    auto node = makeNode("selection-statement");

    if (!matchKeyword("if")) return false;
    node->children.push_back(makeNode("if"));

    if (!matchPunctuator("(")) {
        setError("Expected '(' after if");
        return false;
    }

    std::unique_ptr<ParseNode> condExpr;
    if (!parseExpression(condExpr)) {
        return false;
    }

    if (!matchPunctuator(")")) {
        setError("Expected ')' after condition");
        return false;
    }

    auto condNode = makeNode("condition");
    condNode->children.push_back(std::move(condExpr));
    node->children.push_back(std::move(condNode));

    std::unique_ptr<ParseNode> thenStmt;
    if (!parseStatement(thenStmt)) {
        return false;
    }
    auto thenNode = makeNode("then");
    thenNode->children.push_back(std::move(thenStmt));
    node->children.push_back(std::move(thenNode));

    if (matchKeyword("else")) {
        std::unique_ptr<ParseNode> elseStmt;
        if (!parseStatement(elseStmt)) {
            return false;
        }
        auto elseNode = makeNode("else");
        elseNode->children.push_back(std::move(elseStmt));
        node->children.push_back(std::move(elseNode));
    }

    outNode = std::move(node);
    return true;
}

bool Parser::parseIterationStatement(std::unique_ptr<ParseNode>& outNode) {
    auto node = makeNode("iteration-statement");

    if (!matchKeyword("while")) {
        return false;
    }
    node->children.push_back(makeNode("while"));

    if (!matchPunctuator("(")) {
        setError("Expected '(' after while");
        return false;
    }

    std::unique_ptr<ParseNode> condExpr;
    if (!parseExpression(condExpr)) {
        return false;
    }
    auto condNode = makeNode("condition");
    condNode->children.push_back(std::move(condExpr));
    node->children.push_back(std::move(condNode));

    if (!matchPunctuator(")")) {
        setError("Expected ')' after while condition");
        return false;
    }

    std::unique_ptr<ParseNode> body;
    if (!parseStatement(body)) {
        return false;
    }
    auto bodyNode = makeNode("body");
    bodyNode->children.push_back(std::move(body));
    node->children.push_back(std::move(bodyNode));

    outNode = std::move(node);
    return true;
}

bool Parser::parseJumpStatement(std::unique_ptr<ParseNode>& outNode) {
    auto node = makeNode("jump-statement");

    // return [expr] ;
    if (matchKeyword("return")) {
        node->children.push_back(makeNode("return"));

        // return ;
        if (matchPunctuator(";")) {
            outNode = std::move(node);
            return true;
        }

        // return expr ;
        std::unique_ptr<ParseNode> expr;
        if (!parseExpression(expr)) return false;

        node->children.push_back(std::move(expr));

        if (!matchPunctuator(";")) {
            setError("Expected ';' after return expression");
            return false;
        }

        outNode = std::move(node);
        return true;
    }

    // break ;
    if (matchKeyword("break")) {
        node->children.push_back(makeNode("break"));
        if (!matchPunctuator(";")) {
            setError("Expected ';' after break");
            return false;
        }
        outNode = std::move(node);
        return true;
    }

    // continue ;
    if (matchKeyword("continue")) {
        node->children.push_back(makeNode("continue"));
        if (!matchPunctuator(";")) {
            setError("Expected ';' after continue");
            return false;
        }
        outNode = std::move(node);
        return true;
    }

    // goto identifier ;
    if (matchKeyword("goto")) {
        node->children.push_back(makeNode("goto"));

        if (!matchIdentifier()) {
            setError("Expected identifier after goto");
            return false;
        }
        auto labelNode = makeNode("label");
        labelNode->token = &tokens[position - 1];
        node->children.push_back(std::move(labelNode));

        if (!matchPunctuator(";")) {
            setError("Expected ';' after goto label");
            return false;
        }

        outNode = std::move(node);
        return true;
    }

    setError("Unknown jump statement");
    return false;
}

// --- Expressions -------------------------------------------------------------

bool Parser::parseExpression(std::unique_ptr<ParseNode>& outNode) {
    return parseAssignmentExpression(outNode);
}

// assignment-expression:
//   conditional-expression
// | unary-expression assignment-operator assignment-expression
bool Parser::parseAssignmentExpression(std::unique_ptr<ParseNode>& outNode) {
    std::unique_ptr<ParseNode> left;
    if (!parseConditionalExpression(left)) {
        return false;
    }

    while (true) {
        const Token& tok = current();
        if (tok.getTokenType() == "punctuator" &&
            isAssignmentOperator(tok.getValue())) {

            size_t opPos = position;
            ++position; // consume operator

            std::unique_ptr<ParseNode> right;
            if (!parseAssignmentExpression(right)) {
                return false;
            }

            auto assignNode = makeNode("assignment-expression");

            auto opNode = makeNode("operator");
            opNode->token = &tokens[opPos];
            assignNode->children.push_back(std::move(opNode));

            assignNode->children.push_back(std::move(left));
            assignNode->children.push_back(std::move(right));

            left = std::move(assignNode);
        } else {
            break;
        }
    }

    outNode = std::move(left);
    return true;
}

// conditional-expression:
//   binary-expression
// | binary-expression '?' expression ':' conditional-expression
bool Parser::parseConditionalExpression(std::unique_ptr<ParseNode>& outNode) {
    std::unique_ptr<ParseNode> cond;
    if (!parseBinaryExpression(1, cond)) {
        return false;
    }

    if (!matchPunctuator("?")) {
        outNode = std::move(cond);
        return true;
    }

    auto condExprNode = makeNode("conditional-expression");

    auto condNode = makeNode("condition");
    condNode->children.push_back(std::move(cond));
    condExprNode->children.push_back(std::move(condNode));

    std::unique_ptr<ParseNode> thenExpr;
    if (!parseExpression(thenExpr)) {
        return false;
    }
    auto thenNode = makeNode("then");
    thenNode->children.push_back(std::move(thenExpr));
    condExprNode->children.push_back(std::move(thenNode));

    if (!matchPunctuator(":")) {
        setError("Expected ':' in conditional expression");
        return false;
    }

    std::unique_ptr<ParseNode> elseExpr;
    if (!parseConditionalExpression(elseExpr)) {
        return false;
    }
    auto elseNode = makeNode("else");
    elseNode->children.push_back(std::move(elseExpr));
    condExprNode->children.push_back(std::move(elseNode));

    outNode = std::move(condExprNode);
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
    static std::unordered_set<std::string> ops = { "=" };
    return ops.count(op) > 0;
}

// binary-expression using precedence climbing
bool Parser::parseBinaryExpression(int minPrec, std::unique_ptr<ParseNode>& outNode) {
    std::unique_ptr<ParseNode> left;
    if (!parseUnaryExpression(left)) {
        return false;
    }

    while (true) {
        const Token& tok = current();
        if (tok.getTokenType() != "punctuator") break;

        int prec = getBinaryPrecedence(tok.getValue());
        if (prec < minPrec) break;

        size_t opPos = position;
        ++position; // consume operator

        std::unique_ptr<ParseNode> right;
        if (!parseBinaryExpression(prec + 1, right)) {
            return false;
        }

        auto binNode = makeNode("binary-expression");
        auto opNode = makeNode("operator");
        opNode->token = &tokens[opPos];
        binNode->children.push_back(std::move(opNode));

        binNode->children.push_back(std::move(left));
        binNode->children.push_back(std::move(right));

        left = std::move(binNode);
    }

    outNode = std::move(left);
    return true;
}

// unary-expression:
//   (& | * | - | !) unary-expression
// | sizeof ( type-name | expression )
// | sizeof unary-expression
// | postfix-expression
bool Parser::parseUnaryExpression(std::unique_ptr<ParseNode>& outNode) {
    const Token& tok = current();

    // prefix operators: &, *, -, !
    if (tok.getTokenType() == "punctuator" &&
        (tok.getValue() == "&" || tok.getValue() == "*" ||
         tok.getValue() == "-" || tok.getValue() == "!")) {

        size_t opPos = position;
        ++position; // consume operator

        std::unique_ptr<ParseNode> operand;
        if (!parseUnaryExpression(operand)) {
            return false;
        }

        auto node = makeNode("unary-expression");
        auto opNode = makeNode("operator");
        opNode->token = &tokens[opPos];

        node->children.push_back(std::move(opNode));
        node->children.push_back(std::move(operand));

        outNode = std::move(node);
        return true;
    }

    // sizeof
    if (tok.getTokenType() == "identifier" && tok.getValue() == "sizeof") {
        size_t kwPos = position;
        ++position;

        auto node = makeNode("unary-expression");
        auto kwNode = makeNode("sizeof");
        kwNode->token = &tokens[kwPos];
        node->children.push_back(std::move(kwNode));

        if (matchPunctuator("(")) {
            if (startsTypeName()) {
                std::unique_ptr<ParseNode> typeNode;
                if (!parseTypeName(typeNode)) {
                    return false;
                }
                node->children.push_back(std::move(typeNode));
            } else {
                std::unique_ptr<ParseNode> exprNode;
                if (!parseExpression(exprNode)) {
                    return false;
                }
                node->children.push_back(std::move(exprNode));
            }

            if (!matchPunctuator(")")) {
                setError("Expected ')' after sizeof");
                return false;
            }
            outNode = std::move(node);
            return true;
        }

        // sizeof unary-expression (no parenthesis)
        std::unique_ptr<ParseNode> operand;
        if (!parseUnaryExpression(operand)) {
            return false;
        }
        node->children.push_back(std::move(operand));
        outNode = std::move(node);
        return true;
    }

    // fallback to postfix-expression
    return parsePostfixExpression(outNode);
}

// postfix-expression:
//   primary-expression
//   ( '(' argument-expression-list_opt ')' |
//     '[' expression ']' |
//     '.' identifier |
//     '->' identifier )*
bool Parser::parsePostfixExpression(std::unique_ptr<ParseNode>& outNode) {
    std::unique_ptr<ParseNode> base;
    if (!parsePrimaryExpression(base)) {
        return false;
    }

    while (true) {
        // function call
        if (matchPunctuator("(")) {
            auto callNode = makeNode("function-call");
            callNode->children.push_back(std::move(base));

            if (!matchPunctuator(")")) {
                std::unique_ptr<ParseNode> argList;
                if (!parseArgumentExpressionList(argList)) {
                    return false;
                }
                callNode->children.push_back(std::move(argList));

                if (!matchPunctuator(")")) {
                    setError("Expected ')' after argument list");
                    return false;
                }
            }
            base = std::move(callNode);
        }
        // array subscript
        else if (matchPunctuator("[")) {
            auto subNode = makeNode("array-subscript");
            subNode->children.push_back(std::move(base));

            std::unique_ptr<ParseNode> indexExpr;
            if (!parseExpression(indexExpr)) {
                return false;
            }
            subNode->children.push_back(std::move(indexExpr));

            if (!matchPunctuator("]")) {
                setError("Expected ']' after array subscript");
                return false;
            }
            base = std::move(subNode);
        }
        // member access: .
        else if (matchPunctuator(".")) {
            auto memNode = makeNode("member-access");
            memNode->children.push_back(std::move(base));

            size_t idPos = position;
            if (!matchIdentifier()) {
                setError("Expected identifier after '.'");
                return false;
            }

            auto idNode = makeNode("identifier");
            idNode->token = &tokens[idPos];
            memNode->children.push_back(std::move(idNode));

            base = std::move(memNode);
        }
        // pointer member access: ->
        else if (matchPunctuator("->")) {
            auto memNode = makeNode("pointer-member-access");
            memNode->children.push_back(std::move(base));

            size_t idPos = position;
            if (!matchIdentifier()) {
                setError("Expected identifier after '->'");
                return false;
            }

            auto idNode = makeNode("identifier");
            idNode->token = &tokens[idPos];
            memNode->children.push_back(std::move(idNode));

            base = std::move(memNode);
        }
        else {
            break;
        }
    }

    outNode = std::move(base);
    return true;
}

// primary-expression:
//   identifier
// | literal
// | '(' expression ')'
bool Parser::parsePrimaryExpression(std::unique_ptr<ParseNode>& outNode) {
    // identifier
    {
        size_t idPos = position;
        if (matchIdentifier()) {
            auto node = makeNode("identifier");
            node->token = &tokens[idPos];
            outNode = std::move(node);
            return true;
        }
    }

    // literals
    {
        size_t litPos = position;
        if (matchTokenType("decimal-constant") ||
            matchTokenType("string-literal")   ||
            matchTokenType("character-constant")) {

            auto node = makeNode("literal");
            node->token = &tokens[litPos];
            outNode = std::move(node);
            return true;
        }
    }

    // ( expression )
    if (matchPunctuator("(")) {
        std::unique_ptr<ParseNode> inner;
        if (!parseExpression(inner)) {
            return false;
        }
        if (!matchPunctuator(")")) {
            setError("Expected ')' after expression");
            return false;
        }

        auto node = makeNode("grouped-expression");
        node->children.push_back(std::move(inner));
        outNode = std::move(node);
        return true;
    }

    setError("Expected primary expression");
    return false;
}

bool Parser::parseArgumentExpressionList(std::unique_ptr<ParseNode>& outNode) {
    auto node = makeNode("argument-list");

    std::unique_ptr<ParseNode> arg;
    if (!parseAssignmentExpression(arg)) {
        return false;
    }
    node->children.push_back(std::move(arg));

    while (matchPunctuator(",")) {
        std::unique_ptr<ParseNode> moreArg;
        if (!parseAssignmentExpression(moreArg)) {
            return false;
        }
        node->children.push_back(std::move(moreArg));
    }

    outNode = std::move(node);
    return true;
}

// --- Types and struct --------------------------------------------------------

bool Parser::isTypeSpecifier(const Token& token) const {
    static std::unordered_set<std::string> specifiers = {
        "void", "char", "int", "struct"
    };
    return token.getTokenType() == "identifier" &&
           specifiers.count(token.getValue()) > 0;
}

bool Parser::startsTypeName() {
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

bool Parser::parseTypeName(std::unique_ptr<ParseNode>& outNode) {
    auto node = makeNode("type-name");

    std::vector<std::string> specs;
    std::unique_ptr<ParseNode> specNode;
    if (!parseDeclarationSpecifiers(specs, specNode)) {
        setError("Expected type-name");
        return false;
    }
    node->children.push_back(std::move(specNode));

    // abstract-declarator_opt
    size_t backup = position;
    std::unique_ptr<ParseNode> absNode;
    if (parseAbstractDeclarator(absNode)) {
        node->children.push_back(std::move(absNode));
    } else {
        // no abstract-declarator present → restore position
        position = backup;
    }

    outNode = std::move(node);
    return true;
}

// struct-specifier (C4-restricted):
//   struct identifier_opt '{' (declaration)* '}' 
// | struct identifier
// (We ignore the body in the parse tree here; we only move the tokens.)
bool Parser::parseStructSpecifier() {
    if (!matchKeyword("struct")) {
        return false;
    }

    bool haveTag = false;

    // optional tag (simple heuristic: identifier that's not a type keyword again)
    if (current().getTokenType() == "identifier" &&
        current().getValue() != "struct" &&
        current().getValue() != "void"   &&
        current().getValue() != "char"   &&
        current().getValue() != "int") {
        matchIdentifier();
        haveTag = true;
    }

    if (matchPunctuator("{")) {
        // parse a sequence of declarations; we don't attach them to the tree here
        while (!atEnd() &&
               !(current().getTokenType() == "punctuator" &&
                 current().getValue() == "}")) {

            std::vector<std::string> specs;
            std::unique_ptr<ParseNode> specNode;
            if (!parseDeclarationSpecifiers(specs, specNode)) {
                setError("Expected declaration-specifiers in struct");
                return false;
            }

            std::unique_ptr<ParseNode> declNode;
            if (!parseDeclaration(specs, declNode)) {
                return false;
            }
        }

        if (!matchPunctuator("}")) {
            setError("Expected '}' at end of struct");
            return false;
        }
        return true;
    }

    // struct Tag (no body) allowed; but 'struct' alone is not
    if (!haveTag) {
        setError("Expected identifier or '{' after 'struct'");
        return false;
    }

    return true;
}

// abstract-declarator:
//     pointer
//   | pointer_opt direct-abstract-declarator
bool Parser::parseAbstractDeclarator(std::unique_ptr<ParseNode>& outNode) {
    auto node = makeNode("abstract-declarator");
    size_t backup = position;

    // First, try to parse the pointer part (may be empty)
    std::unique_ptr<ParseNode> ptrNode;
    // parsePointer never fails; it either produces a node or leaves ptrNode == nullptr.
    parsePointer(ptrNode);

    if (ptrNode) {
        // Case 1: pure pointer (no direct-abstract-declarator following)
        if (!(current().getTokenType() == "punctuator" &&
              current().getValue() == "(")) {
            node->children.push_back(std::move(ptrNode));
            outNode = std::move(node);
            return true;
        }

        // Case 2: pointer_opt direct-abstract-declarator
        std::unique_ptr<ParseNode> directNode;
        if (parseDirectAbstractDeclarator(directNode)) {
            node->children.push_back(std::move(ptrNode));
            node->children.push_back(std::move(directNode));
            outNode = std::move(node);
            return true;
        }

        // consumed '*' but couldn't complete the production → backtrack & fail
        position = backup;
        return false;
    }

    // No pointer; try direct-abstract-declarator alone
    std::unique_ptr<ParseNode> directNode;
    if (parseDirectAbstractDeclarator(directNode)) {
        node->children.push_back(std::move(directNode));
        outNode = std::move(node);
        return true;
    }

    // Nothing matched
    position = backup;
    return false;
}


// direct-abstract-declarator:
//     ( abstract-declarator )
//   | direct-abstract-declarator_opt ( parameter-list )
//
// We implement it in a non-left-recursive form:
//   base: optional "( abstract-declarator )"
//   then one-or-more "( parameter-list )" suffixes.
bool Parser::parseDirectAbstractDeclarator(std::unique_ptr<ParseNode>& outNode) {
    size_t backup = position;
    std::unique_ptr<ParseNode> base;

    // Optional base: "(" abstract-declarator ")"
    if (matchPunctuator("(")) {
        auto groupNode = makeNode("abstract-group");
        auto lpar = makeNode("(");
        lpar->token = &tokens[position - 1];
        groupNode->children.push_back(std::move(lpar));

        std::unique_ptr<ParseNode> innerAbs;
        if (!parseAbstractDeclarator(innerAbs)) {
            // If we can't parse an abstract-declarator here, this production fails.
            position = backup;
            return false;
        }

        if (!matchPunctuator(")")) {
            setError("Expected ')' in abstract declarator");
            return false;
        }
        auto rpar = makeNode(")");
        rpar->token = &tokens[position - 1];

        groupNode->children.push_back(std::move(innerAbs));
        groupNode->children.push_back(std::move(rpar));

        base = std::move(groupNode);
    }

    auto node = makeNode("direct-abstract-declarator");
    if (base) {
        node->children.push_back(std::move(base));
    }

    // Suffixes: "( parameter-list )" — we reuse parameter-type-list for parameter-list.
    bool sawSuffix = false;

    while (matchPunctuator("(")) {
        sawSuffix = true;
        auto suffixNode = makeNode("abstract-parameter-list-suffix");

        std::unique_ptr<ParseNode> params;
        if (!parseParameterTypeList(params)) {
            return false;
        }

        if (!matchPunctuator(")")) {
            setError("Expected ')' after parameter list in abstract declarator");
            return false;
        }

        suffixNode->children.push_back(std::move(params));
        node->children.push_back(std::move(suffixNode));
    }

    if (!base && !sawSuffix) {
        // We didn't match any of the productions
        position = backup;
        return false;
    }

    outNode = std::move(node);
    return true;
}

