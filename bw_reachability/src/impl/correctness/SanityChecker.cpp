#include <iostream>
#include <queue>

#include "../BackwardsReachability.h"
#include "../MISTparser/parser.h"

// #define DEBUG_MODE 1
#ifdef DEBUG_MODE
#define Debug(x) x
#else
#define Debug(x)
#endif

// #define PROGRESS_MODE 1
#ifdef PROGRESS_MODE
#define Progr(x) x
#else
#define Progr(x)
#endif

namespace fs = std::filesystem;

using recursive_directory_iterator = fs::recursive_directory_iterator;

struct {
  // m1 < m2 :
  bool operator()(std::vector<int> m1, std::vector<int> m2) const {
    for (int i = 0; i < m1.size(); i++) {
      if (m1[i] < m2[i]) {
        return true;
      } else if (m1[i] > m2[i]) {
        return false;
      }
    }
    return false;
  }
} MarkingLess;

struct SanityChecker {
  TableOfNodes table;
  int markingSize;
  int separatorIndex;
  int paddingIndex;
  int oneIndex;

  SanityChecker(TableOfNodes& table, int markingSize)
      : table{table}, markingSize{markingSize} {
    for (int i = 0; i < table.alphabet.size(); i++) {
      if (table.alphabet[i] == 'T') {
        separatorIndex = i;
      }
      if (table.alphabet[i] == 'X') {
        paddingIndex = i;
      }
      if (table.alphabet[i] == '1') {
        oneIndex = i;
      }
    }
  }

  std::vector<std::vector<int>> computeMinimalMarkings(int dfa);

  void calculateDegrees(int dfa, std::unordered_map<int, int>& map,
                        std::unordered_set<int>& visited);

  std::vector<std::vector<int>> computeMinimalMarkingsFromTableExp(int dfa);

  void computeMinimalMarkingsExp(
      int dfa, std::unordered_map<int, std::vector<std::vector<int>>>& cache,
      int markingPos);

  bool isContainedIn(std::vector<int> m1, std::vector<int> m2);

  std::vector<std::vector<int>> minimalMarkingsFromSet(
      std::vector<std::vector<int>> markings);

  void processMarkingsForOneSucc(std::vector<std::vector<int>>& markings,
                                 int markingPos);
};

std::vector<std::vector<int>> SanityChecker::computeMinimalMarkings(int dfa) {
  std::unordered_map<int, int> degreeOfNodes;
  degreeOfNodes.emplace(dfa, 0);
  std::unordered_set<int> visited;
  calculateDegrees(dfa, degreeOfNodes, visited);
  // first int is node id, second int is markingPos;
  std::queue<std::pair<int, int>> workingSet;
  workingSet.push(std::make_pair(dfa, 0));

  std::unordered_map<int, std::vector<std::vector<int>>> nodeToMinMarkings;

  int loopIteration = 0;

  while (!workingSet.empty()) {
    auto [currentNode, markingPos] = workingSet.front();
    Progr(loopIteration += 1;
          std::cout << "iteration " << loopIteration << "\n";);
    Debug(std::cout << "current working set popped " << currentNode
                    << " with pos " << markingPos << "\n";);
    workingSet.pop();

    if (!nodeToMinMarkings.contains(currentNode)) {
      // start marking, (0,0,...)
      std::vector<std::vector<int>> initial{std::vector<int>(markingSize, 0)};
      nodeToMinMarkings.emplace(currentNode, initial);
    }

    std::vector<std::vector<int>> currentMinMarks =
        minimalMarkingsFromSet(nodeToMinMarkings.at(currentNode));
    Debug(std::cout << "a\n";);

    // set cache
    nodeToMinMarkings.at(currentNode) = currentMinMarks;

    // TODO: segmentation fault without this ??
    if (currentNode == 2) {
      break;
    }

    std::vector<std::vector<int>> oneSuccMinMarks{currentMinMarks};
    Debug(std::cout << "b\n");

    // std::cout << "before processing onesucc \n";
    // for (auto mark : oneSuccMinMarks) {
    //   for (int val : mark) {
    //     std::cout << val << ",";
    //   }
    //   std::cout << "\n";
    // }

    // one succ
    processMarkingsForOneSucc(oneSuccMinMarks, markingPos);
    Debug(std::cout << "c\n";);

    // std::cout << "after processing onesucc \n";
    // for (auto mark : oneSuccMinMarks) {
    //   for (int val : mark) {
    //     std::cout << val << ",";
    //   }
    //   std::cout << "\n";
    // }

    Node& n = table.idToNode[currentNode];
    int oneSucc = n.successors[oneIndex];
    if (oneSucc != currentNode && oneSucc != 0) {
      if (!nodeToMinMarkings.contains(oneSucc)) {
        nodeToMinMarkings.emplace(oneSucc, std::vector<std::vector<int>>{});
      }
      nodeToMinMarkings[oneSucc].insert(nodeToMinMarkings[oneSucc].end(),
                                        oneSuccMinMarks.cbegin(),
                                        oneSuccMinMarks.cend());
      degreeOfNodes[oneSucc] -= 1;
      if (degreeOfNodes[oneSucc] == 0) {
        workingSet.push(std::make_pair(oneSucc, markingPos));
      }
    }
    Debug(std::cout << "d\n";);
    int sepSucc = n.successors[separatorIndex];
    if (sepSucc != 0) {
      if (!nodeToMinMarkings.contains(sepSucc)) {
        nodeToMinMarkings.emplace(sepSucc, std::vector<std::vector<int>>{});
      }
      nodeToMinMarkings[sepSucc].insert(nodeToMinMarkings[sepSucc].end(),
                                        currentMinMarks.cbegin(),
                                        currentMinMarks.cend());
      degreeOfNodes[sepSucc] -= 1;
      if (degreeOfNodes[sepSucc] == 0) {
        workingSet.push(std::make_pair(sepSucc, markingPos + 1));
      }
    }
    Debug(std::cout << "e\n";);

    Debug(std::cout << workingSet.size() << "\n";);
  }

  // print all min markings
  Debug(for (auto [a, b]
             : nodeToMinMarkings) {
    std::cout << "dfa " << a << " has minimal markings \n";
    for (auto mark : b) {
      for (int val : mark) {
        std::cout << val << ",";
      }
      std::cout << "\n";
    }
    std::cout << "\n";
  });

  return nodeToMinMarkings.at(2);
}

