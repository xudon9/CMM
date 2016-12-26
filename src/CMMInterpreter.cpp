#include "CMMInterpreter.h"
#include "NativeFunctions.h"
#include <cmath>

using namespace cmm;

int CMMInterpreter::interpret(int Argc, char *Argv[]) {
  // First run top level statements.
  for (auto &Stmt : TopLevelBlock.getStatementList()) {
    ExecutionResult Res = executeStatement(&TopLevelEnv, Stmt.get());

    switch (Res.Kind) {
    default:
      RuntimeError("bad execution result code: " + std::to_string(Res.Kind));
    case ExecutionResult::BreakStatementResult:
      RuntimeError("break statement should be in a loop");
    case ExecutionResult::ContinueStatementResult:
      RuntimeError("continue statement should be in a loop");
    case ExecutionResult::ReturnStatementResult:
      if (Res.ReturnValue.isInt())
        return Res.ReturnValue.IntVal;
      RuntimeError("top level return statement should return integers, but " +
          cvm::TypeToStr(Res.ReturnValue.Type) + Res.ReturnValue.toString() +
          " is returned");
    case ExecutionResult::NormalStatementResult:
      break;
    }
  }

  // Invoke main function is there is one
  auto MainIt = UserFunctionMap.find("main");
  if (MainIt != UserFunctionMap.end()) {
    std::list<cvm::BasicValue> Args;

    if (MainIt->second.getParameterCount() == 0)
      return callUserFunction(MainIt->second, Args).toInt();

    auto ArgsPtr = std::make_shared<std::vector<cvm::BasicValue>>();
    ArgsPtr->reserve(static_cast<size_t>(Argc));
    for (int I = 0; I < Argc; ++I)
      ArgsPtr->emplace_back(std::string(Argv[I]));
    Args.emplace_back(cvm::StringType, ArgsPtr);
    return callUserFunction(MainIt->second, Args).toInt();
  }

  return 0;
}

void CMMInterpreter::addNativeFunctions() {
  NativeFunctionMap["typeof"] = cvm::Native::TypeOf;
  NativeFunctionMap["len"] = cvm::Native::Length;
  NativeFunctionMap["strlen"] = cvm::Native::StrLength;
  NativeFunctionMap["print"] = cvm::Native::Print;
  NativeFunctionMap["println"] = cvm::Native::PrintLn;
  NativeFunctionMap["puts"] = cvm::Native::PrintLn;
  NativeFunctionMap["system"] = cvm::Native::System;
  NativeFunctionMap["random"] = cvm::Native::Random;
  NativeFunctionMap["rand"] = cvm::Native::Random;
  NativeFunctionMap["srand"] = cvm::Native::Srand;
  NativeFunctionMap["time"] = cvm::Native::Time;
  NativeFunctionMap["exit"] = cvm::Native::Exit;
  NativeFunctionMap["toint"] = cvm::Native::ToInt;
  NativeFunctionMap["todouble"] = cvm::Native::ToDouble;
  NativeFunctionMap["tostring"] = cvm::Native::ToString;
  NativeFunctionMap["str"] = cvm::Native::ToString;
  NativeFunctionMap["tobool"] = cvm::Native::ToBool;
  NativeFunctionMap["read"] = cvm::Native::Read;
  NativeFunctionMap["readln"] = cvm::Native::ReadLn;
  NativeFunctionMap["readint"] = cvm::Native::ReadInt;
  NativeFunctionMap["sqrt"] = cvm::Native::Sqrt;
  NativeFunctionMap["pow"] = cvm::Native::Pow;
  NativeFunctionMap["exp"] = cvm::Native::Exp;
  NativeFunctionMap["log"] = cvm::Native::Log;
  NativeFunctionMap["log10"] = cvm::Native::Log10;

#if defined(__APPLE__) || defined(__linux__)
  NativeFunctionMap["UnixFork"] = cvm::Unix::Fork;

  NativeFunctionMap["NcEndWin"] = cvm::Ncurses::EndWindow;
  NativeFunctionMap["NcInitScr"] = cvm::Ncurses::InitScreen;
  NativeFunctionMap["NcNoEcho"] = cvm::Ncurses::NoEcho;
  NativeFunctionMap["NcCursSet"] = cvm::Ncurses::CursSet;
  NativeFunctionMap["NcKeypad"] = cvm::Ncurses::Keypad;
  NativeFunctionMap["NcTimeout"] = cvm::Ncurses::Timeout;
  NativeFunctionMap["NcGetCh"] = cvm::Ncurses::GetChar;
  NativeFunctionMap["NcMvAddCh"] = cvm::Ncurses::MoveAddChar;
  NativeFunctionMap["NcMvAddStr"] = cvm::Ncurses::MoveAddString;
  NativeFunctionMap["NcGetMaxY"] = cvm::Ncurses::GetMaxY;
  NativeFunctionMap["NcGetMaxX"] = cvm::Ncurses::GetMaxX;
  NativeFunctionMap["NcStartColor"] = cvm::Ncurses::StartColor;
  NativeFunctionMap["NcInitPair"] = cvm::Ncurses::InitPair;
  NativeFunctionMap["NcAttrOn"] = cvm::Ncurses::AttrOn;
  NativeFunctionMap["NcAttrOff"] = cvm::Ncurses::AttrOff;
  NativeFunctionMap["NcColorPair"] = cvm::Ncurses::ColorPair;

#endif // defined(__APPLE__) || defined(__linux__)
}

