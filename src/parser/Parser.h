#pragma once

#include <optional>
#include <string>
#include <vector>
#include <memory>

#include "../helper/structs/Token.h"

// Parse tree node
struct ParseNode {
    std::string name;  // nonterminal name or some label
    const Token* token = nullptr; // for leaves (= actual tokens), nullptr for internal nodes
    std::vector<std::unique_ptr<ParseNode>> children;
};

// Optional: pretty-printer for the parse tree (free function, not a member)
void printNode(const ParseNode* node, int indent = 0);

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);

    // Top-level API
    bool parse();
    std::string getErrorMessage() const { return errorMessage; }
    std::optional<Token> getErrorToken() const { return errorToken; }
    const ParseNode* getParseTree() const { return parseTreeRoot.get(); }

private:
    std::vector<Token> tokens;
    size_t position {0};

    std::unique_ptr<ParseNode> parseTreeRoot; // root of the parse tree

    std::string errorMessage;
    std::optional<Token> errorToken;

    // helper to create nodes
    std::unique_ptr<ParseNode> makeNode(const std::string& name);

    // core helpers
    const Token& current() const;
    bool atEnd() const;
    bool matchPunctuator(const std::string& value);
    bool matchIdentifier(std::optional<std::string> expected = std::nullopt);
    bool matchKeyword(const std::string& keyword);
    bool matchTokenType(const std::string& type);
    void setError(const std::string& message);

    // Grammar helpers (all AST-building via outNode)
    bool parseTranslationUnit();
    bool parseExternalDeclaration(std::unique_ptr<ParseNode>& outNode);
    bool parseFunctionDefinition(const std::vector<std::string>& declSpecifiers,
                                 std::unique_ptr<ParseNode>& outNode);
    bool parseDeclaration(const std::vector<std::string>& declSpecifiers,
                          std::unique_ptr<ParseNode>& outNode);

    bool parseDeclarationSpecifiers(std::vector<std::string>& outSpecifiers,
                                    std::unique_ptr<ParseNode>& outNode);
    bool parseInitDeclaratorList(std::unique_ptr<ParseNode>& outNode);
    bool parseInitDeclarator(bool& isFunctionDecl,
                             std::unique_ptr<ParseNode>& outNode);
    bool parseDeclarator(bool& isFunction,
                         std::unique_ptr<ParseNode>& outNode);
    bool parsePointer(std::unique_ptr<ParseNode>& outNode);
    bool parseDirectDeclarator(bool& isFunction,
                               std::unique_ptr<ParseNode>& outNode);
    bool parseParameterTypeList(std::unique_ptr<ParseNode>& outNode);
    bool parseParameterDeclaration(std::unique_ptr<ParseNode>& outNode);

    // Statements
    bool parseStatement(std::unique_ptr<ParseNode>& outNode);
    bool parseLabeledStatement(std::unique_ptr<ParseNode>& outNode);
    bool parseCompoundStatement(std::unique_ptr<ParseNode>& outNode);
    bool parseBlockItem(std::unique_ptr<ParseNode>& outNode);
    bool parseExpressionStatement(std::unique_ptr<ParseNode>& outNode);
    bool parseSelectionStatement(std::unique_ptr<ParseNode>& outNode);
    bool parseIterationStatement(std::unique_ptr<ParseNode>& outNode);
    bool parseJumpStatement(std::unique_ptr<ParseNode>& outNode);

    // Expressions
    bool parseExpression(std::unique_ptr<ParseNode>& outNode);
    bool parseAssignmentExpression(std::unique_ptr<ParseNode>& outNode);
    bool parseConditionalExpression(std::unique_ptr<ParseNode>& outNode);
    bool parseBinaryExpression(int minPrecedence,
                               std::unique_ptr<ParseNode>& outNode);
    bool parseUnaryExpression(std::unique_ptr<ParseNode>& outNode);
    bool parsePostfixExpression(std::unique_ptr<ParseNode>& outNode);
    bool parsePrimaryExpression(std::unique_ptr<ParseNode>& outNode);
    bool parseArgumentExpressionList(std::unique_ptr<ParseNode>& outNode);

    // Operators / types
    int getBinaryPrecedence(const std::string& op) const;
    bool isAssignmentOperator(const std::string& op) const;
    bool isTypeSpecifier(const Token& token) const;
    bool startsTypeName();
    bool parseTypeName(std::unique_ptr<ParseNode>& outNode);

    // struct-specifier
    bool parseStructSpecifier();

    bool parseAbstractDeclarator(std::unique_ptr<ParseNode>& outNode);
    bool parseDirectAbstractDeclarator(std::unique_ptr<ParseNode>& outNode);
};
