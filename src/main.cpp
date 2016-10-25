#include "CMMLexer.h"
#include <iostream>

using namespace cmm;
using namespace std;

int main(int argc, char *argv[])
{
  if (argc < 2)
    return 1;
  Lexer lexer(argv[1]);
  while (lexer.Lex().isNot(Token::Eof)) {
    switch (lexer.getKind()) {
    default: cerr << "Unknown";
      break;
    case Token::Identifier:
      cout << "Identifier: " << lexer.getStrVal() << endl;
      break;
    case Token::String:
      cout << "String: " << lexer.getStrVal() << endl;
      break;
    case Token::Integer:
      cout << "Integer: " << lexer.getIntVal() << endl;
      break;
    case Token::Double:
      cout << "Double: " << lexer.getDoubleVal() << endl;
      break;
    case Token::Boolean:
      cout << "Boolean: " << (lexer.getDoubleVal() ? "True" : "False") << endl;
      break;
    case Token::LParen:	        cout << "LParen:      (" << endl;	 break;
    case Token::RParen:	        cout << "RParen:      )" << endl;	 break;
    case Token::LBrac:	        cout << "LBrac:       [" << endl;	 break;
    case Token::RBrac:	        cout << "RBrac:       ]" << endl;	 break;
    case Token::LCurly:	        cout << "LCurly:      {" << endl;	 break;
    case Token::RCurly:	        cout << "RCurly:      }" << endl;	 break;
    case Token::Plus:	        cout << "Plus:        +" << endl;	 break;
    case Token::Minus:	        cout << "Minus:       -" << endl;	 break;
    case Token::Star:	        cout << "Star:        *" << endl;	 break;
    case Token::Slash:	        cout << "Slash:       /" << endl;	 break;
    case Token::Semicolon:	    cout << "Semicolon:   ;" << endl;    break;
    case Token::Comma:	        cout << "Comma:       ," << endl;	 break;
    case Token::Equal:	        cout << "Equal:       =" << endl;	 break;
    case Token::Percent:	    cout << "Percent:     %" << endl;	 break;
    case Token::Exclaim:	    cout << "Exclaim:     !" << endl;	 break;
    case Token::AmpAmp:	        cout << "AmpAmp:     &&" << endl;	 break;
    case Token::PipePipe:	    cout << "PipePipe:   ||" << endl;	 break;
    case Token::Less:	        cout << "Less:        <" << endl;	 break;
    case Token::LessEqual:	    cout << "LessEqual:  <=" << endl;    break;
    case Token::EqualEqual:	    cout << "EqualEqual: ==" << endl;	 break;
    case Token::Greater:	    cout << "Greater:     >" << endl;	 break;
    case Token::Amp:	        cout << "Amp:         &" << endl;	 break;
    case Token::Pipe:	        cout << "Pipe:        |" << endl;	 break;
    case Token::LessLess:	    cout << "LessLess:   <<" << endl;	 break;
    case Token::Caret:	        cout << "Caret:       ^" << endl;	 break;
    case Token::Tilde:	        cout << "Tilde:       ~" << endl;	 break;
    case Token::GreaterGreater:	cout << "GreaterGreater:>>" << endl; break;
    case Token::Kw_if:	        cout << "Kw_if" << endl;	 break;
    case Token::Kw_else:	    cout << "Kw_else" << endl;	 break;
    case Token::Kw_for:	        cout << "Kw_for" << endl;	 break;
    case Token::Kw_while:	    cout << "Kw_while" << endl;	 break;
    case Token::Kw_do:	        cout << "Kw_do" << endl;	 break;
    case Token::Kw_break:	    cout << "Kw_break" << endl;	 break;
    case Token::Kw_continue:    cout << "Kw_continue" << endl;	 break;
    case Token::Kw_int:	        cout << "Kw_int" << endl;	 break;
    case Token::Kw_double:	    cout << "Kw_double" << endl;	 break;
    case Token::Kw_bool:	    cout << "Kw_bool" << endl;	 break;
    }
  }
  return 0;
}