void CMMInterpreter::RuntimeError(const std::string &Msg) {
#if defined(__APPLE__) || defined(__linux__)
  const char *StartColor = "\033[1;31m";
  const char *EndColor = "\033[0m";
  std::cerr << StartColor;
#endif // defined(__APPLE__) || defined(__linux__)

  std::cerr << "CMM Runtime Error: ";

#if defined(__APPLE__) || defined(__linux__)
  std::cerr << EndColor;
#endif // defined(__APPLE__) || defined(__linux__)
  std::cerr << Msg << std::endl;
  std::exit(EXIT_FAILURE);
}

CMMInterpreter::ExecutionResult
CMMInterpreter::executeBlock(VariableEnv *OuterEnv, const BlockAST *Block) {

  ExecutionResult Res;  // Stores last execution result.
  VariableEnv CurrentEnv(OuterEnv);

  for (auto &Stmt : Block->getStatementList()) {
    Res = executeStatement(&CurrentEnv, Stmt.get());
    if (Res.Kind != ExecutionResult::NormalStatementResult)
      return Res;
  }
  return Res;
}

CMMInterpreter::ExecutionResult
CMMInterpreter::executeStatement(VariableEnv *Env, const StatementAST *Stmt) {
  if (!Stmt)
    return ExecutionResult();

  switch (Stmt->getKind()) {
  default:
    RuntimeError(std::to_string(Stmt->getKind()) + ": unknown statement kind");
  case StatementAST::DeclarationStatement:
    RuntimeError("single declaration should not be used by user");
  case StatementAST::DeclarationListStatement:
    return executeDeclarationList(Env, Stmt->as_cptr<DeclarationListAST>());
  case StatementAST::ExprStatement:
    return executeExprStatement(Env, Stmt->as_cptr<ExprStatementAST>());
  case StatementAST::BlockStatement:
    return executeBlock(Env, Stmt->as_cptr<BlockAST>());
  case StatementAST::IfStatement:
    return executeIfStatement(Env, Stmt->as_cptr<IfStatementAST>());
  case StatementAST::ReturnStatement:
    return executeReturnStatement(Env, Stmt->as_cptr<ReturnStatementAST>());
  case StatementAST::WhileStatement:
    return executeWhileStatement(Env, Stmt->as_cptr<WhileStatementAST>());
  case StatementAST::ForStatement:
    return executeForStatement(Env, Stmt->as_cptr<ForStatementAST>());
  case StatementAST::ContinueStatement:
    return executeContinueStatement(Env, Stmt->as_cptr<ContinueStatementAST>());
  case StatementAST::BreakStatement:
    return executeBreakStatement(Env, Stmt->as_cptr<BreakStatementAST>());
  }
}

CMMInterpreter::ExecutionResult
CMMInterpreter::executeIfStatement(VariableEnv *Env,
                                   const IfStatementAST *Stmt) {
  if (evaluateExpression(Env, Stmt->getCondition()).toBool()) {
    return executeStatement(Env, Stmt->getStatementThen());
  }
  if (const StatementAST *StatementElse = Stmt->getStatementElse()) {
    return executeStatement(Env, StatementElse);
  }
  return ExecutionResult();
}

