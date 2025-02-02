#include "BackwardsReachability.h"

#include <iostream>

BackwardsReachability::BackwardsReachability(PetriNet p) {
  for (const pnTransition& t : p.allTransitions) {
    this->petriTransitionTransducer.push_back(Transducer{t});
  }
}

bool BackwardsReachability::isBackwardsReachable(
    std::vector<int> initialMarking, std::vector<int> initialMarkingSet,
    std::vector<std::vector<int>> endMarking) {
  int startMarking =
      table.createSetFromMarking(initialMarking, initialMarkingSet);

  int markings = EMTPYSET;
  for (const auto& target : endMarking) {
    markings =
        table.unionOfNodes(markings, table.createUpwardSetFromMarking(target));
  }
  int markingsBefore = EMTPYSET;

  while (true) {
    markingsBefore = markings;
    // for all transducer calculate pre and union
    for (const Transducer& t : this->petriTransitionTransducer) {
      int p = table.preWrapper(t, markings);
      markings = table.unionOfNodes(markings, p);
    }

    // TODO: switch statements and see performance difference

    if (table.interOfNodes(startMarking, markings) != EMTPYSET) {
      return true;
    }
    if (markingsBefore == markings) {
      return false;
    }
  }
}

std::pair<bool, int> BackwardsReachability::isBackwardsReachableWithResult(
    std::vector<int> initialMarking, std::vector<int> initialMarkingSet,
    std::vector<std::vector<int>> endMarking) {
  int startMarking =
      table.createSetFromMarking(initialMarking, initialMarkingSet);

  int markings = EMTPYSET;
  for (const auto& target : endMarking) {
    markings =
        table.unionOfNodes(markings, table.createUpwardSetFromMarking(target));
  }
  int markingsBefore = EMTPYSET;

  while (true) {
    markingsBefore = markings;
    // for all transducer calculate pre and union
    for (const Transducer& t : this->petriTransitionTransducer) {
      int p = table.preWrapper(t, markings);
      markings = table.unionOfNodes(markings, p);
    }

    if (table.interOfNodes(startMarking, markings) != EMTPYSET) {
      return std::make_pair(true, markings);
    }
    if (markingsBefore == markings) {
      return std::make_pair(false, markings);
    }
  }
}
