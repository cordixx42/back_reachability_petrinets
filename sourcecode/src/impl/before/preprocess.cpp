int TableOfNodes::preprocess(std::vector<int> transitionDelta, int marking,
                             int levelSeparators) {
  std::cout << "marking is " << marking << " and levelsep " << levelSeparators
            << "\n";
  if (marking == EMTPYSET || marking == SIGMASTAR) {
    return 0;
  }

  Node& n = idToNode.at(marking);
  int separatorIndex = charToIndex.at('T');
  int paddingIndex = charToIndex.at('X');
  for (int i = 0; i < alphabet.size(); i++) {
    if (i == separatorIndex && n.successors[separatorIndex] != EMTPYSET &&
        levelSeparators < transitionDelta.size() &&
        transitionDelta[levelSeparators] > 0) {
      std::vector<int> newSeparatorSuccessors(alphabet.size(), EMTPYSET);
      newSeparatorSuccessors[separatorIndex] = n.successors[separatorIndex];
      int newSeparator = make(Node{newSeparatorSuccessors, false});
      std::cout << "new padding node with id " << newSeparator << " and next "
                << n.successors[separatorIndex] << std::endl;
      newSeparatorSuccessors[separatorIndex] = EMTPYSET;

      for (int j = 0; j < transitionDelta[levelSeparators] - 1; j++) {
        newSeparatorSuccessors[paddingIndex] = newSeparator;
        newSeparator = make(Node{newSeparatorSuccessors, false});
        std::cout << "new padding node with id " << newSeparator << std::endl;
      }

      preprocess(transitionDelta, n.successors[separatorIndex],
                 levelSeparators + 1);
      n.successors[separatorIndex] = EMTPYSET;
      n.successors[paddingIndex] = newSeparator;

    } else {
      if (n.successors[i] != marking) {
        preprocess(transitionDelta, n.successors[i], levelSeparators);
      }
    }
  }

  return 0;
}