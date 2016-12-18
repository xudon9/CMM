#ifndef AST_H
#define AST_H

#include "SourceMgr.h"
#include "CMMLexer.h"
#include <string>
#include <map>
#include <iostream>
#include <memory>
#include <list>
#include <vector>
#include <cstdlib>

///code.h
namespace cvm {
enum BasicType { BoolType, IntType, DoubleType, StringType, VoidType };
std::string TypeToStr(BasicType Type);

class BasicValue {
public:
  // Public member variables
  BasicType Type;

  std::string StrVal;
  union {
    int IntVal;
    double DoubleVal;
    bool BoolVal;
  };

  std::shared_ptr<std::vector<BasicValue>> ArrayPtr;

public:
  // Public constructors
  BasicValue() : Type(VoidType) {}
  BasicValue(const std::string &S) : Type(StringType), StrVal(S) {}
  BasicValue(int I) : Type(IntType), IntVal(I) {}
  BasicValue(double D) : Type(DoubleType), DoubleVal(D) {}
  BasicValue(bool B) : Type(BoolType), BoolVal(B) {}

  BasicValue(BasicType T);
  BasicValue(BasicType T, const std::list<int> &DimensionList);

private:
  // Private constructor
  BasicValue(BasicType Type,
             std::list<int>::const_iterator It,
             std::list<int>::const_iterator End);

public:
  bool isArray() const { return ArrayPtr != nullptr; }
  bool isInt() const { return Type == IntType; }
  bool isDouble() const { return Type == DoubleType; }
  bool isBool() const { return Type == BoolType; }
  bool isString() const { return Type == StringType; }
  bool isVoid() const { return Type == VoidType; }
  bool isNumeric() const { return isInt() || isDouble(); }

  int toInt() const;
  double toDouble() const;
  bool toBool() const ;
  std::string toString() const;

  bool operator<(const BasicValue &RHS) const;
  bool operator<=(const BasicValue &RHS) const;
  bool operator==(const BasicValue &RHS) const;
  bool operator!=(const BasicValue &RHS) const;
  bool operator>(const BasicValue &RHS) const;
  bool operator>=(const BasicValue &RHS) const;
};
}
///

namespace cmm {

/*
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

class TypeSpecifier {
  cvm::BasicType BasicType;
  DerivedType *Derived;
public:
  TypeSpecifier(cvm::BasicType BasicType)
    : BasicType(BasicType), Derived(nullptr) {}

  cvm::BasicType getBasicType() const {return BasicType; }

  std::string toString() const {
    std::string Res;
    switch (BasicType) {
    default:                Res = "UnknownT"; break;
    case cvm::BoolType:     Res = "BoolT"; break;
    case cvm::IntType:      Res = "IntT"; break;
    case cvm::DoubleType:   Res = "DoubleT"; break;
    case cvm::StringType:   Res = "StringT"; break;
    case cvm::VoidType:     Res = "VoidT";  break;
    }
    return Res;
  }
};
 */

class Parameter {
  std::string Name;
  //std::unique_ptr<TypeSpecifier> Type;
  cvm::BasicType Type;
  SourceMgr::LocTy Loc;
public:
  /*
  Parameter(const std::string &Name, cvm::BasicType Type, SourceMgr::LocTy Loc)
    : Name(Name), Type(new TypeSpecifier(Type)), Loc(Loc) {}
   */
  Parameter(const std::string &Name, cvm::BasicType Type, SourceMgr::LocTy Loc)
      : Name(Name), Type(Type), Loc(Loc) {}

  std::string toString() const {
    return  cvm::TypeToStr(Type) + " " + Name;
  }

 /* hack */
  //cvm::BasicType getType() const { return Type->getBasicType(); }
  cvm::BasicType getType() const { return Type; }
  const std::string &getName() const { return Name; }
};

/*
class FunctionType : public DerivedType {
  std::unique_ptr<TypeSpecifier> Type;
  std::list<Parameter> ParameterList;
};
 */

class AST {
public:
  virtual ~AST() {};
  virtual void dump(const std::string &prefix = "") const {};
};

class ExpressionAST : public AST {
public:
  enum ExpressionKind {
    IntExpression,
    DoubleExpression,
    BoolExpression,
    StringExpression,
    IdentifierExpression,
    FunctionCallExpression,
    InfixOpExpression,
    BinaryOperatorExpression,
    UnaryOperatorExpression
  };
private:
  ExpressionKind Kind;
public:
  // Constructor
  ExpressionAST(ExpressionKind Kind) : Kind(Kind) {}

