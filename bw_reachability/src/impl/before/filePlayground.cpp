#include <fstream>
#include <iostream>
#include <string>

#define WS 32
#define NL 10

// 10 is \n
// 32 is whitespace
// 48-57 is 0-9
// 65-90 is A-Z
// 97-122 is a-z

void skipWhiteSpaces(std::ifstream& i) {
  while (i.peek() == WS) {
    i.get();
  }
}

void skipToNextLine(std::ifstream& i) {
  while (i.get() != NL) {
  }
}

void parseFile(std::string path) {
  int lineCounter = 0;
  std::string pathName = "./benchmarks/" + path;
  // TODO: if exists delete content
  std::ifstream mist(pathName, std::ifstream::in);
  if (!mist.is_open()) {
    throw 131;
  }

  std::cout << mist.tellg() << std::endl;

  skipWhiteSpaces(mist);

  std::cout << mist.tellg() << std::endl;

  skipToNextLine(mist);

  std::cout << mist.tellg() << std::endl;

  //   while (!mist.eof() && mist.tellg() < 40) {
  //     std::cout << "tellg " << mist.tellg() << std::endl;
  //     std::cout << "peek " << mist.peek() << std::endl;
  //     std::cout << "get " << mist.get() << std::endl;
  //   }

  //   std::cout << mist.tellg() << std::endl;

  //   std::string currentLine;
  //   std::getline(mist, currentLine);
  //   std::cout << currentLine << std::endl;
  //   std::cout << mist.peek() << std::endl;
  //   std::cout << mist.tellg() << std::endl;
}

int main() { parseFile("basic.spec"); }