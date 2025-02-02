#pragma once
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "../PetriNet.h"

typedef std::tuple<std::string, std::string, int> guardAtom;
typedef std::vector<guardAtom> guard;
typedef std::vector<guard> target;
typedef std::vector<guard> init;
typedef std::tuple<std::string, std::string, int> assignmentVar;
typedef std::pair<std::string, assignmentVar> effectAtom;
typedef std::vector<effectAtom> effect;
typedef std::pair<guard, effect> singleRule;
typedef std::vector<singleRule> rule;

struct MistParser {
  // input file
  std::ifstream mist;

  // unused tracker of current line to parse
  int lineCounter;

  // id counter for states
  int stateId;

  // a utility  string for parsing
  std::string currentString;

  // map of state name to id
  std::unordered_map<std::string, int> states;

  std::vector<pnTransition> transitions;

  // x = [i] : state with id i (= or >=) x
  std::vector<int> ini;

  // y = [i] :  y = 0 -> state with id i = x
  //            y = 1 -> state with id i >= x
  std::vector<int> iniDetail;

  // multiple targets possible
  // target always  >=
  std::vector<std::vector<int>> tar;

  MistParser(std::string path);

  PetriNet parseAndProcess();

  void parseVars();

  void parseRules(rule& r);
  void processRules(rule& r);

  void parseInit(init& i);
  void processInit(init& t);

  void parseTarget(target& t);
  void processTarget(target& t);

  void parseInvariants();

  void parseInvariant();

  void parseInvariantAtom();

  void parseGuard(guard& g);

  guardAtom parseGuardAtom();

  void parseEffect(effect& e);

  effectAtom parseEffectAtom();

  assignmentVar parseAssignmentVar();

  std::string parseVariable();

  int parseInteger();

  void skipWhiteSpacesAndTabs();

  void skipToNextLine();

  void skipToNextRealContent();

  void skipCommentLines();
};