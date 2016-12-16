#include "CMMInterpreter.h"
#include "NativeFunctions.h"
#include <cassert>

using namespace cmm;

void CMMInterpreter::interpret() {
  for (auto &Stmt : TopLevelBlock.getStatementList()) {
    ExecutionResult Res = executeStatement(&TopLevelEnv, Stmt.get());
    if (Res.Kind != ExecutionResult::NormalStatementResult)
      RuntimeError("unbounded break/continue/return");
  }
}

void CMMInterpreter::addNativeFunctions() {
  NativeFunctionMap["print"] = cvm::NativePrint;
  NativeFunctionMap["println"] = cvm::NativePrintln;
}

void CMMInterpreter::RuntimeError(const std::string &Msg) {
  std::cerr << "Runtime Error: " << Msg << std::endl;
  std::exit(EXIT_FAILURE);
}

CMMInterpreter::ExecutionResult
CMMInterpreter::executeBlock(VariableEnv *OuterEnv, const BlockAST *Block) {
  ExecutionResult Res;
  VariableEnv CurrentEnv(OuterEnv);

  for (auto &Statement : Block->getStatementList()) {
    Res = executeStatement(&CurrentEnv, Statement.get());
    if (Res.Kind != ExecutionResult::NormalStatementResult)
      break;
  }
  return Res;
}

CMMInterpreter::ExecutionResult
CMMInterpreter::executeStatement(VariableEnv *Env, const StatementAST *Stmt) {
  ExecutionResult Res;

  switch (Stmt->getKind()) {
  default:
    RuntimeError("unknown statement kind");
    break;
  case StatementAST::ExprStatement:
    Res = executeExprStatement(Env, static_cast<const ExprStatementAST *>(Stmt));
    break;
  case StatementAST::BlockStatement:
    Res = executeBlock(Env, static_cast<const BlockAST *>(Stmt));
    break;
  case StatementAST::IfStatement:
    //Res = executeIfStatement(Env, static_cast<const IfStatementAST *>(Stmt));
    break;
  case StatementAST::WhileStatement:
  case StatementAST::ForStatement:
  case StatementAST::ReturnStatement:
  case StatementAST::ContinueStatement:
  case StatementAST::BreakStatement:
  //case StatementAST::DeclarationStatement:
  case StatementAST::DeclarationListStatement:
    RuntimeError("unimplemented stmt kind!");
  }

  if (Res.Kind != ExecutionResult::NormalStatementResult) {
    //todo
  }
  return Res;
}

CMMInterpreter::ExecutionResult
CMMInterpreter::executeIfStatement(VariableEnv *Env, IfStatementAST *Stmt) {
  // todo
  return ExecutionResult();
}

CMMInterpreter::ExecutionResult
CMMInterpreter::executeExprStatement(VariableEnv *Env,
                                     const ExprStatementAST *Stmt) {
  evaluateExpression(Env, Stmt->getExpression());
  return ExecutionResult();
}

cvm::BasicValue
CMMInterpreter::evaluateExpression(VariableEnv *Env, const ExpressionAST *Expr) {
  cvm::BasicValue Value;

  switch (Expr->getKind()) {
  default:
    // TODO
    break;
  case ExpressionAST::IntExpression:
    return cvm::BasicValue(static_cast<const IntAST *>(Expr)->getValue());
  case ExpressionAST::DoubleExpression:
    return cvm::BasicValue(static_cast<const DoubleAST *>(Expr)->getValue());
  case ExpressionAST::BoolExpression:
    return cvm::BasicValue(static_cast<const BoolAST *>(Expr)->getValue());
  case ExpressionAST::StringExpression:
    return cvm::BasicValue(static_cast<const StringAST *>(Expr)->getValue());
  case ExpressionAST::IdentifierExpression:
    return evaluateIdentifierExpr(Env, static_cast<const IdentifierAST *>(Expr));
  case ExpressionAST::FunctionCallExpression: {
    return evaluateFunctionCallExpr(Env, static_cast<const FunctionCallAST *>(Expr));
  }
  case ExpressionAST::BinaryOperatorExpression:
    //TODO
  case ExpressionAST::UnaryOperatorExpression:
    //TODO
    exit(-1);
  }
}

