#pragma once
#include <fstream>

#include "../PetriNet.h"
#include "../TableOfNodes.h"
#include "../Transducer.h"

// mode with debug prints

// #define DEBUG_MODE 0  // Or 0 if you dont want to debug
#ifdef DEBUG_PRINT_MODE
#define Dprint(x) std::cout << x
#else
#define Dprint(x)
#endif

// #define DEBUG_MODE 0  // Or 0 if you dont want to debug
#ifdef DEBUG_MODE
#define Debug(x) x
#else
#define Debug(x)
#endif

//  about general data and result of an instance
#define PERF_MODE 1
#ifdef PERF_MODE
#define Perf(x) x
#else
#define Perf(x)
#endif

//  about change in markings and table size through
//  iterations
// #define ITER_MODE 1
#ifdef ITER_MODE
#define Iter(x) x
#else
#define Iter(x)
#endif

// #define DOT_MODE 0  // mode for generating dot files
#ifdef DOT_MODE
#define Dot(x) x
#else
#define Dot(x)
#endif

struct BackwardsReachability {
  TableOfNodes table{std::vector<char>{'1', 'X', 'T'}};

  std::vector<Transducer> petriTransitionTransducer;

  std::ofstream csvFile;

  std::string instanceName;

  BackwardsReachability(PetriNet p, std::string instanceName = "noname",
                        std::string fileName = "perf");

  bool isBackwardsReachable(std::vector<int> initialMarking,
                            std::vector<int> initialMarkingSet,
                            std::vector<std::vector<int>> endMarking);
};