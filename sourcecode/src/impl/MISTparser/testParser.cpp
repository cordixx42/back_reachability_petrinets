#include <filesystem>
#include <iostream>

#include "parser.h"

namespace fs = std::filesystem;

using recursive_directory_iterator = fs::recursive_directory_iterator;

int main(int argc, char* argv[]) {
  fs::path p1 = "./allBenchmarks/soter_unknown";

  //   std::string csvFilePath = "./benchmarkResults/wahl-kroening/pn_data.csv";
  //   std::ofstream csvFile(csvFilePath, std::ofstream::out);

  //   csvFile << "INSTANCE,#STATES,#TRANS\n";

  for (const auto& entry : recursive_directory_iterator(p1)) {
    const auto filenameStr = entry.path().filename().string();
    // check if ends with spec
    if (entry.is_regular_file() &&
        filenameStr.substr(filenameStr.length() - 4) == "spec") {
      std::cout << "file:  " << filenameStr << '\n';
      MistParser p = MistParser(entry.path());
      PetriNet pnet = p.parseAndProcess();
      //   csvFile << entry.path() << "," << pnet.init.size() << ","
      //           << pnet.allTransitions.size() << "\n";
      std::cout << "parsing done" << std::endl;
    }
  }
}