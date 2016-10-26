#include "CMMLexer.h"
#include <iostream>

using namespace cmm;
using namespace std;

int main(int argc, char *argv[])
{
  if (argc < 2)
    return 1;
  SourceManager SrcMgr(argv[1]);
  Lexer Lex(SrcMgr);

  while (Lex.Lex().isNot(Token::Eof)) {
    auto LineCol = SrcMgr.getLineColByLoc(Lex.getLoc());
    cout << "(Line " << LineCol.first + 1 << ", Col "
         << LineCol.second + 1 << ") ";
    switch (Lex.getKind()) {
    default:
      cout << "Unknown Tag: " << Lex.getKind();
      break;
    case Token::Identifier:
      cout << "Identifier: " << Lex.getStrVal(); break;
    case Token::String:
      cout << "String: " << Lex.getStrVal(); break;
    case Token::Integer:
      cout << "Integer: " << Lex.getIntVal(); break;
    case Token::Double:
      cout << "Double: " << Lex.getDoubleVal(); break;
    case Token::Boolean:
      cout << "Boolean: " << (Lex.getDoubleVal() ? "True" : "False");
      break;
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
    }
    cout << "\n";
  }
  return 0;
}