cvm::BasicValue
CMMInterpreter::evaluateFunctionCallExpr(VariableEnv *Env,
                                         const FunctionCallAST *FuncCall) {
  auto UserFuncIt = UserFunctionMap.find(FuncCall->getCallee());
  if (UserFuncIt != UserFunctionMap.end()) {
    auto Args(evaluateArgumentList(Env, FuncCall->getArguments()));
    return callUserFunction(UserFuncIt->second, Args);
  }

  auto NativeFuncIt = NativeFunctionMap.find(FuncCall->getCallee());
  if (NativeFuncIt != NativeFunctionMap.end()) {
    auto Args(evaluateArgumentList(Env, FuncCall->getArguments()));
    return callNativeFunction(NativeFuncIt->second, Args);
  }

  RuntimeError("function `" + FuncCall->getCallee() + "' is undefined");
  return cvm::BasicValue(); // Make the compiler happy.
}


cvm::BasicValue
CMMInterpreter::evaluateIdentifierExpr(VariableEnv *Env,
                                       const IdentifierAST *IdExpr) {
  return searchVariable(Env, IdExpr->getName())->second;
}

cvm::BasicValue
CMMInterpreter::evaluateBinaryOpExpr(VariableEnv *Env,
                                     BinaryOperatorAST *Expr) {

  cvm::BasicValue RHS = evaluateExpression(Env, Expr->getRHS());

  if (Expr->getOpKind() == Expr->Assign) {
    const std::string &Id = static_cast<IdentifierAST *>
                                          (Expr->getLHS())->getName();
    cvm::BasicValue &Var = searchVariable(Env, Id)->second;
    if (Var.Type != RHS.Type)
      RuntimeError("assignment to symbol `" + Id + "' is undefined");
    return Var = RHS;
  }
  if(Expr->getOpKind() == Expr->Index) {
    RuntimeError("array implemented!");
  }

  cvm::BasicValue LHS = evaluateExpression(Env, Expr->getLHS());
}

std::map<std::string, cvm::BasicValue>::iterator
CMMInterpreter::searchVariable(VariableEnv *Env, const std::string &Name) {
  for (VariableEnv *E = Env; E != nullptr; E = E->OuterEnv) {

    std::map<std::string, cvm::BasicValue>::iterator It = E->VarMap.find(Name);
    if (It != E->VarMap.end())
      return It;
  }
  RuntimeError("variable `" + Name + "' is undefined");
}

std::list<cvm::BasicValue>
CMMInterpreter::evaluateArgumentList(VariableEnv *Env,
  const std::list<std::unique_ptr<ExpressionAST>> &Args) {

  std::list<cvm::BasicValue> Res;
  for (auto &P : Args)
    Res.emplace_back(evaluateExpression(Env, P.get()));
  return Res;
}

cvm::BasicValue
CMMInterpreter::callNativeFunction(NativeFunction &Function,
                                   std::list<cvm::BasicValue> &Args) {
  return Function(Args);
}

cvm::BasicValue
CMMInterpreter::callUserFunction(FunctionDefinitionAST &Function,
                                 std::list<cvm::BasicValue> &Args) {
  if (Args.size() != Function.getParameterCount())
    RuntimeError("Function `" + Function.getName() + "' expects " +
      std::to_string(Function.getParameterCount()) + " parameter(s), but " +
      std::to_string(Args.size()) + " argument(s) provided");

  VariableEnv FuncEnv(&TopLevelEnv);

  auto ParaIt = Function.getParameterList().cbegin();
  auto ParaEnd = Function.getParameterList().cend();
  for (cvm::BasicValue &Arg : Args) {
    if (ParaIt->getType() != Arg.Type) {
      RuntimeError("in function `" + Function.getName() + "', parameter `" +
        ParaIt->getName() + "' has type " + cvm::TypeToStr(ParaIt->getType()) +
        ", but argument has type " + cvm::TypeToStr(Arg.Type));
    }
    FuncEnv.VarMap[ParaIt->getName()] = Arg;
    ++ParaIt;
  }
  assert(ParaIt == ParaEnd);
  ExecutionResult Result =
            executeStatement(&FuncEnv, Function.getStatement());
  if (Result.ReturnValue.Type != Function.getType()) {
    RuntimeError("functon `" + Function.getName() + "' ought to return " +
      cvm::TypeToStr(Function.getType()) + ", but got " +
      cvm::TypeToStr(Result.ReturnValue.Type));
  }
  return Result.ReturnValue;
}