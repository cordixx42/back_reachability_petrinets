#include <algorithm>
#include <array>
#include <deque>
#include <fstream>
#include <iostream>
#include <ranges>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "TableOfNodes.h"

bool Node::operator==(const Node& n) const {
  if (n.successors == this->successors && n.b == this->b) {
    return true;
  } else {
    return false;
  }
}

std::string Node::toString() {
  std::ostringstream str;
  str << "[ ";
  for (const auto i : this->successors) {
    if (i == -1) {
      str << "SELF";
    } else {
      str << i;
    }
    str << ", ";
  }
  str << "] ";
  str << this->b;
  return str.str();
}

TableOfNodes::TableOfNodes(std::vector<char> a) {
  std::cout << "alphabet is " << a[0] << a[1] << std::endl;
  alphabet = a;
  // create the mapping
  for (int i = 0; i < alphabet.size(); i++) {
    charToIndex.insert(std::make_pair(alphabet[i], i));
  }
  // successor tuple for
  std::vector<int> s(alphabet.size(), SELF);
  // insert empty state and sigma* state
  Node empty{s, false};
  Node sigma{s, true};
  idToNode.push_back(empty);
  idToNode.push_back(sigma);
  nodeToId.emplace(Node{empty.successors, empty.b}, EMTPYSET);
  nodeToId.emplace(Node{sigma.successors, sigma.b}, SIGMASTAR);
  exprToId.emplace("&#8709;", 0);
  exprToId.emplace("&#931;*", 1);
  idToExpr.emplace(0, "&#8709;");
  idToExpr.emplace(1, "&#931;*");
}

int TableOfNodes::make(Node n) {
  // no check if n.successors all known ?
  if (auto node = nodeToId.find(Node{n.successors, n.b});
      node != nodeToId.end()) {
    return node->second;
  } else {
    int newId = idToNode.size();
    idToNode.push_back(n);
    nodeToId.emplace(Node{n.successors, n.b}, newId);
    return newId;
  }
}

// creates a new node from a very simple weakly acyclic regular expression and
// all its successors into the table expression of the form  r = a in sigma | a*
// (a in sigma) | rr
// TODO: case a*a, a*b*a*, or a*b*a does not work properly
int TableOfNodes::create(std::string expression) {
  // already computed look up
  if (exprToId.contains(expression)) {
    return exprToId[expression];
  }

  std::vector<int> s(alphabet.size(), EMTPYSET);

  // recursion base case
  // state for empty word
  if (expression.empty()) {
    int newId = make(Node{s, true});
    exprToId.emplace("", newId);
    idToExpr.emplace(newId, "");
    return newId;
  }

  char curChar = expression[0];

  // this one is a*
  if (expression.length() > 1 && expression[1] == '*') {
    // a -> a
    s[charToIndex[curChar]] = SELF;
    std::vector<char> nextStars;
    int index = 1;

    while (expression.length() > (index + 2) && expression[index + 2] == '*') {
      nextStars.push_back(expression[index + 1]);
      index += 2;
    }

    for (int i = 0; i < nextStars.size(); i++) {
      char cur = nextStars[i];
      if (cur != curChar) {
        s[charToIndex[cur]] = create(expression.substr(2 + (i * 2)));
      }
    }

    // if index + 1 exists
    if (expression.length() > index + 1) {
      char nextWithoutStar = expression[index + 1];
      // TODO: if next without star neither curchar, nor in the nextStars vector
      if (nextWithoutStar != curChar) {
        s[charToIndex[nextWithoutStar]] = create(expression.substr(index + 2));
      } else {
        // TODO: use union
      }
    }

  }
  // this one is a
  else {
    s[charToIndex[curChar]] = create(expression.substr(1));
  }

  // check if empty word accepted
  auto count = std::count(expression.begin(), expression.end(), '*');
  bool b = false;
  // if half of it are *
  if ((expression.length() % 2 == 0) && (expression.length() / 2 == count)) {
    b = true;
  }

  int newId = make(Node{s, b});

  exprToId.emplace(expression, newId);
  idToExpr.emplace(newId, expression);

  return newId;
}

