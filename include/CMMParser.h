#ifndef CMMPARSER_H
#define CMMPARSER_H

#include "CMMLexer.h"
//#include <cstdint>
#include <memory>
#include <list>
#include <map>
#include <iostream>

///code.h
namespace cvm {
enum BasicType { BoolType, IntType, DoubleType, StringType, VoidType };
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

class AST {
public:
  virtual ~AST() {};
  virtual void dump(const std::string &prefix = "") {};
};

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
    ExprStatement,
    BlockStatement,
    IfStatement,
    WhileStatement,
    ForStatement,
    ReturnStatement,
    ContinueStatement,
    BreakStatement
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
  void dump(const std::string &prefix = "") override {
    std::cout << "(int)" << Value << std::endl;
  }
};

class DoubleAST : public ExpressionAST {
  double Value;
public:
  DoubleAST(double Value) : ExpressionAST(DoubleExpression), Value(Value) {}
  void dump(const std::string &prefix = "") override {
    std::cout << "(double)" << Value << std::endl;
  }
};

class BoolAST : public ExpressionAST {
  bool Value;
public:
  BoolAST(bool Value) : ExpressionAST(BoolExpression), Value(Value) {}
  void dump(const std::string &prefix = "") override {
    std::cout << "(bool)" << (Value ? "true" : "false") << std::endl;
  }
};

class StringAST : public ExpressionAST {
  std::string Value;
public:
  StringAST(const std::string &Value)
    : ExpressionAST(StringExpression), Value(Value) {}
  void dump(const std::string &prefix = "") override {
    std::cout << "(str)" << Value << std::endl;
  }
};

class IdentifierAST : public ExpressionAST {
  std::string Name;
public:
  IdentifierAST(const std::string &Name)
    : ExpressionAST(IdentifierExpression), Name(Name) {}
  void dump(const std::string &prefix = "") override {
    std::cout << "(id)" << Name << std::endl;
  }
};

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
    Less, LessEqual, Equal, Greater, GreaterEqual,
    BitwiseAnd, BitwiseOr, BitwiseXor, LeftShift, RightShift,
    Assign /**, Comma**/
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
  static std::unique_ptr<ExpressionAST> create(Token::TokenKind TokenKind,
    std::unique_ptr<ExpressionAST> LHS, std::unique_ptr<ExpressionAST> RHS);


  void dump(const std::string &prefix = "") override {
    std::string OperatorSymbol;
    switch (Kind) {
    default: break;
    case Add:           OperatorSymbol = "Add"; break;
    case Minus:         OperatorSymbol = "Sub"; break;
    case Multiply:      OperatorSymbol = "Mul"; break;
    case Division:      OperatorSymbol = "Div"; break;
    case Modulo:        OperatorSymbol = "Mod"; break;
    case LogicalAnd:    OperatorSymbol = "And"; break;
    case LogicalOr:     OperatorSymbol = "Or"; break;
    case Less:          OperatorSymbol = "<"; break;
    case LessEqual:     OperatorSymbol = "<="; break;
    case Equal:         OperatorSymbol = "=="; break;
    case Greater:       OperatorSymbol = ">"; break;
    case BitwiseAnd:    OperatorSymbol = "BitAnd"; break;
    case BitwiseOr:     OperatorSymbol = "BitOr"; break;
    case BitwiseXor:    OperatorSymbol = "Xor"; break;
    case LeftShift:     OperatorSymbol = "<<"; break;
    case RightShift:    OperatorSymbol = ">>"; break;
    case Assign:        OperatorSymbol = "Assign"; break;
    case GreaterEqual:  OperatorSymbol = ">="; break;
    }
    std::cout << OperatorSymbol << std::endl;
    std::cout << prefix << "|-- ";
    LHS->dump(prefix + "|   ");
    std::cout << prefix << "`-- ";
    RHS->dump(prefix + "    ");
  }
};

class UnaryOperatorAST : public ExpressionAST {
public:
  enum OperatorKind { Plus, Minus, LogicalNot, BitwiseNot };
private:
  OperatorKind Kind;
  std::unique_ptr<ExpressionAST> Operand;
public:
  UnaryOperatorAST(OperatorKind Kind, std::unique_ptr<ExpressionAST> Operand)
    : ExpressionAST(UnaryOperatorExpression)
    , Kind(Kind), Operand(std::move(Operand)) {}

