#include <filesystem>
#include <iostream>

#include "../MISTparser/parser.h"
#include "BackwardsReachabilityTesting.h"

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

namespace fs = std::filesystem;

using recursive_directory_iterator = fs::recursive_directory_iterator;

int performBackwardsAlg(std::string path, std::string fileName,
                        std::string csvFilePath) {
  MistParser p{path};
  PetriNet net{p.parseAndProcess()};
  BackwardsReachability bw{net, fileName.substr(0, fileName.size() - 5),
                           csvFilePath};
  bool reachable = bw.isBackwardsReachable(p.ini, p.iniDetail, p.tar);
  if (reachable) {
    return 1;
  } else {
    return 0;
  }
}

int main(int argc, char* argv[]) {
  std::string csvFilePath = "";
  Perf(csvFilePath = "./benchmarkResults/testtest/test.csv";);

  Iter(csvFilePath =
           "./benchmarkResults/wahl-kroening/iter_data_easy_all.csv";);

  fs::path p1 = "./allBenchmarks/testtest";
  Perf(std::ofstream csvFile(csvFilePath, std::ofstream::out););
  Iter(std::ofstream csvFile(csvFilePath, std::ofstream::out););
  // Rocco war hier
  Perf(csvFile << "INSTANCE,#STATES,#TRANS,#ITER,#PRES,#TABLENODES,#"
                  "RESULTMARKNODES,MAXPATH,MAXSIZE,RESULT \n");

  Iter(csvFile << "INSTANCE"
               << ","
               << "ITER"
               << ","
               << "TABLESIZE"
               << ","
               << "MARKINGSIZE"
               << "\n";);

  Perf(csvFile.close(););
  Iter(csvFile.close(););

  for (const auto& entry : recursive_directory_iterator(p1)) {
    const auto filenameStr = entry.path().filename().string();
    // check if ends with spec
    if (entry.is_regular_file() &&
        filenameStr.substr(filenameStr.length() - 4) == "spec") {
      std::cout << "file:  " << filenameStr << '\n';
      int res = performBackwardsAlg(entry.path(), entry.path(), csvFilePath);
      std::cout << "result is " << res << std::endl;
    }
  }
}