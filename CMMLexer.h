#ifndef CMMLEXER_H
#define CMMLEXER_H

#include <string>
#include <cstdint>

void show(const std::string &s);

namespace cmm {

class Token {
public:
  enum TokenKind : std::int8_t {
    // String values.
    Identifier,
    String,

    // Integer values.
    Int,

    // Real values.
    Double,

    // Boolean value.
    Bool,

    // No-values.
    LParen, RParen, LBrac, RBrac, LCurly, RCurly,
    Plus, Minus, Star, Slash, Semicolon, Comma,
    Equal, Percent, Exclaim, AmpAmp, PipePipe,
    Less, LessEqual, EqualEqual, Greater, GreaterEqual, ExclaimEqual,
    Amp, Pipe, LessLess, GreaterGreater, Caret, Tilde,

    // Flow control keywords
    If, Else, For, While, Do, Break, Continue
  };

private:
  TokenKind Kind;

  union {
    std::int64_t  IntValue;
    double        DoubleValue;
    bool          BoolValue;
    std::string   StringValue;
  };

public:

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

  ~Token() {
    if (isOneOf(String, Identifier))
      StringValue.~basic_string();
  }
};

}

#endif
