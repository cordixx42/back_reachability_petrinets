#pragma once
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "PetriNet.h"

struct hash_pairs {
  template <class T1, class T2>
  size_t operator()(const std::pair<T1, std::pair<T2, T2>> &p) const {
    auto hash1 = std::hash<T1>{}(p.first);
    auto hash2 = std::hash<T2>{}(p.second.first);
    auto hash3 = std::hash<T2>{}(p.second.second);

    return (hash1 ^ (hash2 ^ hash3));
  }
};

typedef std::unordered_map<std::pair<int, std::pair<char, char>>,
                           std::vector<int>, hash_pairs>
    transducerTransitions;

struct Transducer {
  transducerTransitions transitions;

  // for multiple initial states (easy U for multiple transducers)
  std::unordered_set<int> initialStates;

  int initialState;

  pnTransition pnTrans;

  // TODO: think about removing this by using e.g. negative state ids or
  // indicator bit
  std::unordered_set<int> endStates;

  Transducer(transducerTransitions t, int initial, std::unordered_set<int> end);

  Transducer(pnTransition t);

  Transducer(pnTransition t, bool a);

  void fromPetriNetTransition(pnTransition t);

  void toDot(std::string fileName) const;

  std::string traverse(std::string word);

  std::string toString();
};