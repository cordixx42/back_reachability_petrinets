#include <iostream>

#include "../TableOfNodes.h"
#include "../Transducer.h"

void test1() {
  TableOfNodes table{std::vector<char>{'a', 'b', 'c'}};
  Node emptyword{std::vector<int>{EMTPYSET, EMTPYSET, EMTPYSET}, true};
  int eps = table.make(emptyword);
  Node a{std::vector<int>{SELF, SELF, eps}};
  int dfa = table.make(a);

  table.toDot(dfa, "test1dfa");

  transducerTransitions trans{};
  trans.emplace(std::make_pair(1, std::make_pair('a', 'a')),
                std::vector<int>{1});
  trans.emplace(std::make_pair(1, std::make_pair('b', 'b')),
                std::vector<int>{1});
  trans.emplace(std::make_pair(1, std::make_pair('b', 'c')),
                std::vector<int>{2});
  Transducer t{trans, 1, std::unordered_set<int>{2}};

  t.toDot("test1transducer");

  int r = table.pre(t, dfa);
  table.preToDot(dfa, t, r);

  table.toDot(r, "test1result");
}

void test2() {
  TableOfNodes table{std::vector<char>{'a', 'b', 'c'}};
  Node emptyword{std::vector<int>{EMTPYSET, EMTPYSET, EMTPYSET}, true};
  int eps = table.make(emptyword);
  Node a{std::vector<int>{SELF, eps, EMTPYSET}};
  int dfa = table.make(a);

  transducerTransitions trans{};
  trans.emplace(std::make_pair(1, std::make_pair('a', 'b')),
                std::vector<int>{1});
  trans.emplace(std::make_pair(1, std::make_pair('b', 'a')),
                std::vector<int>{1});
  trans.emplace(std::make_pair(1, std::make_pair('c', 'a')),
                std::vector<int>{2});
  Transducer t{trans, 1, std::unordered_set<int>{2}};
  int r = table.pre(t, dfa);

  std::cout << r;

  table.preToDot(dfa, t, r);
}

void test3() {
  TableOfNodes table{std::vector<char>{'a', 'b', 'c'}};
  Node emptyword{std::vector<int>{EMTPYSET, EMTPYSET, EMTPYSET}, true};
  int eps = table.make(emptyword);
  Node a{std::vector<int>{SELF, eps, EMTPYSET}};
  int dfa = table.make(a);

  transducerTransitions trans{};
  trans.emplace(std::make_pair(1, std::make_pair('a', 'b')),
                std::vector<int>{1});
  trans.emplace(std::make_pair(1, std::make_pair('b', 'b')),
                std::vector<int>{2});
  trans.emplace(std::make_pair(1, std::make_pair('b', 'a')),
                std::vector<int>{3});
  Transducer t{trans, 1, std::unordered_set<int>{2}};

  int r = table.pre(t, dfa);

  std::cout << r;
  table.preToDot(dfa, t, r);
}

// the loop case
void test4() {
  TableOfNodes table{std::vector<char>{'a', 'b'}};
  Node emptyword{std::vector<int>{EMTPYSET, EMTPYSET}, true};
  int eps = table.make(emptyword);
  Node abstar{std::vector<int>{SELF, SELF}, true};
  int dfa = table.make(abstar);

  table.toDot("");

  transducerTransitions trans{};
  trans.emplace(std::make_pair(1, std::make_pair('a', 'a')),
                std::vector<int>{1});
  trans.emplace(std::make_pair(1, std::make_pair('b', 'b')),
                std::vector<int>{1});
  trans.emplace(std::make_pair(1, std::make_pair('a', 'b')),
                std::vector<int>{2});
  Transducer t{trans, 1, std::unordered_set<int>{1}};

  t.toDot("");

  int r = table.pre(t, dfa);

  std::cout << r;
  table.preToDot(dfa, t, r);
}

int main() { test4(); }