  // Other public member functions
  template <typename T>
  const T *as_cptr() const { return static_cast<const T*>(this); }

  ExpressionKind getKind() const { return Kind; }

  bool isIdentifierExpr() const { return getKind() == IdentifierExpression; }
  bool isBinaryOperatorExpression() const {
    return getKind() == BinaryOperatorExpression;
  }
  bool isInt() { return getKind() == IntExpression; }
  bool isDouble() { return getKind() == DoubleExpression; }
  bool isBool() { return getKind() == BoolExpression; }
  bool isString() { return getKind() == StringExpression; }
  bool isNumeric() { return isInt() || isDouble(); }
  bool isConstant() { return isInt() || isDouble() || isBool() || isString(); }

  int asInt() const;
  bool asBool() const;
  double asDouble() const;
  std::string asString() const;
};


class StatementAST : public AST {
public:
  enum StatementKind {
    ExprStatement,
    BlockStatement,
    IfStatement,
    WhileStatement,
    ForStatement,
    ReturnStatement,
    ContinueStatement,
    BreakStatement,
    DeclarationStatement,
    DeclarationListStatement
  };
private:
  StatementKind Kind;
public:
  StatementAST(StatementKind Kind) : Kind(Kind) {}
  StatementKind getKind() const { return Kind; }
  // bool isBlock() { return Kind == BlockStatement; }
  // bool isIfStatement() { return Kind == IfStatement; }
  // bool isWhileStatement() { return Kind == WhileStatement; }
};

class IntAST : public ExpressionAST {
  int Value;
public:
  IntAST(int Value) : ExpressionAST(IntExpression), Value(Value) {}
  void dump(const std::string &prefix = "") const override {
    std::cout << "(int)" << Value << std::endl;
  }
  int getValue() const { return Value; }
};

class DoubleAST : public ExpressionAST {
  double Value;
public:
  DoubleAST(double Value) : ExpressionAST(DoubleExpression), Value(Value) {}
  void dump(const std::string &prefix = "") const override {
    std::cout << "(double)" << Value << std::endl;
  }
  double getValue() const { return Value; }
};

class BoolAST : public ExpressionAST {
  bool Value;
public:
  BoolAST(bool Value) : ExpressionAST(BoolExpression), Value(Value) {}
  void dump(const std::string &prefix = "") const override {
    std::cout << "(bool)" << (Value ? "true" : "false") << std::endl;
  }
  bool getValue() const { return Value; }
};

class StringAST : public ExpressionAST {
  std::string Value;
public:
  StringAST(const std::string &Value)
    : ExpressionAST(StringExpression), Value(Value) {}
  void dump(const std::string &prefix = "") const override {
    std::cout << "(str)" << Value << std::endl;
  }
  const std::string &getValue() const { return Value; }
};

class IdentifierAST : public ExpressionAST {
  std::string Name;
public:
  IdentifierAST(const std::string &Name)
    : ExpressionAST(IdentifierExpression), Name(Name) {}
  void dump(const std::string &prefix = "") const override {
    std::cout << "(id)" << Name << std::endl;
  }
  const std::string &getName() const { return Name; }
};

class InfixOpExprAST : public ExpressionAST {
  std::string Symbol;
  std::unique_ptr<ExpressionAST> LHS, RHS;
public:
  InfixOpExprAST(const std::string &Symbol,
                 std::unique_ptr<ExpressionAST> LHS,
                 std::unique_ptr<ExpressionAST> RHS)
      : ExpressionAST(InfixOpExpression)
      , Symbol(Symbol), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

  const std::string &getSymbol() const { return Symbol; }
  const ExpressionAST *getLHS() const { return LHS.get(); }
  const ExpressionAST *getRHS() const { return RHS.get(); }

