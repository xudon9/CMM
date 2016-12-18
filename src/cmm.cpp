/*
 * cmm.cpp
 * Copyright (C) 2016 wang <hsu@whu.edu.cn>
 *
 * TODO:
 * 1. command line argument
 * 2. dead code elimination
 * 3. functional return
 */

#include <cstdlib>
#include <iostream>
#include "CMMLexer.h"
#include "CMMParser.h"

using namespace cmm;

static int AsParseInput(SourceMgr &SrcMgr) {
  CMMParser Parser(SrcMgr);
  return Parser.Parse();
}

static int AsLexInput(SourceMgr &SrcMgr) {
  CMMLexer Lexer(SrcMgr);

  bool Error = false;
  while (Lexer.Lex().isNot(Token::Eof)) {
    using std::cout;
    auto LineCol = SrcMgr.getLineColByLoc(Lexer.getLoc());
    cout << "(Line " << LineCol.first + 1 << ", Col "
         << LineCol.second + 1 << ") ";

    switch (Lexer.getKind()) {
    default:
      cout << "Unknown Token: " << Lexer.getKind();
      Error = true;
      break;
    case Token::Error:
      Error = true; // error already printed.
      break;
    case Token::Identifier:
      cout << "Identifier: " << Lexer.getStrVal(); break;
    case Token::String:
      cout << "String: " << Lexer.getStrVal(); break;
    case Token::Integer:
      cout << "Integer: " << Lexer.getIntVal(); break;
    case Token::Double:
      cout << "Double: " << Lexer.getDoubleVal(); break;
    case Token::Boolean:
      cout << "Boolean: " << (Lexer.getDoubleVal() ? "True" : "False"); break;
    case Token::InfixOp:
      cout << "InfixOp: " << Lexer.getStrVal(); break;
    case Token::LParen:         cout << "LParen: ("; break;
    case Token::RParen:         cout << "RParen: )"; break;
    case Token::LBrac:          cout << "LBrac: ["; break;
    case Token::RBrac:          cout << "RBrac: ]"; break;
    case Token::LCurly:         cout << "LCurly: {"; break;
    case Token::RCurly:         cout << "RCurly: }"; break;
    case Token::Plus:           cout << "Plus: +"; break;
    case Token::Minus:          cout << "Minus: -"; break;
    case Token::Star:           cout << "Star: *"; break;
    case Token::Slash:          cout << "Slash: /"; break;
    case Token::Semicolon:      cout << "Semicolon: ;"; break;
    case Token::Comma:          cout << "Comma: ,"; break;
    case Token::Equal:          cout << "Equal: ="; break;
    case Token::Percent:        cout << "Percent: %"; break;
    case Token::Exclaim:        cout << "Exclaim: !"; break;
    case Token::AmpAmp:         cout << "AmpAmp: &&"; break;
    case Token::PipePipe:       cout << "PipePipe: ||"; break;
    case Token::Less:           cout << "Less: <"; break;
    case Token::LessEqual:      cout << "LessEqual: <="; break;
    case Token::EqualEqual:     cout << "EqualEqual: =="; break;
    case Token::Greater:        cout << "Greater: >"; break;
    case Token::Amp:            cout << "Amp: &"; break;
    case Token::Pipe:           cout << "Pipe: |"; break;
    case Token::LessLess:       cout << "LessLess: <<"; break;
    case Token::Caret:          cout << "Caret: ^"; break;
    case Token::Tilde:          cout << "Tilde: ~"; break;
    case Token::GreaterGreater: cout << "GreaterGreater: >>"; break;
    case Token::Kw_if:          cout << "Keyword: if"; break;
    case Token::Kw_else:        cout << "Keyword: else"; break;
    case Token::Kw_for:         cout << "Keyword: for"; break;
    case Token::Kw_while:       cout << "Keyword: while"; break;
    case Token::Kw_do:          cout << "Keyword: do"; break;
    case Token::Kw_break:       cout << "Keyword: break"; break;
    case Token::Kw_continue:    cout << "Keyword: continue"; break;
    case Token::Kw_int:         cout << "Keyword: int"; break;
    case Token::Kw_double:      cout << "Keyword: double"; break;
    case Token::Kw_bool:        cout << "Keyword: bool"; break;
    case Token::Kw_void:        cout << "Keyword: void"; break;
    case Token::Kw_return:      cout << "Keyword: return"; break;
    case Token::Kw_infix:       cout << "Keyword: infix"; break;
    }
    cout << "\n";
  }
  return Error;
}

int main(int argc, char *argv[])
{
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " file" << std::endl;
    std::exit(EXIT_FAILURE);
  }
  SourceMgr SrcMgr(argv[1]);
#if 1
  int Res = AsParseInput(SrcMgr);
#else
  int Res = AsLexInput(SrcMgr);
#endif
  std::exit(Res);
}
