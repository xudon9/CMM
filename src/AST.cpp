#include "AST.h"
#include <string>

namespace cvm {
int BasicValue::toInt() const {
  switch (Type) {
  default:          return 0;
  case IntType:     return IntVal;
  case DoubleType:  return static_cast<int>(DoubleVal);
  case BoolType:    return static_cast<int>(BoolVal);
  case StringType:  return std::atoi(StrVal.c_str());
  }
}

double BasicValue::toDouble() const {
  switch (Type) {
  default:          return 0.0;
  case IntType:     return static_cast<double>(IntVal);
  case DoubleType:  return DoubleVal;
  case BoolType:    return static_cast<double>(BoolVal);
  case StringType:  return std::atof(StrVal.c_str());
  }
}

bool BasicValue::toBool() const {
  switch (Type) {
  default:          return false;
  case IntType:     return IntVal != 0;
  case DoubleType:  return DoubleVal != 0.0;
  case BoolType:    return BoolVal;
  case StringType:  return !StrVal.empty();
  }
}

std::string BasicValue::toString() const {
  switch (Type) {
  default:          return "";
  case IntType:     return std::to_string(IntVal);
  case DoubleType:  return std::to_string(DoubleVal);
  case BoolType:    return BoolVal ? "true" : "false";
  case StringType:  return StrVal;
  }
}

bool BasicValue::operator<(const BasicValue &RHS) const {
  if (Type != RHS.Type)
    return false;
  switch (Type) {
    case BoolType:
      return !BoolVal && RHS.BoolVal;
    case IntType:
      return IntVal < RHS.IntVal;
    case DoubleType:
      return DoubleVal < RHS.DoubleVal;
    case StringType:
      return StrVal < RHS.StrVal;
    default:
      return false;
  }
}

bool BasicValue::operator<=(const BasicValue &RHS) const {
  return *this < RHS || *this == RHS;
}

bool BasicValue::operator==(const BasicValue &RHS) const {
  if (Type != RHS.Type)
    return false;
  switch (Type) {
    case BoolType:
      return BoolVal == RHS.BoolVal;
    case IntType:
      return IntVal == RHS.BoolVal;
    case DoubleType:
      return DoubleVal == RHS.DoubleVal;
    case StringType:
      return StrVal == RHS.StrVal;
    case VoidType:
      return true;
    default:
      return false;
  }
}

bool BasicValue::operator!=(const BasicValue &RHS) const {
  return !(*this == RHS);
}

bool BasicValue::operator>(const BasicValue &RHS) const {
  return RHS < *this;
}

bool BasicValue::operator>=(const BasicValue &RHS) const {
  // L >= R  <==>  not L < R;
  return !(*this < RHS);
}
}

using namespace cmm;

std::string cvm::TypeToStr(BasicType Type) {
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
  case Token::ExclaimEqual:   OpKind = BinaryOperatorAST::NotEqual; break;
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
