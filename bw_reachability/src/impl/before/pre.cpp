int TableOfNodes::pre_before(Transducer t, int dfa) {
  preState startB = std::make_pair(t.initialState, dfa);
  // accumulate all transitions of new B here
  preTransitions transB;
  // current working set
  std::set<preState> constructedStatesB;
  constructedStatesB.insert(startB);

  while (!constructedStatesB.empty()) {
    std::pair<int, int> cur = *constructedStatesB.cbegin();
    constructedStatesB.erase(constructedStatesB.begin());
    // std::cout << "workset has size " << constructedStatesB.size() << "\n";
    for (int i = 0; i < alphabet.size(); i++) {
      transB.emplace(
          std::make_pair(cur, alphabet[i]),
          constructPreSingleTrans(cur, alphabet[i], t, constructedStatesB));
    }
  }

  // print transB
  std::cout << "########################################## \n";
  std::cout << "PRE CONTAINS FOLLOWING TRANSITIONS \n";
  for (auto& t : transB) {
    std::cout << "state [" << t.first.first.first << "," << t.first.first.second
              << "] -" << t.first.second << "-> \n";
    for (auto& r : t.second) {
      std::cout << "\t state [" << r.first << "," << r.second << "]\n";
    }
  }

  return preIntoTable(transB, std::vector{startB});
}

// first in pair is transducer state, second is dfa state
std::vector<preState> TableOfNodes::constructPreSingleTrans(
    preState pq, char x, const Transducer& t, std::set<preState>& constructed) {
  std::vector<preState> pqPrimes;
  for (int i = 0; i < alphabet.size(); i++) {
    char c = alphabet[i];
    auto tKey = std::make_pair(pq.first, std::make_pair(x, c));
    if (idToNode[pq.second].successors[i] != EMTPYSET &&
        t.transitions.contains(tKey)) {
      auto pPrimes = t.transitions.at(tKey);
      for (const int& j : pPrimes) {
        preState pqPrime = std::make_pair(j, idToNode[pq.second].successors[i]);
        pqPrimes.push_back(pqPrime);
        if (pqPrime != pq) {
          constructed.insert(pqPrime);
        }
      }
    }
  }
  return pqPrimes;
}

int TableOfNodes::preIntoTable(preTransitions preTrans,
                               std::vector<preState> preStarts) {
  std::vector<int> successors(alphabet.size(), EMTPYSET);
  // TODO: set the b correctly
  bool b = false;
  // std::cout << "in for loop with index " << i << std::endl;

  // case only 1 start
  if (preStarts.size() == 1) {
    for (int i = 0; i < alphabet.size(); i++) {
      std::vector<preState> v =
          preTrans.at(std::make_pair(preStarts[0], alphabet[i]));
      if (!v.empty()) {
        if (v.size() == 1 && v.at(0) == preStarts[0]) {
          successors[i] = SELF;
        } else {
          successors[i] = preIntoTable(preTrans, v);
        }
        // if (v.size() == 1) {
        //   if (v.at(0) == preStarts[0]) {
        //     successors[i] = SELF;
        //   } else {
        //     // successors[i] = v.at(0);
        //     // recursion
        //     successors[i] = preIntoTable(preTrans, std::vector{v.at(0)});
        //   }
        // } else {
        //   successors[i] = preIntoTable(preTrans, std::vector{v.at(0)});
        // }
      }
    }
  } else {
    // make determinization on the fly
    // TODO: use set or vector as preStarts ??
    for (int i = 0; i < alphabet.size(); i++) {
      // for each elem in preStarts, get
      // preTrans.at(std::make_pair(preStarts[0], alphabet[i])); and combine all
      // into a vector or set

      // check if SELF is same as the vector or set ?
      // successors[i] = preIntoTable(preTrans, vector/set)
    }
  }

  return make(Node{successors, b});
}