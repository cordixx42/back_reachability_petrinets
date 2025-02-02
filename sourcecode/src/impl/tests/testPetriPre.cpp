#include <iostream>

#include "../PetriNet.h"
#include "../TableOfNodes.h"
#include "../Transducer.h"

void simple() {
  PetriNet n{std::vector<pnTransition>{
      std::make_pair(std::vector<int>{1, 0}, std::vector<int>{0, 2})}};
  Transducer t{n.allTransitions[0]};
  t.toDot("petriPreTrans");
  TableOfNodes table{std::vector<char>{'1', 'X', 'T'}};
  // v >= (1,2)
  int dfa = table.create("11*XT111*T");
  table.toDot(dfa, "petriPreMark");
  // should be v >= (2,0)
  int res = table.pre(t, dfa);
  table.toDot(res, "petriPreResult");

  std::cout << table.toString() << std::endl;
}

void simpleWithPreprocess() {
  PetriNet n{std::vector<pnTransition>{
      std::make_pair(std::vector<int>{1, 0}, std::vector<int>{0, 2})}};
  Transducer t{n.allTransitions[0]};
  std::vector<int> delta{1, -2};
  std::unordered_map<int, int> cache;
  t.toDot("petriPreTrans");
  TableOfNodes table{std::vector<char>{'1', 'X', 'T'}};
  // v >= (1,2)
  int dfa = table.create("11*T111*T");
  int dfaProcessed = table.preprocess(delta, dfa, 0, cache);
  table.toDot(dfaProcessed, "petriPreMark");
  // should be v >= (2,0)
  int res = table.pre(t, dfaProcessed);
  table.toDot(res, "petriPreResult");

  std::cout << table.toString() << std::endl;
}

void simpleWithPostprocess() {
  TableOfNodes table{std::vector<char>{'1', 'X', 'T'}};
  std::vector<int> delta{0, 3};
  std::unordered_map<int, int> cache;
  // v >= (3,4)
  int dfa = table.create("1111*T11111*XXXT");
  table.toDot(dfa, "beforePostprocess");
  int dfaProcessed = table.postprocess(delta, dfa, 0, cache);
  table.toDot(dfaProcessed, "afterPostprocess");
}

void simple2() {
  PetriNet n{std::vector<pnTransition>{
      std::make_pair(std::vector<int>{1, 0}, std::vector<int>{0, 2})}};
  Transducer t{n.allTransitions[0]};
  t.toDot("petriPreTrans");
  TableOfNodes table{std::vector<char>{'1', 'X', 'T'}};
  int dfa = table.create("1111*XT111111*T");
  table.toDot(dfa, "petriPreMark");
  int res = table.pre(t, dfa);
  table.toDot(res, "petriPreResult");

  std::cout << table.toString() << std::endl;
}

void simple3() {
  PetriNet n{std::vector<pnTransition>{
      std::make_pair(std::vector<int>{1, 2}, std::vector<int>{3, 0})}};
  Transducer t{n.allTransitions[0]};
  t.toDot("petriPreTrans");
  TableOfNodes table{std::vector<char>{'1', 'X', 'T'}};
  int dfa = table.create("111111*T1111111*XXT");
  table.toDot(dfa, "petriPreMark");
  int res = table.pre(t, dfa);
  table.toDot(res, "petriPreResult");

  std::cout << table.toString() << std::endl;
}

void testPreOneMarking() {
  PetriNet n{std::vector<pnTransition>{
      std::make_pair(std::vector<int>{1, 0, 0}, std::vector<int>{0, 2, 3})}};
  Transducer t{n.allTransitions[0]};
  t.toDot("petriPreTrans");
  TableOfNodes table{std::vector<char>{'1', 'X', 'T'}};
  int dfa = table.create("11*XT111*T1111*T");
  table.toDot(dfa, "petriPreMark");
  int res = table.pre(t, dfa);
  table.toDot(res, "petriPreResult");
}

void testPreSetMarkings() {
  PetriNet n{std::vector<pnTransition>{
      std::make_pair(std::vector<int>{2, 0}, std::vector<int>{0, 0})}};
  Transducer t{n.allTransitions[0]};
  t.toDot("petriPreTrans");
  TableOfNodes table{std::vector<char>{'1', 'X', 'T'}};
  // marking v >= (2,1)
  int a = table.create("111*XXT11*T");
  table.toDot(a, "marking1");
  // marking v >= (1,4)
  int b = table.create("11*XXT11111*T");
  table.toDot(b, "marking2");
  int c = table.unionOfNodes(a, b);
  table.toDot(c, "unionMarkings");

  int res = table.pre(t, c);
  table.toDot(res, "petriPreResult");
}

