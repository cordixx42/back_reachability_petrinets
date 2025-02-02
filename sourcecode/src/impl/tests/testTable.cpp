#include <iostream>

#include "../TableOfNodes.h"

void testCreate(TableOfNodes table) {
  table.toDot(table.create("a*ab"), "");
  table.create("b");
  table.toDot("very_simple");
  std::cout << table.maxPathsOf(4) << std::endl;
  //   std::cout << table.create("a") << std::endl;
  //   std::cout << table.toString() << std::endl;

  //   std::cout << table.create("a") << std::endl;
  //   std::cout << table.toString() << std::endl;

  //   std::cout << table.create("ba") << std::endl;
  //   std::cout << table.toString() << std::endl;

  //   std::cout << table.create("a*b") << std::endl;
  //   std::cout << table.toString() << std::endl;

  //   std::cout << table.create("a*ba") << std::endl;
  //   std::cout << table.toString() << std::endl;

  //   std::cout << table.create("a*b*a*") << std::endl;
  //   std::cout << table.toString() << std::endl;
}

void testUnion(TableOfNodes table) {
  int a = table.create("a*b");
  int b = table.create("a");
  int c = table.unionOfNodes(a, b);
  // int d = table.unionOfNodes(c, a);

  std::cout << table.toString() << std::endl;
  std::cout << table.toStringUnions() << std::endl;

  table.toDot("testSize");

  std::cout << table.sizeOf(0) << std::endl;
  std::cout << table.sizeOf(2) << std::endl;
  std::cout << table.sizeOf(3) << std::endl;

  //   std::cout << table.toString() << std::endl;
  //   std::cout << "a is " << a << " and b is " << b << std::endl;
  //   int c = table.unionOfNodes(a, b);
  //   std::cout << "c is " << c << std::endl;
  //   std::cout << table.toString() << std::endl;
}

void testInter(TableOfNodes table) {
  table.create("ab");
  int a = table.create("ab*cca*");
  int b = table.create("ab*c*");

  // a = table.create("a*b*");
  // b = table.create("b*a*");

  int c = table.interOfNodes(a, b);
  int d = table.interOfNodes(b, a);
  std::cout << c << std::endl;
  table.toDot("testSize");

  std::cout << "0 " << table.sizeOf(0) << std::endl;
  std::cout << "2 " << table.sizeOf(2) << std::endl;
  std::cout << "3 " << table.sizeOf(3) << std::endl;
  std::cout << "7 " << table.sizeOf(7) << std::endl;
  std::cout << "8 " << table.sizeOf(8) << std::endl;
}

void testPreprocess(TableOfNodes table) {
  std::unordered_map<int, int> cache;
  int a = table.create("111*T11*T");
  table.toDot(a, "marking1");
  int b = table.create("11*T11111*T");
  table.toDot(b, "marking2");
  int c = table.unionOfNodes(a, b);
  table.toDot(c, "unionMarkings");
  table.toDot(c, "beforePreprocess");
  int res = table.preprocess(std::vector{5, 0}, c, 0, cache);
  table.toDot(res, "afterPreprocess");
}

void testEasyPreprocess(TableOfNodes table) {
  std::unordered_map<int, int> cache;
  int a = table.create("111*T11*T");
  table.toDot(a, "beforePreprocess");
  int res = table.preprocess(std::vector{1, 2}, a, 0, cache);
  table.toDot(res, "afterPreprocess");
}

void testCreateMarkings(TableOfNodes table) {
  int a = table.create("111*XXT11*T");
  table.toDot(a, "marking1");
  int b = table.create("1111*XXT11111*T");
  table.toDot(b, "marking2");
  int c = table.unionOfNodes(a, b);
  table.toDot(c, "unionMarkings");

  std::cout << table.toString() << std::endl;
}

// void testCreateFromMarking(TableOfNodes table) {
//   table.createFromMarking(std::vector<int>{3, 2});
// }

void testCreateFromMarking(TableOfNodes table) {
  table.createSetFromMarking(std::vector<int>{1, 1, 1, 0, 0},
                             std::vector<int>{1, 0, 0, 0, 0});
}

int main() {
  // TableOfNodes table{std::vector<char>{'a', 'b', 'c', 'd'}};
  TableOfNodes table{std::vector<char>{'1', 'X', 'T'}};
  std::cout << "table created" << std::endl;
  std::cout << table.toString() << std::endl;

  testCreate(table);
  // testUnion(table);
  // testInter(table);
  // testPreprocess(table);
  // testCreateFromMarking(table);
  // testEasyPreprocess(table);
  // testCreateMarkings(table);
  // std::cout << table.toString() << std::endl;
}