#include <iostream>

#include "BackwardsReachability.h"
#include "MISTparser/parser.h"

int main(int argc, char *argv[]) {
  std::string path = argv[1];

  MistParser p{path};
  PetriNet net{p.parseAndProcess()};
  BackwardsReachability bw{net};
  bool reachable = bw.isBackwardsReachable(p.ini, p.iniDetail, p.tar);
  if (reachable) {
    return 1;
  } else {
    return 0;
  }
}