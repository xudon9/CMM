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
  case ExpressionAST::FunctionCallExpression:
  case ExpressionAST::BinaryOperatorExpression:
  case ExpressionAST::UnaryOperatorExpression:
    exit(-1);
  }
}
cvm::BasicValue
CMMInterpreter::evaluateIdentifierExpr(VariableEnv *Env, IdentifierAST *IdExpr) {
  for (VariableEnv *E = Env; E != nullptr; E = E->OuterEnv) {
    const auto &VarMap = E->VarMap;
    auto It = VarMap.find(IdExpr->getName());
    if (It != VarMap.end())
      return It->second;
  }
  return cvm::BasicValue();
  // TODO runtime err
}