  void dump(const std::string &prefix = "") const override {
    std::cout << getSymbol() << std::endl;
    std::cout << prefix << "|---";
    LHS->dump(prefix + "|   ");
    std::cout << prefix << "`---";
    RHS->dump(prefix + "    ");
  }
};

class FunctionCallAST : public ExpressionAST {
  std::string Callee;
  std::list<std::unique_ptr<ExpressionAST>> Arguments;
public:
  FunctionCallAST(const std::string &Callee,
                  std::list<std::unique_ptr<ExpressionAST>> Arguments)
    : ExpressionAST(FunctionCallExpression), Callee(Callee)
    , Arguments(std::move(Arguments)) {}

  const std::string &getCallee() const  { return Callee; }
  const decltype(Arguments) &getArguments() const { return Arguments; }

  void dump(const std::string &prefix = "") const override {
    std::cout << "(call)" << Callee << std::endl;
    for (const auto &Arg : Arguments) {
      if (Arg != Arguments.back()) {
        std::cout << prefix << "|---";
        Arg->dump(prefix + "|   ");
      } else {
        std::cout << prefix << "`---";
        Arg->dump(prefix + "    ");
      }
    }
  }
};

class BinaryOperatorAST : public ExpressionAST {
public:
  enum OperatorKind {
    Add, Minus, Multiply, Division, Modulo,   /* arithmetic */
    LogicalAnd, LogicalOr,                    /* logical */
    Less, LessEqual, Equal, NotEqual, Greater, GreaterEqual, /* relational */
    BitwiseAnd, BitwiseOr, BitwiseXor, LeftShift, RightShift, /* bitwise */
    Assign /**, Comma**/, Index
  };
private:
  OperatorKind OpKind;
  std::unique_ptr<ExpressionAST> LHS, RHS;
public:
  BinaryOperatorAST(OperatorKind OpKind,
                    std::unique_ptr<ExpressionAST> LHS,
                    std::unique_ptr<ExpressionAST> RHS)
    : ExpressionAST(BinaryOperatorExpression)
    , OpKind(OpKind), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

  OperatorKind getOpKind() const { return OpKind; }
  ExpressionAST *getLHS() const { return LHS.get(); }
  ExpressionAST *getRHS() const { return RHS.get(); }

  void dump(const std::string &prefix = "") const override {
    std::string OperatorSymbol;
    switch (OpKind) {
    default: break;
    case Add:           OperatorSymbol = "Add"; break;
    case Minus:         OperatorSymbol = "Sub"; break;
    case Multiply:      OperatorSymbol = "Mul"; break;
    case Division:      OperatorSymbol = "Div"; break;
    case Modulo:        OperatorSymbol = "Mod"; break;
    case LogicalAnd:    OperatorSymbol = "And"; break;
    case LogicalOr:     OperatorSymbol = "Or"; break;
    case Less:          OperatorSymbol = "Less"; break;
    case LessEqual:     OperatorSymbol = "LessEq"; break;
    case Equal:         OperatorSymbol = "Equal"; break;
    case NotEqual:      OperatorSymbol = "NotEq"; break;
    case Greater:       OperatorSymbol = "Greater"; break;
    case BitwiseAnd:    OperatorSymbol = "BitAnd"; break;
    case BitwiseOr:     OperatorSymbol = "BitOr"; break;
    case BitwiseXor:    OperatorSymbol = "Xor"; break;
    case LeftShift:     OperatorSymbol = "LShift"; break;
    case RightShift:    OperatorSymbol = "RShift"; break;
    case Assign:        OperatorSymbol = "Assign"; break;
    case GreaterEqual:  OperatorSymbol = "GreaterEq"; break;
    case Index:         OperatorSymbol = "At"; break;
    }
    std::cout << OperatorSymbol << std::endl;
    std::cout << prefix << "|---";
    LHS->dump(prefix + "|   ");
    std::cout << prefix << "`---";
    RHS->dump(prefix + "    ");
  }

