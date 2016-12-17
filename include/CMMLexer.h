#ifndef CMMLEXER_H
#define CMMLEXER_H

#include <string>
#include "SourceMgr.h"

namespace cmm {

class Token {
public:
  enum TokenKind {
    Error, Eof,
    // String values.
    Identifier,
    String,
    Operator,

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
    Less, LessEqual, EqualEqual, ExclaimEqual, Greater, GreaterEqual,
    Amp, Pipe, LessLess, GreaterGreater, Caret, Tilde,
    Kw_if, Kw_else, Kw_for, Kw_while, Kw_do,
    Kw_break, Kw_continue, Kw_return,
    Kw_string, Kw_int, Kw_double, Kw_bool, Kw_void
  };

private:
  TokenKind Kind;

public:
  Token(TokenKind K) : Kind(K) {}
  TokenKind getKind() const { return Kind; }

  /// is/isNot - Predicates to check if this token is a specific kind
  bool is(TokenKind K) const { return Kind == K; }
  bool isNot(TokenKind K) const { return Kind != K; }
  //bool isOneOf(TokenKind K1, TokenKind K2) const { return is(K1) || is(K2); }
  //template <typename... Ts>
  //bool isOneOf(TokenKind K1, TokenKind K2, Ts... Ks) const {
  //  return is(K1) || isOneOf(K2, Ks...);
  //}
};

class CMMLexer {
public:
  using LocTy = SourceMgr::LocTy;
private:
  SourceMgr &SrcMgr;
  /// Information about the current token.
  Token CurTok;
  LocTy TokStartLoc;
  std::string StrVal;
  union {
    int IntVal;
    double DoubleVal;
    bool BoolVal;
  };
public:
  CMMLexer(SourceMgr &SrcMgr) :
      SrcMgr(SrcMgr), CurTok(Token::Boolean) {}
  Token Lex() {
    return CurTok = LexToken();
  }
  /// Getters
  Token getTok() const { return CurTok; }
  Token::TokenKind getKind() const { return CurTok.getKind(); }
  const std::string &getStrVal() const { return StrVal; }
  LocTy getLoc() const { return TokStartLoc; }
  int getIntVal() const { return IntVal; }
  double getDoubleVal() const { return DoubleVal; }
  bool getBoolVal() const { return BoolVal; }

  bool is(Token::TokenKind K) const { return CurTok.is(K); }
  bool isNot(Token::TokenKind K) const { return CurTok.isNot(K); }
  bool isOneOf(Token::TokenKind K1, Token::TokenKind K2) const {
    return is(K1) || is(K2);
  }
  template <typename... Ts>
  bool isOneOf(Token::TokenKind K1, Token::TokenKind K2, Ts... Ks) const {
    return is(K1) || isOneOf(K2, Ks...);
  }

  /// State change
  void seekLoc(LocTy Loc) { SrcMgr.seekLoc(Loc); }

  bool Error(LocTy ErrorLoc, const std::string &Msg);
  bool Error(const std::string &Msg) { return Error(getLoc(), Msg); }
  void Warning(LocTy ErrorLoc, const std::string &Msg);
  void Warning(const std::string &Msg) { Warning(getLoc(), Msg); }

private:
  Token LexToken();

  int peekNextChar();
  int getNextChar();
  void ungetChar();

  Token LexIdentifier();
  Token LexString();
  Token LexDigit();
  void skipLineComment();
  bool skipBlockComment();
};
}

#endif // !CMMLEXER_H
