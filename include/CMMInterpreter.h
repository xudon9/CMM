#ifndef CMMINTERPRETER_H
#define CMMINTERPRETER_H

#include "AST.h"
#include <map>

namespace cmm {
class CMMInterpreter {
  struct ExecutionResult {
    enum ExecutionResultKind {
      NormalStatementResult,
      ReturnStatementResult,
      BreakStatementResult,
      ContinueStatementResult
    } Kind;
    cvm::BasicValue ReturnValue;

    ExecutionResult() : Kind(NormalStatementResult) {}
    //ExecutionResult(ExecutionResultKind K) : Kind(K) {}
  };

  struct VariableEnv {
    VariableEnv *OuterEnv;
    std::map<std::string, cvm::BasicValue> VarMap;
    VariableEnv(VariableEnv *OuterEnv = nullptr) : OuterEnv(OuterEnv) {}
  };
  //typedef cvm::BasicType FunctionType(std::list<Exp)
  //std::map<std::string, >;
  BlockAST &TopLevelBlock;
  std::map<std::string, FunctionDefinitionAST> &FunctionDefinition;

public:
  CMMInterpreter(BlockAST &Block,
                std::map<std::string, FunctionDefinitionAST> &FD)
    : TopLevelBlock(Block), FunctionDefinition(FD) {}

  void interpret();

private:
  ExecutionResult executeBlock(VariableEnv *OuterEnv, BlockAST *Block);
  ExecutionResult executeStatement(VariableEnv *Env, StatementAST *Stmt);
  ExecutionResult executeIfStatement(VariableEnv *Env, IfStatementAST *Stmt);
  ExecutionResult executeExprStatement(VariableEnv *Env, ExprStatementAST *Stmt);

  cvm::BasicValue evaluateExpression(VariableEnv *Env, ExpressionAST *Expr);
  cvm::BasicValue evaluateIdentifierExpr(VariableEnv *Env, IdentifierAST *Expr);
};
}

#endif // !CMMINTEPRETER_H
