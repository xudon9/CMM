#include <iostream>
#include <cctype>
#include "CMMLexer.h"

using namespace cmm;

bool Lexer::Error(LocTy ErrorLoc, const std::string &Msg) {
  SrcMgr.Error(ErrorLoc, Msg);
  return true;
}

void Lexer::Warning(LocTy ErrorLoc, const std::string &Msg) {
  SrcMgr.Warning(ErrorLoc, Msg);
}

static int hexDigitValue(int C) {
  if ('a' <= C && C <= 'f')
    return 10 + C - 'a';
  if ('A' <= C && C <= 'F')
    return 10 + C - 'A';
  return C - '0';
}

static void UnEscapeLexed(std::string &Str) {
  if (Str.empty()) return;

  char *Buffer = &Str[0], *EndBuffer = Buffer + Str.size();
  char *BOut = Buffer;
  for (char *BIn = Buffer; BIn != EndBuffer; ) {
    if (BIn[0] == '\\') {
      if (BIn < EndBuffer-1 && BIn[1] == '\\') {
        *BOut++ = '\\'; // Two \ becomes one
        BIn += 2;
      } else if (BIn < EndBuffer - 2 &&
          isxdigit(static_cast<unsigned char>(BIn[1])) &&
          isxdigit(static_cast<unsigned char>(BIn[2]))) {
        *BOut = static_cast<unsigned char>(16 * hexDigitValue(BIn[1]) +
            hexDigitValue(BIn[2]));
        BIn += 3;                           // Skip over handled chars
        ++BOut;
      } else {
        *BOut++ = *BIn++;
      }
    } else {
      *BOut++ = *BIn++;
    }
  }
  Str.resize(BOut - Buffer);
}

Token Lexer::LexToken() {
  TokStartLoc = SrcMgr.getLoc();
  int CurChar = getNextChar();
  switch (CurChar) {
  default:
    if (std::isalpha(static_cast<unsigned char>(CurChar)) || CurChar == '_') {
      ungetChar();
      return LexIdentifier();
    }
    Error("unknown character " + std::string(1, static_cast<char>(CurChar)));
    return Token::Error;
  case std::char_traits<char>::eof():
    return Token::Eof;
  case '\0': case ' ': case '\t': case '\n': case '\r':
    return LexToken();
  case '/': {
    int NextChar = getNextChar();
    if (NextChar == '/') {
      SkipLineComment();
      return LexToken();
    }
    if (NextChar == '*') {
      SkipBlockComment();
      return LexToken();
    }
    ungetChar();
    return Token::Slash;
  }
  case '"': return LexString();
  case '(': return Token::LParen;
  case ')': return Token::RParen;
  case '[': return Token::LBrac;
  case ']': return Token::RBrac;
  case '{': return Token::LCurly;
  case '}': return Token::RCurly;
  case '+': return Token::Plus;
  case '-': return Token::Minus;
  case '*': return Token::Star;
  case '%': return Token::Percent;
  case ';': return Token::Semicolon;
  case ',': return Token::Comma;
  case '^': return Token::Caret;
  case '~': return Token::Tilde;
  case '=':
    if (peekNextChar() != '=')
      return Token::Equal;
    getNextChar();
    return Token::EqualEqual;
  case '!':
    if (peekNextChar() != '=')
      return Token::ExclaimEqual;
    getNextChar();
    return Token::Exclaim;
  case '&':
    if (peekNextChar() != '&')
      return Token::Amp;
    getNextChar();
    return Token::AmpAmp;
  case '|':
    if (peekNextChar() != '|')
      return Token::Pipe;
    getNextChar();
    return Token::PipePipe;
  case '<': {
    int NextChar = getNextChar();
    if (NextChar == '<')
      return Token::LessLess;
    if (NextChar == '=')
      return Token::LessEqual;
    ungetChar();
    return Token::Less;
  }
  case '>': {
    int NextChar = getNextChar();
    if (NextChar == '>')
      return Token::GreaterGreater;
    if (NextChar == '=')
      return Token::GreaterEqual;
    ungetChar();
    return Token::Greater;
  }
  case '0': case '1': case '2': case '3': case '4':
  case '5': case '6': case '7': case '8': case '9': {
    ungetChar();
    return LexDigit();
  }
  }
}

