#include "parser.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <tuple>

// mode with debug prints

// #define DEBUG_MODE  // Or 0 if you dont want to debug
#ifdef DEBUG_MODE
#define Dprint(x) std::cout << x
#else
#define Dprint(x)
#endif

#define WS 32
#define NL 10
#define TAB 9

// 10 is \n
// 32 is whitespace
// 48-57 is 0-9
// 65-90 is A-Z
// 97-122 is a-z

std::vector<int> operator+(const std::vector<int>& v,
                           const std::vector<int>& w) {
  std::vector<int> result(v.size(), 0);
  for (int i = 0; i < v.size(); i++) {
    result[i] = v[i] + w[i];
  }
  return result;
}

bool isDigit(int c) { return c >= 48 && c <= 57; }

bool isValidVarChar(int c) {
  return (c >= 65 && c <= 90) || (c >= 97 && c <= 122) || isDigit(c) ||
         c == '\'' || c == '_';
}

// constructor opens the file
MistParser::MistParser(std::string path) : stateId{0} {
  if (path == "") {
    throw 404;
  }
  std::string pathName = path;
  mist.open(pathName, std::ifstream::in);

  if (!mist.is_open()) {
    throw 131;
  }
}

PetriNet MistParser::parseAndProcess() {
  skipCommentLines();

  Dprint("START"
         << "\n");

  mist >> currentString;
  assert(currentString == "vars");
  Dprint("before parsing vars"
         << "\n");
  parseVars();
  Dprint("after parsing vars"
         << "\n");

  // CHECK vars

  Dprint("VERIFYING STATES" << std::endl);
  for (const auto [a, b] : states) {
    Dprint(a << " " << b << std::endl);
  }

  mist >> currentString;
  assert(currentString == "rules");

  rule r{};
  parseRules(r);
  processRules(r);

  mist >> currentString;
  assert(currentString == "init");

  init i{};
  parseInit(i);
  processInit(i);

  mist >> currentString;
  assert(currentString == "target");

  Dprint("target read" << std::endl);

  target t{};
  parseTarget(t);
  processTarget(t);

  Dprint("isEndOfFile ? " << mist.eof() << std::endl);

  // TODO: currently ignoring invariants
  if (!mist.eof()) {
    mist >> currentString;
    assert(currentString == "invariants");
    parseInvariants();
  }

  // TODO: create Petri Net here
  PetriNet net{this->transitions, this->ini, this->iniDetail, this->tar};
  return net;
}

// parseVars with recursion
void MistParser::parseVars() {
  int currentPos = mist.tellg();
  mist >> currentString;
  while (currentString != "rules") {
    mist.seekg(currentPos);
    skipToNextRealContent();
    // insert into state map directly
    states.emplace(parseVariable(), stateId++);
    currentPos = mist.tellg();
    mist >> currentString;
  }
  mist.seekg(currentPos);
  return;
}

void MistParser::parseRules(rule& r) {
  int currentPos = mist.tellg();

  // if this was last rule
  mist >> currentString;

  while (currentString != "init") {
    mist.seekg(currentPos);
    skipToNextRealContent();

    guard g;
    parseGuard(g);

    mist >> currentString;
    assert(currentString == "->");

    effect e;
    parseEffect(e);

    mist >> currentString;
    assert(currentString == ";");

    r.push_back(std::make_pair(g, e));

    currentPos = mist.tellg();

    mist >> currentString;
  }
  mist.seekg(currentPos);
  return;
}

void MistParser::parseGuard(guard& g) {
  g.push_back(parseGuardAtom());
  skipToNextRealContent();
  if (mist.peek() == ',') {
    mist.get();
    parseGuard(g);
  } else {
    return;
  }
}

