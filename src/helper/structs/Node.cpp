//
// Created by Vinzenz Brehme on 25.11.25.
//

#include "Node.h"

Node::Node(Symbol type, std::vector<Node> children)
    : type(std::move(type)), children(std::move(children)) {}

Node::Node()
    : type(DUMMY), children() {}

std::ostream& operator<<(std::ostream& os, const Node& node) {
    os << "Node(Type=" << static_cast<int>(node.type) << ", Children=[";
    for (size_t i = 0; i < node.children.size(); ++i) {
        os << node.children[i];
        if (i + 1 < node.children.size()) os << ", ";
    }
    os << "])";
    return os;
}
