#include "TableOfNodes.h"

#include <algorithm>
#include <cassert>
#include <deque>
#include <fstream>
#include <iostream>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <utility>

#define INVALID -1
#define ABORT -2

// mode with debug prints

// #define DEBUG_MODE  // Or 0 if you dont want to debug
#ifdef DEBUG_MODE
#define Debug(x) x
#else
#define Debug(x)
#endif

// flag which activates/deactivates exprToId, idToExpr
// #define VISUAL_MODE  // Or 0 if you dont want to debug
#ifdef VISUAL_MODE
#define Visual(x) x
#else
#define Visual(x)
#endif

std::vector<int> operator-(const std::vector<int>& v,
                           const std::vector<int>& w) {
  std::vector<int> result(v.size(), 0);
  for (int i = 0; i < v.size(); i++) {
    result[i] = v[i] - w[i];
  }
  return result;
}

bool Node::operator==(const Node& n) const {
  if (n.successors == this->successors && n.b == this->b) {
    return true;
  } else {
    return false;
  }
}

TableOfNodes::TableOfNodes(std::vector<char> a) : alphabet{std::move(a)} {
  // create the mapping
  for (int i = 0; i < alphabet.size(); i++) {
    if (alphabet[i] == 'T') {
      separatorIndex = i;
    }
    if (alphabet[i] == 'X') {
      paddingIndex = i;
    }
    if (alphabet[i] == '1') {
      oneIndex = i;
    }
    charToIndex.insert(std::make_pair(alphabet[i], i));
  }
  // successor tuple for
  std::vector<int> s1(alphabet.size(), EMTPYSET);
  std::vector<int> s2(alphabet.size(), SIGMASTAR);
  std::vector<int> s3(alphabet.size(), SELF);
  // insert empty state and sigma* state
  Node empty{s1, false};
  Node sigma{s2, true};
  Node emptyword{s1, true};
  Node emptySelf{s3, false};
  Node sigmaSelf{s3, true};
  idToNode.push_back(empty);
  idToNode.push_back(sigma);
  idToNode.push_back(emptyword);
  nodeToId.emplace(Node{empty.successors, empty.b}, EMTPYSET);
  nodeToId.emplace(Node{sigma.successors, sigma.b}, SIGMASTAR);
  nodeToId.emplace(Node{emptyword.successors, emptyword.b}, EMPTYWORD);
  nodeToIdWithSelf.emplace(Node{emptySelf.successors, empty.b}, EMTPYSET);
  nodeToIdWithSelf.emplace(Node{sigmaSelf.successors, sigma.b}, SIGMASTAR);
  Visual(exprToId.emplace("&#8709;", 0));
  Visual(exprToId.emplace("&#931;*", 1));
  Visual(exprToId.emplace("&#949;", 2));
  Visual(exprToId.emplace("", 2));
  // for create
  Visual(idToExpr.emplace(0, "&#8709;"));
  Visual(idToExpr.emplace(1, "&#931;*"));
  Visual(idToExpr.emplace(2, "&#949;"));
}

int TableOfNodes::make(Node n) {
  // no check if n.successors all known
  // check if successors same length as alphabet
  if (n.successors.size() != alphabet.size()) {
    throw std::invalid_argument("successors not same size as alphabet");
  }
  if (auto node = nodeToId.find(Node{n.successors, n.b});
      node != nodeToId.end()) {
    return node->second;
    // case id:4 [4,2] with [SELF,2] are equal - fixed
    // TODO: combination self and node check ??, now works because only SELF in
    // 1 is possible, one SELF per successor vector
    // TODO: only one nodeToId hash map
  } else if (auto node = nodeToIdWithSelf.find(Node{n.successors, n.b});
             node != nodeToIdWithSelf.end()) {
    return node->second;
  } else {
    int newId = idToNode.size();
    Node withSelf = Node{n.successors, n.b};
    // case only SELF and EMPTYSET
    int selfCount = 0;
    int emptyCount = 0;
    // remove SELF
    for (int i = 0; i < n.successors.size(); i++) {
      if (n.successors[i] == SELF) {
        selfCount++;
        n.successors[i] = newId;
      }
      if (n.successors[i] == EMTPYSET) {
        emptyCount++;
      }
    }
    if (!n.b && (selfCount + emptyCount) == alphabet.size()) {
      return EMTPYSET;
    }
    // TODO: garbage collection
    //  vector freePlace
    idToNode.push_back(n);
    Node withoutSelf = Node{n.successors, n.b};
    if (!(withSelf == withoutSelf)) {
      nodeToIdWithSelf.emplace(withSelf, newId);
    }
    nodeToId.emplace(withoutSelf, newId);
    return newId;
  }
}