guardAtom MistParser::parseGuardAtom() {
  skipToNextRealContent();
  std::string var;
  int i;

  var = parseVariable();

  char c = mist.get();

  if (c == '=') {
    i = parseInteger();
    // Dprint("guards " << var << " = " << i << std::endl);
    return std::make_tuple(var, "=", i);
  } else if (c == '>') {
    assert(mist.get() == '=');
    i = parseInteger();
    // Dprint("guards " << var << " >= " << i << std::endl);
    return std::make_tuple(var, ">=", i);
  } else {
    Dprint("var is " << var << std::endl);
    Dprint("c is " << c << std::endl);
    assert(false);
  }

  throw 21;
}

void MistParser::parseEffect(effect& e) {
  skipToNextRealContent();
  if (mist.peek() == ';') {
    return;
  }
  Dprint("CCC"
         << "\n");
  effectAtom a = parseEffectAtom();
  Dprint("EEE"
         << "\n");
  e.push_back(a);
  Dprint("DDD"
         << "\n");
  skipToNextRealContent();
  if (mist.peek() == ',') {
    mist.get();
    parseEffect(e);
  } else {
    return;
  }
}

effectAtom MistParser::parseEffectAtom() {
  skipToNextRealContent();
  std::string var;

  var = parseVariable();

  Dprint("effects " << var << std::endl);

  assert(mist.get() == '=');

  return std::make_pair(var, parseAssignmentVar());
}

assignmentVar MistParser::parseAssignmentVar() {
  skipToNextRealContent();

  if (isDigit(mist.peek())) {
    int i = parseInteger();
    return std::make_tuple("", "", i);
  }

  std::string var;
  var = parseVariable();

  int c = mist.get();
  if (c == '+') {
    int i = parseInteger();
    // Dprint("plus \n");

    return std::make_tuple(var, "+", i);
    // this should not happen
    // if (mist.peek() == ',' || mist.peek() == ';') {
    //   return;
    // } else {
    //   parseAssignmentVar();
    // }
  } else if (c == '-') {
    // Dprint("minus \n");
    int i = parseInteger();
    return std::make_tuple(var, "-", i);
  } else {
    // Dprint(c << std::endl);
    assert(false);
  }
}

void MistParser::parseInit(init& i) {
  guard g{};
  parseGuard(g);
  i.push_back(g);
  skipToNextRealContent();

  int currentPos = mist.tellg();
  mist >> currentString;
  // if this was last variable
  if (currentString == "target") {
    mist.seekg(currentPos);
    return;
  } else {
    mist.seekg(currentPos);
    parseInit(i);
  }
}

void MistParser::parseTarget(target& t) {
  guard g{};
  parseGuard(g);
  t.push_back(g);

  Dprint("guard length" << g.size() << std::endl);

  skipToNextRealContent();

  // end of file case
  if (mist.eof()) {
    return;
  }

  int currentPos = mist.tellg();
  mist >> currentString;

  Dprint("current string " << currentString << std::endl);

  // if this was last variable
  if (currentString == "invariants") {
    mist.seekg(currentPos);
    return;
  } else {
    mist.seekg(currentPos);
    parseTarget(t);
  }
}

void MistParser::parseInvariants() {
  parseInvariant();
  if (!mist.eof()) {
    parseInvariants();
  }
}

void MistParser::parseInvariant() {
  parseInvariantAtom();
  skipToNextRealContent();
  if (mist.peek() == ',') {
    mist.get();
    parseInvariant();
  } else {
    return;
  }
}

void MistParser::parseInvariantAtom() {
  std::string var = parseVariable();
  assert(mist.get() == '=');
  int i = parseInteger();
  // Dprint("invariant " << var << " " << i << std::endl);
}

// charwise based
std::string MistParser::parseVariable() {
  skipToNextRealContent();
  std::stringstream ss;
  // first can't be digit
  assert(!isDigit(mist.peek()));
  while (isValidVarChar(mist.peek())) {
    ss << char(mist.get());
  }
  skipToNextRealContent();
  Dprint("var " << ss.str() << "\n");
  return ss.str();
}

int MistParser::parseInteger() {
  int i;
  mist >> i;
  return i;
}

void MistParser::skipWhiteSpacesAndTabs() {
  while (mist.peek() == WS || mist.peek() == TAB) {
    mist.get();
  }
}

