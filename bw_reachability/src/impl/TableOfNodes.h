#pragma once
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Transducer.h"

#define SELF -1
#define EMTPYSET 0
#define SIGMASTAR 1
#define EMPTYWORD 2

struct Node {
  // int id;
  std::vector<int> successors;
  bool b;

  bool operator==(const Node& n) const;

  std::string toString();
};

// https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector
template <>
struct std::hash<Node> {
  size_t operator()(const Node& n) const {
    std::size_t seed = n.successors.size();
    for (auto& i : n.successors) {
      seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    // Compute individual hash values for two data members and combine them
    // using XOR and bit shifting
    return seed ^ std::hash<bool>()(n.b);
  }
};

// first in pair is transducer state, second is dfa state
typedef std::pair<int, int> preState;

// hash for std::pair<int,int>
template <>
struct std::hash<preState> {
  size_t operator()(const preState& n) const {
    auto hash1 = std::hash<int>()(n.first);
    auto hash2 = std::hash<int>()(n.first);
    return hash1 ^ hash2;
  }
};

// hash for std::pair<std::pair<T1, T1>, T2>
struct hash_preTrans {
  template <class T1, class T2>
  size_t operator()(const std::pair<std::pair<T1, T1>, T2>& p) const {
    auto hash1 = std::hash<T1>{}(p.first.first);
    auto hash2 = std::hash<T1>{}(p.first.second);
    auto hash3 = std::hash<T2>{}(p.second);

    return (hash1 ^ (hash2 ^ hash3));
  }
};

// not used currently, cache for transitions
typedef std::unordered_map<std::pair<preState, char>,
                           std::unordered_set<preState>, hash_preTrans>
    preTransitions;

template <class T>
inline void hash_combine(std::size_t& seed, const T& v) {
  std::hash<T> hasher;
  seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct hash_preStateSet {
  std::size_t operator()(const std::unordered_set<preState>& c) const {
    std::size_t hash = 0;
    for (auto i = c.begin(); i != c.end(); ++i) {
      hash_combine(hash, i->first);
      hash_combine(hash, i->second);
    }
    return hash;
  }
};

typedef std::unordered_map<std::unordered_set<preState>, int, hash_preStateSet>
    preCacheSet;

// typedef std::unordered_map<std::vector<preState>, int, hash_preStateVec>
//     preCacheVec;

struct TableOfNodes {
  // TODO: define const int alphabetSize

  std::vector<char> alphabet;

  // mapping from alphabet letter to index
  // if removed, then find index with alphabet vector
  std::unordered_map<char, int> charToIndex;

  int separatorIndex;
  int paddingIndex;
  int oneIndex;

  std::vector<Node> idToNode;

  std::unordered_map<Node, int> nodeToId;

  std::unordered_map<Node, int> nodeToIdWithSelf;

  // mapping from already computed unions to the created new state
  std::unordered_map<long, int> unionToId;

  // mapping from already computed intersections to the created new state
  std::unordered_map<long, int> interToId;

  // cache for create
  std::unordered_map<std::string, int> exprToId;
  std::unordered_map<int, std::string> idToExpr;

  // cache for size and height
  std::unordered_map<int, int> idToSize;
  std::unordered_map<int, std::unordered_set<int>> idToReachable;
  std::unordered_map<int, int> idToHeight;

  // constructor sets alphabet and makes two initial nodes
  TableOfNodes(std::vector<char> a);
  // returns q if node exists, else creates new node and returns q
  // assumes that all states in successors already exist in table !!
  int make(Node n);

  // creates a new node and all its successors into the table
  int create(std::string expression);

  int createFromMarking(std::vector<int> marking);

  int createUpwardSetFromMarking(std::vector<int> marking);

  // for set of init markings in benchmarks
  int createSetFromMarking(std::vector<int> marking,
                           std::vector<int> markingSet);

  int unionOfNodes(int n1, int n2);

  int interOfNodes(int n1, int n2);

  int preWrapper(const Transducer& t, int dfa);

  int pre(const Transducer& t, int dfa);

  std::unordered_set<preState> constructPreSingleTrans(const preState& pq,
                                                       char x,
                                                       const Transducer& t);

  int preIntoTable(const Transducer& t,
                   const std::unordered_set<preState>& preStarts,
                   preCacheSet& cache, preTransitions& transCache,
                   std::unordered_set<preState>& loop);

  int preprocess(std::vector<int> transition, int marking, int level,
                 std::unordered_map<int, int>& cache);

  int postprocess(std::vector<int> transition, int marking, int level,
                  std::unordered_map<int, int>& cache);

  std::string toString();

  std::string toStringUnions();

  // number of all reachable nodes
  int sizeOf(int dfa);
  std::unordered_set<int> sizeOfRec(int dfa);

  // number of maximal height of a node to the root (EMPTYSET)
  int maxHeightOf(int dfa);

  // how many consecutive 1s
  int maxTokenOf(int dfa);

  // number of paths in one node
  int maxPathsOf(int dfa);

  // graph visualization for whole graph
  void toDot(std::string fileName);

  // graph visualization form for whole graph without edges to emptyset
  void toDotCompact(std::string fileName);

  // graph visualization from single start node
  void toDot(int id, std::string fileName);

  // graph visualization form single start node without edges to emptyset
  void toDotCompact(int id, std::string fileName);

  // graph visualization for a pre construction
  void preToDot(int dfa, const Transducer& t, int preId);
};
