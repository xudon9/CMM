#include "CMMParser.h"
#include <cassert>

using namespace cmm;

std::unique_ptr<ExpressionAST> BinaryOperatorAST::create(
    Token::TokenKind TokenKind,
    std::unique_ptr<ExpressionAST> LHS, std::unique_ptr<ExpressionAST> RHS) {

  BinaryOperatorAST::OperatorKind OpKind;
  switch (TokenKind) {
  default: return nullptr;
  case Token::Plus:           OpKind = BinaryOperatorAST::Add; break;
  case Token::Minus:          OpKind = BinaryOperatorAST::Minus; break;
  case Token::Star:           OpKind = BinaryOperatorAST::Multiply; break;
  case Token::Slash:          OpKind = BinaryOperatorAST::Division; break;
  case Token::Percent:        OpKind = BinaryOperatorAST::Modulo; break;
  case Token::AmpAmp:         OpKind = BinaryOperatorAST::LogicalAnd; break;
  case Token::PipePipe:       OpKind = BinaryOperatorAST::LogicalOr; break;
  case Token::Less:           OpKind = BinaryOperatorAST::Less; break;
  case Token::LessEqual:      OpKind = BinaryOperatorAST::LessEqual; break;
  case Token::EqualEqual:     OpKind = BinaryOperatorAST::Equal; break;
  case Token::GreaterEqual:   OpKind = BinaryOperatorAST::GreaterEqual; break;
  case Token::Greater:        OpKind = BinaryOperatorAST::Greater; break;
  case Token::Amp:            OpKind = BinaryOperatorAST::BitwiseAnd; break;
  case Token::Pipe:           OpKind = BinaryOperatorAST::BitwiseOr; break;
  case Token::Caret:          OpKind = BinaryOperatorAST::BitwiseXor; break;
  case Token::LessLess:       OpKind = BinaryOperatorAST::LeftShift; break;
  case Token::GreaterGreater: OpKind = BinaryOperatorAST::RightShift; break;
  case Token::Equal:          OpKind = BinaryOperatorAST::Assign; break;
  }

  return std::unique_ptr<ExpressionAST>(new BinaryOperatorAST(OpKind,
                                                              std::move(LHS),
                                                              std::move(RHS)));
}

bool CMMParser::Parse() {
  // TODO: temp code.
  Lex();
//  std::unique_ptr<StatementAST> Statement;
//  while (Lexer.isNot(Token::Eof)) {
//    if (parseStatement(Statement))
//      return true;
//    Statement->dump();
//  }
  while (Lexer.isNot(Token::Eof)) {
    parseFunctionDefinition(cvm::VoidType, "foo");
  }
  return false;
}

/// \brief Parse a function definition
/// functionDefinition ::= ( ) Statement
/// functionDefinition ::= ( parameterList ) Statement
bool CMMParser::parseFunctionDefinition(cvm::BasicType Type,
                                        const std::string &Name) {
  assert(Lexer.is(Token::LParen));
  Lex();  // Eat LParen '('.

  std::list<Parameter> ParameterList;
  if (Lexer.isNot(Token::RParen))
    parseParameterList(ParameterList);
  if (Lexer.isNot(Token::RParen))
    return Error("right parenthesis expected");
  Lex();  // Eat RParen ')'.

  std::unique_ptr<StatementAST> Statement;
  if (parseStatement(Statement))
    return true;

  for (auto &P : ParameterList) {
    std::cout << P.toString() << std::endl;
  }
  Statement->dump();
  return false;
}

/// \brief Parse a parameter list
/// parameterList ::= void
/// parameterList ::= TypeSpecifier Identifier (, TypeSpecifier Identifier)*
bool CMMParser::parseParameterList(std::list<Parameter> &ParameterList) {
  if (Lexer.is(Token::Kw_void)) {
    Lex();
    return false;
  }
  for (;;) {
    std::string Identifier;
    cvm::BasicType Type;
    LocTy Loc;
    if (parseTypeSpecifier(Type))
      return true;
    Loc = Lexer.getLoc();
    if (Lexer.is(Token::Identifier)) {
      Identifier = Lexer.getStrVal();
      Lex();
    } else {
      Warning("missing identifier after type");
    }

    ParameterList.emplace_back(Identifier, Type, Loc);

    if (Lexer.isNot(Token::Comma))
      break;
    Lex();  // Eat the comma.
  }
  return false;
}

