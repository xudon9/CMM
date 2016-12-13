#include "AST.h"

using namespace cmm;

const char *cvm::TypeToStr(BasicType Type) {
  switch (Type) {
  case BoolType:    return "bool";
  case IntType:     return "int";
  case DoubleType:  return "double";
  case StringType:  return "string";
  case VoidType:    return "void";
  default:          return "UnkownType";
  }
}

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
