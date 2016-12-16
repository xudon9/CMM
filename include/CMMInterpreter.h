#ifndef CMMINTERPRETER_H
#define CMMINTERPRETER_H

#include "AST.h"
#include <map>
#include <functional>

namespace cmm {
class CMMInterpreter {

private:  /* private data types */
  struct ExecutionResult {
    enum ExecutionResultKind {
      NormalStatementResult,
      ReturnStatementResult,
      BreakStatementResult,
      ContinueStatementResult
    } Kind;
    cvm::BasicValue ReturnValue;

    ExecutionResult() : Kind(NormalStatementResult) {}
    ExecutionResult(ExecutionResultKind K) : Kind(K) {}
  };

  struct VariableEnv {
    VariableEnv *OuterEnv;
    std::map<std::string, cvm::BasicValue> VarMap;

  public:
    VariableEnv(VariableEnv *OuterEnv = nullptr) : OuterEnv(OuterEnv) {}

    bool contains(const std::string &Name) const { return VarMap.count(Name); }
  };

  typedef std::function<cvm::BasicValue(std::list<cvm::BasicValue> &)>
      NativeFunction;

private:  /*  private member variables  */
  BlockAST &TopLevelBlock;
  std::map<std::string, FunctionDefinitionAST> &UserFunctionMap;
  std::map<std::string, NativeFunction> NativeFunctionMap;
  VariableEnv TopLevelEnv;

public:   /* public member functions */
  CMMInterpreter(BlockAST &Block,
                 std::map<std::string, FunctionDefinitionAST> &F)
      : TopLevelBlock(Block), UserFunctionMap(F) {
    addNativeFunctions();
  }

  void interpret();

private:  /* private member functions */
  void addNativeFunctions();
  void RuntimeError(const std::string &Msg);

  ExecutionResult executeBlock(VariableEnv *Env, const BlockAST *Block);
  ExecutionResult executeStatement(VariableEnv *Env, const StatementAST *Stmt);
  ExecutionResult executeIfStatement(VariableEnv *Env, IfStatementAST *IfStmt);
  ExecutionResult executeExprStatement(VariableEnv *Env,
                                       const ExprStatementAST *ExprStmt);
  ExecutionResult executeReturnStatement(VariableEnv *Env,
                                         const ReturnStatementAST *RetStmt);
  ExecutionResult executeDeclarationList(VariableEnv *Env,
                                         const DeclarationListAST *DeclList);
  ExecutionResult executeDeclaration(VariableEnv *Env,
                                     const DeclarationAST *Decl);

  cvm::BasicValue evaluateExpression(VariableEnv *Env,
                                     const ExpressionAST *Expr);
  cvm::BasicValue evaluateIdentifierExpr(VariableEnv *Env,
                                         const IdentifierAST *Expr);
  cvm::BasicValue evaluateFunctionCallExpr(VariableEnv *Env,
                                           const FunctionCallAST *FuncCall);
  cvm::BasicValue evaluateBinaryOpExpr(VariableEnv *Env,
                                       const BinaryOperatorAST *Expr);
  cvm::BasicValue evaluateBinaryCalc(BinaryOperatorAST::OperatorKind OpKind,
                                     cvm::BasicValue LHS, cvm::BasicValue RHS);
  cvm::BasicValue evaluateBinArith(BinaryOperatorAST::OperatorKind OpKind,
                                        cvm::BasicValue LHS,
                                        cvm::BasicValue RHS);
  cvm::BasicValue evaluateBinLogic(BinaryOperatorAST::OperatorKind OpKind,
                                     cvm::BasicValue LHS, cvm::BasicValue RHS);
  cvm::BasicValue evaluateBinRelation(BinaryOperatorAST::OperatorKind OpKind,
                                      cvm::BasicValue LHS, cvm::BasicValue RHS);
  cvm::BasicValue evaluateBinBitwise(BinaryOperatorAST::OperatorKind OpKind,
                                     cvm::BasicValue LHS, cvm::BasicValue RHS);


  std::list<cvm::BasicValue>
  evaluateArgumentList(VariableEnv *Env,
                       const std::list<std::unique_ptr<ExpressionAST>> &Args);

  cvm::BasicValue callNativeFunction(NativeFunction &Function,
                                     std::list<cvm::BasicValue> &Args);
  cvm::BasicValue callUserFunction(FunctionDefinitionAST &Function,
                                   std::list<cvm::BasicValue> &Args);
  cvm::BasicValue evaluateAssignment(VariableEnv *Env, const std::string &Name,
                                     const ExpressionAST *Expr);

  std::map<std::string, cvm::BasicValue>::iterator
  searchVariable(VariableEnv *Env, const std::string &Name);
};
}

#endif // !CMMINTEPRETER_H