// Assume the '//' is eaten
void Lexer::SkipLineComment() {
  for (;;) {
    if (peekNextChar() == '\n' || peekNextChar() == '\r' ||
        peekNextChar() == std::char_traits<char>::eof())
      return;
    getNextChar();
  }
}

Token Lexer::LexIdentifier() {
  StrVal.clear();
  do {
    StrVal.push_back(static_cast<char>(getNextChar()));
  } while (std::isalnum(peekNextChar()) || peekNextChar() == '_');
#define KEYWORD(STR) do {                                                      \
                       if (StrVal == #STR)                                     \
                         return Token::Kw_##STR;                               \
                     } while (0)
  KEYWORD(if);
  KEYWORD(else);
  KEYWORD(for);
  KEYWORD(while);
  KEYWORD(do);
  KEYWORD(break);
  KEYWORD(continue);
  KEYWORD(int);
  KEYWORD(double);
  KEYWORD(bool);
#undef KEYWORD
  if (StrVal.back() == '_')
    Warning("identifier end with _");
  return Token::Identifier;
}

// Assume the first '"' is eaten
Token Lexer::LexString() {
  StrVal.clear();
  for (;;) {
    int CurChar = getNextChar();
    if (CurChar == std::char_traits<char>::eof()) {
      // TODO: Escape for \"
      Error("end of file in string constant");
      return Token::Error;
    }
    if (CurChar == '"') {
      UnEscapeLexed(StrVal);
      return Token::String;
    }
    StrVal.push_back(static_cast<char>(CurChar));
  }
}

Token Lexer::LexDigit() {
  int HeadChar = getNextChar();
  IntVal = 0;

  if (HeadChar == '0' && (peekNextChar() == 'x' || peekNextChar() == 'X')) {
    getNextChar();
    do {
      IntVal = 16 * IntVal + hexDigitValue(getNextChar());
    } while (std::isxdigit(peekNextChar()));
    return Token::Integer;
  }
  // It's a Double or Decimal Integer
  ungetChar();
  do {
    IntVal = 10 * IntVal + (getNextChar() - '0');
  } while (std::isdigit(peekNextChar()));
  if (getNextChar() != '.') { // Eat the dot
    ungetChar();
    return Token::Integer;
  }
  // It's a Double, and the dot was eaten
  unsigned int Frac = 0;
  unsigned char Scale = 1;
  while (std::isdigit(peekNextChar())) {
    Frac = 10 * Frac + (getNextChar() - '0');
    Scale *= 10;
  }
  DoubleVal = IntVal + static_cast<double>(Frac) / Scale;
  return Token::Double;
}

// Assume the '/*' is eaten
bool Lexer::SkipBlockComment() {
  int CurChar;
  do {
    auto CurLoc = SrcMgr.getLoc();
    CurChar = getNextChar();
    if (CurChar == std::char_traits<char>::eof())
      return Error("unterminated /* comment");

    if (CurChar == '/' && peekNextChar() == '*') {
      Warning(CurLoc, "block comments can't be nested");
      getNextChar();
      SkipBlockComment();
    }
  } while (CurChar != '*');

  // Now next char is the one after '*'
  if (peekNextChar() == '/') {
    getNextChar();
    return false;
  } else {
    return SkipBlockComment();
  }
}

int Lexer::peekNextChar() {
  return SrcMgr.peek();
}
int Lexer::getNextChar() {
  return SrcMgr.get();
}
void Lexer::ungetChar() {
  SrcMgr.unget();
}