  void dump(const std::string &prefix = "") override {
    std::string OperatorSymbol;
    switch (Kind) {
    default: break;
    case Plus:       OperatorSymbol = "+"; break;
    case Minus:      OperatorSymbol = "-"; break;
    case LogicalNot: OperatorSymbol = "!"; break;
    case BitwiseNot: OperatorSymbol = "~"; break;
    }
    std::cout << "(" << OperatorSymbol << ")" << std::endl;
    std::cout << prefix << "`-- ";
    Operand->dump(prefix + "    ");
  }
};

class DeclarationAST : public StatementAST {
  std::string Name;
  std::unique_ptr<TypeSpecifier> Type;
  std::unique_ptr<ExpressionAST> Initializer;
  int VariableIndex;
  bool IsLocalVariable;
public:
  bool isLocal() { return IsLocalVariable; }
};

class BlockAST : public StatementAST {
  std::shared_ptr<BlockAST> OuterBlock;
  std::list<std::unique_ptr<StatementAST>> StatementList;
  std::list<std::unique_ptr<DeclarationAST>> DeclarationList;
};

class ExprStatementAST : public StatementAST {
  std::unique_ptr<ExpressionAST> Expression;
public:
  ExprStatementAST(std::unique_ptr<ExpressionAST> Expression)
    : StatementAST(ExprStatement), Expression(std::move(Expression)) {}

  void dump(const std::string &prefix) {
    std::cout << "(ExprStmt)" << std::endl;
    std::cout << prefix << "`-- ";
    Expression->dump(prefix + "    ");
  }
};

// class StatementBlockAST : public BlockAST {
//   // TODO? Statement   *statement;
//   size_t ContinueLabel;
//   size_t BreakLabel;
// };

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
  std::unique_ptr<StatementAST> StatementThen;
  std::unique_ptr<StatementAST> StatementElse;
public:
  IfStatementAST(std::unique_ptr<ExpressionAST> Condition,
                 std::unique_ptr<StatementAST> StatementThen,
                 std::unique_ptr<StatementAST> StatementElse)
    : StatementAST(IfStatement)
    , Condition(std::move(Condition))
    , StatementThen(std::move(StatementThen))
    , StatementElse(std::move(StatementElse)) {}

  void dump(const std::string &prefix = "") override {
    std::cout << "(if)" << std::endl;
    std::cout << prefix << "|-- ";
    Condition->dump(prefix + "|   ");
    if (StatementElse) {
      std::cout << prefix << "|-- ";
      StatementThen->dump(prefix + "|   ");
      std::cout << prefix << "`-- ";
      StatementElse->dump(prefix + "    ");
    } else {
      std::cout << prefix << "`-- ";
      StatementThen->dump(prefix + "    ");
    }
  }
};

class WhileStatementAST : public StatementAST {
  // char *label? TODO
  std::unique_ptr<ExpressionAST> Condition;
  std::unique_ptr<StatementAST> Statement;
public:
  WhileStatementAST(std::unique_ptr<ExpressionAST> Condition,
                    std::unique_ptr<StatementAST> Statement)
    : StatementAST(WhileStatement)
    , Condition(std::move(Condition))
    , Statement(std::move(Statement)) {}


  void dump(const std::string &prefix) override {
    std::cout << "(while)" << std::endl;
    std::cout << prefix << "|-- ";
    Condition->dump(prefix + "|   ");
    std::cout << prefix << "`-- ";
    Statement->dump(prefix + "    ");
  }
};

class ForStatementAST : public StatementAST {
  // char *label? TODO
  std::unique_ptr<ExpressionAST> Init;
  std::unique_ptr<ExpressionAST> Condition;
  std::unique_ptr<ExpressionAST> Post;
  std::unique_ptr<StatementAST> Statement;
};

class ReturnStatementAST : public StatementAST {
  std::unique_ptr<ExpressionAST> ReturnValue;
public:
  ReturnStatementAST(std::unique_ptr<ExpressionAST> ReturnValue)
    : StatementAST(ReturnStatement), ReturnValue(std::move(ReturnValue)) {}

  void dump(const std::string &prefix = "") override {
    std::cout << "(return)" << std::endl;
    std::cout << prefix << "`-- ";
    ReturnValue->dump(prefix + "    ");
  }
};

class BreakStatementAST : public StatementAST {
public:
  BreakStatementAST() : StatementAST(BreakStatement) {}

