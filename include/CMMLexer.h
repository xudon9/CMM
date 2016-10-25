#ifndef CMMLEXER_H
#define CMMLEXER_H

#include <string>
#include <fstream>

void show(const std::string &s);

namespace cmm {

class Token {
public:
  enum TokenKind {
    Error, Eof,
    // String values.
    Identifier,
    String,

    // Integer values.
    Integer,

    // Double values.
    Double,

    // Boolean value.
    Boolean,

    // No-values.
    LParen, RParen, LBrac, RBrac, LCurly, RCurly,
    Plus, Minus, Star, Slash, Semicolon, Comma,
    Equal, Percent, Exclaim, AmpAmp, PipePipe,
    Less, LessEqual, EqualEqual, Greater, GreaterEqual, ExclaimEqual,
    Amp, Pipe, LessLess, GreaterGreater, Caret, Tilde,
    Kw_if, Kw_else, Kw_for, Kw_while, Kw_do, Kw_break, Kw_continue,
    Kw_int, Kw_double, Kw_bool
  };

private:
  TokenKind Kind;

public:
  Token(TokenKind K) : Kind(K) {}
  TokenKind getKind() const { return Kind; }
  /// is/isNot - Predicates to check if this token is a specific kind
  bool is(TokenKind K) const { return Kind == K; }
  bool isNot(TokenKind K) const { return Kind != K; }
  bool isOneOf(TokenKind K1, TokenKind K2) const {
    return is(K1) || is(K2);
  }
  template <typename... Ts>
  bool isOneOf(TokenKind K1, TokenKind K2, Ts... Ks) const {
    return is(K1) || isOneOf(K2, Ks...);
  }
};

class Lexer {
private:
  std::ifstream SourceStream;
  // Information about the current token.
  struct LocTy { unsigned int line, col; };
  Token CurTok;
  std::string StrVal;
  union {
    int IntVal;
    double DoubleVal;
    bool BoolVal;
  };
public:
  Lexer(const std::string &SourcePath) :
      SourceStream(SourcePath), CurTok(Token::Boolean) {}
  Token Lex() {
    return CurTok = LexToken();
  }
  Token::TokenKind getKind() const { return CurTok.getKind(); }
  const std::string &getStrVal() const { return StrVal; }
  int getIntVal() const { return IntVal; }
  double getDoubleVal() const { return DoubleVal; }
  bool getBoolVal() const { return BoolVal; }

  bool Error(LocTy ErrorLoc, const std::string &Msg);
  void Warning(LocTy ErrorLoc, const std::string &Msg);

private:
  Token LexToken();

  int peekNextChar();
  int getNextChar();
  void ungetChar();

  Token LexIdentifier();
  Token LexString();
  Token LexDigit();
  void SkipLineComment();
  bool SkipBlockComment();
};
}

#endif