// preprocess after union should be same as union preprocess before union
void testPreprocessBeforeUnionSameAsAfterUnion() {
  TableOfNodes table{std::vector<char>{'1', 'X', 'T'}};
  std::unordered_map<int, int> cache;
  int a = table.create("111*T11*T");
  int a1 = table.preprocess(std::vector{3, 2}, a, 0, cache);
  int b = table.create("11*T11111*T");

  cache.clear();
  int b1 = table.preprocess(std::vector{3, 2}, b, 0, cache);

  int d = table.create("1111*T1*T");
  cache.clear();
  int d1 = table.preprocess(std::vector{3, 2}, d, 0, cache);

  int c = table.unionOfNodes(table.unionOfNodes(a, b), d);
  int c1 = table.unionOfNodes(table.unionOfNodes(d1, b1), a1);

  cache.clear();
  int cc = table.preprocess(std::vector{3, 2}, c, 0, cache);

  std::cout << c1 << " " << cc << std::endl;

  table.toDot(c1, "PreprocessBeforeUnion");
  table.toDot(cc, "PreprocessAfterUnion");
}

// case v >= (0,0)
void testPreprocessEdge() {
  TableOfNodes table{std::vector<char>{'1', 'X', 'T'}};
  std::unordered_map<int, int> cache;
  int a = table.create("1*T1*T");
  int a1 = table.preprocess(std::vector{3, 2}, a, 0, cache);
  table.toDot(a1, "PreprocessEdge");
}

void testPreSetMarkingsWithPreprocess() {
  PetriNet n{std::vector<pnTransition>{
      std::make_pair(std::vector<int>{3, 0}, std::vector<int>{2, 3})}};
  Transducer t{n.allTransitions[0]};
  std::vector<int> delta{1, -3};
  std::unordered_map<int, int> cache;
  t.toDot("petriPreTrans");
  TableOfNodes table{std::vector<char>{'1', 'X', 'T'}};
  // v >= (3,5)
  int m1 = table.create("1111*T111111*T");
  // v >= (1,6)
  int m2 = table.create("11*T1111111*T");
  // v >= (3,5) OR v >= (1,6)
  int m12 = table.unionOfNodes(m1, m2);
  int m12Processed = table.preprocess(delta, m12, 0, cache);
  table.toDot(m12Processed, "petriPreMark");
  // should be v >= (4,2) OR v >= (3,3)
  int res = table.pre(t, m12Processed);
  table.toDot(res, "petriPreResult");

  std::cout << table.toString() << std::endl;
}

void testPreSetMarkingsWithWrapper() {
  PetriNet n{std::vector<pnTransition>{
      std::make_pair(std::vector<int>{3, 0}, std::vector<int>{2, 3})}};
  Transducer t{n.allTransitions[0]};
  t.toDot("petriPreTrans");
  TableOfNodes table{std::vector<char>{'1', 'X', 'T'}};
  // v >= (3,5)
  int m1 = table.create("1111*T111111*T");
  int m11 = table.createFromMarking(std::vector<int>{3, 5});
  std::cout << m1 << " and  " << m11 << std::endl;
  // v >= (1,6)
  int m2 = table.create("11*T1111111*T");
  // v >= (3,5) OR v >= (1,6)
  int m12 = table.unionOfNodes(m1, m2);
  std::cout << "m12 is " << m12 << "\n";
  table.toDotCompact(m12, "petriPreArgCompact");
  // should be v >= (4,2) OR v >= (3,3)
  int res = table.preWrapper(t, m12);
  // table.toDot(res, "petriPreResult");
  table.toDotCompact(res, "petriPreResultCompact");

  std::cout << table.toString() << std::endl;
}

void createMarking() {
  TableOfNodes table{std::vector<char>{'1', 'X', 'T'}};
  int a = table.create("11111*T111*T");
  int b = table.create("1111*T1111*T");
  int c = table.unionOfNodes(a, b);
  table.toDot(c, "unionMarkings");
}

void testPreprocess() {
  TableOfNodes table{std::vector<char>{'1', 'X', 'T'}};
  int a = table.create("1111*T11*T");
  std::vector<int> delta{0, 3};
  std::unordered_map<int, int> cache;
  int b = table.preprocess(delta, a, 0, cache);

  table.toDotCompact(b, "bbb");
}

void testPostprocess() {
  TableOfNodes table{std::vector<char>{'1', 'X', 'T'}};
  int a = table.create("1111*T11*T");
  std::vector<int> delta{2, 3};
  std::unordered_map<int, int> cache;
  int b = table.postprocess(delta, a, 0, cache);

  table.toDotCompact(b, "bbb");
}

int main() {
  // testPostprocess();

  testPreSetMarkingsWithWrapper();
}