  static std::unique_ptr<ExpressionAST>
    create(Token::TokenKind TokenKind,
           std::unique_ptr<ExpressionAST> LHS,
           std::unique_ptr<ExpressionAST> RHS);
};

class UnaryOperatorAST : public ExpressionAST {
public:
  enum OperatorKind { Plus, Minus, LogicalNot, BitwiseNot };
private:
  OperatorKind OpKind;
  std::unique_ptr<ExpressionAST> Operand;
public:
  UnaryOperatorAST(OperatorKind Kind, std::unique_ptr<ExpressionAST> Operand)
    : ExpressionAST(UnaryOperatorExpression)
    , OpKind(Kind), Operand(std::move(Operand)) {}

  OperatorKind getOpKind() const { return OpKind; }
  const ExpressionAST *getOperand() const { return Operand.get(); }

  void dump(const std::string &prefix = "") const override {
    std::string OperatorSymbol;
    switch (OpKind) {
    default: break;
    case Plus:       OperatorSymbol = "Positive"; break;
    case Minus:      OperatorSymbol = "Negative"; break;
    case LogicalNot: OperatorSymbol = "Not"; break;
    case BitwiseNot: OperatorSymbol = "BitNot"; break;
    }
    std::cout << "(" << OperatorSymbol << ")" << std::endl;
    std::cout << prefix << "`---";
    Operand->dump(prefix + "    ");
  }
};

class DeclarationAST : public StatementAST {
  std::string Name;
  //std::unique_ptr<TypeSpecifier> Type;
  cvm::BasicType Type;
  std::unique_ptr<ExpressionAST> Initializer;
  std::list<std::unique_ptr<ExpressionAST>> ElementCountList;
public:
  DeclarationAST(const std::string &Name, cvm::BasicType Type,
                 std::unique_ptr<ExpressionAST> Initializer,
                 std::list<std::unique_ptr<ExpressionAST>> ElementCountList)
    : StatementAST(DeclarationStatement), Name(Name), Type(Type)
    , Initializer(std::move(Initializer))
    , ElementCountList(std::move(ElementCountList)) {}

  bool isArray() const { return !ElementCountList.empty(); }

  const std::string &getName() const { return Name; }

  cvm::BasicType getType() const { return Type; }

  const ExpressionAST *getInitializer() const { return Initializer.get(); }

  const decltype(ElementCountList) &getElementCountList() const {
      return ElementCountList;
  }


  void dump(const std::string &prefix = "") const override {
    std::cout << cvm::TypeToStr(Type) << " " << Name << std::endl;
    if (Initializer) {
      std::cout << prefix << " `==";
      Initializer->dump(prefix + "    ");
    } else if (isArray()) {
      for (const auto &E : ElementCountList) {
        if (E != ElementCountList.back()) {
          std::cout << prefix << "|-[]";
          E->dump(prefix + "|   ");
        } else {
          std::cout << prefix << "`-[]";
          E->dump(prefix + "    ");
        }
      }
    }
  }
};

class DeclarationListAST : public StatementAST {
  cvm::BasicType Type;
  std::list<std::unique_ptr<DeclarationAST>> DeclarationList;
public:
  DeclarationListAST(cvm::BasicType Type)
    : StatementAST(DeclarationListStatement), Type(Type) {}

  void addDeclaration(const std::string &Name,
                      std::unique_ptr<ExpressionAST> I,
                      std::list<std::unique_ptr<ExpressionAST>> C) {
    DeclarationList.emplace_back(new DeclarationAST(Name, Type, std::move(I),
                                                                std::move(C)));
  }

  const std::list<std::unique_ptr<DeclarationAST>> &getDeclarationList() const {
    return DeclarationList;
  }

