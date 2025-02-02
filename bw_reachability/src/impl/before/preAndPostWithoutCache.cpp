int TableOfNodes::preprocess(std::vector<int> transitionDelta, int marking,
                             int level, std::unordered_map<int, int>& cache) {
  if (marking == EMTPYSET || marking == SIGMASTAR || marking == EMPTYWORD) {
    return marking;
  }

  // TODO: make these fixed numbers ?
  int separatorIndex = charToIndex.at('T');
  int paddingIndex = charToIndex.at('X');
  int oneIndex = charToIndex.at('1');

  Node n = idToNode.at(marking);

  bool b = n.b;
  std::vector<int> successors(alphabet.size(), EMTPYSET);

  // if current node has outgoing separator edge (T)
  if (n.successors[separatorIndex] != EMTPYSET && transitionDelta[level] > 0) {
    (assert(level < transitionDelta.size()));
    std::vector<int> newSeparatorSuccessors(alphabet.size(), EMTPYSET);
    // successors of the last padding node
    if (n.successors[separatorIndex] == marking) {
      newSeparatorSuccessors[separatorIndex] = SELF;
    } else {
      newSeparatorSuccessors[separatorIndex] = preprocess(
          transitionDelta, n.successors[separatorIndex], level + 1, cache);
    }
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
    if (n.successors[oneIndex] == marking) {
      successors[oneIndex] = SELF;
    } else {
      successors[oneIndex] =
          preprocess(transitionDelta, n.successors[oneIndex], level, cache);
    }
  }
  // if current node no outgoing separator (T) edge
  // TODO: only '1' possible
  else {
    for (int i = 0; i < alphabet.size(); i++) {
      if (n.successors[i] != marking) {
        successors[i] =
            preprocess(transitionDelta, n.successors[i], level, cache);
      } else {
        successors[i] = SELF;
      }
    }
  }

  int newId = make(Node{successors, b});

  return newId;
}

// TODO: cache int -> int
int TableOfNodes::postprocess(std::vector<int> transitionDelta, int marking,
                              int levelSeparators,
                              std::unordered_map<int, int>& cache) {
  if (marking == EMTPYSET || marking == SIGMASTAR || marking == EMPTYWORD) {
    return marking;
  }

  // TODO: make these fixed numbers ?
  int separatorIndex = charToIndex.at('T');
  int paddingIndex = charToIndex.at('X');
  int oneIndex = charToIndex.at('1');

  Node n = idToNode.at(marking);

  bool b = n.b;
  std::vector<int> successors(alphabet.size(), EMTPYSET);

  // if current node has outgoing padding edge (X)
  // TODO: what if outgoing separator edge (T) exists here too ? // cannot be
  // !

  if (n.successors[paddingIndex] != EMTPYSET &&
      levelSeparators < transitionDelta.size()) {
    // TODO: assert(transitionDelta[levelSeparators] > 0)
    Node next = n;
    std::cout << "delta is " << transitionDelta[levelSeparators] << "\n";
    for (int j = 0; j < transitionDelta[levelSeparators]; j++) {
      // TODO: assertions only 'X' transition
      next = idToNode.at(next.successors[paddingIndex]);
    }
    // TODO: assertions only 'T' transition
    std::cout << "next is " << nodeToId[next] << "\n";
    // adapt the successors of current node
    successors[paddingIndex] = EMTPYSET;
    successors[separatorIndex] =
        postprocess(transitionDelta, next.successors[separatorIndex],
                    levelSeparators + 1, cache);
    if (n.successors[oneIndex] == marking) {
      successors[oneIndex] = SELF;
    } else {
      successors[oneIndex] = postprocess(
          transitionDelta, n.successors[oneIndex], levelSeparators, cache);
    }
  }
  // if current node no outgoing separator (X) edge
  else {
    for (int i = 0; i < alphabet.size(); i++) {
      if (n.successors[i] == marking) {
        successors[i] = SELF;
      } else {
        if (i == separatorIndex && n.successors[separatorIndex] != EMTPYSET) {
          successors[i] = postprocess(transitionDelta, n.successors[i],
                                      levelSeparators + 1, cache);
        } else {
          successors[i] = postprocess(transitionDelta, n.successors[i],
                                      levelSeparators, cache);
        }
      }
    }
  }

  int newId = make(Node{successors, b});

  std::cout << "marking " << marking << " with level " << levelSeparators
            << " became newId " << newId << "\n";

  return newId;
}