int TableOfNodes::unionOfNodes(int n1, int n2) {
  // if one is sigma*
  if (n1 == SIGMASTAR || n2 == SIGMASTAR) {
    return SIGMASTAR;
  }
  // if one is empty
  if (n1 == EMTPYSET) {
    return n2;
  } else if (n2 == EMTPYSET) {
    return n1;
  }

  unsigned long concat = (unsigned long)n1 << 32 | n2;
  unsigned long catcon = (unsigned long)n2 << 32 | n1;

  // already computed lookup
  if (unionToId.contains(concat) || unionToId.contains(catcon)) {
    return unionToId[concat];
  }

  std::vector<int> s(alphabet.size(), EMTPYSET);

  for (int i = 0; i < alphabet.size(); i++) {
    std::cout << i << std::endl;
    if (idToNode[n1].successors[i] == SELF &&
        idToNode[n2].successors[i] == SELF) {
      s[i] = SELF;
    } else if (idToNode[n1].successors[i] == SELF) {
      s[i] = idToNode[n1].successors[i];
    } else if (idToNode[n2].successors[i] == SELF) {
      s[i] = idToNode[n2].successors[i];
    } else {
      s[i] =
          unionOfNodes(idToNode[n1].successors[i], idToNode[n2].successors[i]);
    }
  }

  // calculate if either n1 or n2 accepts empty word
  bool b = (idToNode[n1].b || idToNode[n2].b);

  int newId = make(Node{s, b});

  unionToId.emplace(concat, newId);

  // fill in expression
  std::string s1 = idToExpr[n1];
  std::string s2 = idToExpr[n2];
  exprToId.emplace(s1 + "+" + s2, newId);
  idToExpr.emplace(newId, s1 + "+" + s2);

  return newId;
}

std::string TableOfNodes::toString() {
  std::ostringstream str;
  str << "########################################## \n";
  str << "TABLE OF NODES CONTAINS FOLLOWING ELEMENTS \n \n";
  for (int i = 0; i < idToNode.size(); i++) {
    str << i;
    str << " ";
    str << idToNode[i].toString();
    str << "\n";
  }
  str << "\n";
  str << "##########################################";
  return str.str();
}

void TableOfNodes::toDot() {
  std::ofstream str("./visualization/test.dot", std::ofstream::out);
  str << "digraph g\n";
  str << "{\n";
  str << "graph [ splines = false ] \n";
  // str << "graph [ splines = false rankdir=\"LR\" ] \n";

  for (int i = 0; i < idToNode.size(); i++) {
    const Node& n = idToNode[i];
    // label node with id and expr
    std::string expr = idToExpr[i];
    std::string shape = "octagon";
    if (n.b) {
      shape = "doubleoctagon";
    }
    str << i << " [shape=" << shape << " label=\"" << i << " " << expr
        << "\"]\n";
    // for every successor insert edge
    // does this work:  for(const &s : n.successors; const &c : alphabet)
    for (int j = 0; j < alphabet.size(); j++) {
      if (n.successors[j] == SELF) {
        str << i << " -> " << i << " [label=" << alphabet[j] << "] \n";
      } else {
        str << i << " -> " << n.successors[j] << " [label=" << alphabet[j]
            << "] \n";
      }
    }
  }
  str << "}\n";
  str.close();
  system("dot ./visualization/test.dot -Tpng > ./visualization/test.png");
}

void TableOfNodes::toDot(int id) {
  std::ofstream str("./visualization/testSingle.dot", std::ofstream::out);
  str << "digraph g\n";
  str << "{\n";
  //   str << "graph [ splines = false rankdir=\"LR\" ] \n";
  str << "graph [ splines = false ] \n";

  std::deque<int> workset;
  std::set<int> done;
  workset.push_back(id);

  while (!workset.empty()) {
    int current = workset.front();
    workset.pop_front();
    done.insert(current);
    const Node& n = idToNode[current];
    // label node with id and expr
    std::string expr = idToExpr[current];
    std::string shape = "octagon";
    if (n.b) {
      shape = "doubleoctagon";
    }
    str << current << " [shape=" << shape << " label=\"" << current << " "
        << expr << "\"]\n";
    for (int j = 0; j < alphabet.size(); j++) {
      if (n.successors[j] == SELF) {
        str << current << " -> " << current << " [label=" << alphabet[j]
            << "] \n";
      } else {
        if (std::find(done.begin(), done.end(), n.successors[j]) ==
                done.end() &&
            std::find(workset.begin(), workset.end(), n.successors[j]) ==
                workset.end()) {
          workset.push_back(n.successors[j]);
        }
        str << current << " -> " << n.successors[j] << " [label=" << alphabet[j]
            << "] \n";
      }
    }
  }

  str << "}\n";
  str.close();
  system(
      "dot ./visualization/testSingle.dot -Tpng > "
      "./visualization/testSingle.png");
}