  void dump(const std::string &prefix = "") const override {
    std::cout << "(Decl)" << cvm::TypeToStr(Type) << std::endl;
    for (const auto &Declare : DeclarationList) {
      if (Declare != DeclarationList.back()) {
        std::cout << prefix << "|---";
        Declare->dump(prefix + "|   ");
      } else {
        std::cout << prefix << "`---";
        Declare->dump(prefix + "    ");
      }
    }
  }
};

class BlockAST : public StatementAST {
public:
  enum BlockKind { UndefinedBlock, FunctionBlock, NormalBlock };
private:
  BlockAST *OuterBlock;
  std::list<std::unique_ptr<StatementAST>> StatementList;
  std::list<std::unique_ptr<DeclarationAST>> DeclarationList;
public:
  BlockAST(BlockAST *OuterBlock = nullptr)
    : StatementAST(BlockStatement), OuterBlock(OuterBlock) {}
  void addStatement(std::unique_ptr<StatementAST> Statement) {
    StatementList.push_back(std::move(Statement));
  }
  void addDeclaration(std::unique_ptr<DeclarationAST> Declaration) {
    DeclarationList.push_back(std::move(Declaration));
  }
  const decltype(StatementList) &getStatementList() const {
    return StatementList;
  }
  const decltype(DeclarationList) &getDeclarationList() const {
    return DeclarationList;
  }
  BlockAST *getOuterBlock() const { return OuterBlock; }
  std::list<std::unique_ptr<StatementAST>> &getStatementList() {
    return StatementList;
  }

  void dump(const std::string &prefix = "") const override {
    std::cout << "(Block)\n";
    for (const auto &Statement : StatementList) {
      if (Statement != StatementList.back()) {
        std::cout << prefix << "|---";
        Statement->dump(prefix + "|   ");
      } else {
        std::cout << prefix << "`---";
        Statement->dump(prefix + "    ");
      }
    }
  }
};

class ExprStatementAST : public StatementAST {
  std::unique_ptr<ExpressionAST> Expression;
public:
  ExprStatementAST(std::unique_ptr<ExpressionAST> Expression)
    : StatementAST(ExprStatement), Expression(std::move(Expression)) {}

  const ExpressionAST *getExpression() const { return Expression.get(); }

  void dump(const std::string &prefix) const override {
    std::cout << "(ExprStmt)" << std::endl;
    std::cout << prefix << "`---";
    Expression->dump(prefix + "    ");
  }
};

// class StatementBlockAST : public BlockAST {
//   // TODO? Statement   *statement;
//   size_t ContinueLabel;
//   size_t BreakLabel;
// };

class InfixOpDefinitionAST {
  std::string Symbol;
  std::string LHSName, RHSName;
  std::unique_ptr<StatementAST> Statement;

public:
  InfixOpDefinitionAST(const std::string &Sym,
                       const std::string &LHS,
                       const std::string &RHS,
                       std::unique_ptr<StatementAST> Stmt)
  : Symbol(Sym), LHSName(LHS), RHSName(RHS), Statement(std::move(Stmt)) {}

  const std::string &getSymbol() const { return Symbol; }
  const std::string &getLHSName() const { return LHSName; }
  const std::string &getRHSName() const { return RHSName; }
  const StatementAST *getStatement() const { return Statement.get(); }

  void dump() const {
    std::cout << "infix " << getLHSName() << " " << getSymbol() << " "
        << getRHSName() << "\n";
    Statement->dump();
  }
};

class FunctionDefinitionAST /*: public StatementAST*/ {
  std::string Name;
  cvm::BasicType Type;
  std::list<Parameter> ParameterList;
  std::unique_ptr<StatementAST> Statement;
  // std::list<std::unique_ptr<DeclarationAST>> LocalVariableList;
  // int Index;
public:
  FunctionDefinitionAST() = default;
  FunctionDefinitionAST(const std::string &Name,
                        cvm::BasicType Type,
                        std::list<Parameter> &&ParameterList,
                        std::unique_ptr<StatementAST> Statement)
    : Name(Name), Type(Type), ParameterList(std::move(ParameterList))
    , Statement(std::move(Statement)) {}

  cvm::BasicType getType() const { return Type; }
  const std::string &getName() const { return Name; }
  size_t getParameterCount() const { return ParameterList.size(); }
  const std::list<Parameter> &getParameterList() const { return ParameterList; }
  const StatementAST *getStatement() const { return Statement.get(); }

  void dump() const {
    std::cout << cvm::TypeToStr(Type) << " " << Name << "(";
    for (const auto &P : ParameterList) {
      std::cout << P.toString() << ", ";
    }
    std::cout << ")\n";
    Statement->dump();
  }
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