CMMInterpreter::ExecutionResult
CMMInterpreter::executeForStatement(VariableEnv *Env,
                                    const ForStatementAST *ForStmt) {
  const ExpressionAST *Condition = ForStmt->getCondition();
  const ExpressionAST *Post = ForStmt->getPost();
  const StatementAST  *Statement = ForStmt->getStatement();

  if (const ExpressionAST *Init = ForStmt->getInit()) {
    evaluateExpression(Env, Init);
  }

  while (!Condition || evaluateExpression(Env, Condition).toBool()) {
    ExecutionResult Res = executeStatement(Env, Statement);

    if (Res.Kind == Res.ReturnStatementResult)
      return Res;
    if (Res.Kind == Res.BreakStatementResult)
      break;

    if (Post)
      evaluateExpression(Env, Post);
  }
  return ExecutionResult();
}

CMMInterpreter::ExecutionResult
CMMInterpreter::executeWhileStatement(VariableEnv *Env,
                                      const WhileStatementAST *WhileStmt) {
  const ExpressionAST *Condition = WhileStmt->getCondition();
  const StatementAST *Statement = WhileStmt->getStatement();

  while (!Condition || evaluateExpression(Env, Condition).toBool()) {
    ExecutionResult Res = executeStatement(Env, Statement);

    if (Res.Kind == Res.ReturnStatementResult)
      return Res;
    if (Res.Kind == Res.BreakStatementResult)
      break;
  }
  return ExecutionResult();
}

CMMInterpreter::ExecutionResult
CMMInterpreter::executeExprStatement(VariableEnv *Env,
                                     const ExprStatementAST *Stmt) {
  return ExecutionResult(ExecutionResult::NormalStatementResult,
                         evaluateExpression(Env, Stmt->getExpression()));
}

CMMInterpreter::ExecutionResult
CMMInterpreter::executeReturnStatement(VariableEnv *Env,
                                       const ReturnStatementAST *Stmt) {
  ExecutionResult Res(ExecutionResult::ReturnStatementResult);

  // ReturnValueExpr can be null.
  if (const ExpressionAST *ReturnValueExpr = Stmt->getReturnValue())
    Res.ReturnValue = evaluateExpression(Env, ReturnValueExpr);
  return Res;
}

CMMInterpreter::ExecutionResult
CMMInterpreter::executeBreakStatement(VariableEnv *,
                                      const BreakStatementAST *) {
  return ExecutionResult(ExecutionResult::BreakStatementResult);
}

CMMInterpreter::ExecutionResult
CMMInterpreter::executeContinueStatement(VariableEnv *Env,
                                         const ContinueStatementAST *ContStmt) {
  return ExecutionResult(ExecutionResult::ContinueStatementResult);
}

CMMInterpreter::ExecutionResult
CMMInterpreter::executeDeclarationList(VariableEnv *Env,
                                       const DeclarationListAST *DeclList) {
  for (auto &Declaration : DeclList->getDeclarationList()) {
    executeDeclaration(Env, Declaration.get());
  }
  return ExecutionResult();
}

CMMInterpreter::ExecutionResult
CMMInterpreter::executeDeclaration(VariableEnv *Env,
                                   const DeclarationAST *Decl) {
  const std::string& Name = Decl->getName();
  cvm::BasicType Type = Decl->getType();

  if (Env->contain(Name)) {
    RuntimeError("variable `" + Decl->getName() +
        "' is already defined in current scope");
  }

  if (Decl->isArray()) {
    std::list<int> DimensionList;

    for (auto &E : Decl->getElementCountList()) {
      cvm::BasicValue Dimension = evaluateExpression(Env, E.get());

      if (!Dimension.isInt()) {
        RuntimeError("expressions in array declaration `" + Name +
            "' should be integral type");
      }

      if (Dimension.IntVal <= 0) {
        RuntimeError("index range should greater than zero, " +
            std::to_string(Dimension.IntVal) + " provided");
      }

      DimensionList.push_back(Dimension.IntVal);
    }

    Env->VarMap.emplace(std::make_pair(Name,
                                       cvm::BasicValue(Type, DimensionList)));
  }

  // Now it's a normal variable.
  if (Decl->getInitializer()) {
    cvm::BasicValue Val = evaluateExpression(Env, Decl->getInitializer());

    if (Val.Type != Decl->getType()) {
      if (Decl->getType() == cvm::DoubleType && Val.isInt()) {
        Val.Type = cvm::DoubleType;
        Val.DoubleVal = static_cast<double>(Val.IntVal);
      } else {
        RuntimeError("variable `" + Name + "' is declared to be " +
            cvm::TypeToStr(Decl->getType()) + ", but is initialized to be " +
            cvm::TypeToStr(Val.Type));
      }
    }
    Env->VarMap.emplace(std::make_pair(Name, Val));
  } else {
    Env->VarMap.emplace(std::make_pair(Name, Decl->getType()));
  }

  return ExecutionResult();
}

