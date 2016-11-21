#ifndef CMMPARSER_H
#define CMMPARSER_H

#include "CMMLexer.h"
#include <memory>
#include <list>
#include <map>

///code.h
namespace cvm {
enum BasicType { BoolType, IntType, DoubleType, StringType };
}
///

namespace cmm {

class DerivedType {
protected:
  enum DerivedKind {
    FunctionDerive
  };
private:
  DerivedKind Kind;
public:
  DerivedType(DerivedKind Kind) : Kind(Kind) {}
  bool isFunction() { return Kind == FunctionDerive; }
};

class FunctionType : public DerivedType {
  class TypeSpecifier;
  struct Parameter {
    std::string Name;
    std::unique_ptr<TypeSpecifier> Type;
    size_t LineNo;
  };
  std::list<Parameter> ParameterList;
};

class TypeSpecifier {
  cvm::BasicType BasicType;
  DerivedType *Derived;
};

class AST { public: virtual ~AST(); };

class ExpressionAST : public AST {
protected:
  enum ExpressionKind {
    IntExpression,
    DoubleExpression,
    BoolExpression,
    StringExpression,
    IdentifierExpression,
    FunctionCallExpression,
    BinaryOperatorExpression,
    UnaryOperatorExpression
  };
private:
  ExpressionKind Kind;
public:
  ExpressionAST(ExpressionKind Kind) : Kind(Kind) {}
  bool isInt() { return Kind == IntExpression; }
  bool isDouble() { return Kind == DoubleExpression; }
  bool isBool() { return Kind == BoolExpression; }
  bool isString() { return Kind == StringExpression; }
  bool isNumeric() { return isInt() || isDouble(); }
};
class StatementAST : public AST {
protected:
  enum StatementKind {
    BlockStatement,
    IfStatement,
    WhileStatement,
    ForStatement,
    ReturnStatement,
    ContinueStatement
  };
private:
  StatementKind Kind;
public:
  StatementAST(StatementKind Kind) : Kind(Kind) {}
  bool isBlock() { return Kind == BlockStatement; }
  bool isIfStatement() { return Kind == IfStatement; }
  bool isWhileStatement() { return Kind == WhileStatement; }
};

class IntAST : public ExpressionAST {
  int Value;
public:
  IntAST(int Value) : ExpressionAST(IntExpression), Value(Value) {}
};
class DoubleAST : public ExpressionAST {
  double Value;
public:
  DoubleAST(double Value) : ExpressionAST(DoubleExpression), Value(Value) {}
};
class BoolAST : public ExpressionAST {
  bool Value;
public:
  BoolAST(bool Value) : ExpressionAST(BoolExpression), Value(Value) {}
};
class StringAST : public ExpressionAST {
  std::string Value;
public:
  StringAST(const std::string &Value)
    : ExpressionAST(StringExpression), Value(Value) {}
};
class IdentifierAST : public ExpressionAST {
  std::string Name;
public:
  IdentifierAST(const std::string &Name)
    : ExpressionAST(IdentifierExpression), Name(Name) {}
};

// It's a binary operator
//class AssignmentAST : public ExpreesionAST {};

class FunctionCallAST : public ExpressionAST {
  std::string Callee;
  std::vector<std::unique_ptr<ExpressionAST>> Arguments;
public:
  FunctionCallAST(const std::string &Callee,
                  std::vector<std::unique_ptr<ExpressionAST>> Arguments)
    : ExpressionAST(FunctionCallExpression), Callee(Callee)
    , Arguments(std::move(Arguments)) {}
};

// Unary operator
//class CastAST : public ExpressionAST {};

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
    : ExpressionAST(BinaryOperatorExpression)
    , Kind(Kind), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
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
    : ExpressionAST(UnaryOperatorExpression)
    , Kind(Kind), Operand(std::move(Operand)) {}
};

class DeclarationAST : public StatementAST {
  std::string Name;
  std::unique_ptr<TypeSpecifier> Type;
  std::unique_ptr<ExpressionAST> Initializer;
  int VariableIndex;
  bool isLocalVariable;
public:
  bool isLocal() { return isLocalVariable; }
};

class BlockAST : public StatementAST {
  std::shared_ptr<BlockAST> OuterBlock;
  std::list<std::unique_ptr<StatementAST>> StatementList;
  std::list<std::unique_ptr<DeclarationAST>> DeclarationList;
};

class StatementBlockAST : public BlockAST {
  // TODO? Statement   *statement;
  size_t ContinueLabel;
  size_t BreakLabel;
};

class FunctionDefinitionAST : public StatementAST {
  std::string Name;
  TypeSpecifier *Type;
};

class FunctionBlock : public BlockAST {
  std::unique_ptr<FunctionDefinitionAST> Function;
  size_t EndLabel;
};

class IfStatementAST : public StatementAST {
  std::unique_ptr<ExpressionAST> Condition;
  std::unique_ptr<BlockAST> BlockThen;
  std::unique_ptr<BlockAST> BlockElse;
};

class WhileStatementAST : public StatementAST {
  // char *label? TODO
  std::unique_ptr<ExpressionAST> Condition;
  std::unique_ptr<BlockAST> Block;
};

class ForStatementAST : public StatementAST {
  // char *label? TODO
  std::unique_ptr<ExpressionAST> Init;
  std::unique_ptr<ExpressionAST> Condition;
  std::unique_ptr<ExpressionAST> Post;
  std::unique_ptr<BlockAST> Block;
};

class ReturnStatementAST : public StatementAST {
  std::unique_ptr<ExpressionAST> ReturnValue;
};

class BreakStatementAST : public StatementAST {
  // char *label? TODO
};
class ContinueStatementAST : public StatementAST {
  // char *label? TODO
};

/*******************************************************/
class CMMParser {
public:
  using LocTy = CMMLexer::LocTy;
private:
  SourceMgr &SrcMgr;
  CMMLexer Lexer;
  std::list<std::unique_ptr<StatementAST>> TopLevelStatements;
  std::map<Token::TokenKind, char> BinOpPrecedence;
private:
  Token::TokenKind getKind() { return Lexer.getKind(); }
  Token Lex() { return Lexer.Lex(); }
  bool ParseExpression();
  bool ParsePrimaryExpression();
  bool ParseBinOpExpression();
public:
  bool Parse();
};
}

#endif // !CMMPARSER_H