/// \brief Parse a block as a statement
/// block ::= { statement* } ;
bool CMMParser::parseBlock(std::unique_ptr<StatementAST> &Res) {
  CurrentBlock = new BlockAST(CurrentBlock);
  Res.reset(CurrentBlock);

  assert(Lexer.is(Token::LCurly) && "first token in parseBlock()");
  Lex(); // eat the LCurly '{'

  while (Lexer.isNot(Token::RCurly)) {
    std::unique_ptr<StatementAST> Statement;
    if (parseStatement(Statement))
      return true;
    CurrentBlock->addStatement(std::move(Statement));
  }

  assert(CurrentBlock == Res.get() && "block mismatch");
  CurrentBlock = CurrentBlock->getOuterBlock();

  Lex(); // eat the RCurly '}'
  return false;
}

bool CMMParser::parseTypeSpecifier(cvm::BasicType &Type) {
  switch (getKind()) {
  default:                return Error("unknown type specifier");
  case Token::Kw_bool:    Type = cvm::BoolType; break;
  case Token::Kw_int:     Type = cvm::IntType; break;
  case Token::Kw_double:  Type = cvm::DoubleType; break;
  case Token::Kw_void:    Type = cvm::VoidType; break;
  }
  Lex();
  return false;
}

bool CMMParser::parseArgumentList() {
  return false;
}

bool CMMParser::parseStatement(std::unique_ptr<StatementAST> &Res) {
  switch (getKind()) {
  default:
    return Error("unexpected token in statement");
  case Token::LCurly:       return parseBlock(Res);
  case Token::Kw_if:        return parseIfStatement(Res);
  case Token::Kw_while:     return parseWhileStatement(Res);
  case Token::Kw_for:       return parseForStatement(Res);
  case Token::Kw_return:    return parseReturnStatement(Res);
  case Token::Kw_break:     return parseBreakStatement(Res);
  case Token::Kw_continue:  return parseContinueStatement(Res);
  case Token::Kw_bool:
  case Token::Kw_int:
  case Token::Kw_double:
  case Token::Kw_void:
    return parseDeclarationStatement(Res);
  case Token::LParen:   case Token::Identifier:
  case Token::Double:   case Token::String:
  case Token::Boolean:  case Token::Integer:
  case Token::Plus:     case Token::Minus:
  case Token::Tilde:    case Token::Exclaim: 
    return parseExprStatement(Res);
  }
}

bool CMMParser::parseExpression(std::unique_ptr<ExpressionAST> &Res) {
  return parsePrimaryExpression(Res) || parseBinOpRHS(1, Res);
}

int8_t CMMParser::getBinOpPrecedence(Token::TokenKind Kind) {
  //TODO
  return BinOpPrecedence[Kind];
}

/// \brief Parse a paren expression and return it.
/// parenExpr ::= (expr)
bool CMMParser::parseParenExpression(std::unique_ptr<ExpressionAST> &Res) {
  Lex(); // eat the '('.
  if (parseExpression(Res))
    return true;
  if (Lexer.isNot(Token::RParen))
     return Error("expected ')' in parentheses expression");
  Lex(); // eat the ')'.
  return false;
}

/// \brief Parse a primary expression and return it.
///  primaryExpr ::= parenExpr
///  primaryExpr ::= identifierExpr
///  primaryExpr ::= constantExpr
///  primaryExpr ::= ~,+,-,! primaryExpr
bool CMMParser::parsePrimaryExpression(std::unique_ptr<ExpressionAST> &Res) {
  UnaryOperatorAST::OperatorKind UnaryOpKind;
  std::unique_ptr<ExpressionAST> Operand;

  switch (getKind()) {
  default:
    return Error("unexpected token in expression");
  case Token::LParen:
    return parseParenExpression(Res);
  case Token::Identifier:
    return parseIdentifierExpression(Res);
  case Token::Integer:
  case Token::Double:
  case Token::String:
  case Token::Boolean:
    return parseConstantExpression(Res);
  case Token::Plus:     UnaryOpKind = UnaryOperatorAST::Plus; break;
  case Token::Minus:    UnaryOpKind = UnaryOperatorAST::Minus; break;
  case Token::Tilde:    UnaryOpKind = UnaryOperatorAST::BitwiseNot; break;
  case Token::Exclaim:  UnaryOpKind = UnaryOperatorAST::LogicalNot; break;
  }

  Lex(); // Eat the operator: +,-,~,!
  if (parsePrimaryExpression(Operand))
    return true;
  Res.reset(new UnaryOperatorAST(UnaryOpKind, std::move(Operand)));
  return false;
}

