#include "BackwardsReachabilityTesting.h"

#include <iostream>

BackwardsReachability::BackwardsReachability(PetriNet p,
                                             std::string instanceName,
                                             std::string fileName)
    : instanceName{instanceName} {
  for (const pnTransition& t : p.allTransitions) {
    this->petriTransitionTransducer.push_back(Transducer{t});
  }
  Perf(csvFile.open(fileName, std::ofstream::app);
       csvFile << instanceName << "," << p.init.size() << ","
               << p.allTransitions.size() << ",";);
  Iter(csvFile.open(fileName, std::ofstream::app));
  //  csvFile << this->instanceName << ",";)
}

bool BackwardsReachability::isBackwardsReachable(
    std::vector<int> initialMarking, std::vector<int> initialMarkingSet,
    std::vector<std::vector<int>> endMarking) {
  int iterationCounter = 0;
  int preCounter = 0;
  int maxHeight = 0;
  int maxNodes = 0;

  Iter(int steps = 1; int max = 52;);

  int startMarking =
      table.createSetFromMarking(initialMarking, initialMarkingSet);

  Dot(table.toDotCompact(startMarking, "startMarkingCompact"););

  int markings = EMTPYSET;
  for (const auto& target : endMarking) {
    markings =
        table.unionOfNodes(markings, table.createUpwardSetFromMarking(target));
  }

  Perf(maxHeight = table.maxHeightOf(markings););
  Perf(maxNodes = table.sizeOf(markings););

  Dprint("initial markings " << markings << std::endl);
  int markingsBefore = EMTPYSET;

  while (true) {
    Dot(table.toDotCompact(markings,
                           "iteration" + std::to_string(iterationCounter)););
    Iter(csvFile << this->instanceName << "," << iterationCounter << ","
                 << table.idToNode.size() << "," << table.sizeOf(markings)
                 << "\n";);
    iterationCounter += 1;

    markingsBefore = markings;
    // for all transducer calculate pre and union
    for (const Transducer& t : this->petriTransitionTransducer) {
      if (preCounter % 10 == 0) {
        std::cout << "currently on pre iteration " << preCounter << std::endl;
      } else {
        Perf(if (preCounter % 10 == 0) {
          std::cout << "currently on pre iteration " << preCounter << std::endl;
        };);
      };

      preCounter += 1;

      int p = table.preWrapper(t, markings);
      Dprint("transition " << t.pnTrans.first[0] << " " << t.pnTrans.first[1]
                           << " " << t.pnTrans.second[0] << " "
                           << t.pnTrans.second[1] << std::endl;);
      Dprint("preResult  " << p << std::endl;);

      markings = table.unionOfNodes(markings, p);
      Perf(int curHeight = table.maxHeightOf(markings);
           if (curHeight > maxHeight) { maxHeight = curHeight; });
      Perf(int curNodes = table.sizeOf(markings);
           if (curNodes > maxNodes) { maxNodes = curNodes; });
    }

    // Perf(int curHeight = table.maxHeightOf(markings);
    //      if (curHeight > maxHeight) { maxHeight = curHeight; };);
    // Perf(int curNodes = table.sizeOf(markings);
    //      if (curNodes > maxNodes) { maxNodes = curNodes; };);

    if (table.interOfNodes(startMarking, markings) != EMTPYSET) {
      int inter = table.interOfNodes(startMarking, markings);
      Dprint("inter is " << inter << std::endl);
      Dot(table.toDotCompact(markings, "resultMarkingCompact"););
      Dot(table.toDotCompact("wholeTableCompact"););
      Dprint("no of loop iterations: " << iterationCounter << std::endl);
      Dprint("no of pre invocations: " << preCounter << std::endl);
      Perf(csvFile << iterationCounter << "," << preCounter << ","
                   << table.idToNode.size() << "," << table.sizeOf(markings)
                   << "," << maxHeight << "," << maxNodes << ","
                   << "unsafe"
                   << "\n";);
      Perf(csvFile.close());
      Iter(csvFile << "\n"; csvFile.close());
      return true;
    }
    if (markingsBefore == markings) {
      Dot(table.toDotCompact("wholeTableCompact"));
      Dot(table.toDotCompact(markings, "resultMarkingCompact"););
      Dprint("no of loop iterations: " << iterationCounter << std::endl);
      Dprint("no of pre invocations: " << preCounter << std::endl);
      // Dprint("max paths of result marking is " << maxPaths << std::endl);
      Perf(csvFile << iterationCounter << "," << preCounter << ","
                   << table.idToNode.size() << "," << table.sizeOf(markings)
                   << "," << maxHeight << "," << maxNodes << ","
                   << "safe"
                   << "\n";);
      Perf(csvFile.close());
      Iter(csvFile << "\n"; csvFile.close());
      return false;
    }
  }
}
