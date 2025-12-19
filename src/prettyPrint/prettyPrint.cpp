#include "prettyPrint.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace prettyPrint {

static std::string nodeLabel(const Node& node, const Options& opt) {
  std::ostringstream ss;
  ss << node.getType();

  if (opt.showTokenValue && node.getToken().has_value()) {
    ss << "-" << node.getToken()->getValue();
  }
  return ss.str();
}

static void printTreeRec(
    const Node& node,
    std::ostream& os,
    const Options& opt,
    const std::string& prefix,
    bool isLast
) {
  // Branch glyphs
  const char* tee   = opt.unicodeBranches ? "├── " : "|-- ";
  const char* elbow = opt.unicodeBranches ? "└── " : "`-- ";
  const char* pipe  = opt.unicodeBranches ? "│"    : "|";
  const char* blank = " ";

  // Print this node
  os << prefix << (isLast ? elbow : tee) << nodeLabel(node, opt) << "\n";

  // Children
  const auto& children = node.getChildren();
  if (children.empty()) {
    if (opt.showEmptyChildren) {
      // nothing else to show
    }
    return;
  }

  // Prefix for children depends on whether current node is last
  std::string childPrefix = prefix;
  if (opt.unicodeBranches) {
    childPrefix += isLast ? "    " : "│   ";
  } else {
    // ASCII indentation
    // "    " or "|   " style; keep it consistent with above
    childPrefix += isLast ? "    " : "|   ";
  }

  for (size_t i = 0; i < children.size(); ++i) {
    const bool childIsLast = (i + 1 == children.size());
    printTreeRec(children[i], os, opt, childPrefix, childIsLast);
  }
}

void printTree(const Node& root, std::ostream& os, const Options& opt) {
  // Print root label without a leading branch, then its children with branches
  os << nodeLabel(root, opt) << "\n";

  const auto& children = root.getChildren();
  if (children.empty()) return;

  for (size_t i = 0; i < children.size(); ++i) {
    const bool isLast = (i + 1 == children.size());
    // start prefix empty for first level
    printTreeRec(children[i], os, opt, "", isLast);
  }
}

void printForest(const std::vector<Node>& roots, std::ostream& os, const Options& opt) {
  for (size_t i = 0; i < roots.size(); ++i) {
    os << "Root[" << i << "]\n";
    printTree(roots[i], os, opt);
    if (i + 1 < roots.size()) os << "\n";
  }
}

} // namespace prettyPrint
