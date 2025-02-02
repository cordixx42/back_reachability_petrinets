#include <iostream>

#include "../BackwardsReachability.h"

void testx() {
  std::vector<pnTransition> pntrans;
  pntrans.push_back(
      std::make_pair(std::vector<int>{1, 0, 0}, std::vector<int>{0, 2, 1}));
  // pntrans.push_back(
  //     std::make_pair(std::vector<int>{0, 2, 0}, std::vector<int>{0, 0, 1}));
  PetriNet pn{pntrans};
  Transducer t{pntrans[0]};

  BackwardsReachability bw{pn};
  bw.isBackwardsReachable(std::vector{1, 0, 0}, std::vector{0, 0, 0},
                          std::vector(1, std::vector{0, 1, 1}));

  bw.table.toDotCompact("all_simple");

  std::cout << "max path " << bw.table.maxPathsOf(23) << std::endl;

  // BackwardsReachability bw{pn};
  // std::cout << "bwresult "
  //           << bw.isBackwardsReachable(std::vector{1, 0, 0},
  //                                      std::vector{0, 0, 0},
  //                                      std::vector(1, std::vector{0, 1, 1}))
  //           << std::endl;
}

void test1() {
  std::vector<pnTransition> pntrans;
  pntrans.push_back(
      std::make_pair(std::vector<int>{2, 0}, std::vector<int>{1, 1}));
  pntrans.push_back(
      std::make_pair(std::vector<int>{0, 1}, std::vector<int>{0, 2}));
  PetriNet pn{pntrans};
  BackwardsReachability bw{pn};

  for (int i = 0; i < bw.petriTransitionTransducer.size(); i++) {
    std::cout << "transition " << i << "     "
              << bw.petriTransitionTransducer[i].pnTrans.first[0] << " "
              << bw.petriTransitionTransducer[i].pnTrans.first[1] << " "
              << bw.petriTransitionTransducer[i].pnTrans.second[0] << " "
              << bw.petriTransitionTransducer[i].pnTrans.second[1] << std::endl;
    std::string s = "transducer" + std::to_string(i);
    bw.petriTransitionTransducer[i].toDot(s);
  }

  std::cout << "bwresult "
            << bw.isBackwardsReachable(std::vector{2, 0}, std::vector{2, 3},
                                       std::vector(1, std::vector{0, 0}))
            << std::endl;
}

void test11() {
  std::vector<pnTransition> pntrans;
  pntrans.push_back(
      std::make_pair(std::vector<int>{2, 0}, std::vector<int>{1, 1}));
  pntrans.push_back(
      std::make_pair(std::vector<int>{0, 1}, std::vector<int>{2, 0}));
  PetriNet pn{pntrans};
  BackwardsReachability bw{pn};

  // for (int i = 0; i < bw.petriTransitionTransducer.size(); i++) {
  //   std::cout << "transition " << i << "     "
  //             << bw.petriTransitionTransducer[i].pnTrans.first[0] << " "
  //             << bw.petriTransitionTransducer[i].pnTrans.first[1] << " "
  //             << bw.petriTransitionTransducer[i].pnTrans.second[0] << " "
  //             << bw.petriTransitionTransducer[i].pnTrans.second[1] <<
  //             std::endl;
  //   std::string s = "transducer" + std::to_string(i);
  //   bw.petriTransitionTransducer[i].toDot(s);
  // }

  std::cout << "bwresult "
            << bw.isBackwardsReachable(std::vector{2, 0}, std::vector{20, 20},
                                       std::vector(1, std::vector{0, 0}))
            << std::endl;
}

void test2() {
  std::vector<pnTransition> pntrans;
  pntrans.push_back(
      std::make_pair(std::vector<int>{2, 1, 0}, std::vector<int>{0, 0, 5}));
  PetriNet pn{pntrans};
  BackwardsReachability bw{pn};

  std::cout << "bwresult "
            << bw.isBackwardsReachable(std::vector{4, 3, 0},
                                       std::vector{0, 1, 11},
                                       std::vector(1, std::vector{0, 0}))
            << std::endl;
  // should be 0
}

void test3() {
  std::vector<pnTransition> pntrans;
  pntrans.push_back(
      std::make_pair(std::vector<int>{1, 0}, std::vector<int>{2, 1}));
  PetriNet pn{pntrans};
  BackwardsReachability bw{pn};

  std::cout << "bwresult "
            << bw.isBackwardsReachable(std::vector{1, 0}, std::vector{2, 2},
                                       std::vector(1, std::vector{0, 0}))
            << std::endl;
  // result is 1
}

int main() { testx(); }