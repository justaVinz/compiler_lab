#pragma once

#include <iosfwd>
#include <string>
#include <vector>

#include "../helper/structs/Node.h"

namespace prettyPrint {

struct Options {
  bool unicodeBranches = true;   // true: ├── └── │  ; false: |-- `-- |  
  bool showTokenValue = true;    // append "-<tokenValue>" when node has token
  bool showEmptyChildren = false; // print children section even if empty (usually false)
  int  indentSize = 2;           // only used for ascii mode spacing
};

// Print a single tree (one root Node)
void printTree(const Node& root, std::ostream& os, const Options& opt = Options{});

// Convenience: print multiple roots as a forest
void printForest(const std::vector<Node>& roots, std::ostream& os, const Options& opt = Options{});

} // namespace prettyPrint
