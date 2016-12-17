#ifndef CMMINTERPRETER_H
#define CMMINTERPRETER_H

#include "AST.h"
#include <map>

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

    bool contains(const std::string &Name) const {
      return VarMap.count(Name) > 0;
    }
  };

  typedef cvm::BasicValue (*NativeFunction)(std::list<cvm::BasicValue> &);

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
  ExecutionResult executeIfStatement(VariableEnv *Env,
                                     const IfStatementAST *IfStmt);
  ExecutionResult executeWhileStatement(VariableEnv *Env,
                                        const WhileStatementAST *WhileStmt);
  ExecutionResult executeForStatement(VariableEnv *Env,
                                      const ForStatementAST *ForStmt);
  ExecutionResult executeBreakStatement(VariableEnv *Env,
                                        const BreakStatementAST *BreakStmt);
  ExecutionResult executeContinueStatement(VariableEnv *Env,
                                           const ContinueStatementAST *ConStmt);
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
  cvm::BasicValue &evaluateLvalueExpr(VariableEnv *Env,
                                      const ExpressionAST *Expr);
  cvm::BasicValue &evaluateIdentifierExpr(VariableEnv *Env,
                                          const IdentifierAST *Expr);
  cvm::BasicValue &evaluateIndexExpr(VariableEnv *Env,
                                     const ExpressionAST *BaseExpr,
                                     const ExpressionAST *IndexExpr);
  cvm::BasicValue &evaluateAssignment(VariableEnv *Env,
                                      const ExpressionAST *RefExpr,
                                      const ExpressionAST *VarExpr);
  cvm::BasicValue evaluateFunctionCallExpr(VariableEnv *Env,
                                           const FunctionCallAST *FuncCall);
  cvm::BasicValue evaluateUnaryOpExpr(VariableEnv *Env,
                                      const UnaryOperatorAST *Expr);
  cvm::BasicValue evaluateUnaryArith(UnaryOperatorAST::OperatorKind OpKind,
                                     cvm::BasicValue Operand);
  cvm::BasicValue evaluateUnaryLogical(UnaryOperatorAST::OperatorKind OpKind,
                                       cvm::BasicValue Operand);
  cvm::BasicValue evaluateUnaryBitwise(UnaryOperatorAST::OperatorKind OpKind,
                                       cvm::BasicValue Operand);
  cvm::BasicValue evaluateBinaryOpExpr(VariableEnv *Env,
                                       const BinaryOperatorAST *Expr);
  cvm::BasicValue evaluateAssignExpr(VariableEnv *Env,
                                     const BinaryOperatorAST *Expr);
  cvm::BasicValue evaluateBinaryCalc(BinaryOperatorAST::OperatorKind OpKind,
                                     cvm::BasicValue LHS, cvm::BasicValue RHS);
  cvm::BasicValue evaluateBinArith(BinaryOperatorAST::OperatorKind OpKind,
                                   cvm::BasicValue LHS, cvm::BasicValue RHS);
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

  std::map<std::string, cvm::BasicValue>::iterator
  searchVariable(VariableEnv *Env, const std::string &Name);
};
}

#endif // !CMMINTEPRETER_H
