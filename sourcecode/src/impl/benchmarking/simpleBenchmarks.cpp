#include <iostream>

#include "../BackwardsReachability.h"
#include "../MISTparser/parser.h"

void basicSpec() {
  MistParser p{"./benchmarks/safe/basicME.spec"};
  PetriNet net{p.parseAndProcess()};

  BackwardsReachability bw{net};

  // std::cout << "..." << bw.table.sizeOf(2) << "\n";

  // 3 targets
  // tar[0]: x3>=1, x4>=1
  bool reachable = bw.isBackwardsReachable(p.ini, p.iniDetail, p.tar);
  std::cout << "RESULT ### basic bw " << reachable << std::endl;

  std::cout << "..." << bw.table.sizeOf(302) << "\n";
  std::cout << "..." << bw.table.sizeOf(2) << "\n";

  std::unordered_set s = bw.table.idToReachable.at(2);
  for (int i : s) {
    std::cout << i << ",";
  }
  std::cout << "\n";
  // bw.table.toDotCompact("basicWholeTable");
}

void csmSpec() {
  MistParser p{"./benchmarks/safe/csm.spec"};
  PetriNet net{p.parseAndProcess()};

  BackwardsReachability bw{net};

  // 1 target
  bool reachable = bw.isBackwardsReachable(p.ini, p.iniDetail, p.tar);
  std::cout << "RESULT ### csm bw " << reachable << std::endl;
}

void fmsSpec() {
  MistParser p{"./benchmarks/safe/fms.spec"};
  PetriNet net{p.parseAndProcess()};

  BackwardsReachability bw{net};

  // 1 target
  bool reachable = bw.isBackwardsReachable(p.ini, p.iniDetail, p.tar);
  std::cout << "RESULT ### fsm bw " << reachable << std::endl;
}

void mesh1Spec() {
  MistParser p{"./benchmarks/safe/mesh2x2.spec"};
  PetriNet net{p.parseAndProcess()};

  BackwardsReachability bw{net};

  // 1 target
  bool reachable = bw.isBackwardsReachable(p.ini, p.iniDetail, p.tar);
  std::cout << "RESULT ### mesh2x2 bw " << reachable << std::endl;
}

void mesh2Spec() {
  MistParser p{"./benchmarks/safe/mesh3x2.spec"};
  PetriNet net{p.parseAndProcess()};

  BackwardsReachability bw{net};

  // 1 target
  bool reachable = bw.isBackwardsReachable(p.ini, p.iniDetail, p.tar);
  std::cout << "RESULT ### mesh3x2 bw " << reachable << std::endl;
}

void multipoolSpec() {
  MistParser p{"./benchmarks/safe/multipool.spec"};
  PetriNet net{p.parseAndProcess()};

  BackwardsReachability bw{net};

  // 1 target
  bool reachable = bw.isBackwardsReachable(p.ini, p.iniDetail, p.tar);
  std::cout << "RESULT ### multipool bw " << reachable << std::endl;
}

void pncsaSpec() {
  MistParser p{"./benchmarks/unsafe/pncsacover.spec"};
  PetriNet net{p.parseAndProcess()};

  BackwardsReachability bw{net};

  // 1 target
  bool reachable = bw.isBackwardsReachable(p.ini, p.iniDetail, p.tar);
  std::cout << "RESULT ### pncsa bw " << reachable << std::endl;
}

void pingpongSpec() {
  MistParser p{"./benchmarks/pingpong.spec"};
  PetriNet net{p.parseAndProcess()};

  BackwardsReachability bw{net};

  // tar[0]: x3>=1, x4>=1
  std::cout << "bwresult " << bw.isBackwardsReachable(p.ini, p.iniDetail, p.tar)
            << std::endl;
}

void testKnownSpecs() {
  std::cout << " SAFE SPECS " << std::endl;
  basicSpec();
  csmSpec();
  fmsSpec();
  mesh1Spec();
  mesh2Spec();
  multipoolSpec();
  std::cout << " UNSAFE SPECS " << std::endl;
  pncsaSpec();
}

int main() { basicSpec(); }