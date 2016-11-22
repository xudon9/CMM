#include "CMMParser.h"

using namespace cmm;

std::unique_ptr<ExpressionAST> BinaryOperatorAST::create(
  Token::TokenKind TokenKind,
  std::unique_ptr<ExpressionAST> LHS,
  std::unique_ptr<ExpressionAST> RHS) {

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

  return std::unique_ptr<ExpressionAST>(
    new BinaryOperatorAST(OpKind, std::move(LHS), std::move(RHS)));
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
  ParseExpression();
#endif
  return false;
} 


int8_t CMMParser::getBinOpPrecedence(Token::TokenKind Kind) {
  return 0;
}

/// \brief Parse a primary expression and return it.
///  primaryexpr ::= parenExpr
///  primaryexpr ::= identifierExpr
///  primaryexpr ::= constantExpr
///  primaryexpr ::= ~,+,-,! primaryexpr
//bool AsmParser::parsePrimaryExpr(const MCExpr *&Res, SMLoc &EndLoc) {
bool CMMParser::ParsePrimaryExpression(std::unique_ptr<ExpressionAST> &Res) {
  UnaryOperatorAST::OperatorKind UnaryOpKind;
  std::unique_ptr<ExpressionAST> Operand;
  switch (getKind()) {
  default:
    return Error("unknown token in expression");
  case Token::LParen:
    return ParseParenExpression(Res);
  case Token::Identifier:
    return ParseIdentifierExpression(Res);
  case Token::Integer:
  case Token::Double:
  case Token::String:
  case Token::Boolean:
    return ParseConstantExpression(Res);
  case Token::Plus:
    UnaryOpKind = UnaryOperatorAST::Plus;
  case Token::Minus:
    UnaryOpKind = UnaryOperatorAST::Minus;
  case Token::Tilde:
    UnaryOpKind = UnaryOperatorAST::BitwiseNot;
  case Token::Exclaim:
    UnaryOpKind = UnaryOperatorAST::LogicalNot;

    Lex(); // Eat the operator: +,-,~,!
    if (ParsePrimaryExpression(Operand))
      return true;
    Res.reset(new UnaryOperatorAST(UnaryOpKind, std::move(Operand)));
    return false;
  }
}

bool CMMParser::ParseBinOpRHS(int8_t ExprPrec,
                              std::unique_ptr<ExpressionAST> &Res) {
  // If this is a binop, find its precedence.
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
    if (ParsePrimaryExpression(RHS))
      return true;

    // If BinOp binds less tightly with RHS than the operator after RHS, let
    // the pending operator take RHS as its LHS.
    int8_t NextPrec = getBinOpPrecedence(getKind());
    if (TokPrec < NextPrec && ParseBinOpRHS(TokPrec + 1, RHS))
      return true;
    
    // Merge LHS and RHS according to operator.
    Res = BinaryOperatorAST::create(TokenKind, std::move(Res), std::move(RHS));
  }
}