// creates a new node from a very simple weakly acyclic regular expression and
// all its successors into the table expression of the form  r = a in sigma | a*
// (a in sigma) | rr
// TODO: case a*a or a*b*a does not work properly but we ignore it
int TableOfNodes::create(std::string expression) {
  // already computed look up
  Visual(if (exprToId.contains(expression)) { return exprToId[expression]; });

  std::vector<int> s(alphabet.size(), EMTPYSET);

  // TODO: assert this can never happen
  // assert(!expression.empty());
  // recursion base case
  // state for empty word
  if (expression.empty()) {
    int newId = make(Node{s, true});
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
      if (nextWithoutStar != curChar) {
        s[charToIndex[nextWithoutStar]] = create(expression.substr(index + 2));
      } else {
        // use union
        // ignore this case (a*...a)
        if (index == 1 && curChar == nextWithoutStar) {
          std::stringstream ss;
          ss << curChar << "*" << expression.substr(index + 2);
          std::string str = ss.str();
          s[charToIndex[nextWithoutStar]] = create(str);
        }
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

  Visual(exprToId.emplace(expression, newId));
  Visual(idToExpr.emplace(newId, expression));

  return newId;
}

int TableOfNodes::createFromMarking(std::vector<int> marking) {
  std::stringstream str;
  for (int i = 0; i < marking.size(); i++) {
    for (int j = 0; j < marking[i]; j++) {
      str << "1";
    }
    str << "T";
  }
  Debug(std::cout << str.str() << std::endl);
  return create(str.str());
}

int TableOfNodes::createSetFromMarking(std::vector<int> marking,
                                       std::vector<int> markingSet) {
  std::stringstream str;
  for (int i = 0; i < marking.size(); i++) {
    for (int j = 0; j < marking[i]; j++) {
      str << "1";
    }
    if (markingSet[i] == 1) {
      str << "1*";
    }
    str << "T";
  }
  Debug(std::cout << str.str() << std::endl);
  return create(str.str());
}

int TableOfNodes::createUpwardSetFromMarking(std::vector<int> marking) {
  std::stringstream str;
  for (int i = 0; i < marking.size(); i++) {
    for (int j = 0; j < marking[i]; j++) {
      str << "1";
    }
    str << "1*";
    str << "T";
  }
  Debug(std::cout << str.str() << std::endl);
  return create(str.str());
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
  // if both are the same
  if (n1 == n2) {
    return n1;
  }

  // switch n1 and n2

  unsigned long concat = (unsigned long)n1 << 32 | n2;
  unsigned long catcon = (unsigned long)n2 << 32 | n1;

  // already computed lookup, union commutative
  if (unionToId.contains(concat)) {
    return unionToId[concat];
  }
  if (unionToId.contains(catcon)) {
    return unionToId[catcon];
  }

  unionToId.emplace(concat, SELF);

  std::vector<int> s(alphabet.size(), EMTPYSET);

  for (int i = 0; i < alphabet.size(); i++) {
    s[i] = unionOfNodes(idToNode[n1].successors[i], idToNode[n2].successors[i]);
  }

  // calculate if either n1 or n2 accepts empty word
  bool b = (idToNode[n1].b || idToNode[n2].b);

  int newId = make(Node{s, b});

  unionToId[concat] = newId;

  // TODO: make dependent on boolean flag
  // fill in expression
  Visual(std::string s1 = idToExpr[n1]);
  Visual(std::string s2 = idToExpr[n2]);
  // TODO: maybe commutative insertions here

  Visual(exprToId.emplace(s1 + "+" + s2, newId));
  Visual(idToExpr.emplace(newId, s1 + "+" + s2));

  return newId;
}

int TableOfNodes::interOfNodes(int n1, int n2) {
  // if one is empty
  if (n1 == EMTPYSET || n2 == EMTPYSET) {
    return EMTPYSET;
  }
  // if one is sigma*
  if (n1 == SIGMASTAR) {
    return n2;
  } else if (n2 == SIGMASTAR) {
    return n1;
  }
  // if both are the same
  if (n1 == n2) {
    return n1;
  }

  unsigned long concat = (unsigned long)n1 << 32 | n2;
  unsigned long catcon = (unsigned long)n2 << 32 | n1;

  // already computed lookup, intersection commutative
  if (interToId.contains(concat)) {
    return interToId[concat];
  }
  if (interToId.contains(catcon)) {
    return interToId[catcon];
  }

  interToId.emplace(concat, SELF);

  std::vector<int> s(alphabet.size(), EMTPYSET);

  for (int i = 0; i < alphabet.size(); i++) {
    s[i] = interOfNodes(idToNode[n1].successors[i], idToNode[n2].successors[i]);
  }

  // calculate if both n1 or n2 accepts empty word
  bool b = (idToNode[n1].b && idToNode[n2].b);

  int newId = make(Node{s, b});

  interToId[concat] = newId;

  // fill in expression
  std::string s1 = idToExpr[n1];
  std::string s2 = idToExpr[n2];
  // TODO: maybe commutative insertions here
  Visual(exprToId.emplace(s1 + "^" + s2, newId));
  Visual(idToExpr.emplace(newId, s1 + "^" + s2));

  return newId;
}

int TableOfNodes::preprocess(std::vector<int> transitionDelta, int marking,
                             int level, std::unordered_map<int, int>& cache) {
  if (cache.contains(marking)) {
    return cache[marking];
  }

  cache[marking] = SELF;

  Node n = idToNode.at(marking);

  assert(n.successors[paddingIndex] == EMTPYSET);

  bool b = n.b;
  std::vector<int> successors(alphabet.size(), EMTPYSET);

  // if current node has outgoing separator edge (T)
  // AND paddings should be added
  if (n.successors[separatorIndex] != EMTPYSET && transitionDelta[level] > 0) {
    assert(level < transitionDelta.size());
    std::vector<int> newSeparatorSuccessors(alphabet.size(), EMTPYSET);
    // successors of the last padding node
    newSeparatorSuccessors[separatorIndex] = preprocess(
        transitionDelta, n.successors[separatorIndex], level + 1, cache);
    // create the last padding node
    int newSeparator = make(Node{newSeparatorSuccessors, false});
    newSeparatorSuccessors[separatorIndex] = EMTPYSET;
    // add inner padding nodes
    for (int j = 0; j < transitionDelta[level] - 1; j++) {
      newSeparatorSuccessors[paddingIndex] = newSeparator;
      newSeparator = make(Node{newSeparatorSuccessors, false});
    }
    // adapt the successors of current node
    successors[separatorIndex] = EMTPYSET;
    successors[paddingIndex] = newSeparator;
    successors[oneIndex] =
        preprocess(transitionDelta, n.successors[oneIndex], level, cache);
  }
  // if current node no outgoing separator (T) edge
  // OR no paddings should be added
  else {
    for (int i = 0; i < alphabet.size(); i++) {
      // if outgoing separator (T) edge increase level
      if (i == separatorIndex && n.successors[separatorIndex] != EMTPYSET) {
        successors[i] =
            preprocess(transitionDelta, n.successors[i], level + 1, cache);
      } else {
        successors[i] =
            preprocess(transitionDelta, n.successors[i], level, cache);
      }
    }
  }

  int newId = make(Node{successors, b});

  cache[marking] = newId;

  return newId;
}

int TableOfNodes::postprocess(std::vector<int> transitionDelta, int marking,
                              int levelSeparators,
                              std::unordered_map<int, int>& cache) {
  if (cache.contains(marking)) {
    // std::cout << "postprocess cache used for " << marking << " "
    //           << cache[marking] << std::endl;
    return cache[marking];
  }

  cache[marking] = SELF;

  Node n = idToNode.at(marking);

  bool b = n.b;
  std::vector<int> successors(alphabet.size(), EMTPYSET);

  // if current node has outgoing padding edge (X)
  // AND paddings exists which need to be removed
  // TODO: what if outgoing separator edge (T) exists here too ? // cannot be
  if (n.successors[paddingIndex] != EMTPYSET) {
    assert(levelSeparators < transitionDelta.size());
    assert(transitionDelta[levelSeparators] > 0);
    assert(n.successors[separatorIndex] == EMTPYSET);
    Node next = n;
    for (int j = 0; j < transitionDelta[levelSeparators]; j++) {
      if (j > 0) {
        assert(next.successors[paddingIndex] != EMTPYSET &&
               next.successors[separatorIndex] == EMTPYSET &&
               next.successors[oneIndex] == EMTPYSET);
      }
      // TODO: assertions only 'X' transition - done
      next = idToNode.at(next.successors[paddingIndex]);
    }
    // TODO: assertions only 'T' transition - done
    assert(next.successors[paddingIndex] == EMTPYSET &&
           next.successors[separatorIndex] != EMTPYSET &&
           next.successors[oneIndex] == EMTPYSET);
    // adapt the successors of current node
    successors[paddingIndex] = EMTPYSET;
    successors[separatorIndex] =
        postprocess(transitionDelta, next.successors[separatorIndex],
                    levelSeparators + 1, cache);
  }
  // if current node no outgoing separator (X) edge
  else {
    successors[separatorIndex] =
        postprocess(transitionDelta, n.successors[separatorIndex],
                    levelSeparators + 1, cache);
  }

  successors[oneIndex] = postprocess(transitionDelta, n.successors[oneIndex],
                                     levelSeparators, cache);

  int newId = make(Node{successors, b});

  cache[marking] = newId;

  return newId;
}

int TableOfNodes::pre(const Transducer& t, int dfa) {
  preState startB = std::make_pair(t.initialState, dfa);
  // cache for pre
  preCacheSet preToId;
  // cache for transitions
  preTransitions transCache;

  int newId;

  std::unordered_set<preState> loop;
  std::unordered_set<preState> preStart{startB};

  newId = preIntoTable(t, preStart, preToId, transCache, loop);

  return newId;
}

// given start prestate and char calculates end states
std::unordered_set<preState> TableOfNodes::constructPreSingleTrans(
    const preState& pq, char x, const Transducer& t) {
  std::unordered_set<preState> pqPrimes;
  for (int i = 0; i < alphabet.size(); i++) {
    char c = alphabet[i];
    auto tKey = std::make_pair(pq.first, std::make_pair(x, c));
    if (idToNode[pq.second].successors[i] != EMTPYSET &&
        t.transitions.contains(tKey)) {
      auto pPrimes = t.transitions.at(tKey);
      for (const int& j : pPrimes) {
        preState pqPrime = std::make_pair(j, idToNode[pq.second].successors[i]);
        pqPrimes.insert(pqPrime);
      }
    }
  }
  return pqPrimes;
}

// TODO: THINK std::variant for loop
// TODO: THINK struct for t, preStarts, cache, transCache
// recursion here
int TableOfNodes::preIntoTable(const Transducer& t,
                               const std::unordered_set<preState>& preStarts,
                               preCacheSet& cache, preTransitions& transCache,
                               std::unordered_set<preState>& loop) {
  // already in cache
  if (cache.contains(preStarts)) {
    return cache.at(preStarts);
  }
  cache.emplace(preStarts, INVALID);

  std::vector<int> successors(alphabet.size(), EMTPYSET);

  // set b correctly
  bool b = false;
  for (const auto& s : preStarts) {
    if (t.endStates.contains(s.first) && idToNode[s.second].b) {
      b = true;
      break;
    }
  }

  for (int i = 0; i < alphabet.size(); i++) {
    std::unordered_set<preState> nextStates;
    for (const auto& s : preStarts) {
      std::pair<preState, char> startStateChar = std::make_pair(s, alphabet[i]);
      std::unordered_set<preState> nextStatesSingle;
      if (transCache.contains(startStateChar)) {
        // std::cout << "used transCache" << std::endl;
        nextStatesSingle = transCache.at(startStateChar);
      } else {
        // union the sets
        nextStatesSingle = constructPreSingleTrans(s, alphabet[i], t);
        transCache.emplace(startStateChar, nextStatesSingle);
      }
      nextStates.insert(nextStatesSingle.begin(), nextStatesSingle.end());
    }

    if (!nextStates.empty()) {
      if (nextStates == preStarts) {
        successors[i] = SELF;
      } else if (cache.contains(nextStates) &&
                 cache.at(nextStates) == INVALID) {
        // throw std::invalid_argument("not weakly acyclic");
        std::cout << "LOOP DETECTED \n";
        loop = nextStates;
        // weakly acyclic => must be same node
        return ABORT;
      } else {
        successors[i] = preIntoTable(t, nextStates, cache, transCache, loop);
        if (successors[i] == ABORT) {
          // go up recursion layers
          if (loop != preStarts) {
            Debug(std::cout << "abort" << std::endl);
            return ABORT;
          }
          // arrived at top
          else {
            Debug(std::cout << "arrived" << std::endl);
            loop.clear();
            successors[i] = SELF;
          }
        }
      }
    }
  }

  int newId = make(Node{successors, b});

  cache[preStarts] = newId;
  return newId;
}

// input dfa is without paddings, returns node without paddings
int TableOfNodes::preWrapper(const Transducer& t, int dfa) {
  std::vector<int> deltaPreprocess = t.pnTrans.first - t.pnTrans.second;
  std::vector<int> deltaPostprocess = t.pnTrans.second - t.pnTrans.first;

  std::unordered_map<int, int> cache;

  // TODO: add EMPTYSET, SIGMASTAR AND EMPTYWORD to cache

  // toDotCompact(dfa, "beforePreprocessed");

  // preprocess
  int dfaPreprocessed = preprocess(deltaPreprocess, dfa, 0, cache);

  // toDotCompact(dfaPreprocessed, "afterPreprocessed");

  // pre
  int preResult = pre(t, dfaPreprocessed);

  // toDotCompact(preResult, "beforePostprocessed");

  cache.clear();

  // postprocess;
  int resPostprocessed = postprocess(deltaPostprocess, preResult, 0, cache);

  // toDotCompact(resPostprocessed, "afterPostprocessed");

  return resPostprocessed;
}

int TableOfNodes::sizeOf(int dfa) { return sizeOfRec(dfa).size(); }

std::unordered_set<int> TableOfNodes::sizeOfRec(int dfa) {
  if (idToReachable.contains(dfa)) {
    return idToReachable.at(dfa);
  }
  std::unordered_set<int> allReachables{dfa};
  Node& n = idToNode[dfa];
  for (int succ : n.successors) {
    if (succ == dfa) {
      continue;
    }
    std::unordered_set<int> cur = sizeOfRec(succ);
    allReachables.insert(cur.cbegin(), cur.cend());
  }

  idToReachable.emplace(dfa, allReachables);
  return allReachables;
}

int TableOfNodes::maxHeightOf(int dfa) {
  if (idToHeight.contains(dfa)) {
    // std::cout << "used cache height" << std::endl;
    return idToHeight.at(dfa);
  }
  if (dfa == EMTPYSET || dfa == SIGMASTAR) {
    return 0;
  }
  Node& n = idToNode[dfa];
  int maxAll = 0;
  for (int i : n.successors) {
    if (i == dfa) {
      continue;
    }
    int maxSucc = maxHeightOf(i);
    if (maxSucc > maxAll) {
      maxAll = maxSucc;
    }
  }
  idToHeight.emplace(dfa, maxAll + 1);
  return maxAll + 1;
}

int TableOfNodes::maxTokenOf(int dfa) {}

int TableOfNodes::maxPathsOf(int dfa) {
  if (dfa == EMPTYWORD || dfa == SIGMASTAR) {
    return 1;
  }
  int sum = 0;
  Node& n = idToNode[dfa];
  for (auto succ : n.successors) {
    if (succ != EMTPYSET && succ != dfa) {
      sum += maxPathsOf(succ);
    }
  }
  return sum;
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

std::string TableOfNodes::toStringUnions() {
  std::ostringstream str;
  str << "########################################## \n";
  str << "UNIONTOID CONTAINS FOLLOWING ELEMENTS \n \n";
  for (auto& [a, b] : unionToId) {
    int n2 = (a << 32) >> 32;
    int n1 = a >> 32;
    str << "n1 is " << n1 << "\n";
    str << "n2 is " << n2 << "\n";
    str << "union is " << b;
    str << "\n";
  }
  str << "\n";
  str << "##########################################";
  return str.str();
}

void TableOfNodes::toDot(std::string fileName) {
  if (fileName == "") {
    fileName = "table";
  }
  std::string pathName = "./visualization/" + fileName + ".dot";
  std::string imgName = "./visualization/" + fileName + ".png";
  std::ofstream str(pathName, std::ofstream::out);
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
  std::string command = "dot " + pathName + " -Tpng > " + imgName;
  system(command.c_str());
}

void TableOfNodes::toDotCompact(std::string fileName) {
  if (fileName == "") {
    fileName = "tableCompact";
  }
  std::string pathName = "./visualization/" + fileName + ".dot";
  std::string imgName = "./visualization/" + fileName + ".png";
  std::ofstream str(pathName, std::ofstream::out);
  str << "digraph g\n";
  str << "{\n";
  //   str << "graph [ splines = false rankdir=\"LR\" ] \n";
  str << "graph [ splines = false ] \n";

  for (int i = 1; i < idToNode.size(); i++) {
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
      if (n.successors[j] == EMTPYSET) {
        continue;
      }
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
  std::string command = "dot " + pathName + " -Tpng > " + imgName;
  system(command.c_str());
}

void TableOfNodes::toDot(int id, std::string fileName) {
  if (fileName == "") {
    fileName = "tableSingle";
  }
  std::string pathName = "./visualization/" + fileName + ".dot";
  std::string imgName = "./visualization/" + fileName + ".png";
  std::ofstream str(pathName, std::ofstream::out);
  str << "digraph g\n";
  str << "{\n";
  //   str << "graph [ splines = false rankdir=\"LR\" ] \n";
  str << "graph [ splines = false ] \n";

  std::deque<int> workset;
  std::unordered_set<int> done;
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
        Debug(std::cout << "SELF OH NO " << std::endl);
        str << current << " -> " << current << " [label=" << alphabet[j]
            << "] \n";
      } else {
        if (!done.contains(n.successors[j]) &&
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
  std::string command = "dot " + pathName + " -Tpng > " + imgName;
  system(command.c_str());
}

void TableOfNodes::toDotCompact(int id, std::string fileName) {
  if (fileName == "") {
    fileName = "tableSingleCompact";
  }
  std::string pathName = "./visualization/" + fileName + ".dot";
  std::string imgName = "./visualization/" + fileName + ".png";
  std::ofstream str(pathName, std::ofstream::out);
  str << "digraph g\n";
  str << "{\n";
  //   str << "graph [ splines = false rankdir=\"LR\" ] \n";
  str << "graph [ splines = false ] \n";

  std::deque<int> workset;
  std::unordered_set<int> done;
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
      if (n.successors[j] == EMTPYSET) {
        continue;
      }
      if (n.successors[j] == SELF) {
        Debug(std::cout << "SELF OH NO, THIS CAN NOT HAPPEN why ?? "
                        << std::endl);
        str << current << " -> " << current << " [label=" << alphabet[j]
            << "] \n";
      } else {
        if (!done.contains(n.successors[j]) &&
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
  std::string command = "dot " + pathName + " -Tpng > " + imgName;
  system(command.c_str());
}

void TableOfNodes::preToDot(int dfa, const Transducer& t, int preId) {
  toDot(dfa, "preDFA");
  t.toDot("preTransducer");
  toDot(preId, "preResult");
}
