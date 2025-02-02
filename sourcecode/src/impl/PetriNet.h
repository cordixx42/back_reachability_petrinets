// represent one petri net
#pragma once

#include <sstream>
#include <vector>

typedef std::pair<std::vector<int>, std::vector<int>> pnTransition;

struct PetriNet {
  std::vector<pnTransition> allTransitions;

  std::vector<int> init;
  std::vector<int> initDetail;

  // multiple targets possible
  std::vector<std::vector<int>> targets;

  PetriNet(std::vector<pnTransition> t) : allTransitions{t} {}

  PetriNet(std::vector<pnTransition> t, std::vector<int> i, std::vector<int> id,
           std::vector<std::vector<int>> tar)
      : allTransitions{t}, init{i}, initDetail{id}, targets{tar} {}

  std::string toString() {
    std::ostringstream str;
    str << "########################################## \n";
    str << "PETRI NET \n \n";
    str << "INIT AND INITDETAIL \n";
    for (const auto& e : init) {
      str << e << ", ";
    }
    str << "\n";
    for (const auto& e : initDetail) {
      str << e << ", ";
    }
    str << "\n";

    str << "TARGETS \n";
    for (const auto& e : targets) {
      for (const auto& f : e) {
        str << f << ", ";
      }
      str << "\n";
    }
    str << "\n";

    str << "TRANSITIONS \n";
    for (const auto& [a, b] : allTransitions) {
      for (const auto& e : a) {
        str << e << ", ";
      }
      str << "\n";
      for (const auto& e : b) {
        str << e << ", ";
      }
      str << "\n";
      str << "\n";
    }

    return str.str();
  }
};