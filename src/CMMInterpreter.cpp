#include "CMMInterpreter.h"

using namespace cmm;

void CMMInterpreter::interpret() {
  executeBlock(nullptr, &TopLevelBlock);
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
CMMInterpreter::executeStatement(VariableEnv *Env, StatementAST *Stmt) {
  ExecutionResult Res;

  switch (Stmt->getKind()) {
  default:
    std::cerr << "bad statement kind\n";
    break;
  case StatementAST::ExprStatement:
    Res = executeExprStatement(Env, static_cast<ExprStatementAST *>(Stmt));
    break;
  case StatementAST::BlockStatement:
    Res = executeBlock(Env, static_cast<BlockAST *>(Stmt));
    break;
  case StatementAST::IfStatement:
    Res = executeIfStatement(Env, static_cast<IfStatementAST *>(Stmt));
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
}

CMMInterpreter::ExecutionResult
CMMInterpreter::executeIfStatement(VariableEnv *Env, IfStatementAST *Stmt) {
  // todo
  return ExecutionResult();
}

CMMInterpreter::ExecutionResult
CMMInterpreter::executeExprStatement(VariableEnv *Env, ExprStatementAST *Stmt) {
  evaluateExpression(Env, Stmt->getExpression());
  return ExecutionResult();
}

cvm::BasicValue
CMMInterpreter::evaluateExpression(VariableEnv *Env, ExpressionAST *Expr) {
  cvm::BasicValue Value;

  switch (Expr->getKind()) {
  default:
    // TODO
    break;
  case ExpressionAST::IntExpression:
    return cvm::BasicValue(static_cast<IntAST *>(Expr)->getValue());
  case ExpressionAST::DoubleExpression:
    return cvm::BasicValue(static_cast<DoubleAST *>(Expr)->getValue());
  case ExpressionAST::BoolExpression:
    return cvm::BasicValue(static_cast<BoolAST *>(Expr)->getValue());
  case ExpressionAST::StringExpression:
    return cvm::BasicValue(static_cast<StringAST *>(Expr)->getValue());
  case ExpressionAST::IdentifierExpression:
    return evaluateIdentifierExpr(Env, static_cast<IdentifierAST *>(Expr));
  case ExpressionAST::FunctionCallExpression: {
    auto FuncCallAST = static_cast<FunctionCallAST *>(Expr);
  }
  case ExpressionAST::BinaryOperatorExpression:
  case ExpressionAST::UnaryOperatorExpression:
    exit(-1);
  }
}

cvm::BasicValue
CMMInterpreter::evaluateFunctionCallExpr(VariableEnv *Env,
                                         FunctionCallAST *FuncCall) {
  auto NativeIt = NativeFunctionMap.find(FuncCall->getCallee());
  if (NativeIt != NativeFunctionMap.end())
    return callNativeFunction(Env, NativeIt->second /*.TODO*/);
}


cvm::BasicValue
CMMInterpreter::evaluateIdentifierExpr(VariableEnv *Env,
                                       IdentifierAST *IdExpr) {
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
      ; // TODO runtime error
    return Var = RHS;
  }
  if(Expr->getOpKind() == Expr->Index) {
  }

  cvm::BasicValue LHS = evaluateExpression(Env, Expr->getLHS());
}

/*
std::pair<std::string, cvm::BasicValue> &
CMMInterpreter::searchVariable(VariableEnv *Env, const std::string &Name) {
  for (VariableEnv *E = Env; E != nullptr; E = E->OuterEnv) {

    std::map<std::string, cvm::BasicValue>::iterator It = E->VarMap.find(Name);
    if (It != E->VarMap.end())
      return *It;
  }
  // TODO runtime error
}
 */

std::map<std::string, cvm::BasicValue>::iterator
CMMInterpreter::searchVariable(VariableEnv *Env, const std::string &Name) {
  for (VariableEnv *E = Env; E != nullptr; E = E->OuterEnv) {

    std::map<std::string, cvm::BasicValue>::iterator It = E->VarMap.find(Name);
    if (It != E->VarMap.end())
      return It;
  }
  //  TODO runtime error
}