#include <iostream>
#include "CMMLexer.h"

void show(const std::string &s) {
  using namespace std;
  auto tmp = s + ", hello!";
  cout << tmp << endl;
  //cout << sizeof(cmm::Token::TokenValue) << endl;
}