cvm::BasicValue
CMMInterpreter::evaluateExpression(VariableEnv *Env,
                                   const ExpressionAST *Expr) {
  cvm::BasicValue Value;

  switch (Expr->getKind()) {
  default:
    RuntimeError("unknown expression kind");
  case ExpressionAST::IntExpression:
    return Expr->as_cptr<IntAST>()->getValue();

  case ExpressionAST::DoubleExpression:
    return Expr->as_cptr<DoubleAST>()->getValue();

  case ExpressionAST::BoolExpression:
    return Expr->as_cptr<BoolAST>()->getValue();

  case ExpressionAST::StringExpression:
    return Expr->as_cptr<StringAST>()->getValue();

  case ExpressionAST::IdentifierExpression:
    return evaluateIdentifierExpr(Env, Expr->as_cptr<IdentifierAST>());

  case ExpressionAST::FunctionCallExpression:
    return evaluateFunctionCallExpr(Env, Expr->as_cptr<FunctionCallAST>());

  case ExpressionAST::InfixOpExpression:
    return evaluateInfixOpExpr(Env, Expr->as_cptr<InfixOpExprAST>());

  case ExpressionAST::BinaryOperatorExpression:
    return evaluateBinaryOpExpr(Env, Expr->as_cptr<BinaryOperatorAST>());

  case ExpressionAST::UnaryOperatorExpression:
    return evaluateUnaryOpExpr(Env, Expr->as_cptr<UnaryOperatorAST>());
  }
}

cvm::BasicValue
CMMInterpreter::evaluateFunctionCallExpr(VariableEnv *Env,
                                         const FunctionCallAST *FuncCall) {
  auto UserFuncIt = UserFunctionMap.find(FuncCall->getCallee());
  if (UserFuncIt != UserFunctionMap.end()) {
    auto Args(evaluateArgumentList(Env, FuncCall->getArguments()));
    return callUserFunction(UserFuncIt->second, Args,
                            FuncCall->isDynamicBound() ? Env : nullptr);
  }

  auto NativeFuncIt = NativeFunctionMap.find(FuncCall->getCallee());
  if (NativeFuncIt != NativeFunctionMap.end()) {
    auto Args(evaluateArgumentList(Env, FuncCall->getArguments()));
    return callNativeFunction(NativeFuncIt->second, Args);
  }

  RuntimeError("function `" + FuncCall->getCallee() + "' is undefined");
  return evaluateFunctionCallExpr(nullptr, nullptr); // Make the compiler happy.
}

/// \brief Return a lvalue of an expression if possible
/// There are 3 kinds of lvalue expression:
/// 1. IdentifierExpression
/// 2. ArrayIdentifier [ IndexExpression ]
/// 3. IdentifierExpression = Expression
cvm::BasicValue &
CMMInterpreter::evaluateLvalueExpr(VariableEnv *Env,
                                   const ExpressionAST *Expr) {
  if (Expr->isIdentifierExpr())
    return evaluateIdentifierExpr(Env,
                                  static_cast<const IdentifierAST *>(Expr));

  if (Expr->isBinaryOperatorExpression()) {
    auto *BinOpExpr = static_cast<const BinaryOperatorAST *>(Expr);

    if (BinOpExpr->getOpKind() == BinaryOperatorAST::Index)
      return evaluateIndexExpr(Env, BinOpExpr->getLHS(), BinOpExpr->getRHS());
    if (BinOpExpr->getOpKind() == BinaryOperatorAST::Assign)
      return evaluateAssignment(Env, BinOpExpr->getLHS(), BinOpExpr->getRHS());

    RuntimeError("try to evaluate a rvalue binOpExpr as lvalue");
  }

  RuntimeError("try to evaluate a rvalue expression as lvalue");
  return evaluateLvalueExpr(nullptr, nullptr);  // Make the compiler happy.
}

