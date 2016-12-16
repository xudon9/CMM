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
  VariableEnv TopLevelEnv;

public:
  CMMInterpreter(BlockAST &Block,
                std::map<std::string, FunctionDefinitionAST> &F)
    : TopLevelBlock(Block), UserFunctionMap(F) {
    addNativeFunctions();
  }

  void interpret();

private:
  void addNativeFunctions();

  ExecutionResult executeBlock(VariableEnv *OuterEnv, BlockAST *Block);
  ExecutionResult executeStatement(VariableEnv *Env, const StatementAST *Stmt);
  ExecutionResult executeIfStatement(VariableEnv *Env, IfStatementAST *Stmt);
  ExecutionResult executeExprStatement(VariableEnv *Env, const ExprStatementAST *);

  cvm::BasicValue evaluateExpression(VariableEnv *Env, const ExpressionAST *Expr);
  cvm::BasicValue evaluateIdentifierExpr(VariableEnv *Env, const IdentifierAST *Expr);
  cvm::BasicValue evaluateFunctionCallExpr(VariableEnv *Env, const FunctionCallAST *);
  cvm::BasicValue evaluateBinaryOpExpr(VariableEnv *Env,
                                       BinaryOperatorAST *Expr);
  cvm::BasicValue evaluateBinaryOperation(cvm::BasicValue LHS,
                                          cvm::BasicValue RHS);
  cvm::BasicValue evaluateAssignOperation(const std::string &Identifier,
                                          cvm::BasicValue Value);
  std::list<cvm::BasicValue> evaluateArgumentList(VariableEnv *Env,
                         const std::list<std::unique_ptr<ExpressionAST>> &Args);

  cvm::BasicValue callNativeFunction(NativeFunction &Function,
                                     std::list<cvm::BasicValue> &Args);
  cvm::BasicValue callUserFunction(FunctionDefinitionAST &Function,
                                     std::list<cvm::BasicValue> &Args);

  std::map<std::string, cvm::BasicValue>::iterator
  searchVariable(VariableEnv *Env, const std::string &Name);
};
}

#endif // !CMMINTEPRETER_H
