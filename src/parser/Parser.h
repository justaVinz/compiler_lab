#pragma once

#include <optional>
#include <string>
#include <vector>

#include "../helper/structs/Token.h"

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);

    bool parse();
    std::string getErrorMessage() const { return errorMessage; }
    std::optional<Token> getErrorToken() const { return errorToken; }

private:
    std::vector<Token> tokens;
    size_t position {0};
    std::string errorMessage;
    std::optional<Token> errorToken;

    const Token& current() const;
    bool atEnd() const;
    bool matchPunctuator(const std::string& value);
    bool matchIdentifier(std::optional<std::string> expected = std::nullopt);
    bool matchKeyword(const std::string& keyword);
    bool matchTokenType(const std::string& type);
    void setError(const std::string& message);

    // Grammar helpers
    bool parseExternalDeclaration();
    bool parseFunctionDefinition(const std::vector<std::string>& declSpecifiers, bool isTypedef);
    bool parseDeclaration(const std::vector<std::string>& declSpecifiers, bool isTypedef);
    bool parseDeclarationSpecifiers(std::vector<std::string>& outSpecifiers, bool& isTypedef);
    bool parseInitDeclaratorList(bool requireInitializer);
    bool parseInitDeclarator(bool requireInitializer, bool& isFunctionDecl);
    bool parseDeclarator(bool& isFunction);
    bool parsePointer();
    bool parseDirectDeclarator(bool& isFunction);
    bool parseParameterTypeList();
    bool parseParameterDeclaration();
    bool parseInitializer();

    bool parseStatement();
    bool parseLabeledStatement();
    bool parseCompoundStatement();
    bool parseBlockItem();
    bool parseExpressionStatement();
    bool parseSelectionStatement();
    bool parseIterationStatement();
    bool parseJumpStatement();

    bool parseExpression();
    bool parseAssignmentExpression();
    bool parseConditionalExpression();
    bool parseBinaryExpression(int minPrecedence);
    bool parseUnaryExpression();
    bool parsePostfixExpression();
    bool parsePrimaryExpression();
    bool parseArgumentExpressionList();

    int getBinaryPrecedence(const std::string& op) const;
    bool isAssignmentOperator(const std::string& op) const;
    bool isTypeSpecifier(const Token& token) const;
    bool isTypeQualifier(const Token& token) const;
    bool isStorageClassSpecifier(const Token& token) const;
};