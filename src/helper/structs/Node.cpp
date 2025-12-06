//
// Created by Vinzenz Brehme on 25.11.25.
//

#include "Node.h"

Node::Node(Symbol type, std::vector<Node> children, std::optional<Token> tok)
    : type(std::move(type)), children(std::move(children)) , token(tok) {}

Node::Node(Symbol type)
    : type(type), children() {}

Node::Node(std::string val)
    : type(terminal), children(), token(Token("", val, -1, -1)) {}

std::ostream& operator<<(std::ostream& os, const Node& node) {
    os << "Node(Type=" << (node.type) << ", Children=[";
    for (size_t i = 0; i < node.children.size(); ++i) {
        os << node.children[i];
        if (i + 1 < node.children.size()) os << ", ";
    }
    os << "])";
    return os;
}
