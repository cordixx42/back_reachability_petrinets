#include <iostream>
#include <string>
#include <utility>

#include "../Transducer.h"

void testSimpleTransducer() {
  transducerTransitions trans{};
  trans.emplace(std::make_pair(1, std::make_pair('a', 'b')),
                std::vector<int>{2, 3});
  Transducer t{trans, 1, std::unordered_set<int>{3}};
  t.toDot("");
}

void testTransducerFromNet() {
  PetriNet n{std::vector<pnTransition>{
      std::make_pair(std::vector<int>{1, 5, 3}, std::vector<int>{3, 0, 3})}};
  Transducer t{n.allTransitions[0]};
  t.toDot("tfpn");
  std::cout << t.toString();
}

void testSimpleTransducerFromNet() {
  PetriNet n{std::vector<pnTransition>{
      std::make_pair(std::vector<int>{3, 2}, std::vector<int>{2, 3})}};
  Transducer t{n.allTransitions[0]};
  t.toDot("tfpn");
}

void testEpsTransducer() {
  PetriNet n{std::vector<pnTransition>{
      std::make_pair(std::vector<int>{1, 2, 3}, std::vector<int>{2, 1, 3})}};
  Transducer t{n.allTransitions[0], true};
  t.toDot("tfpneps");
  std::cout << t.toString();
}

int main() { testSimpleTransducerFromNet(); }