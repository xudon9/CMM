#include "CMMInterpreter.h"
#include <cassert>

using namespace cmm;

void CMMInterpreter::interpret() {
  for (auto &Stmt : TopLevelBlock.getStatementList()) {
    ExecutionResult Res = executeStatement(&TopLevelEnv, Stmt.get());
    if (Res.Kind != ExecutionResult::NormalStatementResult) {
      // ERROR todo
      break;
    }
  }
}

void CMMInterpreter::addNativeFunctions() {
  NativeFunctionMap["puts"] = [](std::list<cvm::BasicValue> &Args)
    -> cvm::BasicValue {
    for (auto &V : Args) {
      std::cout << V.toString() << " ";
    }
    std::cout << std::endl;
    return cvm::BasicValue();
  };
}

CMMInterpreter::ExecutionResult
CMMInterpreter::executeBlock(VariableEnv *OuterEnv, BlockAST *Block) {
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
    std::cerr << "bad statement kind\n";
    break;
  case StatementAST::ExprStatement:
    Res = executeExprStatement(Env, static_cast<const ExprStatementAST *>(Stmt));
    break;
  case StatementAST::BlockStatement:
    //Res = executeBlock(Env, static_cast<const BlockAST *>(Stmt));
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
    std::cerr << "unimplemented kind\n";
    exit(-1);
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
CMMInterpreter::executeExprStatement(VariableEnv *Env, const ExprStatementAST *Stmt) {
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

  // Run time error TODO
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
    const auto &Id = static_cast<IdentifierAST *>(Expr->getLHS())->getName();
    cvm::BasicValue &Var = searchVariable(Env, Id)->second;
    if (Var.Type != RHS.Type)
      std::cout << ""; // TODO runtime error
    return Var = RHS;
  }
  if(Expr->getOpKind() == Expr->Index) {
    // TODO
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
  //  TODO runtime error
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
  //VariableEnv FunctionEnv(&)
  return Args.front();
}