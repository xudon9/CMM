#include "AST.h"
#include <numeric>
#include <cmath>

namespace cvm {

std::string TypeToStr(BasicType Type) {
  switch (Type) {
  case BoolType:    return "bool";
  case IntType:     return "int";
  case DoubleType:  return "double";
  case StringType:  return "string";
  case VoidType:    return "void";
  default:          return "T";
  }
}

// Constructors and member functions of BasicValue
BasicValue::BasicValue(BasicType T) : Type(T) {
  switch (Type) {
  default: break;
  case cvm::BoolType:   BoolVal = false;  break;
  case cvm::IntType:    IntVal = 0;       break;
  case cvm::DoubleType: DoubleVal = 0.0;  break;
  }
}

BasicValue::BasicValue(BasicType T, const std::list<int> &DimensionList)
    : BasicValue(T, DimensionList.cbegin(), DimensionList.cend()) {}

BasicValue::BasicValue(BasicType T,
                       std::list<int>::const_iterator I,
                       std::list<int>::const_iterator E) : BasicValue(T) {
  if (I == E)
    return;

  int N = *I++;
  ArrayPtr = std::make_shared<std::vector<BasicValue>>();
  ArrayPtr->reserve(N);
  for (size_t Idx = 0; Idx < N; ++Idx)
    ArrayPtr->emplace_back(T, I, E);
}

BasicValue::BasicValue(BasicType T,
                       std::shared_ptr<std::vector<BasicValue>> P)
    : Type(T), ArrayPtr(P) {}

int BasicValue::toInt() const {
  switch (Type) {
  default:          return 0;
  case IntType:     return IntVal;
  case DoubleType:  return static_cast<int>(DoubleVal);
  case BoolType:    return BoolVal;
  case StringType:  return std::stoi(StrVal);
  }
}

double BasicValue::toDouble() const {
  switch (Type) {
  default:          return 0.0;
  case IntType:     return static_cast<double>(IntVal);
  case DoubleType:  return DoubleVal;
  case BoolType:    return static_cast<double>(BoolVal);
  case StringType:  return std::stod(StrVal.c_str());
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

std::string BasicValue::toString(decltype(ArrayPtr) P) const {
  if (isArray()) {
    if (P == ArrayPtr)
      return "[...]";

    if (P == nullptr)
      P = ArrayPtr;

    return "[" +
        std::accumulate(ArrayPtr->begin() + 1,
                        ArrayPtr->end(),
                        ArrayPtr->front().toString(P),
                        [P](std::string S, BasicValue &X) {
                          return S + ", " + X.toString(P);
                        }) + "]";
  }

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
    return IntVal == RHS.IntVal;
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
  // L >= R  <===>  not L < R;
  return !(*this < RHS);
}
}

/******************************************************************************/

namespace cmm {

// bool BinaryOperatorAST::isLogical() const {
//   return getOpKind() == LogicalAnd || getOpKind() == LogicalOr;
// }

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

std::unique_ptr<StatementAST>
IfStatementAST::create(std::unique_ptr<ExpressionAST> Condition,
                       std::unique_ptr<StatementAST> StatementThen,
                       std::unique_ptr<StatementAST> StatementElse) {
  if (!Condition->isConstant()) {
    auto *IfStmt = new IfStatementAST(std::move(Condition),
                                      std::move(StatementThen),
                                      std::move(StatementElse));
    return std::unique_ptr<StatementAST>(IfStmt);
  }

  if (Condition->asBool())
    return StatementThen;

  if (StatementElse)
    return StatementElse;

  // The IfStatement has no 'Else', return a null.
  return nullptr;
}

std::unique_ptr<StatementAST>
WhileStatementAST::create(std::unique_ptr<ExpressionAST> Condition,
                          std::unique_ptr<StatementAST> Statement) {
  if (!Condition->isConstant()) {
    auto *WhileStmt = new WhileStatementAST(std::move(Condition),
                                            std::move(Statement));
    return std::unique_ptr<StatementAST>(WhileStmt);
  }

  // Forever
  if (Condition->asBool()) {
    auto *WhileStmt = new WhileStatementAST(nullptr,std::move(Statement));
    return std::unique_ptr<StatementAST>(WhileStmt);
  }

  // Never
  return nullptr;
}


std::unique_ptr<StatementAST>
ForStatementAST::create(std::unique_ptr<ExpressionAST> Init,
                        std::unique_ptr<ExpressionAST> Condition,
                        std::unique_ptr<ExpressionAST> Post,
                        std::unique_ptr<StatementAST> Statement) {

  if (Condition == nullptr || !Condition->isConstant()) {
    auto *ForStmt = new ForStatementAST(std::move(Init), std::move(Condition),
                                        std::move(Post), std::move(Statement));
    return std::unique_ptr<StatementAST>(ForStmt);
  }

  // Forever
  if (Condition->asBool()) {
    auto *ForStmt = new ForStatementAST(std::move(Init), nullptr,
                                        std::move(Post), std::move(Statement));
    return std::unique_ptr<StatementAST>(ForStmt);
  }

  // Never
  if (Init) {
    auto *ExprStmt = new ExprStatementAST(std::move(Init));
    return std::unique_ptr<StatementAST>(ExprStmt);
  }
  return nullptr;
}


int ExpressionAST::asInt() const {
  switch (getKind()) {
  default:
    return 0;
  case IntExpression:
    return as_cptr<IntAST>()->getValue();
  case DoubleExpression:
    return static_cast<int>(as_cptr<DoubleAST>()->getValue());
  case StringExpression:
    return std::stoi(as_cptr<StringAST>()->getValue());
  case BoolExpression:
    return static_cast<int>(as_cptr<BoolAST>()->getValue());
  }
}

bool ExpressionAST::asBool() const {
  switch (getKind()) {
  default:
    return false;
  case IntExpression:
    return as_cptr<IntAST>()->getValue() != 0;
  case DoubleExpression:
    return as_cptr<DoubleAST>()->getValue() != 0.0;
  case StringExpression:
    return !as_cptr<StringAST>()->getValue().empty();
  case BoolExpression:
    return as_cptr<BoolAST>()->getValue();
  }
}

double ExpressionAST::asDouble() const {
  switch (getKind()) {
  default:
    return 0.0;
  case IntExpression:
    return static_cast<double>(as_cptr<IntAST>()->getValue());
  case DoubleExpression:
    return as_cptr<DoubleAST>()->getValue();
  case StringExpression:
    return std::stod(as_cptr<StringAST>()->getValue());
  case BoolExpression:
    return static_cast<double>(as_cptr<BoolAST>()->getValue());
  }
}

std::string ExpressionAST::asString() const {
  switch (getKind()) {
  default:
    return "";
  case IntExpression:
    return std::to_string(as_cptr<IntAST>()->getValue());
  case DoubleExpression:
    return std::to_string(as_cptr<DoubleAST>()->getValue());
  case StringExpression:
    return as_cptr<StringAST>()->getValue();
  case BoolExpression:
    return as_cptr<BoolAST>()->getValue() ? "true" : "false";
  }
}

std::unique_ptr<ExpressionAST>
BinaryOperatorAST::tryFoldBinOp(Token::TokenKind TokenKind,
                                std::unique_ptr<ExpressionAST> LHS,
                                std::unique_ptr<ExpressionAST> RHS) {
  if (!LHS->isConstant() || !RHS->isConstant()) {
    return BinaryOperatorAST::create(TokenKind, std::move(LHS), std::move(RHS));
  }

  if (TokenKind == Token::Plus && (LHS->isString() || RHS->isString())) {
    return std::unique_ptr<ExpressionAST>(new StringAST(LHS->asString() +
        RHS->asString()));
  }

  switch (TokenKind) {
  default:
    break;
  case Token::Plus:
  case Token::Slash:
  case Token::Minus:
  case Token::Star:
  case Token::Percent:
    return tryFoldBinOpArith(TokenKind, std::move(LHS), std::move(RHS));
  case Token::AmpAmp:
  case Token::PipePipe:
    return tryFoldBinOpLogic(TokenKind, std::move(LHS), std::move(RHS));
  case Token::Less:
  case Token::LessEqual:
  case Token::EqualEqual:
  case Token::ExclaimEqual:
  case Token::GreaterEqual:
  case Token::Greater:
    return tryFoldBinOpRelation(TokenKind, std::move(LHS), std::move(RHS));
  case Token::Amp:
  case Token::Pipe:
  case Token::Caret:
  case Token::LessLess:
  case Token::GreaterGreater:
    return tryFoldBinOpBitwise(TokenKind, std::move(LHS), std::move(RHS));
  case Token::Equal:
    break;
  }

  return create(TokenKind, std::move(LHS), std::move(RHS));
}

std::unique_ptr<ExpressionAST>
BinaryOperatorAST::tryFoldBinOpArith(Token::TokenKind TokenKind,
                                     std::unique_ptr<ExpressionAST> LHS,
                                     std::unique_ptr<ExpressionAST> RHS) {

  if (LHS->isInt() && RHS->isInt()) {
    int Value;
    int L = LHS->as_cptr<IntAST>()->getValue();
    int R = RHS->as_cptr<IntAST>()->getValue();

    switch (TokenKind) {
    default:              Value = 0;      break;
    case Token::Plus:     Value = L + R;  break;
    case Token::Minus:    Value = L - R;  break;
    case Token::Star:     Value = L * R;  break;
    case Token::Slash:
      Value = R == 0 ? std::numeric_limits<int>::max() : L / R;
      break;
    case Token::Percent:
      Value = R == 0 ? 0 : L % R;
      break;
    }
    return std::unique_ptr<ExpressionAST>(new IntAST(Value));
  }

  if (LHS->isNumeric() || RHS->isNumeric()) {
    double Value;
    double L = LHS->asDouble();
    double R = RHS->asDouble();

    switch (TokenKind) {
    default:              Value = 0.0;    break;
    case Token::Plus:     Value = L + R;  break;
    case Token::Minus:    Value = L - R;  break;
    case Token::Star:     Value = L * R;  break;
    case Token::Slash:    Value = L / R;  break;
    case Token::Percent:  Value = std::fmod(L, R); break;
    }
    return std::unique_ptr<ExpressionAST>(new DoubleAST(Value));
  }
  return BinaryOperatorAST::create(TokenKind, std::move(LHS), std::move(RHS));
}


/// \brief Fold two expression for logicalAnd and logicalOr
/// LHS and RHS should be constantExpr
std::unique_ptr<ExpressionAST>
BinaryOperatorAST::tryFoldBinOpLogic(Token::TokenKind TokenKind,
                                     std::unique_ptr<ExpressionAST> LHS,
                                     std::unique_ptr<ExpressionAST> RHS) {
  bool Value;

  switch (TokenKind) {
  default:                Value = false; break;
  case Token::AmpAmp:     Value = LHS->asBool() && RHS->asBool(); break;
  case Token::PipePipe:   Value = LHS->asBool() || RHS->asBool(); break;
  }

  return std::unique_ptr<ExpressionAST>(new BoolAST(Value));
}

std::unique_ptr<ExpressionAST>
BinaryOperatorAST::tryFoldBinOpRelation(Token::TokenKind TokenKind,
                                        std::unique_ptr<ExpressionAST> LHS,
                                        std::unique_ptr<ExpressionAST> RHS) {

#define CASE(TOKEN_KIND, OPERATOR)                                             \
  case Token::TOKEN_KIND:                                                      \
    return std::unique_ptr<BoolAST>(new BoolAST(L OPERATOR R))

#define TRY_COMPARE(type, Type)                                                \
  do {                                                                         \
    if (LHS->is##Type() && RHS->is##Type()) {                                  \
      type L = LHS->as_cptr<Type##AST>()->getValue();                          \
      type R = RHS->as_cptr<Type##AST>()->getValue();                          \
      switch (TokenKind) {                                                     \
      default: break;                                                          \
      CASE(Less, <);                                                           \
      CASE(LessEqual, <=);                                                     \
      CASE(EqualEqual, ==);                                                    \
      CASE(ExclaimEqual, !=);                                                  \
      CASE(GreaterEqual, >=);                                                  \
      CASE(Greater, >);                                                        \
      }                                                                        \
    }                                                                          \
  } while (0)

  TRY_COMPARE(int, Int);
  TRY_COMPARE(std::string, String);
  TRY_COMPARE(double, Double);
  TRY_COMPARE(bool, Bool);

#undef TRY_COMPARE
#undef CASE

  // TODO: Not very elegant, but this is ok.
  return BinaryOperatorAST::create(TokenKind, std::move(LHS), std::move(RHS));
}

std::unique_ptr<ExpressionAST>
BinaryOperatorAST::tryFoldBinOpBitwise(Token::TokenKind TokenKind,
                                       std::unique_ptr<ExpressionAST> LHS,
                                       std::unique_ptr<ExpressionAST> RHS) {
  if (LHS->isInt() && RHS->isInt()) {
    int L = LHS->as_cptr<IntAST>()->getValue();
    int R = RHS->as_cptr<IntAST>()->getValue();

    switch (TokenKind) {
    default:break;
    case Token::LessLess:
      return std::unique_ptr<IntAST>(new IntAST(L << R));
    case Token::GreaterGreater:
      return std::unique_ptr<IntAST>(new IntAST(L >> R));
    case Token::Amp:
      return std::unique_ptr<IntAST>(new IntAST(L & R));
    case Token::Pipe:
      return std::unique_ptr<IntAST>(new IntAST(L | R));
    case Token::Caret:
      return std::unique_ptr<IntAST>(new IntAST(L ^ R));
    }
  }
  return BinaryOperatorAST::create(TokenKind, std::move(LHS), std::move(RHS));
}



std::unique_ptr<ExpressionAST>
UnaryOperatorAST::tryFoldUnaryOp(OperatorKind OpKind,
                                 std::unique_ptr<ExpressionAST> Operand) {
  if (Operand->isConstant()) {
    switch (OpKind) {
    default:
      break;
    case Plus:
      return Operand;
    case Minus:
      if (Operand->isInt()) {
        return std::unique_ptr<ExpressionAST>(new IntAST(
            -Operand->as_cptr<IntAST>()->getValue()));
      }
      if (Operand->isDouble()) {
        return std::unique_ptr<ExpressionAST>(new DoubleAST(
            -Operand->as_cptr<DoubleAST>()->getValue()));
      }
      break;
    case BitwiseNot:
      if (Operand->isInt()) {
        return std::unique_ptr<ExpressionAST>(new IntAST(
            ~Operand->as_cptr<IntAST>()->getValue()));
      }
      break;
    case LogicalNot:
      return std::unique_ptr<ExpressionAST>(new BoolAST(!Operand->asBool()));
    }
  }

  return std::unique_ptr<ExpressionAST>(
      new UnaryOperatorAST(OpKind, std::move(Operand)));
}

void IntAST::dump(const std::string &prefix) const {
  std::cout << "(int)" << getValue() << "\n";
}

void BoolAST::dump(const std::string &prefix) const {
  std::cout << "(bool)" << (getValue() ? "true" : "false") << "\n";
}

void DoubleAST::dump(const std::string &prefix) const {
  std::cout << "(double)" << getValue() << "\n";
}

void StringAST::dump(const std::string &prefix) const {
  std::cout << "(string)\"";
  for (char C : getValue()) {
  switch (C) {
  default:    std::cout << C; break;
  case '\a': std::cout << "\\a"; break;
  case '\b': std::cout << "\\b"; break;
  case '\f': std::cout << "\\f"; break;
  case '\n': std::cout << "\\n"; break;
  case '\r': std::cout << "\\r"; break;
  case '\t': std::cout << "\\t"; break;
  case '\v': std::cout << "\\v"; break;
  case '\?': std::cout << "\\?"; break;
  case '\0': std::cout << "\\0"; break;
  case '\\': std::cout << "\\\\"; break;
  case '\'': std::cout << "\\\'"; break;
  case '\"': std::cout << "\\\""; break;
  }
  }
  std::cout << "\"\n";
}

void IdentifierAST::dump(const std::string &prefix) const {
  std::cout << "(id)" << Name << "\n";
}

void InfixOpExprAST::dump(const std::string &prefix) const {
  std::cout << getSymbol() << "\n";

  std::cout << prefix << "|---";
  LHS->dump(prefix + "|   ");

  std::cout << prefix << "`---";
  RHS->dump(prefix + "    ");
}

void FunctionCallAST::dump(const std::string &prefix) const {
  std::cout << (isDynamicBound() ? "(DynCall)" : "(Call)") << "\n";

  for (const auto &Arg : getArguments()) {
    if (Arg != Arguments.back()) {
      std::cout << prefix << "|---";
      Arg->dump(prefix + "|   ");
    } else {
      std::cout << prefix << "`---";
      Arg->dump(prefix + "    ");
    }
  }
}

void BinaryOperatorAST::dump(const std::string &prefix) const
{
  std::string OperatorSymbol;

  switch (getOpKind()) {
  default: break;
  case Add:           OperatorSymbol = "Add"; break;
  case Minus:         OperatorSymbol = "Sub"; break;
  case Multiply:      OperatorSymbol = "Mul"; break;
  case Division:      OperatorSymbol = "Div"; break;
  case Modulo:        OperatorSymbol = "Mod"; break;
  case LogicalAnd:    OperatorSymbol = "And"; break;
  case LogicalOr:     OperatorSymbol = "Or"; break;
  case Less:          OperatorSymbol = "Less"; break;
  case LessEqual:     OperatorSymbol = "LessEq"; break;
  case Equal:         OperatorSymbol = "Equal"; break;
  case NotEqual:      OperatorSymbol = "NotEq"; break;
  case Greater:       OperatorSymbol = "Greater"; break;
  case BitwiseAnd:    OperatorSymbol = "BitAnd"; break;
  case BitwiseOr:     OperatorSymbol = "BitOr"; break;
  case BitwiseXor:    OperatorSymbol = "Xor"; break;
  case LeftShift:     OperatorSymbol = "LShift"; break;
  case RightShift:    OperatorSymbol = "RShift"; break;
  case Assign:        OperatorSymbol = "Assign"; break;
  case GreaterEqual:  OperatorSymbol = "GreaterEq"; break;
  case Index:         OperatorSymbol = "At"; break;
  }

  std::cout << OperatorSymbol << "\n";

  std::cout << prefix << "|---";
  LHS->dump(prefix + "|   ");

  std::cout << prefix << "`---";
  RHS->dump(prefix + "    ");
}

void UnaryOperatorAST::dump(const std::string &prefix) const {
  std::string OperatorSymbol;

  switch (OpKind) {
  default: break;
  case Plus:       OperatorSymbol = "Positive"; break;
  case Minus:      OperatorSymbol = "Negative"; break;
  case LogicalNot: OperatorSymbol = "Not";      break;
  case BitwiseNot: OperatorSymbol = "BitNot";   break;
  }

  std::cout << "(" << OperatorSymbol << ")\n";
  std::cout << prefix << "`---";
  Operand->dump(prefix + "    ");
}

void DeclarationAST::dump(const std::string &prefix) const {
  std::cout << cvm::TypeToStr(Type) << " " << Name << "\n";

  if (Initializer) {
    std::cout << prefix << " `==";
    Initializer->dump(prefix + "    ");
  } else if (isArray()) {
    for (const auto &E : ElementCountList) {
      if (E != ElementCountList.back()) {
        std::cout << prefix << "|-[]";
        E->dump(prefix + "|   ");
      } else {
        std::cout << prefix << "`-[]";
        E->dump(prefix + "    ");
      }

    }
  }
}

void DeclarationListAST::dump(const std::string &prefix) const {
  std::cout << "(Decl)" << cvm::TypeToStr(Type) << "\n";

  for (const auto &Declare : DeclarationList) {
    if (Declare != DeclarationList.back()) {
      std::cout << prefix << "|---";
      Declare->dump(prefix + "|   ");
    } else {
      std::cout << prefix << "`---";
      Declare->dump(prefix + "    ");
    }
  }
}

void BlockAST::dump(const std::string &prefix) const {
  std::cout << "(Block)\n";

  for (const auto &Statement : StatementList) {
    if (Statement != StatementList.back()) {
      std::cout << prefix << "|---";
      Statement->dump(prefix + "|   ");
    } else {
      std::cout << prefix << "`---";
      Statement->dump(prefix + "    ");
    }
  }
}

void ExprStatementAST::dump(const std::string &prefix) const {
  std::cout << "(ExprStmt)\n";

  std::cout << prefix << "`---";
  Expression->dump(prefix + "    ");
}

void InfixOpDefinitionAST::dump() const {
  std::cout << "infix " << getLHSName() << " " << getSymbol()
            << " " << getRHSName() << " = ";
  if (Statement) {
    std::cout << "\n";
    Statement->dump();
  } else {
    std::cout << "(emptyStmt)\n";
  }
}

void FunctionDefinitionAST::dump() const {
  std::cout << "Function: " << cvm::TypeToStr(getType()) << " " << Name << "(";

  for (const auto &P : getParameterList()) {
    std::cout << P.toString() << (&P == &ParameterList.back() ? "" : ", ");
  }

  std::cout << ") => ";

  if (Statement) {
    std::cout << "\n";
    Statement->dump();
  } else {
    std::cout << "(empty)\n";
  }
}

void IfStatementAST::dump(const std::string &prefix) const {
  std::cout << "if\n";
  std::cout << prefix << "|---";

  Condition->dump(prefix + "|   ");

  std::cout << prefix << "|---";
  if (StatementThen)
    StatementThen->dump(prefix + "|    ");
  else
    std::cout << "(emptyThen)\n";

  std::cout << prefix << "`---";
  if (StatementElse)
    StatementElse->dump(prefix + "    ");
  else
    std::cout << "(emptyElse)\n";
}

void WhileStatementAST::dump(const std::string &prefix) const {
  std::cout << "while\n";
  std::cout << prefix << "|---";

  if (Condition)
    Condition->dump(prefix + "|   ");
  else
    std::cout << "(forever)\n";

  std::cout << prefix << "`---";
  if (Statement)
    Statement->dump(prefix + "    ");
  else
    std::cout << "(empty)";
}

void ForStatementAST::dump(const std::string &prefix) const {
  std::cout << "for\n";

  std::cout << prefix << "|--+";
  if (Init)
    Init->dump(prefix + "|   ");
  else
    std::cout << "(nullInit)\n";

  std::cout << prefix << "|--+";
  if (Condition)
    Condition->dump(prefix + "|   ");
  else
    std::cout << "(forever)\n";

  std::cout << prefix << "|--+";
  if (Post)
    Post->dump(prefix + "|   ");
  else
    std::cout << "(nullPost)\n";

  std::cout << prefix << "`---";
  if (Statement)
    Statement->dump(prefix + "    ");
  else
    std::cout << "(empty)\n";
}

void ReturnStatementAST::dump(const std::string &prefix) const {
  std::cout << "return\n";
  if (!ReturnValue)
    return;
  std::cout << prefix << "`---";
  ReturnValue->dump(prefix + "    ");
}

void BreakStatementAST::dump(const std::string &/*prefix*/) const {
  std::cout << "break\n";
}

void ContinueStatementAST::dump(const std::string &/*prefix*/) const {
  std::cout << "continue\n";
}

}