/// \brief Return the reference of an identifier
cvm::BasicValue &
CMMInterpreter::evaluateIdentifierExpr(VariableEnv *Env,
                                       const IdentifierAST *IdExpr) {
  return searchVariable(Env, IdExpr->getName())->second;
}

cvm::BasicValue
CMMInterpreter::evaluateUnaryOpExpr(VariableEnv *Env,
                                    const UnaryOperatorAST *Expr) {

  cvm::BasicValue Operand = evaluateExpression(Env, Expr->getOperand());

  switch (auto OpKind = Expr->getOpKind()) {
  default:
    RuntimeError("unknown unary operator kind (code :" +
        std::to_string(OpKind) + ")");

  case UnaryOperatorAST::Plus:
  case UnaryOperatorAST::Minus:
    return evaluateUnaryArith(OpKind, Operand);

  case UnaryOperatorAST::LogicalNot:
    return evaluateUnaryLogical(OpKind, Operand);

  case UnaryOperatorAST::BitwiseNot:
    return evaluateUnaryBitwise(OpKind, Operand);
  }
}

/// \brief Perform unary arithmetic operation (+,-) on value
cvm::BasicValue
CMMInterpreter::evaluateUnaryArith(UnaryOperatorAST::OperatorKind OpKind,
                                   cvm::BasicValue Operand) {
  if (!Operand.isNumeric()) {
    RuntimeError("operands of unary arithmetic operations should be numeric");
  }
  if (OpKind == UnaryOperatorAST::Plus)
    return Operand;
  if (OpKind == UnaryOperatorAST::Minus) {
    if (Operand.isInt())
      return -Operand.IntVal;
    else
      return -Operand.DoubleVal;
  }

  RuntimeError(std::to_string(OpKind) +
      " is not valid unary arithmetic operation kind");
  return cvm::BasicValue(); // Make the compiler happy.
}

/// \brief Perform unary bitwise operation (!) on value
cvm::BasicValue
CMMInterpreter::evaluateUnaryLogical(UnaryOperatorAST::OperatorKind OpKind,
                                     cvm::BasicValue Operand) {
  if (OpKind != UnaryOperatorAST::LogicalNot) {
    RuntimeError(std::to_string(OpKind) +
        " is not valid unary logical operation kind");
  }
  return !Operand.toBool();
}

/// \brief Perform unary bitwise operation (~) on value
cvm::BasicValue
CMMInterpreter::evaluateUnaryBitwise(UnaryOperatorAST::OperatorKind OpKind,
                                     cvm::BasicValue Operand) {
  if (OpKind != UnaryOperatorAST::BitwiseNot) {
    RuntimeError(std::to_string(OpKind) +
        " is not valid unary bitwise operation kind");
  }
  if (!Operand.isInt()) {
    RuntimeError("operand of unary bitwise operation should be int");
  }
  return ~Operand.IntVal;
}

cvm::BasicValue
CMMInterpreter::evaluateBinaryOpExpr(VariableEnv *Env,
                                     const BinaryOperatorAST *Expr) {
  switch (Expr->getOpKind()) {
  default: {
    cvm::BasicValue LHS = evaluateExpression(Env, Expr->getLHS());
    cvm::BasicValue RHS = evaluateExpression(Env, Expr->getRHS());
    return evaluateBinaryCalc(Expr->getOpKind(), LHS, RHS);
  }
  case BinaryOperatorAST::Assign:
    return evaluateAssignment(Env, Expr->getLHS(), Expr->getRHS());
  case BinaryOperatorAST::Index:
    return evaluateIndexExpr(Env, Expr->getLHS(), Expr->getRHS());
  case BinaryOperatorAST::LogicalAnd:
    return evaluateLogicalAnd(Env, Expr->getLHS(), Expr->getRHS());
  case BinaryOperatorAST::LogicalOr:
    return evaluateLogicalOr(Env, Expr->getLHS(), Expr->getRHS());
  }
}