  const ExpressionAST *getCondition() const { return Condition.get(); }
  const StatementAST *getStatementThen() const { return StatementThen.get(); }
  const StatementAST *getStatementElse() const { return StatementElse.get(); }

  void dump(const std::string &prefix = "") const override {
    std::cout << "if" << std::endl;
    std::cout << prefix << "|---";
    Condition->dump(prefix + "|   ");
    if (StatementElse) {
      std::cout << prefix << "|---";
      StatementThen->dump(prefix + "|   ");
      std::cout << prefix << "`---";
      StatementElse->dump(prefix + "    ");
    } else {
      std::cout << prefix << "`---";
      StatementThen->dump(prefix + "    ");
    }
  }
};

class WhileStatementAST : public StatementAST {
  std::unique_ptr<ExpressionAST> Condition;
  std::unique_ptr<StatementAST> Statement;
public:
  WhileStatementAST(std::unique_ptr<ExpressionAST> Condition,
                    std::unique_ptr<StatementAST> Statement)
    : StatementAST(WhileStatement)
    , Condition(std::move(Condition))
    , Statement(std::move(Statement)) {}

  const ExpressionAST *getCondition() const { return Condition.get(); }
  const StatementAST *getStatement() const { return Statement.get(); }

  void dump(const std::string &prefix) const override {
    std::cout << "while" << std::endl;
    std::cout << prefix << "|---";
    Condition->dump(prefix + "|   ");
    std::cout << prefix << "`---";
    Statement->dump(prefix + "    ");
  }
};

class ForStatementAST : public StatementAST {
  std::unique_ptr<ExpressionAST> Init;
  std::unique_ptr<ExpressionAST> Condition;
  std::unique_ptr<ExpressionAST> Post;
  std::unique_ptr<StatementAST> Statement;
public:
  ForStatementAST(std::unique_ptr<ExpressionAST> Init,
                  std::unique_ptr<ExpressionAST> Condition,
                  std::unique_ptr<ExpressionAST> Post,
                  std::unique_ptr<StatementAST> Statement)
    : StatementAST(ForStatement)
    , Init(std::move(Init)), Condition(std::move(Condition))
    , Post(std::move(Post)), Statement(std::move(Statement)) {}

  const ExpressionAST *getInit() const { return Init.get(); }
  const ExpressionAST *getCondition() const { return Condition.get(); }
  const ExpressionAST *getPost() const { return Post.get(); }
  const StatementAST *getStatement() const { return Statement.get(); }

  void dump(const std::string &prefix) const override {
    std::cout << "for" << std::endl;

    std::cout << prefix << "|--+";
    if (Init)   Init->dump(prefix + "|   ");
    else        std::cout << "(nullInit)\n";

    std::cout << prefix << "|--+";
    if (Condition)  Condition->dump(prefix + "|   ");
    else            std::cout << "(forever)\n";

    std::cout << prefix << "|--+";
    if (Post)   Post->dump(prefix + "|   ");
    else        std::cout << "(nullPost)\n";

    std::cout << prefix << "`---"; Statement->dump(prefix + "    ");
  }
};

class ReturnStatementAST : public StatementAST {
  std::unique_ptr<ExpressionAST> ReturnValue;
public:
  ReturnStatementAST(std::unique_ptr<ExpressionAST> ReturnValue)
    : StatementAST(ReturnStatement), ReturnValue(std::move(ReturnValue)) {}

  const ExpressionAST *getReturnValue() const { return ReturnValue.get(); }

  void dump(const std::string &prefix = "") const override {
    std::cout << "return" << std::endl;
    if (!ReturnValue)
      return;
    std::cout << prefix << "`---";
    ReturnValue->dump(prefix + "    ");
  }
};

class BreakStatementAST : public StatementAST {
public:
  BreakStatementAST() : StatementAST(BreakStatement) {}

  void dump(const std::string &prefix = "") const override {
    std::cout << "break" << std::endl;
  }
};

class ContinueStatementAST : public StatementAST {
public:
  ContinueStatementAST() : StatementAST(ContinueStatement) {}

  void dump(const std::string &prefix = "") const override {
    std::cout << "continue" << std::endl;
  }
};

}

#endif // !AST_H