bool CMMParser::parseBinOpRHS(int8_t ExprPrec,
                              std::unique_ptr<ExpressionAST> &Res) {
  std::unique_ptr<ExpressionAST> RHS;

  // Handle assignment expression first.
  if (Lexer.getTok().is(Token::Equal)) {
    Lex();
    if (parseExpression(RHS))
      return true;
    Res = BinaryOperatorAST::create(Token::Equal, std::move(Res),
      std::move(RHS));
    return false;
  }
  for (;;) {
    Token::TokenKind TokenKind = getKind();

    // If this is a binOp, find its precedence.
    int8_t TokPrec = getBinOpPrecedence(TokenKind);
    // If the next token is lower precedence than we are allowed to eat,
    // return successfully with what we ate already.
    if (TokPrec < ExprPrec)
      return false;

    // Eat the binary operator.
    Lex();
    // Eat the next primary expression.
    if (parsePrimaryExpression(RHS))
      return true;

    // If BinOp binds less tightly with RHS than the operator after RHS, let
    // the pending operator take RHS as its LHS.
    int8_t NextPrec = getBinOpPrecedence(getKind());
    if (TokPrec < NextPrec && parseBinOpRHS(++TokPrec, RHS))
      return true;
    
    // Merge LHS and RHS according to operator.
    Res = BinaryOperatorAST::create(TokenKind, std::move(Res), std::move(RHS));
  }
}

/// \brief Parse an identifier expression
/// identifierExpression ::= identifier
/// identifierExpression ::= identifier '(' [argumentList] ')'
bool CMMParser::parseIdentifierExpression(std::unique_ptr<ExpressionAST> &Res) {
  // TODO: handle function call in this function
  assert(Lexer.is(Token::Identifier));
  Res.reset(new IdentifierAST(Lexer.getStrVal()));
  Lex(); // eat the identifier
  return false;
}

/// \brief Parse a constant expression.
/// constantExpr ::= IntExpression
/// constantExpr ::= DoubleExpression
/// constantExpr ::= BoolExpression
/// constantExpr ::= StringExpression
bool CMMParser::parseConstantExpression(std::unique_ptr<ExpressionAST> &Res) {
  switch (getKind()) {
  default:  return Error("unknown token in literal constant expression");
  case Token::Integer:  Res.reset(new IntAST(Lexer.getIntVal())); break;
  case Token::Double:   Res.reset(new DoubleAST(Lexer.getDoubleVal())); break;
  case Token::Boolean:  Res.reset(new BoolAST(Lexer.getBoolVal())); break;
  case Token::String:   Res.reset(new StringAST(Lexer.getStrVal())); break;
  }
  Lex(); // eat the string,bool,int,double.
  return false;
}

/// \brief Parse an if statement.
/// ifStatement ::= if ( expr ) statement
/// ifStatement ::= if ( expr ) statement else statement
bool CMMParser::parseIfStatement(std::unique_ptr<StatementAST> &Res) {
  std::unique_ptr<ExpressionAST> Condition;
  std::unique_ptr<StatementAST> StatementThen, StatementElse;

  assert(Lexer.is(Token::Kw_if));
  Lex();  // eat 'if'.
  if (Lexer.isNot(Token::LParen))
    return Error("left parenthesis expected");
  Lex();  // eat LParen '('.
  if (parseExpression(Condition))
    return true;
  if (Lexer.isNot(Token::RParen))
    return Error("right parenthesis expected");
  Lex();  // eat RParen ')'.
  if (parseStatement(StatementThen))
    return true;
  // Parse the else branch is there is one.
  if (Lexer.is(Token::Kw_else)) {
    Lex();  // eat 'else'
    if (parseStatement(StatementElse))
      return true;
  }
  Res.reset(new IfStatementAST(std::move(Condition), std::move(StatementThen),
                               std::move(StatementElse)));
  return false;
}