cvm::BasicValue &
CMMInterpreter::evaluateIndexExpr(VariableEnv *Env,
                                  const ExpressionAST *BaseExpr,
                                  const ExpressionAST *IndexExpr) {

  cvm::BasicValue &Base = evaluateLvalueExpr(Env, BaseExpr);
  if (!Base.isArray())
    RuntimeError("too many index or index expression didn't start with array");

  cvm::BasicValue Index = evaluateExpression(Env, IndexExpr);
  if (!Index.isInt())
    RuntimeError("non-int index in index expression");

  size_t ArraySize = Base.ArrayPtr->size();
  if (Index.IntVal < 0 || Index.IntVal >= static_cast<int>(ArraySize)) {
    RuntimeError("index out of range: should within [0," +
        std::to_string(ArraySize) + "); actually got index " +
        std::to_string(Index .IntVal));
  }
  return Base.ArrayPtr->at(static_cast<size_t>(Index.IntVal));
}

cvm::BasicValue
CMMInterpreter::evaluateBinaryCalc(BinaryOperatorAST::OperatorKind OpKind,
                                   cvm::BasicValue LHS, cvm::BasicValue RHS) {
  switch (OpKind) {
  default:
    RuntimeError("unknown binary operator kind (code :" +
        std::to_string(OpKind) + ")");
  case BinaryOperatorAST::Add:
    if (LHS.isString() || RHS.isString())
      return LHS.toString() + RHS.toString();
    /* fall Through */
  case BinaryOperatorAST::Minus:
  case BinaryOperatorAST::Multiply:
  case BinaryOperatorAST::Division:
  case BinaryOperatorAST::Modulo:
    return evaluateBinArith(OpKind, LHS, RHS);

  case BinaryOperatorAST::Less:
  case BinaryOperatorAST::LessEqual:
  case BinaryOperatorAST::Equal:
  case BinaryOperatorAST::NotEqual:
  case BinaryOperatorAST::Greater:
  case BinaryOperatorAST::GreaterEqual:
    return evaluateBinRelation(OpKind, LHS, RHS);

  case BinaryOperatorAST::BitwiseAnd:
  case BinaryOperatorAST::BitwiseOr:
  case BinaryOperatorAST::BitwiseXor:
  case BinaryOperatorAST::LeftShift:
  case BinaryOperatorAST::RightShift:
    return evaluateBinBitwise(OpKind, LHS, RHS);

  case BinaryOperatorAST::LogicalAnd:
  case BinaryOperatorAST::LogicalOr:
  case BinaryOperatorAST::Assign:
  case BinaryOperatorAST::Index:
    RuntimeError("assignment/index/logicalBinOp "
                     "should be handled in evaluateBinaryOpExpr");
  }
  return cvm::BasicValue(); // Make the compiler happy.
}

/// \brief Perform binary arithmetic operation (+,-,*,/) on values
cvm::BasicValue
CMMInterpreter::evaluateBinArith(BinaryOperatorAST::OperatorKind OpKind,
                                 cvm::BasicValue LHS, cvm::BasicValue RHS) {
  if (!LHS.isNumeric() || !RHS.isNumeric()) {
    RuntimeError("operands of binary arithmetic operations should be numeric");
  }

  if (LHS.isInt() && RHS.isInt()) {
    switch (OpKind) {
    default:
      RuntimeError(std::to_string(OpKind) +
          " is not valid binary arithmetic operation kind");
    case BinaryOperatorAST::Add:
      return LHS.IntVal + RHS.IntVal;
    case BinaryOperatorAST::Minus:
      return LHS.IntVal - RHS.IntVal;
    case BinaryOperatorAST::Multiply:
      return LHS.IntVal * RHS.IntVal;
    case BinaryOperatorAST::Modulo:
      if (RHS.IntVal == 0)
        RuntimeError("int modulo by zero");
      return LHS.IntVal % RHS.IntVal;
    case BinaryOperatorAST::Division:
      if (RHS.IntVal == 0)
        RuntimeError("int division by zero");
      return LHS.IntVal / RHS.IntVal;
    }
  }

  double L = LHS.toDouble(), R = RHS.toDouble();
  switch (OpKind) {
  default:
    RuntimeError(std::to_string(OpKind) +
        " is not a valid binary arithmetic operation kind");
  case BinaryOperatorAST::Add:
    return L + R;
  case BinaryOperatorAST::Minus:
    return L - R;
  case BinaryOperatorAST::Multiply:
    return L * R;
  case BinaryOperatorAST::Division:
    return L / R;
  case BinaryOperatorAST::Modulo:
    //RuntimeError("operands of modulo operator should be int");
    return std::fmod(L, R);
  }
  return cvm::BasicValue(); // Make the compiler happy.
}

