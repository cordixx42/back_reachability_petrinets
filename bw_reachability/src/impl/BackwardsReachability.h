#pragma once
#include <fstream>

#include "PetriNet.h"
#include "TableOfNodes.h"
#include "Transducer.h"

struct BackwardsReachability {
  TableOfNodes table{std::vector<char>{'1', 'X', 'T'}};

  std::vector<Transducer> petriTransitionTransducer;

  BackwardsReachability(PetriNet p);

  bool isBackwardsReachable(std::vector<int> initialMarking,
                            std::vector<int> initialMarkingSet,
                            std::vector<std::vector<int>> endMarking);

  std::pair<bool, int> isBackwardsReachableWithResult(
      std::vector<int> initialMarking, std::vector<int> initialMarkingSet,
      std::vector<std::vector<int>> endMarking);
};