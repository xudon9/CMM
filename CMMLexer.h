#ifndef CMMLEXER_H
#define CMMLEXER_H

#include <iostream>
#include <string>

void show(const std::string &s);

namespace cmm {

class Token {
public:
  enum TokenKind {
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

    // Flow Control keywords
    If, Else, For, While, Do, Break, Continue
  };

private:
  TokenKind Kind;
};

}

#endif