void MistParser::skipToNextLine() {
  while (mist.get() != NL) {
  }
  if (mist.peek() == '#') {
    skipToNextLine();
  }
}

void MistParser::skipToNextRealContent() {
  skipWhiteSpacesAndTabs();
  if (mist.peek() == '#') {
    skipToNextLine();
  }
  while (mist.peek() == NL) {
    skipToNextLine();
    skipToNextRealContent();
  }
}

void MistParser::skipCommentLines() {
  // skip all comments lines
  skipWhiteSpacesAndTabs();
  while (mist.peek() == '#') {
    skipToNextLine();
    skipWhiteSpacesAndTabs();
  }
}

void MistParser::processTarget(target& t) {
  Dprint("PROCESSING TARGETS" << std::endl);

  // initialize correctly
  std::vector<std::vector<int>> targets(t.size(),
                                        std::vector<int>(states.size(), 0));

  for (int i = 0; i < t.size(); i++) {
    const auto g = t[i];
    for (const auto [var, eq, n] : g) {
      assert(eq == ">=");
      Dprint(var << eq << n << std::endl);
      targets[i][states[var]] = n;
    }
    Dprint("\n");
  }

  Dprint("VERIFYING TARGETS" << std::endl);

  for (const auto elem : targets) {
    for (const auto e : elem) {
      Dprint(e << " ");
    }
    Dprint("\n");
  }

  this->tar = std::move(targets);
}

void MistParser::processInit(init& t) {
  Dprint("PROCESSING INIT" << std::endl);

  // initialize correctly
  std::vector<int> initt(states.size(), 0);
  std::vector<int> inittDetail(states.size(), 0);

  // check if only one init configuration
  assert(t.size() == 1);

  for (const auto [var, eq, n] : t[0]) {
    Dprint(var << eq << n << std::endl);
    initt[states[var]] = n;
    if (eq == ">=") {
      inittDetail[states[var]] = 1;
    }
  }

  Dprint("VERIFYING INIT" << std::endl);

  for (const auto e : initt) {
    Dprint(e << " ");
  }
  Dprint("\n");

  for (const auto e : inittDetail) {
    Dprint(e << " ");
  }

  this->ini = std::move(initt);
  this->iniDetail = std::move(inittDetail);
}

void MistParser::processRules(rule& r) {
  Dprint("PROCESSING RULES" << std::endl);
  // for each rule g -> e
  for (const auto& [guard, effect] : r) {
    pnTransition currentTransition;

    std::vector<int> guardVector(states.size(), 0);
    for (const auto& [var, sign, n] : guard) {
      assert(sign == ">=");
      guardVector[states[var]] = n;
    }

    std::vector<int> effectVector(states.size(), 0);
    for (const auto& [var, eAtom] : effect) {
      // x0 = x0'
      // assert(std::get<0>(eAtom) == var);
      std::string var_ = std::get<0>(eAtom);
      std::string op = std::get<1>(eAtom);
      if (op == "-") {
        Dprint(var_ << " " << op << " " << std::get<2>(eAtom) << "\n");
        effectVector[states[var_]] = -std::get<2>(eAtom);
      } else if (op == "+") {
        effectVector[states[var_]] = std::get<2>(eAtom);
      } else {
        throw 21;
      }
    }

    Dprint("VERIFYING RULES" << std::endl);
    Dprint("guard vector \n ");
    for (const auto e : guardVector) {
      Dprint(e << ", ");
    }
    Dprint("\n effect vector \n");

    for (const auto e : effectVector) {
      Dprint(e << ", ");
    }
    Dprint("\n");

    // TODO: maybe use move
    currentTransition.first = guardVector;

    currentTransition.second = guardVector + effectVector;

    this->transitions.push_back(currentTransition);

    Dprint("result sum vector \n ");
    for (const auto e : currentTransition.second) {
      Dprint(e << ", ");
    }
    Dprint("\n");
  }
}