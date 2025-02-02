#include "Transducer.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

Transducer::Transducer(transducerTransitions t, int initial,
                       std::unordered_set<int> end)
    : transitions{t}, initialState{initial}, endStates{end} {}

// transducer with encoding for marking (a,b,c) 1^a X* T 1^b X* T 1^c X*
Transducer::Transducer(pnTransition t) : pnTrans{t}, initialState{0} {
  int counter = 0;
  for (int i = 0; i < t.first.size(); i++) {
    int delta = t.second[i] - t.first[i];

    // minimum
    int maxj;
    if (delta > 0) {
      maxj = t.first[i];
    } else {
      maxj = t.second[i];
    }

    for (int j = 0; j < maxj; j++) {
      this->transitions.emplace(
          std::make_pair(counter, std::make_pair('1', '1')),
          std::vector<int>{++counter});
    }

    this->transitions.emplace(std::make_pair(counter, std::make_pair('1', '1')),
                              std::vector<int>{counter});

    if (delta > 0) {
      for (int j = 0; j < delta; j++) {
        this->transitions.emplace(
            std::make_pair(counter, std::make_pair('X', '1')),
            std::vector<int>{++counter});
      }
    } else if (delta < 0) {
      for (int j = 0; j < (-delta); j++) {
        this->transitions.emplace(
            std::make_pair(counter, std::make_pair('1', 'X')),
            std::vector<int>{++counter});
      }
    }

    this->transitions.emplace(std::make_pair(counter, std::make_pair('T', 'T')),
                              std::vector<int>{++counter});
  }
  this->endStates = std::unordered_set<int>{counter};
}

// transducer with eps transitions
// unused
Transducer::Transducer(pnTransition t, bool a) {
  this->initialState = 0;
  int counter = 0;
  for (int i = 0; i < t.first.size(); i++) {
    int delta = t.second[i] - t.first[i];

    int maxj;
    if (delta > 0) {
      maxj = t.first[i];
    } else {
      maxj = t.second[i];
    }

    for (int j = 0; j < maxj; j++) {
      this->transitions.emplace(
          std::make_pair(counter, std::make_pair('1', '1')),
          std::vector<int>{++counter});
    }

    this->transitions.emplace(std::make_pair(counter, std::make_pair('1', '1')),
                              std::vector<int>{counter});

    if (delta > 0) {
      for (int j = 0; j < delta; j++) {
        this->transitions.emplace(
            std::make_pair(counter, std::make_pair('e', '1')),
            std::vector<int>{++counter});
      }
    } else if (delta < 0) {
      for (int j = 0; j < (-delta); j++) {
        this->transitions.emplace(
            std::make_pair(counter, std::make_pair('1', 'e')),
            std::vector<int>{++counter});
      }
    } else {
      for (int j = 0; j < (-delta); j++) {
        this->transitions.emplace(
            std::make_pair(counter, std::make_pair('1', '1')),
            std::vector<int>{++counter});
      }
    }

    this->transitions.emplace(std::make_pair(counter, std::make_pair('T', 'T')),
                              std::vector<int>{++counter});
  }
  this->endStates = std::unordered_set<int>{counter};
}

std::string Transducer::traverse(std::string word) {
  for (auto& c : word) {
  }
}

std::string Transducer::toString() {
  std::ostringstream str;

  for (auto& t : transitions) {
    str << "start " << t.first.first << " chars (" << t.first.second.first
        << "," << t.first.second.second << ") \n";
    str << "\t";
    for (auto& e : t.second) {
      str << e << " ";
    }
    str << "\n";
    str << "\n";
  }
  return str.str();
}

void Transducer::toDot(std::string fileName) const {
  if (fileName == "") {
    fileName = "transducer";
  }
  std::string pathName = "./visualization/" + fileName + ".dot";
  std::string imgName = "./visualization/" + fileName + ".png";
  std::ofstream str(pathName, std::ofstream::out);
  str << "digraph g\n";
  str << "{\n";
  // str << "graph [ splines = false ] \n";

  std::string shape = "octagon";

  for (auto& t : transitions) {
    int startState = t.first.first;
    if (endStates.contains(startState)) {
      shape = "doubleoctagon";
    }
    str << startState << " [shape=" << shape << " label=\"" << startState
        << "\"]\n";
    std::pair<char, char> chars = t.first.second;
    for (auto s : t.second) {
      shape = "octagon";
      if (endStates.contains(s)) {
        shape = "doubleoctagon";
      }
      str << s << " [shape=" << shape << " label=\"" << s << "\"]\n";
      str << startState << " -> " << s << " [label=\""
          << "(" << chars.first << "," << chars.second << ")"
          << "\"]\n";
    }
  }
  str << "}\n";
  str.close();
  std::string command = "dot " + pathName + " -Tpng > " + imgName;
  system(command.c_str());
}