#include "CMMParser.h"

using namespace cmm;

bool CMMParser::Parse() {
  Lex();
  for (;;) {
    switch (getKind()) {
    default:
      break;
    case Token::Kw_if:
      break;
    case Token::Kw_while:
      break;
    case Token::Kw_for:
      break;
    case Token::Kw_do:
      break;
    case Token::Kw_bool:
    case Token::Kw_int:
    case Token::Kw_double:
      break;
    }
  }
  return false;
}