/// \brief Perform binary logical and (&&) on expressions
/// It's a special case because its short circuit feature
cvm::BasicValue
CMMInterpreter::evaluateLogicalAnd(VariableEnv *Env,
                                   const ExpressionAST *LHS,
                                   const ExpressionAST *RHS) {
  return evaluateExpression(Env, LHS).toBool() &&
      evaluateExpression(Env, RHS).toBool();
}

/// \brief Perform binary logical or (||) on expressions
/// It's a special case because its short circuit feature
cvm::BasicValue
CMMInterpreter::evaluateLogicalOr(VariableEnv *Env,
                                   const ExpressionAST *LHS,
                                   const ExpressionAST *RHS) {
  return evaluateExpression(Env, LHS).toBool() ||
      evaluateExpression(Env, RHS).toBool();
}

/// \brief Perform binary relational operation (<, <=, ==, !=, >, >=) on values
cvm::BasicValue
CMMInterpreter::evaluateBinRelation(BinaryOperatorAST::OperatorKind OpKind,
                                    cvm::BasicValue LHS, cvm::BasicValue RHS) {
  if (LHS.Type != RHS.Type) {
    if (LHS.isNumeric() && RHS.isNumeric()) {
      return evaluateBinRelation(OpKind, LHS.toDouble(), RHS.toDouble());
    }
    RuntimeError("relational operator should apply to identical type");
  }

  switch (OpKind) {
  default:
    RuntimeError(std::to_string(OpKind) +
        " is not a valid binary relational operation kind");
  case cmm::BinaryOperatorAST::Less:
    return LHS < RHS;
  case cmm::BinaryOperatorAST::LessEqual:
    return LHS <= RHS;
  case cmm::BinaryOperatorAST::Equal:
    return LHS == RHS;
  case cmm::BinaryOperatorAST::NotEqual:
    return LHS != RHS;
  case cmm::BinaryOperatorAST::Greater:
    return LHS > RHS;
  case cmm::BinaryOperatorAST::GreaterEqual:
    return LHS >= RHS;
  }
}

/// \brief Perform binary bitwise operation (<<,>>,&,|) on values
cvm::BasicValue
CMMInterpreter::evaluateBinBitwise(BinaryOperatorAST::OperatorKind OpKind,
                                   cvm::BasicValue LHS, cvm::BasicValue RHS) {
  if (!LHS.isInt() || !RHS.isInt()) {
    RuntimeError("operands of bitwise operations should be int");
  }

  switch (OpKind) {
  default:
    RuntimeError(std::to_string(OpKind) +
        " is not a valid binary bitwise operation kind");
  case cmm::BinaryOperatorAST::BitwiseAnd:
    return LHS.IntVal & RHS.IntVal;
  case cmm::BinaryOperatorAST::BitwiseOr:
    return LHS.IntVal | RHS.IntVal;
  case cmm::BinaryOperatorAST::BitwiseXor:
    return LHS.IntVal ^ RHS.IntVal;
  case cmm::BinaryOperatorAST::LeftShift:
    return LHS.IntVal << RHS.IntVal;
  case cmm::BinaryOperatorAST::RightShift:
    return LHS.IntVal >> RHS.IntVal;
  }
}


std::map<std::string, cvm::BasicValue>::iterator
CMMInterpreter::searchVariable(VariableEnv *Env, const std::string &Name) {

  for (VariableEnv *E = Env; E != nullptr; E = E->OuterEnv) {
    std::map<std::string, cvm::BasicValue>::iterator It = E->VarMap.find(Name);
    if (It != E->VarMap.end())
      return It;
  }
  RuntimeError("variable `" + Name + "' is undefined");
  return searchVariable(nullptr, nullptr); // Make the compiler happy.
}