  void dump(const std::string &prefix = "") override {
    std::cout << "(break)" << std::endl;
  }
};

class ContinueStatementAST : public StatementAST {
public:
  ContinueStatementAST() : StatementAST(ContinueStatement) {}

  void dump(const std::string &prefix = "") override {
    std::cout << "(continue)" << std::endl;
  }
};

/************************** Parser class ****************************/
class CMMParser {
public:
  using LocTy = CMMLexer::LocTy;
private:
  SourceMgr &SrcMgr;
  CMMLexer Lexer;
  //BlockAST TopLevelBlock;
  std::map<Token::TokenKind, int8_t> BinOpPrecedence;
private:
  Token::TokenKind getKind() { return Lexer.getKind(); }
  Token Lex() { return Lexer.Lex(); }

  bool Error(LocTy Loc, const std::string &Msg) { return Lexer.Error(Loc, Msg);}
  bool Error(const std::string &Msg) { return Lexer.Error(Msg); }
  void Warning(LocTy Loc, const std::string &Msg) { Lexer.Warning(Loc, Msg); }
  void Warning(const std::string &Msg) { Lexer.Warning(Msg); }

  int8_t getBinOpPrecedence(Token::TokenKind Kind);

  bool parseToplevel();
  bool parseFunctionDefinition();
  bool parseStatement(std::unique_ptr<StatementAST> &Res);
  bool parseBlock();
  bool parseTypeSpecifier(cvm::BasicType &Type);
  bool parseParameterList();
  bool parseArgumentList();
  bool parseExprStatement(std::unique_ptr<StatementAST> &Res);
  bool parseIfStatement(std::unique_ptr<StatementAST> &Res);
  bool parseWhileStatement(std::unique_ptr<StatementAST> &Res);
  bool parseForStatement(std::unique_ptr<StatementAST> &Res);
  bool parseReturnStatement(std::unique_ptr<StatementAST> &Res);
  bool parseBreakStatement(std::unique_ptr<StatementAST> &Res);
  bool parseContinueStatement(std::unique_ptr<StatementAST> &Res);
  bool parseDeclarationStatement(std::unique_ptr<StatementAST> &Res);
  bool parseDeclarationStatement(cvm::BasicType Type,
                                 std::unique_ptr<StatementAST> &Res);
  // First: LParen,Id,Int,Double,Str,Bool,Plus,Minus,Tilde,Exclaim
  bool parseExpression(std::unique_ptr<ExpressionAST> &Res);
  bool parsePrimaryExpression(std::unique_ptr<ExpressionAST> &Res);
  bool parseBinOpRHS(int8_t ExprPrec, std::unique_ptr<ExpressionAST> &Res);
  bool parseParenExpression(std::unique_ptr<ExpressionAST> &Res);
  bool parseIdentifierExpression(std::unique_ptr<ExpressionAST> &Res);
  bool parseConstantExpression(std::unique_ptr<ExpressionAST> &Res);
public:
  CMMParser(SourceMgr &SrcMgr) : SrcMgr(SrcMgr), Lexer(SrcMgr) {
    BinOpPrecedence[Token::Equal] = 1;

    BinOpPrecedence[Token::PipePipe] = 2;

    BinOpPrecedence[Token::AmpAmp] = 3;

    BinOpPrecedence[Token::Pipe] = 4;

    BinOpPrecedence[Token::Caret] = 5;

    BinOpPrecedence[Token::Amp] = 6;

    BinOpPrecedence[Token::EqualEqual] = 7;
    BinOpPrecedence[Token::ExclaimEqual] = 7;

    BinOpPrecedence[Token::Less] = 8;
    BinOpPrecedence[Token::LessEqual] = 8;
    BinOpPrecedence[Token::Greater] = 8;
    BinOpPrecedence[Token::GreaterEqual] = 8;

    BinOpPrecedence[Token::LessLess] = 9;
    BinOpPrecedence[Token::GreaterGreater] = 9;

    BinOpPrecedence[Token::Plus] = 10;
    BinOpPrecedence[Token::Minus] = 10;

    BinOpPrecedence[Token::Star] = 11;
    BinOpPrecedence[Token::Slash] = 11;
    BinOpPrecedence[Token::Percent] = 11;
  }
  bool Parse();
};
}

#endif // !CMMPARSER_H
