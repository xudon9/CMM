#ifndef CMMPARSER_H
#define CMMPARSER_H

#include "CMMLexer.h"
#include "AST.h"
#include <list>
#include <memory>


namespace cmm {
/************************** Parser class ****************************/
class CMMParser {
public:
  using LocTy = CMMLexer::LocTy;
private:
  SourceMgr &SrcMgr;
  CMMLexer Lexer;
  BlockAST TopLevelBlock;
  BlockAST *CurrentBlock;

  std::map<std::string, int8_t> BinOpPrecedence;
  std::map<std::string, FunctionDefinitionAST> FunctionDefinition;
private:
  Token::TokenKind getKind() { return Lexer.getKind(); }
  Token Lex() { return Lexer.Lex(); }

  bool Error(LocTy Loc, const std::string &Msg) { return Lexer.Error(Loc, Msg);}
  bool Error(const std::string &Msg) { return Lexer.Error(Msg); }
  void Warning(LocTy Loc, const std::string &Msg) { Lexer.Warning(Loc, Msg); }
  void Warning(const std::string &Msg) { Lexer.Warning(Msg); }

  int8_t getBinOpPrecedence();

  bool parseToplevel(); //TODO
  bool parseFunctionDefinition();
  bool parseFunctionDefinition(cvm::BasicType Type, const std::string &Name);
  bool parseStatement(std::unique_ptr<StatementAST> &Res);
  bool parseBlock(std::unique_ptr<StatementAST> &Res);
  bool parseTypeSpecifier(cvm::BasicType &Type); //?
  bool parseParameterList(std::list<Parameter> &ParameterList);
  bool parseOptionalArgList(std::list<std::unique_ptr<ExpressionAST>> &ArgList);
  bool parseArgumentList(std::list<std::unique_ptr<ExpressionAST>> &ArgList);
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
  bool parseIdentifierExpression(std::unique_ptr<ExpressionAST> &Res); //TODO
  bool parseConstantExpression(std::unique_ptr<ExpressionAST> &Res);
public:
  CMMParser(SourceMgr &SrcMgr)
    : SrcMgr(SrcMgr), Lexer(SrcMgr), CurrentBlock(&TopLevelBlock) {
  }
  bool Parse();
};
}

#endif // !CMMPARSER_H
