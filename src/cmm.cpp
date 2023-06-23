/*
 * cmm.cpp
 * Copyright (C) 2016 wang <hsu [AT] whu [DOT] edu [DOT] cn>
 *
 * TODO:
 * 1. ++ -- += -= *=...
 */

#include <cstdlib>
#include <iostream>
#include <cstring>
#include "CMMLexer.h"
#include "CMMParser.h"
#include "CMMInterpreter.h"

static void Error(const char *Name, const char *Msg);

static void Usage(const char *Name);
static int DumpFile(cmm::SourceMgr &SrcMgr);
static int AsLexInput(cmm::SourceMgr &SrcMgr);
static int Interpret(cmm::SourceMgr &SrcMgr, int Argc, char **Argv,
                     bool Verbose = false);
static int DumpAST(cmm::SourceMgr &SrcMgr);

static bool EqualOneOf(const char *S, const char *S1) {
  return !std::strcmp(S, S1);
}
template <typename... Tn>
static bool EqualOneOf(const char *S, const char *S1, Tn... Sn) {
  return EqualOneOf(S, S1) || EqualOneOf(S, Sn...);
}

int main(int argc, char *argv[])
{
  enum ActionKind {
    DefaultAct, LexAct, ParseAct, DebugAct, DumpFileAct
  } Action = DefaultAct;
  const char *ProgName = argv[0];
  const char *Input = nullptr;
  int Index;
  int Res;

  if (argc < 2)
    Error(ProgName, "too few arguments");

  for (Index = 1; Index < argc; ++Index) {
    if (argv[Index][0] == '-') {
      if (Action != DefaultAct)
        Error(ProgName, "too many options");

      if (EqualOneOf(argv[Index], "-l", "-L", "-lex", "--lex")) {
        Action = LexAct;
        continue;
      }

      if (EqualOneOf(argv[Index], "-f", "-F", "-file", "--file")) {
        Action = DumpFileAct;
        continue;
      }

      if (EqualOneOf(argv[Index], "-p", "-P", "-parse", "--parse")) {
        Action = ParseAct;
        continue;
      }

      if (EqualOneOf(argv[Index], "-d", "-D", "-debug", "--debug")) {
        Action = DebugAct;
        continue;
      }

      if (EqualOneOf(argv[Index], "-h", "-H", "-help", "--help")) {
        Usage(ProgName);
        std::exit(EXIT_SUCCESS);
      }

      std::cerr << ProgName << ": invalid option `" << argv[Index] << "'\n\n";
      Usage(ProgName);
      std::exit(EXIT_FAILURE);
    } else {
      Input = argv[Index++];
      break;
    }
  }

  if (!Input)
    Error(ProgName, "no input file");

  cmm::SourceMgr SrcMgr(Input);

  switch (Action) {
  default:
    Res = EXIT_FAILURE;
  case DumpFileAct:
    Res = DumpFile(SrcMgr);
    break;
  case DefaultAct:
    Res = Interpret(SrcMgr, argc - Index, argv + Index);
    break;
  case LexAct:
    Res = AsLexInput(SrcMgr);
    break;
  case ParseAct:
    Res = DumpAST(SrcMgr);
    break;
  case DebugAct:
    Res = Interpret(SrcMgr, argc - Index, argv + Index, true);
    break;
  }

  return Res;
}

void Error(const char *Name, const char *Message) {
  std::cerr << Name << ": " << Message << "\n\n";
  Usage(Name);
  std::exit(EXIT_FAILURE);
}

void Usage(const char *Name) {
  std::cerr
      << "USAGE: " << Name
      << " [options] <input file> Args...\n\n"
         "OPTIONS:\n\n"
         "  -h  --help       print this usage and exit\n"
         "  -f  --file       dump a file and exit (for debugging)\n"
         "  -l  --lex        lex tokens from a CMM source code file\n"
         "  -p  --parse      parse a CMM source code file and dump AST\n"
         "  -d  --debug      interpret a file with extra information dumped\n\n"
         "Report bugs to <hsu [at] whu [dot] edu [dot] cn>.\n";
}

int DumpFile(cmm::SourceMgr &SrcMgr) {
  SrcMgr.dumpFile();
  return EXIT_SUCCESS;
}

