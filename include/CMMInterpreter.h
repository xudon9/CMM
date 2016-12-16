#ifndef CMMINTERPRETER_H
#define CMMINTERPRETER_H

#include "AST.h"
#include <map>
#include <functional>

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

  typedef std::function<cvm::BasicValue(std::list<cvm::BasicValue>)>
      NativeFunction;

  //typedef cvm::BasicType FunctionType(std::list<Exp)
  //std::map<std::string, >;
  BlockAST &TopLevelBlock;
  std::map<std::string, FunctionDefinitionAST> &UserFunctionMap;
  std::map<std::string, NativeFunction> NativeFunctionMap;

public:
  CMMInterpreter(BlockAST &Block,
                std::map<std::string, FunctionDefinitionAST> &F)
    : TopLevelBlock(Block), UserFunctionMap(F) {}

  void interpret();

private:
  ExecutionResult executeBlock(VariableEnv *OuterEnv, BlockAST *Block);
  ExecutionResult executeStatement(VariableEnv *Env, StatementAST *Stmt);
  ExecutionResult executeIfStatement(VariableEnv *Env, IfStatementAST *Stmt);
  ExecutionResult executeExprStatement(VariableEnv *Env, ExprStatementAST *);

  cvm::BasicValue evaluateExpression(VariableEnv *Env, ExpressionAST *Expr);
  cvm::BasicValue evaluateIdentifierExpr(VariableEnv *Env, IdentifierAST *Expr);
  cvm::BasicValue evaluateFunctionCallExpr(VariableEnv *Env, FunctionCallAST *);
  cvm::BasicValue evaluateBinaryOpExpr(VariableEnv *Env,
                                       BinaryOperatorAST *Expr);
  cvm::BasicValue evaluateBinaryOperation(cvm::BasicValue LHS,
                                          cvm::BasicValue RHS);
  cvm::BasicValue evaluateAssignOperation(const std::string &Identifier,
                                          cvm::BasicValue Value);


  cvm::BasicValue callNativeFunction(VariableEnv *Env, NativeFunction
  &F/*TODO*/);

  std::map<std::string, cvm::BasicValue>::iterator
  searchVariable(VariableEnv *Env, const std::string &Name);
};
}

#endif // !CMMINTEPRETER_H
