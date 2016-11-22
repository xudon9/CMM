#include "CMMParser.h"
#include <cassert>

using namespace cmm;

std::unique_ptr<ExpressionAST> BinaryOperatorAST::create(
    Token::TokenKind TokenKind,
    std::unique_ptr<ExpressionAST> LHS, std::unique_ptr<ExpressionAST> RHS) {

  BinaryOperatorAST::OperatorKind OpKind;
  switch (TokenKind) {
  default: return nullptr;
  case Token::Plus:           OpKind = BinaryOperatorAST::Add; break;
  case Token::Minus:          OpKind = BinaryOperatorAST::Minus; break;
  case Token::Star:           OpKind = BinaryOperatorAST::Multiply; break;
  case Token::Slash:          OpKind = BinaryOperatorAST::Division; break;
  case Token::Percent:        OpKind = BinaryOperatorAST::Modulo; break;
  case Token::AmpAmp:         OpKind = BinaryOperatorAST::LogicalAnd; break;
  case Token::PipePipe:       OpKind = BinaryOperatorAST::LogicalOr; break;
  case Token::Less:           OpKind = BinaryOperatorAST::Less; break;
  case Token::LessEqual:      OpKind = BinaryOperatorAST::LessEqual; break;
  case Token::EqualEqual:     OpKind = BinaryOperatorAST::Equal; break;
  case Token::GreaterEqual:   OpKind = BinaryOperatorAST::GreaterEqual; break;
  case Token::Greater:        OpKind = BinaryOperatorAST::Greater; break;
  case Token::Amp:            OpKind = BinaryOperatorAST::BitwiseAnd; break;
  case Token::Pipe:           OpKind = BinaryOperatorAST::BitwiseOr; break;
  case Token::Caret:          OpKind = BinaryOperatorAST::BitwiseXor; break;
  case Token::LessLess:       OpKind = BinaryOperatorAST::LeftShift; break;
  case Token::GreaterGreater: OpKind = BinaryOperatorAST::RightShift; break;
  case Token::Equal:          OpKind = BinaryOperatorAST::Assign; break;
  }

  return std::unique_ptr<ExpressionAST>(new BinaryOperatorAST(OpKind,
                                                              std::move(LHS),
                                                              std::move(RHS)));
}

bool CMMParser::Parse() {
  Lex();
#if 0
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
#else
  std::unique_ptr<ExpressionAST> Expr;
  parseExpression(Expr);
  //TopLevelStatements.emplace_back(std::move(Expr));
  //TopLevelStatements.front()->dump();
  Expr->dump();
#endif
  return false;
}

bool CMMParser::parseExpression(std::unique_ptr<ExpressionAST> &Res) {
  return parsePrimaryExpression(Res) || parseBinOpRHS(1, Res);
}

int8_t CMMParser::getBinOpPrecedence(Token::TokenKind Kind) {
  //TODO
  return BinOpPrecedence[Kind];
}

/// \brief Parse a paren expression and return it.
/// parenExpr ::= (expr)
bool CMMParser::parseParenExpression(std::unique_ptr<ExpressionAST> &Res) {
  Lex(); // eat the '('.
  if (parseExpression(Res))
    return true;
  if (Lexer.isNot(Token::RParen))
     return Error("expected ')' in parentheses expression");
  Lex(); // eat the ')'.
  return false;
}

/// \brief Parse a primary expression and return it.
///  primaryexpr ::= parenExpr
///  primaryexpr ::= identifierExpr
///  primaryexpr ::= constantExpr
///  primaryexpr ::= ~,+,-,! primaryexpr
bool CMMParser::parsePrimaryExpression(std::unique_ptr<ExpressionAST> &Res) {
  UnaryOperatorAST::OperatorKind UnaryOpKind;
  std::unique_ptr<ExpressionAST> Operand;

  switch (getKind()) {
  default:
    return Error("unknown token in expression");
  case Token::LParen:
    return parseParenExpression(Res);
  case Token::Identifier:
    return parseIdentifierExpression(Res);
  case Token::Integer:
  case Token::Double:
  case Token::String:
  case Token::Boolean:
    return parseConstantExpression(Res);
  case Token::Plus:     UnaryOpKind = UnaryOperatorAST::Plus; break;
  case Token::Minus:    UnaryOpKind = UnaryOperatorAST::Minus; break;
  case Token::Tilde:    UnaryOpKind = UnaryOperatorAST::BitwiseNot; break;
  case Token::Exclaim:  UnaryOpKind = UnaryOperatorAST::LogicalNot; break;
  }

  Lex(); // Eat the operator: +,-,~,!
  if (parsePrimaryExpression(Operand))
    return true;
  Res.reset(new UnaryOperatorAST(UnaryOpKind, std::move(Operand)));
  return false;
}

bool CMMParser::parseBinOpRHS(int8_t ExprPrec,
                              std::unique_ptr<ExpressionAST> &Res) {
  // If this is a binOp, find its precedence.
  for (;;) {
    int8_t TokPrec = getBinOpPrecedence(getKind());

    // If the next token is lower precedence than we are allowed to eat,
    // return successfully with what we ate already.
    if (TokPrec < ExprPrec)
      return false;

    Token::TokenKind TokenKind = getKind();
    Lex();

    // Eat the next primary expression.
    std::unique_ptr<ExpressionAST> RHS;
    if (parsePrimaryExpression(RHS))
      return true;

    // If BinOp binds less tightly with RHS than the operator after RHS, let
    // the pending operator take RHS as its LHS.
    int8_t NextPrec = getBinOpPrecedence(getKind());
    if (TokPrec < NextPrec && parseBinOpRHS(++TokPrec, RHS))
      return true;
    
    // Merge LHS and RHS according to operator.
    Res = BinaryOperatorAST::create(TokenKind, std::move(Res), std::move(RHS));
  }
}

bool CMMParser::parseIdentifierExpression(std::unique_ptr<ExpressionAST> &Res) {
  // TODO: handle function call in this function
  assert(Lexer.is(Token::Identifier));
  Res.reset(new IdentifierAST(Lexer.getStrVal()));
  Lex(); // eat the identifier
  return false;
}


bool CMMParser::parseConstantExpression(std::unique_ptr<ExpressionAST> &Res) {
  switch (getKind()) {
  default:  return Error("unknown token in literal constant expression");
  case Token::Integer:  Res.reset(new IntAST(Lexer.getIntVal())); break;
  case Token::Double:   Res.reset(new DoubleAST(Lexer.getDoubleVal())); break;
  case Token::Boolean:  Res.reset(new BoolAST(Lexer.getBoolVal())); break;
  case Token::String:   Res.reset(new StringAST(Lexer.getStrVal())); break;
  }
  Lex(); // eat the string,bool,int,double.
  return false;
}