void SanityChecker::calculateDegrees(int dfa, std::unordered_map<int, int>& map,
                                     std::unordered_set<int>& visited) {
  Debug(std::cout << "recursion degree " << dfa << "\n";);
  if (visited.contains(dfa)) {
    return;
  }
  Node& n = table.idToNode[dfa];
  int oneSucc = n.successors[oneIndex];
  if (oneSucc != dfa && oneSucc != 0) {
    if (!map.contains(oneSucc)) {
      map[oneSucc] = 1;
    } else {
      map[oneSucc] += 1;
    }
    calculateDegrees(oneSucc, map, visited);
  }
  int sepSucc = n.successors[separatorIndex];
  if (sepSucc != 0) {
    if (!map.contains(sepSucc)) {
      map[sepSucc] = 1;
    } else {
      map[sepSucc] += 1;
    }
    calculateDegrees(sepSucc, map, visited);
  }

  visited.insert(dfa);
}

// true->m1 <=m2->m1 is minimal
bool SanityChecker::isContainedIn(std::vector<int> m1, std::vector<int> m2) {
  for (int i = 0; i < m1.size(); i++) {
    if (m1[i] > m2[i]) {
      return false;
    }
  }
  return true;
}

void SanityChecker::processMarkingsForOneSucc(
    std::vector<std::vector<int>>& markings, int markingPos) {
  for (auto& marking : markings) {
    marking[markingPos] += 1;
  }
}

std::vector<std::vector<int>> SanityChecker::minimalMarkingsFromSet(
    std::vector<std::vector<int>> markings) {
  // std::cout << "before unique \n";
  // for (auto mark : markings) {
  //   for (int val : mark) {
  //     std::cout << val << ",";
  //   }
  //   std::cout << "\n";
  // }

  // make unique
  std::set<std::vector<int>> s(markings.begin(), markings.end());
  markings.assign(s.begin(), s.end());

  // std::cout << "before sort \n";
  // for (auto mark : markings) {
  //   for (int val : mark) {
  //     std::cout << val << ",";
  //   }
  //   std::cout << "\n";
  // }

  std::sort(markings.begin(), markings.end(), MarkingLess);

  // std::cout << "after sort \n";
  // for (auto mark : markings) {
  //   for (int val : mark) {
  //     std::cout << val << ",";
  //   }
  //   std::cout << "\n";
  // }

  std::set<int, std::greater<int>> toDelete{};
  for (int i = 0; i < markings.size() - 1; i++) {
    if (toDelete.contains(i)) {
      continue;
    }
    for (int j = i + 1; j < markings.size(); j++) {
      if (toDelete.contains(j)) {
        continue;
      }
      if (isContainedIn(markings[i], markings[j])) {
        toDelete.insert(j);
      }
    }
  }

  for (int i : toDelete) {
    markings.erase(markings.begin() + i);
  }
  return markings;
}