/// \brief Parse a for statement
/// forStatement ::= for ( expr;expr;expr ) statement
bool CMMParser::parseForStatement(std::unique_ptr<StatementAST> &Res) {
  std::unique_ptr<ExpressionAST> Init, Condition, Post;
  std::unique_ptr<StatementAST> Statement;

  assert(Lexer.is(Token::Kw_for));
  Lex();  // eat the 'for'.
  if (Lexer.isNot(Token::LParen))
    return Error("left parenthesis expected in for loop");
  Lex();  // eat the LParen '('.

  if (parseExpression(Init))
    return true;
  if (Lexer.isNot(Token::Semicolon))
    return Error("missing semicolon for initial expression in for loop");
  Lex();  // eat the semicolon.

  if (parseExpression(Condition))
    return true;
  if (Lexer.isNot(Token::Semicolon))
    return Error("missing semicolon for conditional expression in for loop");
  Lex();  // eat the semicolon.

  if (parseExpression(Post))
    return true;
  if (Lexer.isNot(Token::RParen))
    return Error("missing semicolon for post expression in for loop");
  Lex();  // eat the ')'.

  if (parseStatement(Statement))
    return true;
  Res.reset(new ForStatementAST(std::move(Init), std::move(Condition),
                                std::move(Post), std::move(Statement)));
  return false;
}

/// \brief Parse a while statement.
/// whileStatement ::= while ( expr ) statement
bool CMMParser::parseWhileStatement(std::unique_ptr<StatementAST> &Res) {
  std::unique_ptr<ExpressionAST> Condition;
  std::unique_ptr<StatementAST> Statement;

  assert(Lexer.is(Token::Kw_while));
  Lex();  // eat 'while'
  if (Lexer.isNot(Token::LParen))
    return Error("left parenthesis expected in while loop");
  Lex();  // eat LParen '('.
  if (parseExpression(Condition))
    return true;
  if (Lexer.isNot(Token::RParen))
    return Error("right parenthesis expected in while loop");
  Lex();  // eat RParen ')'.
  if (parseStatement(Statement))
    return true;
  Res.reset(new WhileStatementAST(std::move(Condition), std::move(Statement)));
  return false;
}

/// \brief Parse an expression statement.
/// exprStatement ::= expression ;
bool CMMParser::parseExprStatement(std::unique_ptr<StatementAST> &Res) {
  std::unique_ptr<ExpressionAST> Expression;
  if (parseExpression(Expression))
    return true;
  if (Lexer.isNot(Token::Semicolon))
    return Error("missing semicolon in statement");
  Lex();  // eat the semicolon
  Res.reset(new ExprStatementAST(std::move(Expression)));
  return false;
}

/// \brief Parse a return statement.
/// returnStatement ::= return ;
/// returnStatement ::= return Expr ;
bool CMMParser::parseReturnStatement(std::unique_ptr<StatementAST> &Res) {
  std::unique_ptr<ExpressionAST> ReturnValue;

  assert(Lexer.is(Token::Kw_return));
  Lex();  // eat the 'return'.

  if (Lexer.isNot(Token::Semicolon) && parseExpression(ReturnValue))
      return true;
  if (Lexer.isNot(Token::Semicolon))
    return Error("unexpected token after return value");
  Lex();  // eat the semicolon.
  Res.reset(new ReturnStatementAST(std::move(ReturnValue)));
  return false;
}

/// \brief Parse a break statement.
/// breakStatement ::= break ;
bool CMMParser::parseBreakStatement(std::unique_ptr<StatementAST> &Res) {
  assert(Lexer.is(Token::Kw_break));
  Lex();  // eat the 'break'.
  if (Lexer.isNot(Token::Semicolon))
    return Error("unexpected token after break");
  Lex();  // eat the semicolon.
  Res.reset(new BreakStatementAST);
  return false;
}

/// \brief Parse a continue statement.
/// continueStatement ::= continue ;
bool CMMParser::parseContinueStatement(std::unique_ptr<StatementAST> &Res) {
  assert(Lexer.is(Token::Kw_continue));
  Lex();  // eat the 'continue'.
  if (Lexer.isNot(Token::Semicolon))
    return Error("unexpected token after continue");
  Lex();  // eat the semicolon
  Res.reset(new ContinueStatementAST);
  return false;
}

bool CMMParser::parseDeclarationStatement(std::unique_ptr<StatementAST> &Res) {
  return false;
}
