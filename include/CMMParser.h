#ifndef CMMPARSER_H
#include "CMMLexer.h"
#include <memory>

namespace cmm {

class AST {};

class ExpressionAST : public AST {};
class Statement : public AST {};

class IntAST : public ExpressionAST {
  int Value;
public:
  IntAST(int Value) : Value(Value) {}
};
class DoubleAST : public ExpressionAST {
  double Value;
public:
  DoubleAST(double Value) : Value(Value) {}
};
class BoolAST : public ExpressionAST {
  bool Value;
public:
  BoolAST(bool Value) : Value(Value) {}
};
class StringAST : public ExpressionAST {
  std::string Value;
public:
  StringAST(const std::string &Value) : Value(Value) {}
};
class IdentifierAST : public ExpressionAST {
  std::string Name;
public:
  IdentifierAST(const std::string &Name) : Name(Name) {}
};

//class AssignmentAST : public ExpreesionAST {};

class FunctionCallAST : public ExpressionAST {
  std::string Callee;
  std::vector<std::unique_ptr<ExpressionAST>> Arguments;
public:
  FunctionCallAST(const std::string &Callee,
                  std::vector<std::unique_ptr<ExpressionAST>> Arguments)
    : Callee(Callee), Arguments(std::move(Arguments)) {}
};

class CastAST : public ExpressionAST {};

class BinaryOperatorAST : public ExpressionAST {
public:
  enum OperatorKind {
    Add, Minus, Multiply, Division, Modulo,
    LogicalAnd, LogicalOr,
    Less, LessEqual, Equal, Greater, GeaterEqual,
    BitwiseAnd, BitwiseOr, BitwiseXor, LeftShift, RightShift,
    Assign, Comma
  };
private:
  OperatorKind Kind;
  std::unique_ptr<ExpressionAST> LHS, RHS;
public:
  BinaryOperatorAST(OperatorKind Kind,
                    std::unique_ptr<ExpressionAST> LHS,
                    std::unique_ptr<ExpressionAST> RHS)
    : Kind(Kind), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
};

class UnaryOperatorAST : public ExpressionAST {
public:
  enum OperatorKind { Minus, LogicalNot, BitwiseNot };
private:
  OperatorKind Kind;
  std::unique_ptr<ExpressionAST> Operand;
public:
  UnaryOperatorAST(OperatorKind Kind,
                   std::unique_ptr<ExpressionAST> Operand)
    : Kind(Kind), Operand(std::move(Operand)) {}
};

/***************/
class CMMParser {
public:
  using LocTy = CMMLexer::LocTy;
private:
public:
};
}

#endif // !CMMPARSER_H
