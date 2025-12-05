
#ifndef COMPILER_LAB_NODE_H
#define COMPILER_LAB_NODE_H
#include "./../Symbol.h"
#include <vector>
#include <ostream>
#include "Token.h"
#include <optional>

class Node {
    Symbol type;
    std::vector<Node> children;
    std::optional<Token> token;

public:
    Node(Symbol type, std::vector<Node> children, std::optional<Token> tok);
    Node(Symbol type);
    Node(std::string val);

    Symbol getType() const { return type; }
    void setType(Symbol type) { this->type = type; }
    std::vector<Node> getChildren() const { return children; }
    void setChildren(const std::vector<Node> &children) { this->children = children; }
    std::optional<Token> getToken() const { return token; }
    void setToken(const Token &tok) { token = tok; }

    void addChild(Node child) {
        children.push_back(child);
    }

    friend std::ostream& operator<<(std::ostream& os, const Node& node);
};


#endif //COMPILER_LAB_NODE_H