int AsLexInput(cmm::SourceMgr &SrcMgr) {
  using namespace cmm;
  using std::cout;
  CMMLexer Lexer(SrcMgr);

  bool Err = false;
  while (Lexer.Lex().isNot(Token::Eof)) {
    auto LineCol = SrcMgr.getLineColByLoc(Lexer.getLoc());
    cout << "(Line " << LineCol.first + 1 << ", Col "
         << LineCol.second + 1 << ") ";

    switch (Lexer.getKind()) {
    default:
      cout << "Unknown token: " << Lexer.getKind();
      Err = true;
      break;
    case Token::Error:
      Err = true; // error already printed.
      break;
    case Token::Identifier:
      cout << "Identifier: " << Lexer.getStrVal();  break;
    case Token::String:
      cout << "String: " << Lexer.getStrVal(); break;
    case Token::Integer:
      cout << "Integer: " << Lexer.getIntVal(); break;
    case Token::Double:
      cout << "Double: " << Lexer.getDoubleVal(); break;
    case Token::Boolean:
      cout << "Boolean: " << (Lexer.getBoolVal() ? "True" : "False"); break;
    case Token::InfixOp:
      cout << "InfixOp: " << Lexer.getStrVal(); break;
    case Token::LParen:         
      cout << "LParen: ("; 
      break;
    case Token::RParen:         cout << "RParen: )"; break;
    case Token::LBrac:          cout << "LBrac: ["; break;
    case Token::RBrac:          cout << "RBrac: ]"; break;
    case Token::LCurly:         cout << "LCurly: {"; break;
    case Token::RCurly:         cout << "RCurly: }"; break;
    case Token::Plus:           cout << "Plus: +"; break;
    case Token::Minus:          cout << "Minus: -"; break;
    case Token::Star:           cout << "Star: *"; break;
    case Token::Slash:          cout << "Slash: /"; break;
    case Token::Semicolon:      cout << "Semicolon: ;"; break;
    case Token::Comma:          cout << "Comma: ,"; break;
    case Token::Equal:          cout << "Equal: ="; break;
    case Token::Percent:        cout << "Percent: %"; break;
    case Token::Exclaim:        cout << "Exclaim: !"; break;
    case Token::AmpAmp:         cout << "AmpAmp: &&"; break;
    case Token::PipePipe:       cout << "PipePipe: ||"; break;
    case Token::Less:           cout << "Less: <"; break;
    case Token::LessEqual:      cout << "LessEqual: <="; break;
    case Token::EqualEqual:     cout << "EqualEqual: =="; break;
    case Token::Greater:        cout << "Greater: >"; break;
    case Token::Amp:            cout << "Amp: &"; break;
    case Token::Pipe:           cout << "Pipe: |"; break;
    case Token::LessLess:       cout << "LessLess: <<"; break;
    case Token::Caret:          cout << "Caret: ^"; break;
    case Token::Tilde:          cout << "Tilde: ~"; break;
    case Token::GreaterGreater: cout << "GreaterGreater: >>"; break;
    case Token::Kw_if:          cout << "Keyword: if"; break;
    case Token::Kw_else:        cout << "Keyword: else"; break;
    case Token::Kw_for:         cout << "Keyword: for"; break;
    case Token::Kw_while:       cout << "Keyword: while"; break;
    case Token::Kw_do:          cout << "Keyword: do"; break;
    case Token::Kw_break:       cout << "Keyword: break"; break;
    case Token::Kw_continue:    cout << "Keyword: continue"; break;
    case Token::Kw_int:         cout << "Keyword: int"; break;
    case Token::Kw_double:      cout << "Keyword: double"; break;
    case Token::Kw_bool:        cout << "Keyword: bool"; break;
    case Token::Kw_void:        cout << "Keyword: void"; break;
    case Token::Kw_return:      cout << "Keyword: return"; break;
    case Token::Kw_infix:       cout << "Keyword: infix"; break;
    }
    cout << "\n";
  }
  return Err;
}

int Interpret(cmm::SourceMgr &SrcMgr, int Argc, char **Argv, bool Verbose) {
  using namespace cmm;
  CMMParser Parser(SrcMgr);

  int Err = Parser.parse();
  if (!Err) {
    if (Verbose) {
      Parser.dumpAST();
      std::cout << "\n\n****** Interpreter started ******\n\n";
    }

    CMMInterpreter Interpreter(Parser.getTopLevelBlock(),
                               Parser.getFunctionDefinition(),
                               Parser.getInfixOpDefinition());
    Err = Interpreter.interpret(Argc, Argv);
  }
  return Err;
}


int DumpAST(cmm::SourceMgr &SrcMgr) {
  using namespace cmm;
  CMMParser Parser(SrcMgr);

  int Err = Parser.parse();
  if (!Err) {
    Parser.dumpAST();
  }
  return Err;
}