std::list<cvm::BasicValue>
CMMInterpreter::evaluateArgumentList(VariableEnv *Env, const std::list
    <std::unique_ptr<ExpressionAST>> &Args) {

  std::list<cvm::BasicValue> Res;
  for (auto &P : Args) {
    Res.emplace_back(evaluateExpression(Env, P.get()));
  }
  return Res;
}

cvm::BasicValue
CMMInterpreter::callNativeFunction(const NativeFunction &Function,
                                   std::list<cvm::BasicValue> &Args) {
  return Function(Args);
}

cvm::BasicValue
CMMInterpreter::evaluateInfixOpExpr(VariableEnv *Env,
                                    const InfixOpExprAST *Expr) {
  auto InfixOpIt = InfixOpMap.find(Expr->getSymbol());

  if (InfixOpIt == InfixOpMap.end()) {
    RuntimeError("Infix operator " + Expr->getSymbol() + " is undefined");
  }

  const InfixOpDefinitionAST &InfixOpDef = InfixOpIt->second;
  VariableEnv InfixOpEnv(&TopLevelEnv);

  cvm::BasicValue LHSVal = evaluateExpression(Env, Expr->getLHS());
  cvm::BasicValue RHSVal = evaluateExpression(Env, Expr->getRHS());
  InfixOpEnv.VarMap.emplace(std::make_pair(InfixOpDef.getLHSName(), LHSVal));
  InfixOpEnv.VarMap.emplace(std::make_pair(InfixOpDef.getRHSName(), RHSVal));

  ExecutionResult Result = executeStatement(&InfixOpEnv,
                                            InfixOpDef.getStatement());

  if (Result.ReturnValue.isVoid()) {
    RuntimeError("infix operator didn't return any value");
  }
  return Result.ReturnValue;
}

cvm::BasicValue
CMMInterpreter::callUserFunction(const FunctionDefinitionAST &Function,
                                 std::list<cvm::BasicValue> &Args,
                                 VariableEnv *Env) {
  if (Args.size() != Function.getParameterCount()) {
    RuntimeError("Function `" + Function.getName() + "' expects " +
        std::to_string(Function.getParameterCount()) + " parameter(s), " +
        std::to_string(Args.size()) + " argument(s) provided");
  }

  VariableEnv FuncEnv(Env ? Env : &TopLevelEnv);

  auto It = Function.getParameterList().cbegin();
  auto End = Function.getParameterList().cend();
  for (cvm::BasicValue &Arg : Args) {
    if (It->getType() != Arg.Type) {
      if (Arg.isInt() && It->getType() == cvm::DoubleType) {
        Arg.DoubleVal = Arg.IntVal;
        Arg.Type = cvm::DoubleType;
      } else {
        RuntimeError("in function `" + Function.getName() + "', parameter `" +
          It->getName() + "' has type " + cvm::TypeToStr(It->getType()) +
          ", but argument is " + cvm::TypeToStr(Arg.Type));
      }
    }

    if (!It->getName().empty()) // We allow empty parameter name.
      FuncEnv.VarMap.emplace(It->getName(), Arg);
    ++It;
  }

  ExecutionResult Result = executeStatement(&FuncEnv, Function.getStatement());
  if (Result.Kind == ExecutionResult::ReturnStatementResult &&
      Result.ReturnValue.Type != Function.getType()) {
    RuntimeError("function `" + Function.getName() + "' ought to return " +
        cvm::TypeToStr(Function.getType()) + ", but got " +
        cvm::TypeToStr(Result.ReturnValue.Type));
  }
  return Result.ReturnValue;
}

cvm::BasicValue &
CMMInterpreter::evaluateAssignment(VariableEnv *Env,
                                   const ExpressionAST *RefExpr,
                                   const ExpressionAST *ValExpr) {
  cvm::BasicValue &Variable = evaluateLvalueExpr(Env, RefExpr);
  cvm::BasicValue Value = evaluateExpression(Env, ValExpr);

  if (Variable.isArray()) {
    RuntimeError("cannot assign value to array directly");
  }

  if (Variable.Type != Value.Type) {
    if (Variable.isDouble() && Value.isInt()) {
      Variable.DoubleVal = Value.IntVal;
      return Variable;
    } else {
      RuntimeError("assignment to " + cvm::TypeToStr(Variable.Type) +
          " variable with " + cvm::TypeToStr(Value.Type) + " expression");
    }
  }
  return Variable = Value;
}