std::vector<std::vector<int>> SanityChecker::computeMinimalMarkingsFromTableExp(
    int dfa) {
  std::unordered_map<int, std::vector<std::vector<int>>> cacheMinMarks;
  computeMinimalMarkingsExp(dfa, cacheMinMarks, 0);
  Debug(for (auto [a, b]
             : cacheMinMarks) {
    std::cout << "dfa " << a << " has minimal markings \n";
    for (auto mark : b) {
      for (int val : mark) {
        std::cout << val << ",";
      }
      std::cout << "\n";
    }
    std::cout << "\n";
  });

  return cacheMinMarks.at(2);
}

void SanityChecker::computeMinimalMarkingsExp(
    int dfa, std::unordered_map<int, std::vector<std::vector<int>>>& cache,
    int markingPos) {
  if (!cache.contains(dfa)) {
    // start marking, (0,0,...)
    std::vector<std::vector<int>> initial{std::vector<int>(markingSize, 0)};
    cache.emplace(dfa, initial);
  }

  Debug(std::cout << "processing " << dfa << "\n";);

  // process current min markings !!!
  std::vector<std::vector<int>> currentMinMarks =
      minimalMarkingsFromSet(cache.at(dfa));

  // set cache
  cache.at(dfa) = currentMinMarks;

  std::vector<std::vector<int>> oneSuccMinMarks{currentMinMarks};

  // one succ
  processMarkingsForOneSucc(oneSuccMinMarks, markingPos);

  Node& n = table.idToNode[dfa];

  int oneSucc = n.successors[oneIndex];
  if (!(oneSucc == dfa) && oneSucc != 0) {
    // std::cout << "one succ " << oneSucc << "\n";
    if (!cache.contains(oneSucc)) {
      cache.emplace(oneSucc, std::vector<std::vector<int>>{});
    }
    cache[oneSucc].insert(cache[oneSucc].end(), oneSuccMinMarks.cbegin(),
                          oneSuccMinMarks.cend());
    // std::cout << "after insertion one succ \n";
    computeMinimalMarkingsExp(oneSucc, cache, markingPos);
  }
  int sepSucc = n.successors[separatorIndex];
  if (sepSucc != 0) {
    // std::cout << "sep succ " << sepSucc << "\n";
    if (!cache.contains(sepSucc)) {
      cache.emplace(sepSucc, std::vector<std::vector<int>>{});
    }
    cache[sepSucc].insert(cache[sepSucc].end(), currentMinMarks.cbegin(),
                          currentMinMarks.cend());
    // std::cout << "after insertion sep succ \n";
    computeMinimalMarkingsExp(sepSucc, cache, markingPos + 1);
  }
}

// For testing later
void singleBenchmark() {
  MistParser p{"./allBenchmarks/mist/boundedPN/newdekker.spec"};
  PetriNet net{p.parseAndProcess()};

  BackwardsReachability bw{net};

  auto [reachable, resMarking] =
      bw.isBackwardsReachableWithResult(p.ini, p.iniDetail, p.tar);
  std::cout << "RESULT ### basic bw " << reachable << std::endl;

  SanityChecker check{bw.table, int(net.init.size())};

  int size = check.computeMinimalMarkings(resMarking).size();
  std::cout << "minimal markings size " << size << std::endl;
}

void testParts() {
  TableOfNodes t{std::vector{'1', 'T', 'X'}};
  SanityChecker s{t, 3};

  std::vector a{1, 0, 0};
  std::vector b{0, 0, 1};
  std::vector c{0, 0, 3};
  std::vector d{3, 0, 0};

  auto vec = std::vector{b, a};

  for (auto mark : vec) {
    for (int val : mark) {
      std::cout << val << ",";
    }
    std::cout << "\n";
  }

  std::cout << "process \n";

  s.processMarkingsForOneSucc(vec, 2);
  std::vector res = s.minimalMarkingsFromSet(std::vector{b, a, d, c});

  for (auto mark : vec) {
    for (int val : mark) {
      std::cout << val << ",";
    }
    std::cout << "\n";
  }
}

int main() {
  fs::path p1 = "./allBenchmarks/sanity_check";
  for (const auto& entry : recursive_directory_iterator(p1)) {
    const auto filenameStr = entry.path().filename().string();
    // check if ends with spec
    if (entry.is_regular_file() &&
        filenameStr.substr(filenameStr.length() - 4) == "spec") {
      std::cout << "file:  " << filenameStr << '\n';
      MistParser p{entry.path()};
      PetriNet net{p.parseAndProcess()};
      BackwardsReachability bw{net};
      auto [reachable, resMarking] =
          bw.isBackwardsReachableWithResult(p.ini, p.iniDetail, p.tar);
      SanityChecker check{bw.table, int(net.init.size())};
      int size = check.computeMinimalMarkings(resMarking).size();
      std::cout << "minimal markings size " << size << std::endl;
    }
  }
}